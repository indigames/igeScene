#include "systems/navigation/NavManager.h"
#include "scene/SceneObject.h"
#include "components/TransformComponent.h"

#include <DetourCrowd.h>

namespace ige::scene
{
    NavManager::NavManager()
    {
        // Allocate crowd
        m_crowd = dtAllocCrowd();

        // Register event listeners
        NavAgent::getCreatedEvent().addListener(std::bind(static_cast<void (NavManager::*)(NavAgent *)>(&NavManager::onCreated), this, std::placeholders::_1));
        NavAgent::getDestroyedEvent().addListener(std::bind(static_cast<void (NavManager::*)(NavAgent *)>(&NavManager::onDestroyed), this, std::placeholders::_1));
        NavAgent::getActivatedEvent().addListener(std::bind(static_cast<void (NavManager::*)(NavAgent *)>(&NavManager::onActivated), this, std::placeholders::_1));
        NavAgent::getDeactivatedEvent().addListener(std::bind(static_cast<void (NavManager::*)(NavAgent *)>(&NavManager::onDeactivated), this, std::placeholders::_1));
    }

    NavManager::~NavManager()
    {
        // Unregister event listeners
        NavAgent::getCreatedEvent().removeAllListeners();
        NavAgent::getDestroyedEvent().removeAllListeners();
        NavAgent::getActivatedEvent().removeAllListeners();
        NavAgent::getDeactivatedEvent().removeAllListeners();

        // Deallocate crowd
        dtFreeCrowd(m_crowd);
        m_crowd = nullptr;
    }

    //! NavAgent created/destroyed events
    void NavManager::onCreated(NavAgent *agent)
    {
        m_agents.push_back(agent);
    }

    void NavManager::onDestroyed(NavAgent *agent)
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
    void NavManager::onActivated(NavAgent *agent)
    {
        if (!m_crowd || !m_navMesh || !agent)
            return;
        dtCrowdAgentParams params;
        params.userData = agent;
        if (agent->getRadius() == 0.f)
            agent->setRadius(m_navMesh->getAgentRadius());
        if (agent->getHeight() == 0.f)
            agent->setHeight(m_navMesh->getAgentHeight());
        params.queryFilterType = (uint8_t)agent->getQueryFilterType();
        auto position = agent->getOwner()->getTransform()->getWorldPosition();
        auto agentId = m_crowd->addAgent(position.P(), &params);
        agent->setAgentId(agentId);
    }

    void NavManager::onDeactivated(NavAgent *agent)
    {
        if (!m_crowd || !agent)
            return;
        dtCrowdAgent *agt = m_crowd->getEditableAgent(agent->getAgentId());
        if (agt)
            agt->params.userData = nullptr;
        m_crowd->removeAgent(agent->getAgentId());
        agent->setAgentId(-1);
    }

    //! Update
    void NavManager::onUpdate(float dt)
    {
        m_crowd->update(dt, nullptr);
    }

    //! Get the detour crowd agent.
    const dtCrowdAgent *NavManager::getDetourCrowdAgent(int agent) const
    {
        return m_crowd ? m_crowd->getAgent(agent) : nullptr;
    }

    //! Get the detour query filter.
    const dtQueryFilter *NavManager::getDetourQueryFilter(uint32_t queryFilterType) const
    {
        return m_crowd ? m_crowd->getFilter(queryFilterType) : nullptr;
    }

    //! Initialize crowd
    bool NavManager::initCrowd()
    {
        if (!m_navMesh || !m_navMesh->initializeQuery())
            return false;

        // Initialize the crowd
        if (m_maxAgentRadius == 0.f)
            m_maxAgentRadius = m_navMesh->getAgentRadius();

        if (!m_crowd->init(m_maxAgents, m_maxAgentRadius, m_navMesh->getNavMesh()))
        {
            return false;
        }

        return true;
    }

    //! Find the nearest point
    Vec3 NavManager::findNearestPoint(const Vec3 &point, int queryFilterType, dtPolyRef *nearestRef)
    {
        if (nearestRef)
            *nearestRef = 0;
        if (!m_crowd || !m_navMesh)
            return point;
        return m_navMesh->findNearestPoint(point, (*(Vec3 *)m_crowd->getQueryExtents()), m_crowd->getFilter(queryFilterType), nearestRef);
    }

    //! Move along the surface
    Vec3 NavManager::moveAlongSurface(const Vec3 &start, const Vec3 &end, int queryFilterType, int maxVisited)
    {
        if (!m_crowd || !m_navMesh)
            return end;
        return m_navMesh->moveAlongSurface(start, end, (*(Vec3 *)m_crowd->getQueryExtents()), maxVisited, m_crowd->getFilter(queryFilterType));
    }

    //! Find a path between points
    void NavManager::findPath(std::vector<Vec3> &dest, const Vec3 &start, const Vec3 &end, int queryFilterType)
    {
        if (m_crowd && m_navMesh)
            m_navMesh->findPath(dest, start, end, (*(Vec3 *)m_crowd->getQueryExtents()), m_crowd->getFilter(queryFilterType));
    }

    //! Return a random point on the navigation mesh
    Vec3 NavManager::getRandomPoint(int queryFilterType, dtPolyRef *randomRef)
    {
        if (randomRef)
            *randomRef = 0;
        if (!m_crowd || !m_navMesh)
            return {0.f, 0.f, 0.f};
        return m_navMesh->getRandomPoint(m_crowd->getFilter(queryFilterType), randomRef);
    }

    //! Return a random point on the navigation mesh within a circle
    Vec3 NavManager::getRandomPointInCircle(const Vec3 &center, float radius, int queryFilterType, dtPolyRef *randomRef)
    {
        if (randomRef)
            *randomRef = 0;
        if (!m_crowd || !m_navMesh)
            return center;
        return m_navMesh->getRandomPointInCircle(center, radius, (*(Vec3 *)m_crowd->getQueryExtents()), m_crowd->getFilter(queryFilterType), randomRef);
    }

    //! Return distance to wall from a point
    float NavManager::getDistanceToWall(const Vec3 &point, float radius, int queryFilterType, Vec3 *hitPos, Vec3 *hitNormal)
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
    Vec3 NavManager::raycast(const Vec3 &start, const Vec3 &end, int queryFilterType, Vec3 *hitNormal)
    {
        if (hitNormal)
            *hitNormal = {0.f, -1.f, 0.f};
        if (!m_crowd || !m_navMesh)
            return end;
        return m_navMesh->raycast(start, end, (*(Vec3 *)m_crowd->getQueryExtents()), m_crowd->getFilter(queryFilterType), hitNormal);
    }

    //! Number of configured area in the specified query filter type.
    uint32_t NavManager::getNumAreas(uint32_t queryFilterType) const
    {
        return queryFilterType < m_numQueryFilterTypes ? m_numAreas[queryFilterType] : 0;
    }

    /// Get the include flags for the specified query filter type.
    uint16_t NavManager::getIncludeFlags(uint32_t queryFilterType) const
    {
        const auto filter = getDetourQueryFilter(queryFilterType);
        return (uint16_t)(filter ? filter->getIncludeFlags() : 0xffff);
    }

    /// Get the exclude flags for the specified query filter type.
    uint16_t NavManager::getExcludeFlags(uint32_t queryFilterType) const
    {
        const auto filter = getDetourQueryFilter(queryFilterType);
        return (uint16_t)(filter ? filter->getExcludeFlags() : 0xffff);
    }

    /// Get the cost of an area for the specified query filter type.
    float NavManager::getAreaCost(uint32_t queryFilterType, uint32_t areaID) const
    {
        const auto filter = getDetourQueryFilter(queryFilterType);
        return filter ? filter->getAreaCost((int)areaID) : 1.f;
    }
} // namespace ige::scene
