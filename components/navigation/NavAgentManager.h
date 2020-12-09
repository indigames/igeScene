#pragma once

#include <memory>
#include <vector>
#include <string>
#include <optional>

#include "event/Event.h"

#include "components/Component.h"
#include "components/navigation/NavAgent.h"
#include "components/navigation/NavMesh.h"

class dtCrowd;
struct dtCrowdAgent;

namespace ige::scene
{
    /**
     * Class NavAgentManager: manage navigation system
     */
    class NavAgentManager : public Component
    {
    public:
        //! Obstacle avoidance params
        struct ObstacleParams
        {
            float velBias;
            float weightDesVel;
            float weightCurVel;
            float weightSide;
            float weightToi;
            float horizTime;
            uint8_t gridSize;
            uint8_t adaptiveDivs;
            uint8_t adaptiveRings;
            uint8_t adaptiveDepth;
        };

    public:
        NavAgentManager(SceneObject &owner);
        virtual ~NavAgentManager();

        //! Get name
        std::string getName() const override { return "NavAgentManager"; }

        //! Update
        void onUpdate(float dt);

        //! NavMesh for which the crowd was created.
        NavMesh *getNavMesh() const { return m_navMesh; }
        void setNavMesh(NavMesh *navMesh) { m_navMesh = navMesh; }

        //! The maximum number of agents the crowd can manage.
        uint32_t getMaxAgentNumber() const { return m_maxAgents; }
        void setMaxAgentNumber(uint32_t max) { m_maxAgents = max; }

        //! The maximum radius of any agent that will be added to the crowd.
        float getMaxAgentRadius() const { return m_maxAgentRadius; }
        void setMaxAgentRadius(float radius) { m_maxAgentRadius = radius; }

        //! Number of query filter types configured in the crowd. Limit to 16.
        uint32_t getNumQueryFilterTypes() const { return m_numQueryFilterTypes; }
        void setNumQueryFilterTypes(uint32_t num) { m_numQueryFilterTypes = num; }

        //! Number of configured area in each filter type. Limit to 64 each.
        const std::vector<uint32_t> &getNumAreas() const { return m_numAreas; }

        //! Number of configured area in the specified query filter type.
        uint32_t getNumAreas(uint32_t queryFilterType) const;

        //! Get the include flags for the specified query filter type.
        uint16_t getIncludeFlags(uint32_t queryFilterType) const;

        //! Set the include flags for the specified query filter type.
        void setIncludeFlags(uint32_t queryFilterType, uint16_t flag);

        //! Get the exclude flags for the specified query filter type.
        uint16_t getExcludeFlags(uint32_t queryFilterType) const;

        //! Set the exclude flags for the specified query filter type.
        void setExcludeFlags(uint32_t queryFilterType, uint16_t flag);

        //! Get the cost of an area for the specified query filter type.
        float getAreaCost(uint32_t queryFilterType, uint32_t areaID) const;

        //! Set the cost of an area for the specified query filter type.
        void setAreaCost(uint32_t queryFilterType, uint32_t areaID, float cost);

        //! Number of obstacle avoidance types configured in the crowd. Limit to 8.
        uint32_t getNumObstacleAvoidanceType() const { return m_numObstacleAvoidanceTypes; }
        void setNumObstacleAvoidanceType(uint32_t num) { m_numObstacleAvoidanceTypes = num; }

        //! The params for the specified obstacle avoidance type.
        ObstacleParams getObstacleAvoidanceParams(uint32_t obstacleAvoidanceType) const;
        void setObstacleAvoidanceParams(uint32_t obstacleAvoidanceType, const ObstacleParams& params);

        //! Agents
        const std::vector<NavAgent *> &getAgents() const { return m_agents; }

        //! Find the nearest point on the navigation mesh to a given point using the crowd initialized query extent and the specified query filter type.
        Vec3 findNearestPoint(const Vec3 &point, int queryFilterType, dtPolyRef *nearestRef = nullptr);

        //! Try to move along the surface from one point to another using the crowd initialized query extentand the specified query filter type.
        Vec3 moveAlongSurface(const Vec3 &start, const Vec3 &end, int queryFilterType, int maxVisited = 3);

        //! Find a path between world space points using the crowd initialized query extent and the specified query filter type. Return non-empty list of points if successful.
        void findPath(std::vector<Vec3> &dest, const Vec3 &start, const Vec3 &end, int queryFilterType);

        //! Return a random point on the navigation mesh using the crowd initialized query extent and the specified query filter type.
        Vec3 getRandomPoint(int queryFilterType, dtPolyRef *randomRef = nullptr);

        //! Return a random point on the navigation mesh within a circle using the crowd initialized query extent and the specified query filter type. The circle radius is only a guideline and in practice the returned point may be further away.
        Vec3 getRandomPointInCircle(const Vec3 &center, float radius, int queryFilterType, dtPolyRef *randomRef = nullptr);

        //! Return distance to wall from a point using the crowd initialized query extent and the specified query filter type. Maximum search radius must be specified.
        float getDistanceToWall(const Vec3 &point, float radius, int queryFilterType, Vec3 *hitPos = nullptr, Vec3 *hitNormal = nullptr);

        //! Perform a walkability raycast on the navigation mesh between start and end using the crowd initialized query extent and the specified query filter type. Return the point where a wall was hit, or the end point if no walls.
        Vec3 raycast(const Vec3 &start, const Vec3 &end, int queryFilterType, Vec3 *hitNormal = nullptr);

        //! Get the detour crowd agent.
        const dtCrowdAgent* getDetourCrowdAgent(int agent) const;

        //! Get the detour query filter.
        const dtQueryFilter* getDetourQueryFilter(uint32_t queryFilterType) const;

        //! Initialize crowd, can call multiple time to purge and reinitialize
        bool initCrowd();

        //! Get the internal detour crowd component.
        dtCrowd *getCrowd() const { return m_crowd; }

        //! Deactivate all agents
        void deactivateAllAgents();

        //! Reactivate all agents
        void reactivateAllAgents();

    protected:
        //! NavAgent created/destroyed events
        void onCreated(NavAgent *agent);
        void onDestroyed(NavAgent *agent);

        //! NavAgent activate/deactivate events
        void onActivated(NavAgent *object);
        void onDeactivated(NavAgent *object);

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

    protected:
        //! Agents
        std::vector<NavAgent *> m_agents;

        //! NavMesh for which the crowd was created.
        NavMesh *m_navMesh = nullptr;

        //! The maximum number of agents the crowd can manage.
        uint32_t m_maxAgents = 512;

        //! The maximum radius of any agent that will be added to the crowd.
        float m_maxAgentRadius = 1.0f;

        //! Number of configured area in each filter type. Limit to 64.
        std::vector<uint32_t> m_numAreas;

        //! Internal Detour crowd object.
        dtCrowd *m_crowd = nullptr;

        //! Number of query filter types configured in the crowd. Limit to 16.
        uint32_t m_numQueryFilterTypes = 0;

        //! Number of obstacle avoidance types configured in the crowd. Limit to 8.
        uint32_t m_numObstacleAvoidanceTypes = 0;

        //! Cache initialized status
        bool m_bInitialized = false;
    };
} // namespace ige::scene
