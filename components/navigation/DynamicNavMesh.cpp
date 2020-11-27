#include "components/navigation/DynamicNavMesh.h"
#include "components/navigation/NavObstacle.h"
#include "components/navigation/OffMeshLink.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"
#include "external/lz4/lz4.h"

#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourTileCache.h>
#include <DetourTileCacheBuilder.h>
#include <Recast.h>

#define TILECACHE_MAXLAYERS 255

namespace ige::scene
{
    DynamicNavBuildData::DynamicNavBuildData(dtTileCacheAlloc *allocator)
        : NavBuildData(), contourSet(nullptr), polyMesh(nullptr), heightFieldLayers(nullptr), alloc(allocator)
    {
    }

    DynamicNavBuildData::~DynamicNavBuildData()
    {
        if (alloc)
        {
            if (contourSet)
                dtFreeTileCacheContourSet(alloc, contourSet);

            if (polyMesh)
                dtFreeTileCachePolyMesh(alloc, polyMesh);

            if (heightFieldLayers)
                rcFreeHeightfieldLayerSet(heightFieldLayers);
        }
        contourSet = nullptr;
        polyMesh = nullptr;
        heightFieldLayers = nullptr;
        alloc = nullptr;
    }

    //! TileCompressor: use LZ4 for performance
    struct TileCompressor : public dtTileCacheCompressor
    {
        int maxCompressedSize(const int bufferSize) override
        {
            return (int)(bufferSize * 1.05f);
        }

        dtStatus compress(const unsigned char *buffer, const int bufferSize,
                          unsigned char *compressed, const int /*maxCompressedSize*/, int *compressedSize) override
        {
            *compressedSize = LZ4_compress_default((const char *)buffer, (char *)compressed, bufferSize, LZ4_compressBound(bufferSize));
            return DT_SUCCESS;
        }

        dtStatus decompress(const unsigned char *compressed, const int compressedSize,
                            unsigned char *buffer, const int maxBufferSize, int *bufferSize) override
        {
            *bufferSize = LZ4_decompress_safe((const char *)compressed, (char *)buffer, compressedSize, maxBufferSize);
            return *bufferSize < 0 ? DT_FAILURE : DT_SUCCESS;
        }
    };

    //! MeshProcess: process mesh data
    struct MeshProcess : public dtTileCacheMeshProcess
    {
        MeshProcess(DynamicNavMesh *owner)
            : owner(owner)
        {
        }

        void process(struct dtNavMeshCreateParams *params, unsigned char *polyAreas, unsigned short *polyFlags) override
        {
            // Update poly flags from areas.
            for (int i = 0; i < params->polyCount; ++i)
                if (polyAreas[i] != RC_NULL_AREA)
                    polyFlags[i] = RC_WALKABLE_AREA;

            AABBox bounds;
            rcVcopy(bounds.MinEdge.P(), params->bmin);
            rcVcopy(bounds.MaxEdge.P(), params->bmin);

            // collect off-mesh connections
            std::vector<OffMeshLink *> offMeshlinks = owner->collectOffMeshLinks(bounds);
            if (offMeshlinks.size() > 0)
            {
                if (offMeshlinks.size() != offMeshRadius.size())
                {
                    clearLinkData();

                    auto inverse = owner->getOwner()->getTransform()->getWorldMatrix().Inverse();
                    for (unsigned i = 0; i < offMeshlinks.size(); ++i)
                    {
                        auto *link = offMeshlinks[i];
                        auto start = inverse * link->getOwner()->getTransform()->getWorldPosition();
                        auto end = inverse * link->getEndPoint()->getTransform()->getWorldPosition();

                        offMeshVertices.push_back(start);
                        offMeshVertices.push_back(end);
                        offMeshRadius.push_back(link->getRadius());
                        offMeshFlags.push_back((uint16_t)link->getMask());
                        offMeshAreas.push_back((uint8_t)link->getAreaId());
                        offMeshDir.push_back((uint8_t)(link->isBidirectional() ? DT_OFFMESH_CON_BIDIR : 0));
                    }
                }
                params->offMeshConCount = offMeshRadius.size();
                params->offMeshConVerts = offMeshVertices[0].P();
                params->offMeshConRad = &offMeshRadius[0];
                params->offMeshConFlags = &offMeshFlags[0];
                params->offMeshConAreas = &offMeshAreas[0];
                params->offMeshConDir = &offMeshDir[0];
            }
        }

        //! Clear link data
        void clearLinkData()
        {
            offMeshVertices.clear();
            offMeshRadius.clear();
            offMeshFlags.clear();
            offMeshAreas.clear();
            offMeshDir.clear();
        }

        //! Members
        DynamicNavMesh *owner;
        std::vector<Vec3> offMeshVertices;
        std::vector<float> offMeshRadius;
        std::vector<uint16_t> offMeshFlags;
        std::vector<uint8_t> offMeshAreas;
        std::vector<uint8_t> offMeshDir;
    };

    // From the Detour/Recast Sample_TempObstacles.cpp
    struct LinearAllocator : public dtTileCacheAlloc
    {
        unsigned char *buffer;
        int capacity;
        int top;
        int high;

        LinearAllocator(const int cap)
            : buffer(nullptr), capacity(0), top(0), high(0)
        {
            resize(cap);
        }

        ~LinearAllocator() override
        {
            dtFree(buffer);
        }

        void resize(const int cap)
        {
            if (buffer)
                dtFree(buffer);
            buffer = (unsigned char *)dtAlloc(cap, DT_ALLOC_PERM);
            capacity = cap;
        }

        void reset() override
        {
            high = Max(high, top);
            top = 0;
        }

        void *alloc(const size_t size) override
        {
            if (!buffer)
                return nullptr;
            if (top + size > capacity)
                return nullptr;
            unsigned char *mem = &buffer[top];
            top += size;
            return mem;
        }

        void free(void *) override
        {
            // Do nothing
        }
    };

    //! Constructor
    DynamicNavMesh::DynamicNavMesh(SceneObject &owner)
        : NavMesh(owner)
    {
        m_allocator = std::make_unique<LinearAllocator>(32000); //32kb to start
        m_compressor = std::make_unique<TileCompressor>();
        m_meshProcessor = std::make_unique<MeshProcess>(this);
    }

    DynamicNavMesh::~DynamicNavMesh()
    {
        releaseNavMesh();

        m_allocator = nullptr;
        m_compressor = nullptr;
        m_meshProcessor = nullptr;
    }

    //! Build/rebuild the entire navigation mesh
    bool DynamicNavMesh::build()
    {
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
            uint32_t maxTiles = next_power_of_two((uint32_t)(m_numTilesX * m_numTilesZ)) * m_maxLayers;
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

            dtTileCacheParams tileCacheParams;
            memset(&tileCacheParams, 0, sizeof(tileCacheParams));
            rcVcopy(tileCacheParams.orig, m_boundingBox.MinEdge.P());
            tileCacheParams.ch = m_cellHeight;
            tileCacheParams.cs = m_cellSize;
            tileCacheParams.width = m_tileSize;
            tileCacheParams.height = m_tileSize;
            tileCacheParams.maxSimplificationError = m_edgeMaxError;
            tileCacheParams.maxTiles = (m_numTilesX * m_numTilesZ) * m_maxLayers;
            tileCacheParams.maxObstacles = m_maxObstacles;

            // Settings from NavMesh
            tileCacheParams.walkableClimb = m_agentMaxClimb;
            tileCacheParams.walkableHeight = m_agentHeight;
            tileCacheParams.walkableRadius = m_agentRadius;

            m_tileCache = dtAllocTileCache();
            if (!m_tileCache)
            {
                releaseNavMesh();
                return false;
            }

            if (dtStatusFailed(m_tileCache->init(&tileCacheParams, m_allocator.get(), m_compressor.get(), m_meshProcessor.get())))
            {
                releaseNavMesh();
                return false;
            }

            // Build each tile
            unsigned numTiles = 0;
            for (int z = 0; z < m_numTilesZ; ++z)
            {
                for (int x = 0; x < m_numTilesX; ++x)
                {
                    TileCacheData tiles[TILECACHE_MAXLAYERS];
                    int layerCt = buildTile(geometryList, x, z, tiles);
                    for (int i = 0; i < layerCt; ++i)
                    {
                        dtCompressedTileRef tileRef;
                        int status = m_tileCache->addTile(tiles[i].data, tiles[i].dataSize, DT_COMPRESSEDTILE_FREE_DATA, &tileRef);
                        if (dtStatusFailed((dtStatus)status))
                        {
                            dtFree(tiles[i].data);
                            tiles[i].data = nullptr;
                        }
                    }
                    m_tileCache->buildNavMeshTilesAt(x, z, m_navMesh);
                    ++numTiles;
                }
            }

            // For a full build it's necessary to update the nav mesh
            // not doing so will cause dependent components to crash, like CrowdManager
            m_tileCache->update(0, m_navMesh);

            // Scan for obstacles to insert into us
            //PODVector<Node*> obstacles;
            //GetScene()->GetChildrenWithComponent<Obstacle>(obstacles, true);
            //for (unsigned i = 0; i < obstacles.Size(); ++i)
            //{
            //    auto* obs = obstacles[i]->GetComponent<Obstacle>();
            //    if (obs && obs->IsEnabledEffective())
            //        AddObstacle(obs);
            //}

            return true;
        }
    }

    //! Remove tile from navigation mesh.
    void DynamicNavMesh::removeTile(const Vec2 &tile)
    {
        if (!m_navMesh)
            return;

        dtCompressedTileRef existing[TILECACHE_MAXLAYERS];
        const int existingCt = m_tileCache->getTilesAt(tile.X(), tile.Y(), existing, m_maxLayers);
        for (int i = 0; i < existingCt; ++i)
        {
            unsigned char *data = nullptr;
            if (!dtStatusFailed(m_tileCache->removeTile(existing[i], &data, nullptr)) && data != nullptr)
                dtFree(data);
        }

        NavMesh::removeTile(tile);
    }

    /// Remove all tiles from navigation mesh.
    void DynamicNavMesh::removeAllTiles()
    {
        auto numTiles = m_tileCache->getTileCount();
        for (int i = 0; i < numTiles; ++i)
        {
            const dtCompressedTile *tile = m_tileCache->getTile(i);
            if (tile->header)
                m_tileCache->removeTile(m_tileCache->getTileRef(tile), nullptr, nullptr);
        }

        NavMesh::removeAllTiles();
    }

    //! Set navigation data attribute.
    void DynamicNavMesh::setNavDataAttr(MemBuffer &buffer)
    {
        releaseNavMesh();

        if (buffer.empty())
            return;

        buffer.read<AABBox>(m_boundingBox);
        buffer.read<int>(m_numTilesX);
        buffer.read<int>(m_numTilesZ);

        dtNavMeshParams params;
        buffer.read<dtNavMeshParams>(params);

        m_navMesh = dtAllocNavMesh();
        if (!m_navMesh)
        {
            return;
        }

        if (dtStatusFailed(m_navMesh->init(&params)))
        {
            releaseNavMesh();
            return;
        }

        dtTileCacheParams tcParams;
        buffer.read<dtTileCacheParams>(tcParams);

        m_tileCache = dtAllocTileCache();
        if (!m_tileCache)
        {
            releaseNavMesh();
            return;
        }

        if (dtStatusFailed(m_tileCache->init(&tcParams, m_allocator.get(), m_compressor.get(), m_meshProcessor.get())))
        {
            releaseNavMesh();
            return;
        }

        readTiles(buffer);
    }

    //! Return navigation data attribute.
    MemBuffer DynamicNavMesh::getNavDataAttr() const
    {
        MemBuffer ret;
        if (m_navMesh && m_tileCache)
        {
            ret.write<AABBox>(m_boundingBox);
            ret.write<int>(m_numTilesX);
            ret.write<int>(m_numTilesZ);

            const auto *params = m_navMesh->getParams();
            ret.write<dtNavMeshParams>(*params);

            const auto *tcParams = m_tileCache->getParams();
            ret.write<dtTileCacheParams>(*tcParams);

            for (int z = 0; z < m_numTilesZ; ++z)
                for (int x = 0; x < m_numTilesX; ++x)
                    writeTiles(ret, x, z);
        }
        return ret;
    }

    /// Return whether the obstacle is touching the given tile.
    bool DynamicNavMesh::isObstacleInTile(NavObstacle *obstacle, const Vec2 &tile) const
    {
        const auto tileBoundingBox = getTileBoundingBox(tile);
        const auto obstaclePosition = obstacle->getOwner()->getTransform()->getWorldPosition();
        return tileBoundingBox.isPointInside(obstaclePosition);
    }

    //! Build one tile of the navigation mesh. Return true if successful.
    int DynamicNavMesh::buildTile(std::vector<NavGeoInfo> &geometryList, int x, int z, TileCacheData *tiles)
    {
        m_tileCache->removeTile(m_navMesh->getTileRefAt(x, z, 0), nullptr, nullptr);
        const auto tileBoundingBox = getTileBoundingBox(Vec2(x, z));
        auto build = DynamicNavBuildData(m_allocator.get());

        rcConfig cfg;
        memset(&cfg, 0, sizeof cfg);
        cfg.cs = m_cellSize;
        cfg.ch = m_cellHeight;
        cfg.walkableSlopeAngle = m_agentMaxSlope;
        cfg.walkableHeight = (int)ceilf(m_agentHeight / cfg.ch);
        cfg.walkableClimb = (int)floorf(m_agentMaxClimb / cfg.ch);
        cfg.walkableRadius = (int)ceilf(m_agentRadius / cfg.cs);
        cfg.maxEdgeLen = (int)(m_edgeMaxLength / m_cellSize);
        cfg.maxSimplificationError = m_edgeMaxError;
        cfg.minRegionArea = (int)sqrtf(m_regionMinSize);
        cfg.mergeRegionArea = (int)sqrtf(m_regionMergeSize);
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
            return 0; // Nothing to do

        build.heightField = rcAllocHeightfield();
        if (!build.heightField)
        {
            return 0;
        }

        if (!rcCreateHeightfield(build.ctx, *build.heightField, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch))
        {
            return 0;
        }

        auto numTriangles = build.indices.size() / 3;
        auto triAreas = new uint8_t[numTriangles];
        memset(triAreas, 0, numTriangles);

        rcMarkWalkableTriangles(build.ctx, cfg.walkableSlopeAngle, &(build.vertices[0][0]), build.vertices.size(), &(build.indices[0]), numTriangles, triAreas);
        rcRasterizeTriangles(build.ctx, &(build.vertices[0][0]), build.vertices.size(), &(build.indices[0]), triAreas, numTriangles, *build.heightField, cfg.walkableClimb);
        rcFilterLowHangingWalkableObstacles(build.ctx, cfg.walkableClimb, *build.heightField);

        rcFilterWalkableLowHeightSpans(build.ctx, cfg.walkableHeight, *build.heightField);
        rcFilterLedgeSpans(build.ctx, cfg.walkableHeight, cfg.walkableClimb, *build.heightField);

        build.compactHeightField = rcAllocCompactHeightfield();
        if (!build.compactHeightField)
            return 0;

        if (!rcBuildCompactHeightfield(build.ctx, cfg.walkableHeight, cfg.walkableClimb, *build.heightField, *build.compactHeightField))
            return 0;

        if (!rcErodeWalkableArea(build.ctx, cfg.walkableRadius, *build.compactHeightField))
            return 0;

        // Mark area volumes
        for (size_t i = 0; i < build.navAreas.size(); ++i)
            rcMarkBoxArea(build.ctx, build.navAreas[i].bounds.MinEdge.P(), build.navAreas[i].bounds.MaxEdge.P(), build.navAreas[i].areaID, *build.compactHeightField);

        if (m_partitionType == NavMesh::EPartitionType::WATERSHED)
        {
            if (!rcBuildDistanceField(build.ctx, *build.compactHeightField))
                return 0;

            if (!rcBuildRegions(build.ctx, *build.compactHeightField, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea))
                return 0;
        }
        else
        {
            if (!rcBuildRegionsMonotone(build.ctx, *build.compactHeightField, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea))
                return 0;
        }

        build.heightFieldLayers = rcAllocHeightfieldLayerSet();
        if (!build.heightFieldLayers)
        {
            return 0;
        }

        if (!rcBuildHeightfieldLayers(build.ctx, *build.compactHeightField, cfg.borderSize, cfg.walkableHeight, *build.heightFieldLayers))
        {
            return 0;
        }

        int retCt = 0;
        for (int i = 0; i < build.heightFieldLayers->nlayers; ++i)
        {
            dtTileCacheLayerHeader header;
            header.magic = DT_TILECACHE_MAGIC;
            header.version = DT_TILECACHE_VERSION;
            header.tx = x;
            header.ty = z;
            header.tlayer = i;

            rcHeightfieldLayer *layer = &build.heightFieldLayers->layers[i];

            // Tile info.
            rcVcopy(header.bmin, layer->bmin);
            rcVcopy(header.bmax, layer->bmax);
            header.width = (unsigned char)layer->width;
            header.height = (unsigned char)layer->height;
            header.minx = (unsigned char)layer->minx;
            header.maxx = (unsigned char)layer->maxx;
            header.miny = (unsigned char)layer->miny;
            header.maxy = (unsigned char)layer->maxy;
            header.hmin = (unsigned short)layer->hmin;
            header.hmax = (unsigned short)layer->hmax;

            if (dtStatusFailed(
                    dtBuildTileCacheLayer(m_compressor.get(), &header, layer->heights, layer->areas, layer->cons, &(tiles[retCt].data), &tiles[retCt].dataSize)))
            {
                return 0;
            }
            else
                ++retCt;
        }

        return retCt;
    }

    //! Build tiles in the rectangular area. Return number of built tiles.
    uint32_t DynamicNavMesh::buildTiles(std::vector<NavGeoInfo> &geometryList, const Vec2 &from, const Vec2 &to)
    {
        uint32_t numTiles = 0;
        for (int z = from.Y(); z <= to.Y(); ++z)
        {
            for (int x = from.X(); x <= to.X(); ++x)
            {
                dtCompressedTileRef existing[TILECACHE_MAXLAYERS];
                const int existingCt = m_tileCache->getTilesAt(x, z, existing, m_maxLayers);
                for (int i = 0; i < existingCt; ++i)
                {
                    unsigned char *data = nullptr;
                    if (!dtStatusFailed(m_tileCache->removeTile(existing[i], &data, nullptr)) && data != nullptr)
                        dtFree(data);
                }

                TileCacheData tiles[TILECACHE_MAXLAYERS];
                int layerCt = buildTile(geometryList, x, z, tiles);
                for (int i = 0; i < layerCt; ++i)
                {
                    dtCompressedTileRef tileRef;
                    int status = m_tileCache->addTile(tiles[i].data, tiles[i].dataSize, DT_COMPRESSEDTILE_FREE_DATA, &tileRef);
                    if (dtStatusFailed((dtStatus)status))
                    {
                        dtFree(tiles[i].data);
                        tiles[i].data = nullptr;
                    }
                    else
                    {
                        m_tileCache->buildNavMeshTile(tileRef, m_navMesh);
                        ++numTiles;
                    }
                }
            }
        }
        return numTiles;
    }

    //! Off-mesh links to be rebuilt in the mesh processor.
    std::vector<OffMeshLink *> DynamicNavMesh::collectOffMeshLinks(const AABBox &bounds)
    {
        std::vector<Component *> offMeshLinkComps;
        std::vector<OffMeshLink *> offMeshLinks;
        getOwner()->getComponentsRecursive(offMeshLinkComps, "OffMeshLink");
        for (auto comp : offMeshLinkComps)
        {
            auto link = static_cast<OffMeshLink *>(comp);
            if (link && link->isEnabled() && link->getEndPoint())
            {
                offMeshLinks.push_back(link);
            }
        }
        return offMeshLinks;
    }

    //! Release the navigation mesh, query, and tile cache.
    void DynamicNavMesh::releaseNavMesh()
    {
        NavMesh::releaseNavMesh();
        releaseTileCache();
    }

    //! Release tile cache
    void DynamicNavMesh::releaseTileCache()
    {
        if (m_tileCache)
        {
            dtFreeTileCache(m_tileCache);
            m_tileCache = nullptr;
        }
    }

    //! Write tile data.
    void DynamicNavMesh::writeTiles(MemBuffer &dest, int x, int z) const
    {
        dtCompressedTileRef tiles[TILECACHE_MAXLAYERS];
        const int ct = m_tileCache->getTilesAt(x, z, tiles, m_maxLayers);
        for (int i = 0; i < ct; ++i)
        {
            const dtCompressedTile *tile = m_tileCache->getTileByRef(tiles[i]);
            if (!tile || !tile->header || !tile->dataSize)
                continue; // Don't write "void-space" tiles
            dest.write<dtTileCacheLayerHeader>(*tile->header);
            dest.write<int>(tile->dataSize);
            dest.write(tile->data, (size_t)tile->dataSize);
        }
    }

    //! Read tile data to the navigation mesh.
    bool DynamicNavMesh::readTiles(MemBuffer &source)
    {
        m_tileQueue.clear();
        while (!source.isEof())
        {
            dtTileCacheLayerHeader header;
            source.read<dtTileCacheLayerHeader>(header);
            int dataSize;
            source.read<int>(dataSize);

            auto *data = (uint8_t *)dtAlloc(dataSize, DT_ALLOC_PERM);
            if (!data)
            {
                return false;
            }

            source.read(data, (size_t)dataSize);
            if (dtStatusFailed(m_tileCache->addTile(data, dataSize, DT_TILE_FREE_DATA, nullptr)))
            {
                dtFree(data);
                return false;
            }

            const Vec2 tileIdx = Vec2(header.tx, header.ty);
            if (m_tileQueue.empty() || m_tileQueue.back() != tileIdx)
                m_tileQueue.push_back(tileIdx);
        }

        for (size_t i = 0; i < m_tileQueue.size(); ++i)
            m_tileCache->buildNavMeshTilesAt(m_tileQueue[i].X(), m_tileQueue[i].Y(), m_navMesh);

        m_tileCache->update(0, m_navMesh);
        return true;
    }

    //! Create/Destroy event
    void DynamicNavMesh::onCreated(NavObstacle *obstacle)
    {
    }

    void DynamicNavMesh::onDestroyed(NavObstacle *obstacle)
    {
    }

    //! Activate/Deactivate event
    void DynamicNavMesh::onActivated(NavObstacle *obstacle)
    {
        if (m_tileCache)
        {
            float pos[3];
            auto obsPos = obstacle->getOwner()->getTransform()->getWorldPosition();
            rcVcopy(pos, obsPos.P());
            dtObstacleRef refHolder;
            if (dtStatusFailed(m_tileCache->addObstacle(pos, obstacle->getRadius(), obstacle->getHeight(), &refHolder)))
                return;
            obstacle->setObstacleId(refHolder);
        }
    }

    void DynamicNavMesh::onDeactivated(NavObstacle *obstacle)
    {
        if (m_tileCache && obstacle->getObstacleId() > 0)
        {

            if (dtStatusFailed(m_tileCache->removeObstacle(obstacle->getObstacleId())))
            {
                return;
            }
            obstacle->setObstacleId(0);
        }
    }

    //! Serialize
    void DynamicNavMesh::to_json(json &j) const
    {
        NavMesh::to_json(j);
        j["maxObs"] = getMaxObstacles();
        j["maxLayers"] = getMaxLayers();
    }

    //! Deserialize
    void DynamicNavMesh::from_json(const json &j)
    {
        setTileSize(j.value("maxObs", 1024));
        setMaxLayers(j.value("maxLayers", 16));
        NavMesh::from_json(j);
    }

} // namespace ige::scene
