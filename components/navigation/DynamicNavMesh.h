#pragma once

#include "components/navigation/NavMesh.h"
#include "core/MemBuffer.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourTileCache.h>
#include <Recast.h>

// Fix for MSVC Debug configuration
#if defined(free)
    #undef free
#endif
#include <DetourTileCacheBuilder.h>

namespace ige::scene
{
    class NavObstacle;
    class OffMeshLink;

    //! Tile cache data
    struct TileCacheData
    {
        uint8_t *data;
        int dataSize;
    };

    //! DynamicNavBuildData
    struct DynamicNavBuildData : public NavBuildData
    {
        /// Constructor.
        DynamicNavBuildData(dtTileCacheAlloc* allocator);

        /// Destructor.
        ~DynamicNavBuildData() override;

        /// TileCache specific recast contour set.
        dtTileCacheContourSet* contourSet;

        /// TileCache specific recast poly mesh.
        dtTileCachePolyMesh* polyMesh;

        /// Recast heightfield layer set.
        rcHeightfieldLayerSet* heightFieldLayers;

        /// Allocator from DynamicNavigationMesh instance.
        dtTileCacheAlloc* alloc;
    };

    // Class DynamicNavMesh: Navigation mesh with dynamic obstacles
    class DynamicNavMesh : public NavMesh
    {
    public:
        //! Constructor
        DynamicNavMesh(SceneObject &owner);

        //! Destructor
        virtual ~DynamicNavMesh();

        //! Get name
        std::string getName() const override { return "DynamicNavMesh"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::DynamicNavMesh; }

        //! Max obstacles
        uint32_t getMaxObstacles() const { return m_maxObstacles; }
        void setMaxObstacles(uint32_t max) { m_maxObstacles = max; }

        //! Max layers
        uint32_t getMaxLayers() const { return m_maxLayers; }
        void setMaxLayers(uint32_t max) { m_maxLayers = max; }

        //! Build/rebuild the entire navigation mesh
        bool build() override;

        //! Remove tile from navigation mesh.
        void removeTile(const Vec2 &tile) override;

        /// Remove all tiles from navigation mesh.
        void removeAllTiles() override;

        //! Set navigation data attribute.
        void setNavDataAttr(MemBuffer &value) override;

        //! Return navigation data attribute.
        MemBuffer getNavDataAttr() const override;

        /// Return whether the obstacle is touching the given tile.
        bool isObstacleInTile(NavObstacle *obstacle, const Vec2 &tile) const;

        //! Build one tile of the navigation mesh. Return true if successful.
        int buildTile(std::vector<NavGeoInfo>& geometryList, int x, int z, TileCacheData* tiles);

        //! Build tiles in the rectangular area. Return number of built tiles.
        uint32_t buildTiles(std::vector<NavGeoInfo>& geometryList, const Vec2& from, const Vec2& to) override;

        //! Off-mesh links to be rebuilt in the mesh processor.
        std::vector<OffMeshLink*> collectOffMeshLinks(const AABBox& bounds);

        //! Release the navigation mesh, query, and tile cache.
        void releaseNavMesh() override;

    protected:
        //! Write tile data.
        void writeTiles(MemBuffer& dest, int x, int z) const;

        //! Read tile data to the navigation mesh.
        bool readTiles(MemBuffer& source);

        //! Release tile cache
        void releaseTileCache();

        //! Create/Destroy event
        void onCreated(NavObstacle* obstacle);
        void onDestroyed(NavObstacle* obstacle);

        //! Activate/Deactivate event
        void onActivated(NavObstacle* obstacle);
        void onDeactivated(NavObstacle* obstacle);

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Serialize finished event
        virtual void onSerializeFinished(Scene* scene) override;

        //! Update
        virtual void onUpdate(float dt) override;

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Detour tile cache instance that works with the nav mesh.
        dtTileCache *m_tileCache = nullptr;

        //! Used by dtTileCache to allocate blocks of memory.
        std::unique_ptr<dtTileCacheAlloc> m_allocator = nullptr;

        //! Used by dtTileCache to compress the original tiles to use when reconstructing for changes.
        std::unique_ptr<dtTileCacheCompressor> m_compressor = nullptr;

        //! Mesh processor used by Detour, in this case a 'pass-through' processor.
        std::unique_ptr<dtTileCacheMeshProcess> m_meshProcessor = nullptr;

        //! Maximum number of obstacle objects allowed.
        uint32_t m_maxObstacles = 1024;

        //! Maximum number of layers that are allowed to be constructed.
        uint32_t m_maxLayers = 16;

        //! Queue of tiles to be built.
        std::vector<Vec2> m_tileQueue;
    };
} // namespace ige::scene
