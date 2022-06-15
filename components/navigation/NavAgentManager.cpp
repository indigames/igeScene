#include "components/navigation/NavAgentManager.h"
#include "components/navigation/DynamicNavMesh.h"
#include "scene/SceneObject.h"
#include "components/TransformComponent.h"

#include <DetourCrowd.h>

namespace ige::scene
{
    NavAgentManager::NavAgentManager(SceneObject& owner)
        : RuntimeComponent(owner)
    {
        m_navMesh.reset();
        // Register event listeners
        NavAgent::getCreatedEvent().addListener(std::bind(static_cast<void (NavAgentManager::*)(NavAgent *)>(&NavAgentManager::onCreated), this, std::placeholders::_1));
        NavAgent::getDestroyedEvent().addListener(std::bind(static_cast<void (NavAgentManager::*)(NavAgent *)>(&NavAgentManager::onDestroyed), this, std::placeholders::_1));
        NavAgent::getActivatedEvent().addListener(std::bind(static_cast<void (NavAgentManager::*)(NavAgent *)>(&NavAgentManager::onActivated), this, std::placeholders::_1));
        NavAgent::getDeactivatedEvent().addListener(std::bind(static_cast<void (NavAgentManager::*)(NavAgent *)>(&NavAgentManager::onDeactivated), this, std::placeholders::_1));
    }

    NavAgentManager::~NavAgentManager()
    {
        // Unregister event listeners
        NavAgent::getCreatedEvent().removeAllListeners();
        NavAgent::getDestroyedEvent().removeAllListeners();
        NavAgent::getActivatedEvent().removeAllListeners();
        NavAgent::getDeactivatedEvent().removeAllListeners();

        m_bInitialized = false;

        // Deallocate crowd
        if(m_crowd)
        {
            dtFreeCrowd(m_crowd);
            m_crowd = nullptr;
        }

        // Release nav mesh
        m_navMesh.reset();

        // Clear agents
        m_agents.clear();
    }

    //! NavAgent created/destroyed events
    void NavAgentManager::onCreated(NavAgent *agent)
    {
        //TODO: optimize
        auto agentPtr = std::dynamic_pointer_cast<NavAgent>(agent->getOwner()->getComponent(agent->getInstanceId()));
        m_agents.push_back(agentPtr);
        if (agent->isEnabled()) {
            onActivated(agent);
        }
    }

    void NavAgentManager::onDestroyed(NavAgent *agent)
    {
        auto found = std::find_if(m_agents.begin(), m_agents.end(), [&agent](auto element) {
            return agent && !element.expired() && agent->getInstanceId() == element.lock()->getInstanceId();
        });

        if (found != m_agents.end())
        {
            m_agents.erase(found);
        }
    }

    //! NavAgent activate/deactivate events
    void NavAgentManager::onActivated(NavAgent *agent)
    {
        initCrowd();

        if (!m_crowd || !agent || m_navMesh.expired())
            return;

        dtCrowdAgentParams params;
        params.userData = agent;
        params.queryFilterType = (uint8_t)agent->getQueryFilterType();
        auto position = agent->getOwner()->getTransform()->getPosition();
        auto agentId = m_crowd->addAgent(position.P(), &params);

        if (agent->getRadius() == 0.f)
            agent->setRadius(m_navMesh.lock()->getAgentRadius());
        if (agent->getHeight() == 0.f)
            agent->setHeight(m_navMesh.lock()->getAgentHeight());

        agent->setAgentId(agentId);
        agent->setManager(this);
    }

    void NavAgentManager::onDeactivated(NavAgent *agent)
    {
        if (!m_crowd || !agent)
            return;
        dtCrowdAgent *agt = m_crowd->getEditableAgent(agent->getAgentId());
        if (agt)
            agt->params.userData = nullptr;
        m_crowd->removeAgent(agent->getAgentId());
        agent->setAgentId(-1);
        agent->setManager(nullptr);
    }

    void NavAgentManager::Initialize() {
        RuntimeComponent::Initialize();
        reactivateAllAgents();
    }

    void NavAgentManager::Clear() {
        int size = m_agents.size();
        for(int i = size - 1; i >= 0; i--)
        {
            if (!m_agents[i].expired())
                onDeactivated(m_agents[i].lock().get());
            else
                m_agents.erase(m_agents.begin() + i);
        }
    }

    //! Update
    void NavAgentManager::onRuntimeUpdate(float dt)
    {
        if (!m_bInitialized)
            return;
        m_crowd->update(dt, nullptr);
    }

    void NavAgentManager::onRuntimeFixedUpdate(float dt)
    {
    }

    void NavAgentManager::onRuntimeLateUpdate(float dt)
    {
    }

    //! Get the detour crowd agent.
    const dtCrowdAgent *NavAgentManager::getDetourCrowdAgent(int agent) const
    {
        return m_crowd ? m_crowd->getAgent(agent) : nullptr;
    }

    //! Get the detour query filter.
    const dtQueryFilter *NavAgentManager::getDetourQueryFilter(uint32_t queryFilterType) const
    {
        return m_crowd ? m_crowd->getFilter(queryFilterType) : nullptr;
    }

    //! Remove all agents
    void NavAgentManager::deactivateAllAgents()
    {
        for (auto& agent : m_agents)
        {
            if(!agent.expired())
                onDeactivated(agent.lock().get());
        }

        // Deallocate crowd
        if (m_crowd)
        {
            dtFreeCrowd(m_crowd);
            m_crowd = nullptr;
        }
        m_navMesh.reset();
        m_bInitialized = false;
    }

    //! Reactivate all agents
    void NavAgentManager::reactivateAllAgents()
    {
        for (auto& agent : m_agents)
        {
            if(!agent.expired() && agent.lock()->isEnabled())
                onActivated(agent.lock().get());
        }
    }


    //! Initialize crowd
    bool NavAgentManager::initCrowd()
    {
        if (!m_bInitialized)
        {
            if (m_navMesh.expired())
            {
                auto dynNavMesh = std::dynamic_pointer_cast<DynamicNavMesh>(getOwner()->getFirstComponentRecursive(Component::Type::DynamicNavMesh));
                if (dynNavMesh) {
                    m_navMesh = dynNavMesh;
                }
                else {
                    auto navMesh = std::dynamic_pointer_cast<NavMesh>(getOwner()->getFirstComponentRecursive(Component::Type::NavMesh));
                    m_navMesh = navMesh;
                }

                if (m_navMesh.expired())
                    return false;

                if (m_navMesh.lock()->getNavMesh() == nullptr) {
                    m_navMesh.lock()->build();
                }
            }

            if (!m_navMesh.lock()->initializeQuery())
                return false;

            // Initialize the crowd
            if (m_maxAgentRadius == 0.f)
                m_maxAgentRadius = m_navMesh.lock()->getAgentRadius();

            // Allocate crowd
            m_crowd = dtAllocCrowd();
            if (!m_crowd->init(m_maxAgents, m_maxAgentRadius, m_navMesh.lock()->getNavMesh()))
            {
                return false;
            }

            m_bInitialized = true;
        }
        return m_bInitialized;
    }

    //! Find the nearest point
    Vec3 NavAgentManager::findNearestPoint(const Vec3 &point, int queryFilterType, dtPolyRef *nearestRef)
    {
        if (nearestRef)
            *nearestRef = 0;
        if (!m_crowd || m_navMesh.expired())
            return point;
        auto extend = m_crowd->getQueryExtents();
        return m_navMesh.lock()->findNearestPoint(point, Vec3(extend[0], extend[1], extend[2]), m_crowd->getFilter(queryFilterType), nearestRef);
    }

    //! Move along the surface
    Vec3 NavAgentManager::moveAlongSurface(const Vec3 &start, const Vec3 &end, int queryFilterType, int maxVisited)
    {
        if (!m_crowd || m_navMesh.expired())
            return end;
        auto extend = m_crowd->getQueryExtents();
        return m_navMesh.lock()->moveAlongSurface(start, end, Vec3(extend[0], extend[1], extend[2]), maxVisited, m_crowd->getFilter(queryFilterType));
    }

    //! Find a path between points
    void NavAgentManager::findPath(std::vector<Vec3> &dest, const Vec3 &start, const Vec3 &end, int queryFilterType)
    {
        if (m_crowd && !m_navMesh.expired())
        {
            auto extend = m_crowd->getQueryExtents();
            m_navMesh.lock()->findPath(dest, start, end, Vec3(extend[0], extend[1], extend[2]), m_crowd->getFilter(queryFilterType));
        }
    }

    //! Return a random point on the navigation mesh
    Vec3 NavAgentManager::getRandomPoint(int queryFilterType, dtPolyRef *randomRef)
    {
        if (randomRef)
            *randomRef = 0;
        if (!m_crowd || m_navMesh.expired())
            return {0.f, 0.f, 0.f};
        return m_navMesh.lock()->getRandomPoint(m_crowd->getFilter(queryFilterType), randomRef);
    }

    //! Return a random point on the navigation mesh within a circle
    Vec3 NavAgentManager::getRandomPointInCircle(const Vec3 &center, float radius, int queryFilterType, dtPolyRef *randomRef)
    {
        if (randomRef)
            *randomRef = 0;
        if (!m_crowd || m_navMesh.expired())
            return center;
        auto extend = m_crowd->getQueryExtents();
        return m_navMesh.lock()->getRandomPointInCircle(center, radius, Vec3(extend[0], extend[1], extend[2]), m_crowd->getFilter(queryFilterType), randomRef);
    }

    //! Return distance to wall from a point
    float NavAgentManager::getDistanceToWall(const Vec3 &point, float radius, int queryFilterType, Vec3 *hitPos, Vec3 *hitNormal)
    {
        if (hitPos)
            *hitPos = {0.f, 0.f, 0.f};
        if (hitNormal)
            *hitNormal = {0.f, -1.f, 0.f};
        if (!m_crowd || m_navMesh.expired())
            return radius;
        auto extend = m_crowd->getQueryExtents();
        return m_navMesh.lock()->getDistanceToWall(point, radius, Vec3(extend[0], extend[1], extend[2]), m_crowd->getFilter(queryFilterType), hitPos, hitNormal);
    }

    //! Perform a walkability raycast on the navigation mesh
    Vec3 NavAgentManager::raycast(const Vec3 &start, const Vec3 &end, int queryFilterType, Vec3 *hitNormal)
    {
        if (hitNormal)
            *hitNormal = {0.f, -1.f, 0.f};
        if (!m_crowd || m_navMesh.expired())
            return end;
        auto extend = m_crowd->getQueryExtents();
        return m_navMesh.lock()->raycast(start, end, Vec3(extend[0], extend[1], extend[2]), m_crowd->getFilter(queryFilterType), hitNormal);
    }

    //! Number of configured area in the specified query filter type.
    uint32_t NavAgentManager::getNumAreas(uint32_t queryFilterType) const
    {
        return queryFilterType < m_numQueryFilterTypes ? m_numAreas[queryFilterType] : 0;
    }

    //! Get the include flags for the specified query filter type.
    uint16_t NavAgentManager::getIncludeFlags(uint32_t queryFilterType) const
    {
        const auto filter = getDetourQueryFilter(queryFilterType);
        return (uint16_t)(filter ? filter->getIncludeFlags() : 0xffff);
    }

    //! Set the include flags for the specified query filter type.
    void NavAgentManager::setIncludeFlags(uint32_t queryFilterType, uint16_t flags)
    {
        auto* filter = const_cast<dtQueryFilter*>(getDetourQueryFilter(queryFilterType));
        if(filter)
        {
            filter->setIncludeFlags(flags);
            if (m_numQueryFilterTypes < queryFilterType + 1)
                m_numQueryFilterTypes = queryFilterType + 1;
        }
    }

    //! Get the exclude flags for the specified query filter type.
    uint16_t NavAgentManager::getExcludeFlags(uint32_t queryFilterType) const
    {
        const auto filter = getDetourQueryFilter(queryFilterType);
        return (uint16_t)(filter ? filter->getExcludeFlags() : 0xffff);
    }

    //! Set the exclude flags for the specified query filter type.
    void NavAgentManager::setExcludeFlags(uint32_t queryFilterType, uint16_t flags)
    {
        auto* filter = const_cast<dtQueryFilter*>(getDetourQueryFilter(queryFilterType));
        if(filter)
        {
            filter->setExcludeFlags(flags);
            if (m_numQueryFilterTypes < queryFilterType + 1)
                m_numQueryFilterTypes = queryFilterType + 1;
        }
    }

    //! Get the cost of an area for the specified query filter type.
    float NavAgentManager::getAreaCost(uint32_t queryFilterType, uint32_t areaID) const
    {
        const auto filter = getDetourQueryFilter(queryFilterType);
        return filter ? filter->getAreaCost((int)areaID) : 1.f;
    }

    //! Get the cost of an area for the specified query filter type.
    void NavAgentManager::setAreaCost(uint32_t queryFilterType, uint32_t areaID, float cost)
    {
        auto* filter = const_cast<dtQueryFilter*>(getDetourQueryFilter(queryFilterType));
        if(filter)
        {
            filter->setAreaCost((int)areaID, cost);
            if (m_numQueryFilterTypes < queryFilterType + 1)
                m_numQueryFilterTypes = queryFilterType + 1;
            if (m_numAreas[queryFilterType] < areaID + 1)
                m_numAreas[queryFilterType] = areaID + 1;
        }
    }

    //! The params for the specified obstacle avoidance type.
    const NavAgentManager::ObstacleParams& NavAgentManager::getObstacleAvoidanceParams(uint32_t obstacleAvoidanceType) const {
        static const ObstacleParams EMPTY_PARAMS = ObstacleParams();
        const dtObstacleAvoidanceParams* params = m_crowd ? m_crowd->getObstacleAvoidanceParams(obstacleAvoidanceType) : nullptr;
        return params ? *reinterpret_cast<const ObstacleParams*>(params) : EMPTY_PARAMS;
    }

    void NavAgentManager::setObstacleAvoidanceParams(uint32_t obstacleAvoidanceType, const ObstacleParams& params) {
        if (m_crowd && obstacleAvoidanceType < DT_CROWD_MAX_OBSTAVOIDANCE_PARAMS) {
            m_crowd->setObstacleAvoidanceParams(obstacleAvoidanceType, reinterpret_cast<const dtObstacleAvoidanceParams*>(&params));
            if (m_numObstacleAvoidanceTypes < obstacleAvoidanceType + 1) {
                m_numObstacleAvoidanceTypes = obstacleAvoidanceType + 1;
            }
        }
    }

    //! Serialize
    void NavAgentManager::to_json(json &j) const
    {
        Component::to_json(j);
        j["maxAgents"] = getMaxAgentNumber();
        j["maxAgentRadius"] = getMaxAgentRadius();
    }

    //! Deserialize
    void NavAgentManager::from_json(const json &j)
    {
        setMaxAgentNumber(j.value("maxAgents", 512));
        setMaxAgentRadius(j.value("maxAgentRadius", 1.f));
        Component::from_json(j);
    }
} // namespace ige::scene
