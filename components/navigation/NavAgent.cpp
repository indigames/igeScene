#include "components/navigation/NavAgent.h"
#include "components/navigation/NavAgentManager.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include <DetourCommon.h>
#include <DetourCrowd.h>

namespace ige::scene
{
    //! Initialize static members
    Event<NavAgent *> NavAgent::m_onCreatedEvent;
    Event<NavAgent *> NavAgent::m_onDestroyedEvent;
    Event<NavAgent *> NavAgent::m_onActivatedEvent;
    Event<NavAgent *> NavAgent::m_onDeactivatedEvent;

    //! Constructor
    NavAgent::NavAgent(SceneObject &owner)
        : Component(owner)
    {
        getCreatedEvent().invoke(this);
        setEnabled(true);
    }

    NavAgent::~NavAgent()
    {
        setEnabled(false);
        getDestroyedEvent().invoke(this);
        m_manager = nullptr;
    }

    //! Update
    void NavAgent::onUpdate(float dt)
    {
        const auto *agent = getDetourCrowdAgent();
        if (!agent)
            return;

        auto newPos = Vec3(agent->npos[0], agent->npos[1], agent->npos[2]);
        auto newVel = Vec3(agent->vel[0], agent->vel[1], agent->vel[2]);

        // Notify parent node of the reposition
        if (newPos != m_previousPosition)
        {
            m_previousPosition = newPos;

            if (m_bUpdateNodePosition)
            {
                m_bIgnoreTransformChanges = true;
                getOwner()->getTransform()->setWorldPosition(newPos);
                m_bIgnoreTransformChanges = false;
            }
        }

        // Send a notification event if we've reached the destination
        TargetState newTargetState = getTargetState();
        AgentState newAgentState = getAgentState();
        if (newAgentState != m_previousAgentState || newTargetState != m_previousTargetState)
        {
            m_previousAgentState = getAgentState();
            m_previousTargetState = getTargetState();
        }
    }

    //! Enable/disable
    void NavAgent::setEnabled(bool enable)
    {
        Component::setEnabled(enable);

        if (isEnabled() && !m_bIsActivated)
        {
            getActivatedEvent().invoke(this);
            m_bIsActivated = true;
            m_previousPosition = getOwner()->getTransform()->getWorldPosition();

            if (isInCrowd())
            {
                auto* agent = const_cast<dtCrowdAgent*>(getDetourCrowdAgent());
                if (!agent)
                    return;

                auto& agentPos = reinterpret_cast<Vec3&>(agent->npos);
                if (agentPos != m_previousPosition)
                    agentPos = m_previousPosition;

                dtPolyRef nearestRef;
                auto nearestPos = m_manager->findNearestPoint(m_targetPosition, m_queryFilterType, &nearestRef);
                m_manager->getCrowd()->requestMoveTarget(getAgentId(), nearestRef, nearestPos.P());
            }
        }

        if (!isEnabled() && m_bIsActivated)
        {
            getDeactivatedEvent().invoke(this);
            m_bIsActivated = false;
        }
    }

    const dtCrowdAgent *NavAgent::getDetourCrowdAgent() const
    {
        return isInCrowd() ? m_manager->getDetourCrowdAgent(getAgentId()) : nullptr;
    }

    //! Update Detour base parameter.
    void NavAgent::updateParameters()
    {
        const auto *agent = getDetourCrowdAgent();
        if (agent)
        {
            dtCrowdAgentParams params = agent->params;

            switch (m_navQuality)
            {
            case NavQuality::LOW:
                params.updateFlags = 0u | DT_CROWD_OPTIMIZE_VIS | DT_CROWD_ANTICIPATE_TURNS;
                break;

            case NavQuality::MEDIUM:
                params.updateFlags = 0u | DT_CROWD_OPTIMIZE_TOPO | DT_CROWD_OPTIMIZE_VIS | DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_SEPARATION;
                break;

            case NavQuality::HIGH:
                params.updateFlags = 0u
                                     // Path finding
                                     | DT_CROWD_OPTIMIZE_TOPO | DT_CROWD_OPTIMIZE_VIS
                                     // Steering
                                     | DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_SEPARATION
                                     // Velocity planning
                                     | DT_CROWD_OBSTACLE_AVOIDANCE;
                break;
            }

            switch (m_navPushiness)
            {
            case NavPushiness::LOW:
                params.separationWeight = 4.0f;
                params.collisionQueryRange = m_radius * 16.0f;
                break;

            case NavPushiness::MEDIUM:
                params.separationWeight = 2.0f;
                params.collisionQueryRange = m_radius * 8.0f;
                break;

            case NavPushiness::HIGH:
                params.separationWeight = 0.5f;
                params.collisionQueryRange = m_radius * 1.0f;
                break;

            default:
                params.separationWeight = 0.0f;
                params.collisionQueryRange = m_radius * 1.0f;
                break;
            }

            params.radius = m_radius;
            params.height = m_height;
            params.maxAcceleration = m_maxAccel;
            params.maxSpeed = m_maxSpeed;
            params.pathOptimizationRange = m_radius * 30.0f;
            params.queryFilterType = (uint8_t)m_queryFilterType;
            params.obstacleAvoidanceType = (uint8_t)m_obstacleAvoidanceType;

            if (m_manager)
                m_manager->getCrowd()->updateAgentParameters(getAgentId(), &params);
        }
    }

    //! Return true when the agent has arrived at its target
    bool NavAgent::hasArrived() const
    {
        if (!m_manager)
            return false;
        const auto *agent = m_manager->getDetourCrowdAgent(m_agentId);
        if (!agent || !agent->ncorners)
            return false;
        return (agent->cornerFlags[agent->ncorners - 1] & DT_STRAIGHTPATH_END && dtVdist2D(agent->npos, &agent->cornerVerts[(agent->ncorners - 1) * 3]) <= agent->params.radius);
    }

    //! Return true when the agent is in crowd
    bool NavAgent::isInCrowd() const
    {
        return (m_manager != nullptr && m_agentId != -1);
    }

    //! Set target position
    void NavAgent::setTargetPosition(const Vec3 &pos)
    {
        if (m_targetPosition != pos)
        {
            m_targetPosition = pos;

            if (isInCrowd())
            {
                dtPolyRef nearestRef;
                auto nearestPos = m_manager->findNearestPoint(pos, m_queryFilterType, &nearestRef);
                m_manager->getCrowd()->requestMoveTarget(getAgentId(), nearestRef, nearestPos.P());

                // Start moving
                auto* agent = const_cast<dtCrowdAgent*>(getDetourCrowdAgent());
                if (agent && agent->state == DT_CROWDAGENT_STATE_INVALID)
                    agent->state = DT_CROWDAGENT_STATE_WALKING;
            }
        }
    }

    //! Reset target
    void NavAgent::resetTarget()
    {
        if (isInCrowd())
            m_manager->getCrowd()->resetMoveTarget(getAgentId());
    }

    //! Set update node position
    void NavAgent::setUpdateNodePosition(bool update)
    {
        m_bUpdateNodePosition = update;
    }

    //! Set max acceleration
    void NavAgent::setMaxAcceleration(float acc)
    {
        if (m_maxAccel != acc)
        {
            m_maxAccel = acc;
            updateParameters();
        }
    }

    //! Set max speed
    void NavAgent::setMaxSpeed(float speed)
    {
        if (m_maxSpeed != speed)
        {
            m_maxSpeed = speed;
            updateParameters();
        }
    }

    //! Set radius
    void NavAgent::setRadius(float radius)
    {
        if (m_radius != radius)
        {
            m_radius = radius;
            updateParameters();
        }
    }

    //! Set height
    void NavAgent::setHeight(float height)
    {
        if (m_height != height)
        {
            m_height = height;
            updateParameters();
        }
    }

    //! Set query filter type
    void NavAgent::setQueryFilterType(uint32_t type)
    {
        if (m_queryFilterType != type)
        {
            m_queryFilterType = type;
            updateParameters();
        }
    }

    //! Set obstacle avoidance type
    void NavAgent::setObstacleAvoidanceType(uint32_t type)
    {
        if (m_obstacleAvoidanceType != type)
        {
            m_obstacleAvoidanceType = type;
            updateParameters();
        }
    }

    //! Set navigation quality
    void NavAgent::setNavigationQuality(NavQuality quality)
    {
        if (m_navQuality != quality)
        {
            m_navQuality = quality;
            updateParameters();
        }
    }

    //! Set navigation pushiness
    void NavAgent::setNavigationPushiness(NavPushiness pushiness)
    {
        if (m_navPushiness != pushiness)
        {
            m_navPushiness = pushiness;
            updateParameters();
        }
    }

    //! Get target state
    NavAgent::TargetState NavAgent::getTargetState() const
    {
        const auto *agent = getDetourCrowdAgent();
        return agent ? (NavAgent::TargetState)agent->targetState : NavAgent::TargetState::NONE;
    }

    //! Get agent state
    NavAgent::AgentState NavAgent::getAgentState() const
    {
        const auto *agent = getDetourCrowdAgent();
        return agent ? (NavAgent::AgentState)agent->state : NavAgent::AgentState::INVALID;
    }

    //! Actual position
    Vec3 NavAgent::getPosition() const
    {
        const auto *agent = getDetourCrowdAgent();
        return agent ? *(Vec3 *)(agent->npos) : getOwner()->getTransform()->getWorldPosition();
    }

    //! Actual velocity
    Vec3 NavAgent::getVelocity() const
    {
        const auto *agent = getDetourCrowdAgent();
        return agent ? *(Vec3 *)(agent->vel) : Vec3(0.f, 0.f, 0.f);
    }

    //! Serialize
    void NavAgent::to_json(json &j) const
    {
        Component::to_json(j);
        j["targetPos"] = getTargetPosition();
        j["syncPos"] = isUpdateNodePosition();
        j["maxAcc"] = getMaxAcceleration();
        j["maxSpeed"] = getMaxSpeed();
        j["radius"] = getRadius();
        j["height"] = getHeight();
        j["filter"] = getQueryFilterType();
        j["obsAvoid"] = getObstacleAvoidanceType();
        j["navQuality"] = (int)getNavigationQuality();
        j["navPushiness"] = (int)getNavigationPushiness();
    }

    //! Deserialize
    void NavAgent::from_json(const json &j)
    {
        setTargetPosition(j.value("targetPos", Vec3(0.f, 0.f, 0.f)));
        setUpdateNodePosition(j.value("syncPos", true));
        setMaxAcceleration(j.value("maxAcc", 5.f));
        setMaxSpeed(j.value("maxSpeed", 3.f));
        setRadius(j.value("radius", 0.f));
        setHeight(j.value("height", 0.f));
        setQueryFilterType(j.value("filter", 0));
        setObstacleAvoidanceType(j.value("obsAvoid", 0));
        setNavigationQuality((NavQuality)j.value("navQuality", 0));
        setNavigationPushiness((NavPushiness)j.value("navPushiness", 0));
        Component::from_json(j);
    }

} // namespace ige::scene
