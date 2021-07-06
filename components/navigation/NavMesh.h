#pragma once
#include "components/Component.h"
#include "core/MemBuffer.h"

#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>
#include <Recast.h>

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#define MAX_POLYS 2048

//! Declare Recast/Detour classes
class dtNavMesh;
class dtNavMeshQuery;
class dtQueryFilter;

namespace ige::scene
{
    class NavArea;

    //! Path finding data
    struct FindPathData
    {
        // Polygons.
        dtPolyRef polys[MAX_POLYS] = {};

        // Polygons on the path.
        dtPolyRef pathPolys[MAX_POLYS] = {};

        // Points on the path.
        Vec3 pathPoints[MAX_POLYS] = {};

        // Flags on the path.
        uint8_t pathFlags[MAX_POLYS] = {};
    };

    //! NavGeoInfo: Navigation Geometry Info
    struct NavGeoInfo
    {
        //! Component
        Component *component;

        //! Geometry LOD level (if applicable)
        uint8_t lodLevel;

        //! Transfrom (relative to root node)
        Mat4 transform;

        //! Bounding box (relative to root node)
        AABBox boundingBox;
    };

    //! NavPathPoint: Navigation path point
    struct NavPathPoint
    {
        enum class Flag
        {
            NONE = 0,
            START = 0x01,
            END = 0x02,
            OFF_MESH = 0x04
        };

        //! Position
        Vec3 position;

        //! Detour flag
        Flag flag;

        //! Detour area ID.
        uint8_t areaID;
    };

    //! NavAreaStub: Navigation area stub.
    struct NavAreaStub
    {
        //! Bounding box.
        AABBox bounds;

        //! Area ID.
        uint8_t areaID;
    };

    //! NavBuildData:: Navigation build data.
    struct NavBuildData
    {
        //! Constructor
        NavBuildData();

        //! Destructor
        virtual ~NavBuildData();

        //! World-space bounding box of the navigation mesh tile.
        AABBox worldBoundingBox;

        //! Vertices from geometries
        std::vector<std::array<float, 3>> vertices;

        //! Triangle indices from geometries
        std::vector<int> indices;

        //! Off-mesh link vertices
        std::vector<Vec3> offMeshVertices;

        //! Offmesh connection radius
        std::vector<float> offMeshRadius;

        //! Offmesh connection flags
        std::vector<uint16_t> offMeshFlags;

        //! Offmesh connection areas
        std::vector<uint8_t> offMeshAreas;

        //! Offmesh connection direction
        std::vector<uint8_t> offMeshDir;

        //! Recast context
        rcContext *ctx;

        //! Recast heightfield
        rcHeightfield *heightField;

        //! Recast compact heightfield
        rcCompactHeightfield *compactHeightField;

        //! Navigation areas (pretransformed)
        std::vector<NavAreaStub> navAreas;
    };

    struct SimpleNavBuildData : public NavBuildData
    {
        //! Constructor
        SimpleNavBuildData();

        //! Destructor
        ~SimpleNavBuildData();

        //! Recast contour set
        rcContourSet *contourSet;

        /// Recast poly mesh.
        rcPolyMesh *polyMesh;

        /// Recast detail poly mesh.
        rcPolyMeshDetail *polyMeshDetail;
    };


    //! PhysicMesh
    class NavMesh : public Component
    {
    public:
        enum class EPartitionType
        {
            WATERSHED = 0,
            MONOTONE
        };

    public:
        //! Constructor
        NavMesh(SceneObject &owner);

        //! Destructor
        virtual ~NavMesh();

        //! Get name
        std::string getName() const override { return "NavMesh"; }

        //! Build the entire navigation mesh
        virtual bool build();

        //! Rebuild part of the navigation mesh contained by the world-space bounding box
        virtual bool build(const AABBox& boundingBox);

        //! Rebuild part of the navigation mesh in the rectangular area
        virtual bool build(const Vec2& from, const Vec2& to);

        //! Return tile data.
        virtual MemBuffer getTileData(const Vec2& tile) const;

        //! Add tile to navigation mesh.
        virtual bool addTile(MemBuffer& tileData);

        //! Remove tile from navigation mesh
        virtual void removeTile(const Vec2 &tile);

        //! Remove all tiles from navigation mesh
        virtual void removeAllTiles();

        //! Return whether the navigation mesh has tile
        bool hasTile(const Vec2 &tile) const;

        //! Return number of tiles.
        Vec2 getNumTiles() const { return Vec2(m_numTilesX, m_numTilesZ); }

        //! Return bounding box of the tile in the node space
        AABBox getTileBoundingBox(const Vec2 &tile) const;

        //! Return index of the tile at the position
        Vec2 getTileIndex(const Vec3 &position) const;

        //! Try to move along the surface from one point to another
        Vec3 moveAlongSurface(const Vec3 &start, const Vec3 &end, const Vec3 &extents = {1.f, 1.f, 1.f}, int maxVisited = 3, const dtQueryFilter *filter = nullptr);

        //! Find the nearest point on the navigation mesh to a given
        Vec3 findNearestPoint(const Vec3 &point, const Vec3 &extents = {1.f, 1.f, 1.f}, const dtQueryFilter *filter = nullptr, dtPolyRef *nearestRef = nullptr);

        //! Find a path between world space points. Return non-empty list of points if successful. Extents specifies how far off the navigation mesh the points can be.
        void findPath(std::vector<Vec3> &dest, const Vec3 &start, const Vec3 &end, const Vec3 &extents = {1.f, 1.f, 1.f}, const dtQueryFilter *filter = nullptr);

        //! Find a path between world space points. Return non-empty list of navigation path points if successful. Extents specifies how far off the navigation mesh the points can be.
        void findPath(std::vector<NavPathPoint> &dest, const Vec3 &start, const Vec3 &end, const Vec3 &extents = {1.f, 1.f, 1.f}, const dtQueryFilter *filter = nullptr);

        //! Return a random point on the navigation mesh.
        Vec3 getRandomPoint(const dtQueryFilter *filter = nullptr, dtPolyRef *randomRef = nullptr);

        //! Return a random point on the navigation mesh within a circle. The circle radius is only a guideline and in practice the returned point may be further away.
        Vec3 getRandomPointInCircle(const Vec3 &center, float radius, const Vec3 &extents = {1.f, 1.f, 1.f}, const dtQueryFilter *filter = nullptr, dtPolyRef *randomRef = nullptr);

        //! Return distance to wall from a point. Maximum search radius must be specified.
        float getDistanceToWall(const Vec3 &point, float radius, const Vec3 &extents = {1.f, 1.f, 1.f}, const dtQueryFilter *filter = nullptr, Vec3 *hitPos = nullptr, Vec3 *hitNormal = nullptr);

        //! Perform a walkability raycast on the navigation mesh between start and end and return the point where a wall was hit, or the end point if no walls.
        Vec3 raycast(const Vec3 &start, const Vec3 &end, const Vec3 &extents = {1.f, 1.f, 1.f}, const dtQueryFilter *filter = nullptr, Vec3 *hitNormal = nullptr);

        //! Set navigation data attribute.
        virtual void setNavDataAttr(MemBuffer& value);

        //! Return navigation data attribute.
        virtual MemBuffer getNavDataAttr() const;

        //! Tile size
        int getTileSize() const { return m_tileSize; }
        void setTileSize(int size) { m_tileSize = size; }

        //! Cell size
        float getCellSize() const { return m_cellSize; }
        void setCellSize(float size) { m_cellSize = size; }

        //! Cell height
        float getCellHeight() const { return m_cellHeight; }
        void setCellHeight(float height) { m_cellHeight = height; }

        //! Agent height
        float getAgentHeight() const { return m_agentHeight; }
        void setAgentHeight(float height) { m_agentHeight = height; }

        //! Agent radius
        float getAgentRadius() const { return m_agentRadius; }
        void setAgentRadius(float radius) { m_agentRadius = radius; }

        //! Agent max vertical climb
        float getAgentMaxClimb() const { return m_agentMaxClimb; }
        void setAgentMaxClimb(float climb) { m_agentMaxClimb = climb; }

        //! Agent max slope
        float getAgentMaxSlope() const { return m_agentMaxSlope; }
        void setAgentMaxSlope(float slope) { m_agentMaxSlope = slope; }

        //! Region minimum size
        float getRegionMinSize() const { return m_regionMinSize; }
        void setRegionMinSize(float size) { m_regionMinSize = size; }

        //! Region merge size
        float getRegionMergeSize() const { return m_regionMergeSize; }
        void setRegionMergeSize(float size) { m_regionMergeSize = size; }

        //! Edge max length
        float getEdgeMaxLength() const { return m_edgeMaxLength; }
        void setEdgeMaxLength(float length) { m_edgeMaxLength = length; }

        //! Edge max error
        float getEdgeMaxError() const { return m_edgeMaxError; }
        void setEdgeMaxError(float error) { m_edgeMaxError = error; }

        //! Detail sampling distance
        float getDetailSampleDistance() const { return m_detailSampleDistance; }
        void setDetailSampleDistance(float distance) { m_detailSampleDistance = distance; }

        //! Detail sampling maximum error
        float getDetailSampleMaxError() const { return m_detailSampleMaxError; };
        void setDetailSampleMaxError(float error) { m_detailSampleMaxError = error; };

        //! Bounding box padding
        const Vec3 &getPadding() const { return m_padding; }
        void setPadding(const Vec3 &padding) { m_padding = padding; }

        //! Partitioning type
        EPartitionType getPartitionType() const { return m_partitionType; }
        void setPartitionType(EPartitionType type) { m_partitionType = type; }

        //! Debug
        bool isShowDebug() const { return m_bShowDebug; }
        void setShowDebug(bool show = true) { m_bShowDebug = show; }

        //! Number of tiles in X direction
        int getNumTilesX() const { return m_numTilesX; }

        //! Number of tiles in Z direction
        int getNumTilesZ() const { return m_numTilesZ; }

        //! Navigation mesh bounding box
        const AABBox &getBoundingBox() const { return m_boundingBox; };

        //! World bounding box
        AABBox getWorldBoundingBox() const;

        //! Initialize navigation mesh query
        bool initializeQuery();

        //! Detour navigation mesh
        dtNavMesh *getNavMesh() const { return m_navMesh; }

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Release the navigation mesh and the mesh query
        virtual void releaseNavMesh();

        //! Collect geometry from under Navigable components
        void collectGeometries(std::vector<NavGeoInfo> &geometryList);

        //! Visit nodes and collect navigable geometry
        void collectGeometries(std::vector<NavGeoInfo> &geometryList, SceneObject *node, std::unordered_set<SceneObject *> &processedNodes, bool recursive);

        //! Get geometry data within a bounding box
        void getTileGeometry(NavBuildData *build, std::vector<NavGeoInfo> &geometryList, AABBox &box);

        //! Add a triangle mesh to the geometry data.
        void addTriMeshGeometry(NavBuildData *build, Figure *figure, const Mat4 &transform);

        //! Build one tile of the navigation mesh
        virtual bool buildTile(std::vector<NavGeoInfo> &geometryList, int x, int z);

        //! Build tiles in the rectangular area. Return number of built tiles.
        virtual uint32_t buildTiles(std::vector<NavGeoInfo> &geometryList, const Vec2 &from, const Vec2 &to);

        //! Write tile data.
        void writeTile(MemBuffer& dest, int x, int z) const;

        //! Read tile data to the navigation mesh.
        bool readTile(MemBuffer& source);

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Serialize finished event
        virtual void onSerializeFinished(Scene* scene) override;

        //! Update
        virtual void onUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

    protected:
        //! Detour navigation mesh
        dtNavMesh *m_navMesh = nullptr;

        //! Detour navigation mesh query
        dtNavMeshQuery *m_navMeshQuery = nullptr;

        //! Temporary data for finding a path
        std::unique_ptr<FindPathData> m_pathData = nullptr;

        //! Cell size
        float m_cellSize;

        //! Cell height
        float m_cellHeight;

        //! Agent height
        float m_agentHeight;

        //! Agent radius
        float m_agentRadius;

        //! Agent max vertical climb
        float m_agentMaxClimb;

        //! Agent max slope
        float m_agentMaxSlope;

        //! Region minimum size
        float m_regionMinSize;

        //! Region merge size
        float m_regionMergeSize;

        //! Edge max length
        float m_edgeMaxLength;

        //! Edge max error
        float m_edgeMaxError;

        //! Detail sampling distance
        float m_detailSampleDistance;

        //! Detail sampling maximum error
        float m_detailSampleMaxError;

        //! Bounding box padding
        Vec3 m_padding = {1.f, 1.f, 1.f};

        //! Tile size
        int m_tileSize;

        //! Number of tiles in X direction
        int m_numTilesX = 0;

        //! Number of tiles in Z direction
        int m_numTilesZ = 0;

        //! Navigation mesh bounding box
        AABBox m_boundingBox;

        //! Partitioning type
        EPartitionType m_partitionType;

        //! NavAreas
        std::vector<NavArea*> m_areas;

        //! Debug
        bool m_bShowDebug = false;
    };
} // namespace ige::scene
