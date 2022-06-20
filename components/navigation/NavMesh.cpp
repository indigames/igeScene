#include "components/navigation/NavMesh.h"
#include "components/navigation/Navigable.h"
#include "components/navigation/NavArea.h"
#include "components/navigation/NavAgent.h"
#include "components/navigation/NavObstacle.h"
#include "components/navigation/OffMeshLink.h"
#include "components/navigation/NavAgentManager.h"
#include "components/physic/collider/MeshCollider.h"
#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "scene/SceneObject.h"
#include "utils/ShapeDrawer.h"

#define DEFAULT_TILE_SIZE 64
#define DEFAULT_CELL_SIZE 0.3f
#define DEFAULT_CELL_HEIGHT 0.05f
#define DEFAULT_AGENT_HEIGHT 2.0f
#define DEFAULT_AGENT_RADIUS 0.6f
#define DEFAULT_AGENT_MAX_CLIMB 0.9f
#define DEFAULT_AGENT_MAX_SLOPE 45.0f
#define DEFAULT_REGION_MIN_SIZE 8.0f
#define DEFAULT_REGION_MERGE_SIZE 20.0f
#define DEFAULT_EDGE_MAX_LENGTH 12.0f
#define DEFAULT_EDGE_MAX_ERROR 1.3f
#define DEFAULT_DETAIL_SAMPLE_DISTANCE 6.0f
#define DEFAULT_DETAIL_SAMPLE_MAX_ERROR 1.0f

#ifndef INFINITY
    #include <limits>
    #define INFINITY std::numeric_limits<float>::max()
#endif

namespace ige::scene
{
    NavBuildData::NavBuildData() : ctx(new rcContext(true)),
                                   heightField(nullptr),
                                   compactHeightField(nullptr)
    {
    }

    NavBuildData::~NavBuildData()
    {
        if (ctx)
        {
            delete (ctx);
            ctx = nullptr;
        }

        if (heightField)
        {
            rcFreeHeightField(heightField);
            heightField = nullptr;
        }

        if (compactHeightField)
        {
            rcFreeCompactHeightfield(compactHeightField);
            compactHeightField = nullptr;
        }
    }

    SimpleNavBuildData::SimpleNavBuildData() : NavBuildData(),
                                               contourSet(nullptr),
                                               polyMesh(nullptr),
                                               polyMeshDetail(nullptr)
    {
    }

    SimpleNavBuildData::~SimpleNavBuildData()
    {
        if (contourSet)
        {
            rcFreeContourSet(contourSet);
            contourSet = nullptr;
        }

        if (polyMesh)
        {
            rcFreePolyMesh(polyMesh);
            polyMesh = nullptr;
        }

        if (polyMeshDetail)
        {
            rcFreePolyMeshDetail(polyMeshDetail);
            polyMeshDetail = nullptr;
        }
    }

    //! Constructor
    NavMesh::NavMesh(SceneObject& owner)
        : Component(owner),
          m_tileSize(DEFAULT_TILE_SIZE),
          m_cellSize(DEFAULT_CELL_SIZE),
          m_cellHeight(DEFAULT_CELL_HEIGHT),
          m_agentHeight(DEFAULT_AGENT_HEIGHT),
          m_agentRadius(DEFAULT_AGENT_RADIUS),
          m_agentMaxClimb(DEFAULT_AGENT_MAX_CLIMB),
          m_agentMaxSlope(DEFAULT_AGENT_MAX_SLOPE),
          m_regionMinSize(DEFAULT_REGION_MIN_SIZE),
          m_regionMergeSize(DEFAULT_REGION_MERGE_SIZE),
          m_edgeMaxLength(DEFAULT_EDGE_MAX_LENGTH),
          m_edgeMaxError(DEFAULT_EDGE_MAX_ERROR),
          m_detailSampleDistance(DEFAULT_DETAIL_SAMPLE_DISTANCE),
          m_detailSampleMaxError(DEFAULT_DETAIL_SAMPLE_MAX_ERROR),
          m_partitionType(EPartitionType::WATERSHED)
    {
        m_pathData = std::make_unique<FindPathData>();

        // Create navigation agent manager for this mesh
        m_navAgentManager = getOwner()->getRoot()->getComponent<NavAgentManager>();
        if (m_navAgentManager.expired())
            m_navAgentManager = getOwner()->getRoot()->addComponent<NavAgentManager>();
    }

    NavMesh::~NavMesh()
    {
        releaseNavMesh();

        m_navMesh = nullptr;
        m_navMeshQuery = nullptr;
        m_pathData.reset();
        m_navAgentManager.reset();
    }

    void NavMesh::releaseNavMesh()
    {
        if (!m_navAgentManager.expired())
            m_navAgentManager.lock()->deactivateAllAgents();

        if (m_navMesh)
        {
            dtFreeNavMesh(m_navMesh);
            m_navMesh = nullptr;
        }

        if (m_navMeshQuery)
        {
            dtFreeNavMeshQuery(m_navMeshQuery);
            m_navMeshQuery = nullptr;
        }

        m_numTilesX = 0;
        m_numTilesZ = 0;
        m_boundingBox.reset({ Vec3(INFINITY, INFINITY, INFINITY), Vec3(-INFINITY, -INFINITY, -INFINITY) });
    }

    bool NavMesh::build()
    {
        if(!isEnabled())
            return false;

        if (m_navMesh != nullptr)
            return true;

        // Create navigation agent manager for this mesh
        if (m_navAgentManager.expired()) return false;
        m_navAgentManager.lock()->deactivateAllAgents();

        // Release old data
        releaseNavMesh();

        // Collect geometry info
        std::vector<NavGeoInfo> geometryList;
        collectGeometries(geometryList);
        if (geometryList.empty())
            return true;

        // Extend bounding box
        for (size_t i = 0; i < geometryList.size(); ++i)
            m_boundingBox.addInternalBox(geometryList[i].boundingBox);

        // Append bounding box padding
        m_boundingBox.MinEdge -= m_padding;
        m_boundingBox.MaxEdge += m_padding;

        // Build navigation mesh
        {
            // Calculate number of tiles
            int gridW = 0, gridH = 0;
            float tileEdgeLength = (float)m_tileSize * m_cellSize;
            rcCalcGridSize(m_boundingBox.MinEdge.P(), m_boundingBox.MaxEdge.P(), m_cellSize, &gridW, &gridH);

            // Calculate number of tiles by X and Z coordinators
            m_numTilesX = (gridW + m_tileSize - 1) / m_tileSize;
            m_numTilesZ = (gridH + m_tileSize - 1) / m_tileSize;

            // Calculate max number of tiles and polygons
            uint32_t maxTiles = next_power_of_two((uint32_t)(m_numTilesX * m_numTilesZ));
            uint32_t tileBits = (uint32_t)std::log2(maxTiles);
            uint32_t maxPolys = 1u << (22 - tileBits);

            // Setup params
            dtNavMeshParams params;
            rcVcopy(params.orig, m_boundingBox.MinEdge.P());
            params.tileWidth = tileEdgeLength;
            params.tileHeight = tileEdgeLength;
            params.maxTiles = maxTiles;
            params.maxPolys = maxPolys;

            // Allocate nav mesh
            m_navMesh = dtAllocNavMesh();
            if (!m_navMesh)
                return false;

            // Initialize nav mesh
            if (dtStatusFailed(m_navMesh->init(&params)))
            {
                releaseNavMesh();
                return false;
            }

            // Build tiles
            buildTiles(geometryList, Vec2(0.f, 0.f), Vec2(getNumTilesX() - 1.f, getNumTilesZ() - 1.f));
        }

        m_navAgentManager.lock()->reactivateAllAgents();
        return true;
    }

    //! Rebuild part of the navigation mesh contained by the world-space bounding box
    bool NavMesh::build(const AABBox &boundingBox)
    {
        if (!m_navMesh)
            return false;

        auto inverse = getOwner()->getTransform()->getWorldMatrix().Inverse();
        auto localSpaceBox = boundingBox.Transform(inverse);
        auto tileEdgeLength = m_tileSize * m_cellSize;

        std::vector<NavGeoInfo> geometryList;
        collectGeometries(geometryList);

        int sx = std::clamp((int)((localSpaceBox.MinEdge.X() - m_boundingBox.MinEdge.X()) / tileEdgeLength), 0, m_numTilesX - 1);
        int sz = std::clamp((int)((localSpaceBox.MinEdge.Z() - m_boundingBox.MinEdge.Z()) / tileEdgeLength), 0, m_numTilesZ - 1);
        int ex = std::clamp((int)((localSpaceBox.MaxEdge.X() - m_boundingBox.MinEdge.X()) / tileEdgeLength), 0, m_numTilesX - 1);
        int ez = std::clamp((int)((localSpaceBox.MaxEdge.Z() - m_boundingBox.MinEdge.Z()) / tileEdgeLength), 0, m_numTilesZ - 1);

        auto numTiles = buildTiles(geometryList, Vec2(sx, sz), Vec2(ex, ez));
        return true;
    }

    //! Rebuild part of the navigation mesh in the rectangular area
    bool NavMesh::build(const Vec2 &from, const Vec2 &to)
    {
        std::vector<NavGeoInfo> geometryList;
        collectGeometries(geometryList);
        auto numTiles = buildTiles(geometryList, from, to);
        return true;
    }

    //! Collect geometry from under Navigable components
    void NavMesh::collectGeometries(std::vector<NavGeoInfo> &geometryList)
    {
        // Get Navigable components from child nodes
        std::vector<Component *> navigables;
        getOwner()->getComponentsRecursive(navigables, "Navigable");

        std::unordered_set<SceneObject *> processedNodes;
        for (size_t i = 0; i < navigables.size(); ++i)
        {
            auto navigable = static_cast<Navigable *>(navigables[i]);
            if (navigable->isEnabled())
            {
                // Ensure transform updated
                collectGeometries(geometryList, navigable->getOwner(), processedNodes, navigable->isRecursive());
            }
        }

        auto inverse = getOwner()->getTransform()->getWorldMatrix().Inverse();
        std::vector<Component *> offMeshLinks;
        getOwner()->getComponentsRecursive(offMeshLinks, "OffMeshLink");
        for (size_t i = 0; i < offMeshLinks.size(); ++i)
        {
            auto link = static_cast<OffMeshLink *>(offMeshLinks[i]);
            if (link->isEnabled() && link->getEndPoint())
            {
                const auto &pos = link->getOwner()->getTransform()->getPosition();

                auto aabbMin = pos - Vec3(link->getRadius(), link->getRadius(), link->getRadius());
                auto aabbMax = pos + Vec3(link->getRadius(), link->getRadius(), link->getRadius());
                auto aabb = AABBox(aabbMin, aabbMax).Transform(inverse);

                NavGeoInfo info;
                info.component = link;
                info.boundingBox = aabb;
                geometryList.push_back(info);
            }
        }

        // Get nav area volumes
        std::vector<Component *> navAreas;
        getOwner()->getComponentsRecursive(navAreas, "NavArea");
        m_areas.clear();
        for (auto areaComp : navAreas)
        {
            auto area = static_cast<NavArea *>(areaComp);
            if (area && area->isEnabled())
            {
                NavGeoInfo info;
                info.component = area;
                info.boundingBox = area->getWorldBoundingBox();
                geometryList.push_back(info);
                m_areas.push_back(area);
            }
        }
    }

    //! Visit nodes and collect navigable geometry
    void NavMesh::collectGeometries(std::vector<NavGeoInfo> &geometryList, SceneObject *node, std::unordered_set<SceneObject *> &processedNodes, bool recursive)
    {
        // Make sure nodes are not included twice
        if (processedNodes.count(node) > 0)
            return;

        // Exclude obstacles and crowd agents from consideration
        if (node->getComponent<NavObstacle>() || node->getComponent<NavAgent>())
            return;

        processedNodes.insert(node);

        // Get figure component
        auto figure = node->getComponent<FigureComponent>();
        if (figure && figure->isEnabled() && !figure->isSkipSerialize())
        {
            NavGeoInfo info;
            info.component = figure.get();
            info.transform = Mat4::IdentityMat();
            info.boundingBox = node->getWorldAABB();
            geometryList.push_back(info);
        }

        // Recursive to child nodes
        if (recursive)
        {
            const auto &children = node->getChildren();
            for (size_t i = 0; i < children.size(); ++i)
                if(!children[i].expired())
                    collectGeometries(geometryList, children[i].lock().get(), processedNodes, recursive);
        }
    }

    //! Get geometry data within a bounding box
    void NavMesh::getTileGeometry(NavBuildData *build, std::vector<NavGeoInfo> &geometryList, AABBox &box)
    {
        auto inverse = getOwner()->getTransform()->getWorldMatrix().Inverse();

        for (size_t i = 0; i < geometryList.size(); ++i)
        {
            if (box.IsPartiallyInside(geometryList[i].boundingBox))
            {
                const auto &transform = geometryList[i].transform;

                if (geometryList[i].component->getName() == "OffMeshLink")
                {
                    auto *link = static_cast<OffMeshLink *>(geometryList[i].component);
                    auto start = inverse * link->getOwner()->getTransform()->getPosition();
                    auto end = inverse * link->getEndPoint()->getTransform()->getPosition();
                    build->offMeshVertices.push_back(start);
                    build->offMeshVertices.push_back(end);
                    build->offMeshRadius.push_back(link->getRadius());
                    build->offMeshFlags.push_back((uint16_t)link->getMask());
                    build->offMeshAreas.push_back((uint8_t)link->getAreaId());
                    build->offMeshDir.push_back((uint8_t)(link->isBidirectional() ? DT_OFFMESH_CON_BIDIR : 0));
                    continue;
                }
                else if (geometryList[i].component->getName() == "NavArea")
                {
                    auto* area = static_cast<NavArea*>(geometryList[i].component);
                    NavAreaStub stub;
                    stub.areaID = area->getAreaId();
                    stub.bounds = area->getWorldBoundingBox();
                    build->navAreas.push_back(stub);
                    continue;
                }

                auto figure = dynamic_cast<FigureComponent *>(geometryList[i].component);
                if (figure && figure->getFigure())
                    addTriMeshGeometry(build, figure->getFigure(), transform);
            }
        }
    }

    //! Add a triangle mesh to the geometry data.
    void NavMesh::addTriMeshGeometry(NavBuildData *build, Figure *figure, const Mat4 &transform)
    {
        if (!figure)
            return;

        if (figure->NumMeshes() > 0)
        {
            int space = Space::WorldSpace;
            std::vector<Vec3> positions;

            for (int i = 0; i < figure->NumMeshes(); ++i)
            {
                auto mesh = figure->GetMesh(i);

                // Ignore thrash values
                if (mesh->numVerticies <= 0 || mesh->numIndices <= 0)
                    continue;

                // Read vertices from vertices buffer
                positions.clear();
                float *palettebuffer = nullptr;
                float *inbindSkinningMatrices = nullptr;
                figure->AllocTransformBuffer(space, palettebuffer, inbindSkinningMatrices);
                figure->ReadPositions(i, 0, mesh->numVerticies, space, palettebuffer, inbindSkinningMatrices, &positions);

                auto destVertexStart = build->vertices.size();
                for (const auto& pos : positions)
                {
                    build->vertices.push_back({ pos[0], pos[1], pos[2] });
                }
                positions.clear();

                if (inbindSkinningMatrices)
                    PYXIE_FREE_ALIGNED(inbindSkinningMatrices);
                if (palettebuffer)
                    PYXIE_FREE_ALIGNED(palettebuffer);

                // Read indices from indices buffer
                for (int k = 0; k < mesh->numIndices; ++k)
                {
                    if (mesh->numVerticies > 65535)
                        build->indices.push_back(((uint32_t*)mesh->indices)[k] + destVertexStart);
                    else
                        build->indices.push_back(mesh->indices[k] + destVertexStart);
                }
            }
        }
    }

    //! Build one tile of the navigation mesh
    bool NavMesh::buildTile(std::vector<NavGeoInfo> &geometryList, int x, int z)
    {
        m_navMesh->removeTile(m_navMesh->getTileRefAt(x, z, 0), nullptr, nullptr);
        const auto &tileBoundingBox = getTileBoundingBox(Vec2(x, z));
        SimpleNavBuildData build;

        rcConfig cfg;
        memset(&cfg, 0, sizeof cfg);
        cfg.cs = m_cellSize;
        cfg.ch = m_cellHeight;
        cfg.walkableSlopeAngle = m_agentMaxSlope;
        cfg.walkableHeight = (int)std::ceil(m_agentHeight / cfg.ch);
        cfg.walkableClimb = (int)std::floor(m_agentMaxClimb / cfg.ch);
        cfg.walkableRadius = (int)std::ceil(m_agentRadius / cfg.cs);
        cfg.maxEdgeLen = (int)(m_edgeMaxLength / m_cellSize);
        cfg.maxSimplificationError = m_edgeMaxError;
        cfg.minRegionArea = (int)std::sqrtf(m_regionMinSize);
        cfg.mergeRegionArea = (int)std::sqrtf(m_regionMergeSize);
        cfg.maxVertsPerPoly = 6;
        cfg.tileSize = m_tileSize;
        cfg.borderSize = cfg.walkableRadius + 3; // Add padding
        cfg.width = cfg.tileSize + cfg.borderSize * 2;
        cfg.height = cfg.tileSize + cfg.borderSize * 2;
        cfg.detailSampleDist = m_detailSampleDistance < 0.9f ? 0.0f : m_cellSize * m_detailSampleDistance;
        cfg.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

        rcVcopy(cfg.bmin, tileBoundingBox.MinEdge.P());
        rcVcopy(cfg.bmax, tileBoundingBox.MaxEdge.P());
        cfg.bmin[0] -= cfg.borderSize * cfg.cs;
        cfg.bmin[2] -= cfg.borderSize * cfg.cs;
        cfg.bmax[0] += cfg.borderSize * cfg.cs;
        cfg.bmax[2] += cfg.borderSize * cfg.cs;

        auto expandedBox = AABBox(*reinterpret_cast<Vec3 *>(cfg.bmin), *reinterpret_cast<Vec3 *>(cfg.bmax));
        getTileGeometry(&build, geometryList, expandedBox);

        if (build.vertices.empty() || build.indices.empty())
            return true;

        build.heightField = rcAllocHeightfield();
        if (!build.heightField)
            return false;

        if (!rcCreateHeightfield(build.ctx, *build.heightField, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch))
            return false;

        auto numTriangles = build.indices.size() / 3;
        auto triAreas = new uint8_t[numTriangles];
        memset(triAreas, 0, numTriangles);

        rcMarkWalkableTriangles(build.ctx, cfg.walkableSlopeAngle, &(build.vertices[0][0]), build.vertices.size(), &build.indices[0], numTriangles, triAreas);
        rcRasterizeTriangles(build.ctx, &(build.vertices[0][0]), build.vertices.size(), &build.indices[0], triAreas, numTriangles, *build.heightField, cfg.walkableClimb);
        rcFilterLowHangingWalkableObstacles(build.ctx, cfg.walkableClimb, *build.heightField);

        rcFilterWalkableLowHeightSpans(build.ctx, cfg.walkableHeight, *build.heightField);
        rcFilterLedgeSpans(build.ctx, cfg.walkableHeight, cfg.walkableClimb, *build.heightField);

        build.compactHeightField = rcAllocCompactHeightfield();
        if (!build.compactHeightField)
            return false;

        if (!rcBuildCompactHeightfield(build.ctx, cfg.walkableHeight, cfg.walkableClimb, *build.heightField, *build.compactHeightField))
            return false;

        if (!rcErodeWalkableArea(build.ctx, cfg.walkableRadius, *build.compactHeightField))
            return false;

        // Mark area volumes
        for (size_t i = 0; i < build.navAreas.size(); ++i)
            rcMarkBoxArea(build.ctx, build.navAreas[i].bounds.MinEdge.P(), build.navAreas[i].bounds.MaxEdge.P(), build.navAreas[i].areaID, *build.compactHeightField);

        if (m_partitionType == NavMesh::EPartitionType::WATERSHED)
        {
            if (!rcBuildDistanceField(build.ctx, *build.compactHeightField))
                return false;

            if (!rcBuildRegions(build.ctx, *build.compactHeightField, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea))
                return false;
        }
        else
        {
            if (!rcBuildRegionsMonotone(build.ctx, *build.compactHeightField, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea))
                return false;
        }

        build.contourSet = rcAllocContourSet();
        if (!build.contourSet)
            return false;

        if (!rcBuildContours(build.ctx, *build.compactHeightField, cfg.maxSimplificationError, cfg.maxEdgeLen, *build.contourSet))
            return false;

        build.polyMesh = rcAllocPolyMesh();
        if (!build.polyMesh)
            return false;

        if (!rcBuildPolyMesh(build.ctx, *build.contourSet, cfg.maxVertsPerPoly, *build.polyMesh))
            return false;

        build.polyMeshDetail = rcAllocPolyMeshDetail();
        if (!build.polyMeshDetail)
            return false;

        if (!rcBuildPolyMeshDetail(build.ctx, *build.polyMesh, *build.compactHeightField, cfg.detailSampleDist, cfg.detailSampleMaxError, *build.polyMeshDetail))
            return false;

        // Set polygon flags
        for (int i = 0; i < build.polyMesh->npolys; ++i)
        {
            if (build.polyMesh->areas[i] != RC_NULL_AREA)
                build.polyMesh->flags[i] = 0x1;
        }

        uint8_t *navData = nullptr;
        int navDataSize = 0;

        dtNavMeshCreateParams params;
        memset(&params, 0, sizeof params);
        params.verts = build.polyMesh->verts;
        params.vertCount = build.polyMesh->nverts;
        params.polys = build.polyMesh->polys;
        params.polyAreas = build.polyMesh->areas;
        params.polyFlags = build.polyMesh->flags;
        params.polyCount = build.polyMesh->npolys;
        params.nvp = build.polyMesh->nvp;
        params.detailMeshes = build.polyMeshDetail->meshes;
        params.detailVerts = build.polyMeshDetail->verts;
        params.detailVertsCount = build.polyMeshDetail->nverts;
        params.detailTris = build.polyMeshDetail->tris;
        params.detailTriCount = build.polyMeshDetail->ntris;
        params.walkableHeight = m_agentHeight;
        params.walkableRadius = m_agentRadius;
        params.walkableClimb = m_agentMaxClimb;
        params.tileX = x;
        params.tileY = z;
        rcVcopy(params.bmin, build.polyMesh->bmin);
        rcVcopy(params.bmax, build.polyMesh->bmax);
        params.cs = cfg.cs;
        params.ch = cfg.ch;
        params.buildBvTree = true;

        // Add off-mesh links if have them
        if (build.offMeshRadius.size())
        {
            params.offMeshConCount = build.offMeshRadius.size();
            params.offMeshConVerts = build.offMeshVertices[0].P();
            params.offMeshConRad = &build.offMeshRadius[0];
            params.offMeshConFlags = &build.offMeshFlags[0];
            params.offMeshConAreas = &build.offMeshAreas[0];
            params.offMeshConDir = &build.offMeshDir[0];
        }

        if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
            return false;

        if (dtStatusFailed(m_navMesh->addTile(navData, navDataSize, DT_TILE_FREE_DATA, 0, nullptr)))
            return false;

        return true;
    }

    //! Build tiles in the rectangular area. Return number of built tiles.
    uint32_t NavMesh::buildTiles(std::vector<NavGeoInfo> &geometryList, const Vec2 &from, const Vec2 &to)
    {
        uint32_t numTiles = 0;
        for (int z = from.Y(); z <= to.Y(); ++z)
        {
            for (int x = from.X(); x <= to.X(); ++x)
            {
                if (buildTile(geometryList, x, z))
                    ++numTiles;
            }
        }
        return numTiles;
    }

    //! Return tile data.
    MemBuffer NavMesh::getTileData(const Vec2 &tile) const
    {
        MemBuffer dest;
        writeTile(dest, tile.X(), tile.Y());
        return dest;
    }

    //! Write tile data.
    void NavMesh::writeTile(MemBuffer &dest, int x, int z) const
    {
        const auto *meshTile = m_navMesh->getTileAt(x, z, 0);
        if (!meshTile)
            return;
        dest.write<int>(x);
        dest.write<int>(z);
        dest.write<uint32_t>(m_navMesh->getTileRef(meshTile));
        dest.write<uint32_t>((uint32_t)meshTile->dataSize);
        dest.write(meshTile->data, (size_t)meshTile->dataSize);
    }

    //! Read tile data to the navigation mesh.
    bool NavMesh::readTile(MemBuffer &source)
    {
        int x, z;
        source.read<int>(x);
        source.read<int>(z);

        uint32_t tileRef;
        source.read<uint32_t>(tileRef);

        uint32_t navDataSize;
        source.read<uint32_t>(navDataSize);

        auto *navData = (uint8_t *)dtAlloc(navDataSize, DT_ALLOC_PERM);
        if (!navData)
            return false;
        source.read(navData, navDataSize);

        if (dtStatusFailed(m_navMesh->addTile(navData, navDataSize, DT_TILE_FREE_DATA, 0, nullptr)))
        {
            dtFree(navData);
            return false;
        }
        return true;
    }

    //! Add tile to navigation mesh.
    bool NavMesh::addTile(MemBuffer &tileData)
    {
        return readTile(tileData);
    }

    //! Remove tile from navigation mesh
    void NavMesh::removeTile(const Vec2 &tile)
    {
        if (!m_navMesh)
            return;

        const dtTileRef tileRef = m_navMesh->getTileRefAt(tile.X(), tile.Y(), 0);
        if (!tileRef)
            return;

        m_navMesh->removeTile(tileRef, nullptr, nullptr);
    }

    //! Remove all tiles from navigation mesh
    void NavMesh::removeAllTiles()
    {
        for (int i = 0; i < m_navMesh->getMaxTiles(); ++i)
        {
            const auto tile = static_cast<const dtNavMesh *>(m_navMesh)->getTile(i);
            if (tile && tile->header)
                m_navMesh->removeTile(m_navMesh->getTileRef(tile), nullptr, nullptr);
        }
    }

    //! Return whether the navigation mesh has tile
    bool NavMesh::hasTile(const Vec2 &tile) const
    {
        if (m_navMesh)
            return (m_navMesh->getTileAt(tile.X(), tile.Y(), 0) != nullptr);
        return false;
    }

    //! Return bounding box of the tile in the node space
    AABBox NavMesh::getTileBoundingBox(const Vec2 &tile) const
    {
        const float tileEdgeLength = (float)m_tileSize * m_cellSize;
        return AABBox(
            Vec3(m_boundingBox.MinEdge.X() + tileEdgeLength * (float)tile.X(),
                m_boundingBox.MinEdge.Y(),
                m_boundingBox.MinEdge.Z() + tileEdgeLength * (float)tile.Y()),
            Vec3(m_boundingBox.MinEdge.X() + tileEdgeLength * (float)(tile.X() + 1),
                m_boundingBox.MaxEdge.Y(),
                m_boundingBox.MinEdge.Z() + tileEdgeLength * (float)(tile.Y() + 1))
        );
    }

    //! Return index of the tile at the position
    Vec2 NavMesh::getTileIndex(const Vec3 &position) const
    {
        const float tileEdgeLength = (float)m_tileSize * m_cellSize;
        const auto localPosition = (getOwner()->getTransform()->getWorldMatrix().Inverse() * position) - m_boundingBox.MinEdge;
        int xIdx = std::min(std::max(0, (int)floor(localPosition.X() / tileEdgeLength)), getNumTilesX() - 1);
        int zIdx = std::min(std::max(0, (int)floor(localPosition.Z() / tileEdgeLength)), getNumTilesZ() - 1);
        return Vec2(xIdx, zIdx);
    }

    //! Try to move along the surface from one point to another
    Vec3 NavMesh::moveAlongSurface(const Vec3 &start, const Vec3 &end, const Vec3 &extents, int maxVisited, const dtQueryFilter *filter)
    {
        if (!filter || !initializeQuery())
            return end;

        const auto &transform = getOwner()->getTransform()->getWorldMatrix();
        auto inverse = transform.Inverse();

        auto localStart = inverse *start;
        auto localEnd = inverse * end;

        dtPolyRef startRef;
        m_navMeshQuery->findNearestPoly(localStart.P(), extents.P(), filter, &startRef, nullptr);
        if (!startRef)
            return end;

        Vec3 resultPos;
        int visitedCount = 0;
        maxVisited = Max(maxVisited, 0);
        std::vector<dtPolyRef> visited((uint32_t)maxVisited);
        m_navMeshQuery->moveAlongSurface(startRef, localStart.P(), localEnd.P(), filter, resultPos.P(), maxVisited ? &visited[0] : nullptr, &visitedCount, maxVisited);
        return transform * resultPos;
    }

    //! Find the nearest point on the navigation mesh to a given
    Vec3 NavMesh::findNearestPoint(const Vec3 &point, const Vec3 &extents, const dtQueryFilter *filter, dtPolyRef *nearestRef)
    {
        if (!filter || !initializeQuery())
            return point;

        const auto &transform = getOwner()->getTransform()->getWorldMatrix();
        auto inverse = transform.Inverse();

        auto localPoint = inverse * point;
        Vec3 nearestPoint;

        dtPolyRef pointRef;
        if (!nearestRef)
            nearestRef = &pointRef;

        m_navMeshQuery->findNearestPoly(localPoint.P(), extents.P(), filter, nearestRef, nearestPoint.P());
        return *nearestRef ? transform * nearestPoint : point;
    }

    //! Find a path between world space points. Return non-empty list of points if successful. Extents specifies how far off the navigation mesh the points can be.
    void NavMesh::findPath(std::vector<Vec3> &dest, const Vec3 &start, const Vec3 &end, const Vec3 &extents, const dtQueryFilter *filter)
    {
        std::vector<NavPathPoint> navPathPoints;
        findPath(navPathPoints, start, end, extents, filter);

        dest.clear();
        for (size_t i = 0; i < navPathPoints.size(); ++i)
            dest.push_back(navPathPoints[i].position);
    }

    //! Find a path between world space points. Return non-empty list of navigation path points if successful. Extents specifies how far off the navigation mesh the points can be.
    void NavMesh::findPath(std::vector<NavPathPoint> &dest, const Vec3 &start, const Vec3 &end, const Vec3 &extents, const dtQueryFilter *filter)
    {
        dest.clear();

        if (!filter || !initializeQuery())
            return;

        // Navigation data is in local space. Transform path points from world to local
        const auto &transform = getOwner()->getTransform()->getWorldMatrix();
        auto inverse = transform.Inverse();

        auto localStart = inverse * start;
        auto localEnd = inverse * end;

        dtPolyRef startRef;
        dtPolyRef endRef;
        m_navMeshQuery->findNearestPoly(localStart.P(), extents.P(), filter, &startRef, nullptr);
        m_navMeshQuery->findNearestPoly(localEnd.P(), extents.P(), filter, &endRef, nullptr);

        if (!startRef || !endRef)
            return;

        int numPolys = 0;
        int numPathPoints = 0;

        m_navMeshQuery->findPath(startRef, endRef, localStart.P(), localEnd.P(), filter, m_pathData->polys, &numPolys, MAX_POLYS);
        if (!numPolys)
            return;

        auto actualLocalEnd = localEnd;

        // If full path was not found, clamp end point to the end polygon
        if (m_pathData->polys[numPolys - 1] != endRef)
            m_navMeshQuery->closestPointOnPoly(m_pathData->polys[numPolys - 1], localEnd.P(), actualLocalEnd.P(), nullptr);

        m_navMeshQuery->findStraightPath(localStart.P(), actualLocalEnd.P(), m_pathData->polys, numPolys,
                                         m_pathData->pathPoints[0].P(), m_pathData->pathFlags, m_pathData->pathPolys, &numPathPoints, MAX_POLYS);

        // Transform path result back to world space
        for (int i = 0; i < numPathPoints; ++i)
        {
            NavPathPoint pt;
            pt.position = transform *m_pathData->pathPoints[i];
            pt.flag = (NavPathPoint::Flag)m_pathData->pathFlags[i];
            uint32_t nearestNavAreaID = 0; // 0 is the default nav area ID

            // Walk through all NavAreas and find nearest
            float nearestDistance = std::numeric_limits<float>::max();
            for (auto area: m_areas)
            {
                if (area && area->isEnabled())
                {
                    auto aabb = area->getWorldBoundingBox();
                    if (aabb.isPointInside(pt.position))
                    {
                        auto areaWorldCenter = area->getOwner()->getTransform()->getPosition();
                        float distance = Vec3::LengthSqr(areaWorldCenter - pt.position);
                        if (distance < nearestDistance)
                        {
                            nearestDistance = distance;
                            nearestNavAreaID = area->getAreaId();
                        }
                    }
                }
            }
            pt.areaID = (uint8_t)nearestNavAreaID;
            dest.push_back(pt);
        }
    }

    //! Return a random point on the navigation mesh.
    Vec3 NavMesh::getRandomPoint(const dtQueryFilter *filter, dtPolyRef *randomRef)
    {
        if (!filter || !initializeQuery())
            return {0.f, 0.f, 0.f};

        dtPolyRef polyRef;
        Vec3 point = {0.f, 0.f, 0.f};

        auto random = []() { return std::rand() / float(RAND_MAX); };
        m_navMeshQuery->findRandomPoint(filter, random, randomRef ? randomRef : &polyRef, point.P());

        const auto &transform = getOwner()->getTransform()->getWorldMatrix();
        return transform * point;
    }

    //! Return a random point on the navigation mesh within a circle. The circle radius is only a guideline and in practice the returned point may be further away.
    Vec3 NavMesh::getRandomPointInCircle(const Vec3 &center, float radius, const Vec3 &extents, const dtQueryFilter *filter, dtPolyRef *randomRef)
    {
        if (randomRef)
            *randomRef = 0;

        if (!filter || !initializeQuery())
            return center;

        const auto &transform = getOwner()->getTransform()->getWorldMatrix();
        auto inverse = transform.Inverse();
        auto localCenter = inverse * center;

        dtPolyRef startRef;
        m_navMeshQuery->findNearestPoly(localCenter.P(), extents.P(), filter, &startRef, nullptr);
        if (!startRef)
            return center;

        dtPolyRef polyRef;
        if (!randomRef)
            randomRef = &polyRef;

        auto point = localCenter;
        auto random = []() { return std::rand() / float(RAND_MAX); };
        m_navMeshQuery->findRandomPointAroundCircle(startRef, localCenter.P(), radius, filter, random, randomRef, point.P());
        return transform * point;
    }

    //! Return distance to wall from a point. Maximum search radius must be specified.
    float NavMesh::getDistanceToWall(const Vec3 &point, float radius, const Vec3 &extents, const dtQueryFilter *filter, Vec3 *hitPos, Vec3 *hitNormal)
    {
        if (hitPos)
            *hitPos = {0.f, 0.f, 0.f};

        if (hitNormal)
            *hitNormal = {0.f, -1.f, 0.f};

        if (!filter || !initializeQuery())
            return radius;

        const auto &transform = getOwner()->getTransform()->getWorldMatrix();
        auto inverse = transform.Inverse();
        auto localPoint = inverse * point;

        dtPolyRef startRef;
        m_navMeshQuery->findNearestPoly(localPoint.P(), extents.P(), filter, &startRef, nullptr);
        if (!startRef)
            return radius;
        float hitDist = radius;
        Vec3 pos;
        if (!hitPos)
            hitPos = &pos;
        Vec3 normal;
        if (!hitNormal)
            hitNormal = &normal;
        m_navMeshQuery->findDistanceToWall(startRef, localPoint.P(), radius, filter, &hitDist, hitPos->P(), hitNormal->P());
        return hitDist;
    }

    //! Perform a walkability raycast on the navigation mesh between start and end and return the point where a wall was hit, or the end point if no walls.
    Vec3 NavMesh::raycast(const Vec3 &start, const Vec3 &end, const Vec3 &extents, const dtQueryFilter *filter, Vec3 *hitNormal)
    {
        if (hitNormal)
            *hitNormal = {0.f, -1.f, 0.f};

        if (!filter || !initializeQuery())
            return end;

        const auto &transform = getOwner()->getTransform()->getWorldMatrix();
        auto inverse = transform.Inverse();
        auto localStart = inverse * start;
        auto localEnd = inverse * end;

        dtPolyRef startRef;
        m_navMeshQuery->findNearestPoly(localStart.P(), extents.P(), filter, &startRef, nullptr);
        if (!startRef)
            return end;

        Vec3 normal;
        if (!hitNormal)
            hitNormal = &normal;
        float t;
        int numPolys;

        m_navMeshQuery->raycast(startRef, localStart.P(), localEnd.P(), filter, &t, hitNormal->P(), m_pathData->polys, &numPolys, MAX_POLYS);
        if (t == FLT_MAX)
            t = 1.0f;

        return (start * (1.0f - t) + end * t); // equivalent to start.lerp(end, t)
    }

    //! Set navigation data attribute.
    void NavMesh::setNavDataAttr(MemBuffer &buffer)
    {
        releaseNavMesh();

        if (buffer.size() == 0)
            return;

        buffer.read<AABBox>(m_boundingBox);
        buffer.read<int>(m_numTilesX);
        buffer.read<int>(m_numTilesZ);

        dtNavMeshParams params;
        rcVcopy(params.orig, m_boundingBox.MinEdge.P());
        buffer.read<float>(params.tileWidth);
        buffer.read<float>(params.tileHeight);
        buffer.read<int>(params.maxTiles);
        buffer.read<int>(params.maxPolys);

        m_navMesh = dtAllocNavMesh();
        if (!m_navMesh)
            return;

        if (dtStatusFailed(m_navMesh->init(&params)))
        {
            releaseNavMesh();
            return;
        }

        unsigned numTiles = 0;
        while (!buffer.isEof())
        {
            if (addTile(buffer))
                ++numTiles;
            else
                return;
        }
    }

    //! Return navigation data attribute.
    MemBuffer NavMesh::getNavDataAttr() const
    {
        MemBuffer ret;
        if (m_navMesh)
        {
            ret.write<AABBox>(m_boundingBox);
            ret.write<int>(m_numTilesX);
            ret.write<int>(m_numTilesZ);

            const dtNavMeshParams *params = m_navMesh->getParams();
            ret.write<float>(params->tileWidth);
            ret.write<float>(params->tileHeight);
            ret.write<int>(params->maxTiles);
            ret.write<int>(params->maxPolys);

            const dtNavMesh *navMesh = m_navMesh;
            for (int z = 0; z < m_numTilesZ; ++z)
                for (int x = 0; x < m_numTilesX; ++x)
                    writeTile(ret, x, z);
        }
        return ret;
    }

    bool NavMesh::initializeQuery()
    {
        if (!m_navMesh)
            return false;

        if (m_navMeshQuery)
            return true;

        m_navMeshQuery = dtAllocNavMeshQuery();
        if (!m_navMeshQuery)
        {
            return false;
        }

        if (dtStatusFailed(m_navMeshQuery->init(m_navMesh, MAX_POLYS)))
        {
            return false;
        }

        return true;
    }

    AABBox NavMesh::getWorldBoundingBox() const
    {
        const auto &transform = getOwner()->getTransform()->getWorldMatrix();
        auto worldAabb = m_boundingBox.Transform(transform);
        return worldAabb;
    }

    //! Update
    void NavMesh::onUpdate(float dt)
    {

    }

    //! Render
    void NavMesh::onRender()
    {
        if (isShowDebug())
        {
            // Render navigation mesh
            if (!getNavMesh())
                return;

            const dtNavMesh* mesh = getNavMesh();
            const auto& worldTransform = getOwner()->getRoot()->getTransform()->getWorldMatrix();

            for (int i = 0; i < mesh->getMaxTiles(); ++i)
            {
                const auto* tile = mesh->getTile(i);
                if (!tile || !tile->header)
                    continue;

                for (int j = 0; j < tile->header->polyCount; ++j)
                {
                    auto poly = tile->polys + j;
                    for (int k = 0; k < poly->vertCount; ++k)
                    {
                        auto start = worldTransform * *reinterpret_cast<const Vec3*>(&tile->verts[poly->verts[k] * 3]);
                        auto end = worldTransform * *reinterpret_cast<const Vec3*>(&tile->verts[poly->verts[(k + 1) % poly->vertCount] * 3]);
                        ShapeDrawer::drawLine(start, end, { 1.f, 1.f, 0.f });
                    }
                }
            }

            // Render off-mesh links
            std::vector<Component*> offMeshLinkComps;
            getOwner()->getRoot()->getComponentsRecursive(offMeshLinkComps, "OffMeshLink");

            std::vector<OffMeshLink*> offMeshLinks;
            for (auto comp : offMeshLinkComps)
            {
                auto link = static_cast<OffMeshLink*>(comp);
                if (link && link->isEnabled() && link->getEndPoint())
                {
                    const auto& start = link->getOwner()->getTransform()->getPosition();
                    const auto& end = link->getEndPoint()->getTransform()->getPosition();;
                    ShapeDrawer::drawLine(start, end, { 1.f, 0.f, 0.f });
                }
            }
        }
    }

    //! Serialize
    void NavMesh::to_json(json &j) const
    {
        Component::to_json(j);
        j["tileSize"] = getTileSize();
        j["cellSize"] = getCellSize();
        j["cellHeight"] = getCellHeight();
        j["agentHeight"] = getAgentHeight();
        j["agentRadius"] = getAgentRadius();
        j["agentMaxClimb"] = getAgentMaxClimb();
        j["agentMaxSlope"] = getAgentMaxSlope();
        j["regionMinSize"] = getRegionMinSize();
        j["regionMergeSize"] = getRegionMergeSize();
        j["edgeMaxLength"] = getEdgeMaxLength();
        j["edgeMaxError"] = getEdgeMaxError();
        j["sampleDist"] = getDetailSampleDistance();
        j["sampleError"] = getDetailSampleMaxError();
        j["padding"] = getPadding();
        j["partType"] = (int)getPartitionType();
        j["debug"] = isShowDebug();
    }

    //! Deserialize
    void NavMesh::from_json(const json &j)
    {
        setTileSize(j.value("tileSize", DEFAULT_TILE_SIZE));
        setCellSize(j.value("cellSize", DEFAULT_CELL_SIZE));
        setCellHeight(j.value("cellHeight", DEFAULT_CELL_HEIGHT));
        setAgentHeight(j.value("agentHeight", DEFAULT_AGENT_HEIGHT));
        setAgentRadius(j.value("agentRadius", DEFAULT_AGENT_RADIUS));
        setAgentMaxClimb(j.value("agentMaxClimb", DEFAULT_AGENT_MAX_CLIMB));
        setAgentMaxSlope(j.value("agentMaxSlope", DEFAULT_AGENT_MAX_SLOPE));
        setRegionMinSize(j.value("regionMinSize", DEFAULT_REGION_MIN_SIZE));
        setRegionMergeSize(j.value("regionMergeSize", DEFAULT_REGION_MERGE_SIZE));
        setEdgeMaxLength(j.value("edgeMaxLength", DEFAULT_EDGE_MAX_LENGTH));
        setEdgeMaxError(j.value("edgeMaxError", DEFAULT_EDGE_MAX_ERROR));
        setDetailSampleDistance(j.value("sampleDist", DEFAULT_DETAIL_SAMPLE_DISTANCE));
        setDetailSampleMaxError(j.value("sampleError", DEFAULT_DETAIL_SAMPLE_MAX_ERROR));
        setPadding(j.value("padding", Vec3(0.f, 0.f, 0.f)));
        setPartitionType((EPartitionType)j.value("partType", (int)EPartitionType::WATERSHED));
        setShowDebug(j.value("debug", false));
        Component::from_json(j);
    }

    void NavMesh::onSerializeFinished(Scene* scene)
    {
        Component::onSerializeFinished(scene);

        // Ensure transform updated
        getOwner()->getTransform()->onUpdate(0.f);

        // Build after load finish
        build();
    }

    //! Update property by key value
    void NavMesh::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("tileSize") == 0)
        {
            setTileSize(val);
        }
        else if (key.compare("cellSize") == 0)
        {
            setCellSize(val);
        }
        else if (key.compare("cellHeight") == 0)
        {
            setCellHeight(val);
        }
        else if (key.compare("agentHeight") == 0)
        {
            setAgentHeight(val);
        }
        else if (key.compare("agentRadius") == 0)
        {
            setAgentRadius(val);
        }
        else if (key.compare("agentMaxClimb") == 0)
        {
            setAgentMaxClimb(val);
        }
        else if (key.compare("agentMaxSlope") == 0)
        {
            setAgentMaxSlope(val);
        }
        else if (key.compare("regionMinSize") == 0)
        {
            setRegionMinSize(val);
        }
        else if (key.compare("regionMergeSize") == 0)
        {
            setRegionMergeSize(val);
        }
        else if (key.compare("edgeMaxLength") == 0)
        {
            setEdgeMaxLength(val);
        }
        else if (key.compare("edgeMaxError") == 0)
        {
            setEdgeMaxError(val);
        }
        else if (key.compare("sampleDist") == 0)
        {
            setDetailSampleDistance(val);
        }
        else if (key.compare("sampleError") == 0)
        {
            setDetailSampleMaxError(val);
        }
        else if (key.compare("padding") == 0)
        {
            setPadding(val);
        }
        else if (key.compare("partType") == 0)
        {
            setPartitionType(val);
        }
        else if (key.compare("debug") == 0)
        {
            setShowDebug(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }


} // namespace ige::scene
