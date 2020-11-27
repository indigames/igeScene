#include "components/navigation/NavAgentManager.h"
#include "components/navigation/DynamicNavMesh.h"
#include "scene/SceneObject.h"
#include "components/TransformComponent.h"

#include <DetourCrowd.h>

namespace ige::scene
{
    NavAgentManager::NavAgentManager(SceneObject &owner)
        : Component(owner), m_navMesh(nullptr)
    {
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
        m_navMesh = nullptr;
    }

    //! NavAgent created/destroyed events
    void NavAgentManager::onCreated(NavAgent *agent)
    {
        m_agents.push_back(agent);
    }

    void NavAgentManager::onDestroyed(NavAgent *agent)
    {
        auto found = std::find_if(m_agents.begin(), m_agents.end(), [&agent](auto element) {
            return agent == element;
        });

        if (found != m_agents.end())
        {
            m_agents.erase(found);
        }
    }

    //! NavAgent activate/deactivate events
    void NavAgentManager::onActivated(NavAgent *agent)
    {
        if (!m_crowd || !m_navMesh || !agent)
            return;
        agent->setManager(this);
        initCrowd();
        dtCrowdAgentParams params;
        params.userData = agent;
        if (agent->getRadius() == 0.f)
            agent->setRadius(m_navMesh->getAgentRadius());
        if (agent->getHeight() == 0.f)
            agent->setHeight(m_navMesh->getAgentHeight());
        params.queryFilterType = (uint8_t)agent->getQueryFilterType();

        auto newPos = m_navMesh->findNearestPoint(agent->getOwner()->getTransform()->getWorldPosition(), Vec3(15.0f, 15.0f, 15.0f));
        agent->getOwner()->getTransform()->setWorldPosition(newPos);

        auto position = agent->getOwner()->getTransform()->getWorldPosition();
        auto agentId = m_crowd->addAgent(position.P(), &params);
        agent->setAgentId(agentId);
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

    //! Update
    void NavAgentManager::onUpdate(float dt)
    {
        if (!m_bInitialized)
            return;
        m_crowd->update(dt, nullptr);
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

    //! Initialize crowd
    bool NavAgentManager::initCrowd()
    {
        if (!m_bInitialized)
        {
            if (!m_navMesh)
            {
                auto dynNavMesh = getOwner()->getComponent<DynamicNavMesh>();
                if (dynNavMesh)
                {
                    m_navMesh = dynNavMesh.get();
                }
                else
                {
                    auto navMesh = getOwner()->getComponent<NavMesh>();
                    m_navMesh = navMesh.get();
                }

                if (!m_navMesh)
                    return false;
            }

            if (!m_navMesh->initializeQuery())
                return false;

            // Initialize the crowd
            if (m_maxAgentRadius == 0.f)
                m_maxAgentRadius = m_navMesh->getAgentRadius();

            // Allocate crowd
            m_crowd = dtAllocCrowd();
            if (!m_crowd->init(m_maxAgents, m_maxAgentRadius, m_navMesh->getNavMesh()))
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
        if (!m_crowd || !m_navMesh)
            return point;
        return m_navMesh->findNearestPoint(point, (*(Vec3 *)m_crowd->getQueryExtents()), m_crowd->getFilter(queryFilterType), nearestRef);
    }

    //! Move along the surface
    Vec3 NavAgentManager::moveAlongSurface(const Vec3 &start, const Vec3 &end, int queryFilterType, int maxVisited)
    {
        if (!m_crowd || !m_navMesh)
            return end;
        return m_navMesh->moveAlongSurface(start, end, (*(Vec3 *)m_crowd->getQueryExtents()), maxVisited, m_crowd->getFilter(queryFilterType));
    }

    //! Find a path between points
    void NavAgentManager::findPath(std::vector<Vec3> &dest, const Vec3 &start, const Vec3 &end, int queryFilterType)
    {
        if (m_crowd && m_navMesh)
            m_navMesh->findPath(dest, start, end, (*(Vec3 *)m_crowd->getQueryExtents()), m_crowd->getFilter(queryFilterType));
    }

    //! Return a random point on the navigation mesh
    Vec3 NavAgentManager::getRandomPoint(int queryFilterType, dtPolyRef *randomRef)
    {
        if (randomRef)
            *randomRef = 0;
        if (!m_crowd || !m_navMesh)
            return {0.f, 0.f, 0.f};
        return m_navMesh->getRandomPoint(m_crowd->getFilter(queryFilterType), randomRef);
    }

    //! Return a random point on the navigation mesh within a circle
    Vec3 NavAgentManager::getRandomPointInCircle(const Vec3 &center, float radius, int queryFilterType, dtPolyRef *randomRef)
    {
        if (randomRef)
            *randomRef = 0;
        if (!m_crowd || !m_navMesh)
            return center;
        return m_navMesh->getRandomPointInCircle(center, radius, (*(Vec3 *)m_crowd->getQueryExtents()), m_crowd->getFilter(queryFilterType), randomRef);
    }

    //! Return distance to wall from a point
    float NavAgentManager::getDistanceToWall(const Vec3 &point, float radius, int queryFilterType, Vec3 *hitPos, Vec3 *hitNormal)
    {
        if (hitPos)
            *hitPos = {0.f, 0.f, 0.f};
        if (hitNormal)
            *hitNormal = {0.f, -1.f, 0.f};
        if (!m_crowd || !m_navMesh)
            return radius;
        return m_navMesh->getDistanceToWall(point, radius, (*(Vec3 *)m_crowd->getQueryExtents()), m_crowd->getFilter(queryFilterType), hitPos, hitNormal);
    }

    //! Perform a walkability raycast on the navigation mesh
    Vec3 NavAgentManager::raycast(const Vec3 &start, const Vec3 &end, int queryFilterType, Vec3 *hitNormal)
    {
        if (hitNormal)
            *hitNormal = {0.f, -1.f, 0.f};
        if (!m_crowd || !m_navMesh)
            return end;
        return m_navMesh->raycast(start, end, (*(Vec3 *)m_crowd->getQueryExtents()), m_crowd->getFilter(queryFilterType), hitNormal);
    }

    //! Number of configured area in the specified query filter type.
    uint32_t NavAgentManager::getNumAreas(uint32_t queryFilterType) const
    {
        return queryFilterType < m_numQueryFilterTypes ? m_numAreas[queryFilterType] : 0;
    }

    /// Get the include flags for the specified query filter type.
    uint16_t NavAgentManager::getIncludeFlags(uint32_t queryFilterType) const
    {
        const auto filter = getDetourQueryFilter(queryFilterType);
        return (uint16_t)(filter ? filter->getIncludeFlags() : 0xffff);
    }

    /// Get the exclude flags for the specified query filter type.
    uint16_t NavAgentManager::getExcludeFlags(uint32_t queryFilterType) const
    {
        const auto filter = getDetourQueryFilter(queryFilterType);
        return (uint16_t)(filter ? filter->getExcludeFlags() : 0xffff);
    }

    /// Get the cost of an area for the specified query filter type.
    float NavAgentManager::getAreaCost(uint32_t queryFilterType, uint32_t areaID) const
    {
        const auto filter = getDetourQueryFilter(queryFilterType);
        return filter ? filter->getAreaCost((int)areaID) : 1.f;
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
