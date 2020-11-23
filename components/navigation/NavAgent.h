#pragma once

#include <limits>

#include "components/Component.h"
#include "event/Event.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

class dtCrowdAgent;

namespace ige::scene
{
    //! NavAgent
    class NavAgent : public Component
    {
    public:
        //! RequestedTarget
        enum RequestedTarget
        {
            NONE = 0,
            POSITION,
            VELOCITY
        };

        //! TargetState: agent target state
        enum class TargetState
        {
            NONE = 0,
            FAILED,
            VALID,
            REQUESTING,
            WAITING_FOR_QUEUE,
            WAITING_FOR_PATH,
            VELOCITY
        };

        //! AgentState: agent moving state
        enum class AgentState
        {
            INVALID = 0,
            WALKING,
            OFFMESH
        };

        //! NavQuality: navigation quality
        enum class NavQuality
        {
            LOW = 0,
            MEDIUM,
            HIGH
        };

        //! NavPushiness: navigation pushiness
        enum class NavPushiness
        {
            LOW = 0,
            MEDIUM,
            HIGH,
            NONE
        };

    public:
        //! Constructor
        NavAgent(SceneObject &owner);

        //! Destructor
        virtual ~NavAgent();

        //! Get name
        std::string getName() const override { return "NavAgent"; }

        //! Enable/disable
        void setEnabled(bool enable = true) override;

        //! Agent Id
        int getAgentId() const { return m_agentId; }
        void setAgentId(int id) { m_agentId = id; }

        //! Target position
        const Vec3 &getTargetPosition() const { return m_targetPosition; }
        void setTargetPosition(const Vec3 &pos);

        //! Actual position
        Vec3 getPosition() const;

        //! Target velocity
        const Vec3 &getTargetVelocity() const { return m_targetVelocity; }
        void setTargetVelocity(const Vec3 &vel);

        //! Actual velocity
        Vec3 getVelocity() const;

        //! Update position by Detour crowd manager
        bool isUpdateNodePosition() const { return m_bUpdateNodePosition; }
        void setUpdateNodePosition(bool update);

        //! Agent's max acceleration
        float getMaxAcceleration() const { return m_maxAccel; }
        void setMaxAcceleration(float acc);

        //! Agent's max velocity
        float getMaxSpeed() const { return m_maxSpeed; }
        void setMaxSpeed(float speed);

        //! Agent's radius
        float getRadius() const { return m_radius; }
        void setRadius(float radius);

        //! Agent's height
        float getHeight() const { return m_height; }
        void setHeight(float height);

        //! Agent's query filter type
        uint32_t getQueryFilterType() const { return m_queryFilterType; }
        void setQueryFilterType(uint32_t type);

        //! Agent's obstacle avoidance type
        uint32_t getObstacleAvoidanceType() const { return m_obstacleAvoidanceType; }
        void setObstacleAvoidanceType(uint32_t type);

        //! Agent's navigation quality
        NavQuality getNavigationQuality() const { return m_navQuality; }
        void setNavigationQuality(NavQuality quality);

        //! Agent's navigation pushiness.
        NavPushiness getNavigationPushiness() const { return m_navPushiness; }
        void setNavigationPushiness(NavPushiness pushiness);

        //! Agent's previous position
        const Vec3 &getPreviousPosition() const { return m_previousPosition; }

        //! Agent's previous target state
        TargetState getPreviousTargetState() const { return m_previousTargetState; }
        TargetState getTargetState() const;

        //! Agent's previous agent state
        AgentState getPreviousAgentState() const { return m_previousAgentState; }
        AgentState getAgentState() const;

        //! Internal flag to ignore transform changes
        bool isIgnoreTransformChanges() const { return m_bIgnoreTransformChanges; }

        //! Return true when the agent has a target
        bool hasRequestedTarget() const { return m_requestedTargetType != RequestedTarget::NONE; }

        //! Return true when the agent has arrived at its target
        bool hasArrived() const;

        //! Return true when the agent is in crowd (being managed by a crowd manager)
        bool isInCrowd() const;

        //! Reset target request
        void resetTarget();

        //! Get internal Detour crowd agent
        const dtCrowdAgent *getDetourCrowdAgent() const;

        //! Update
        virtual void onUpdate(float dt) override;

        //! Created/Destroyed events
        static Event<NavAgent *> &getCreatedEvent() { return m_onCreatedEvent; }
        static Event<NavAgent *> &getDestroyedEvent() { return m_onDestroyedEvent; }

        //! Activated/ Deactivated events
        static Event<NavAgent *> &getActivatedEvent() { return m_onActivatedEvent; }
        static Event<NavAgent *> &getDeactivatedEvent() { return m_onDeactivatedEvent; }

    protected:
        //! Update Detour parameters.
        void updateParameters();

    protected:
        //! Agent Id
        int m_agentId = -1;

        //! Target position
        Vec3 m_targetPosition = {0.f, 0.f, 0.f};

        //! Target velocity
        Vec3 m_targetVelocity = {0.f, 0.f, 0.f};

        //! Update position by Detour crowd manager
        bool m_bUpdateNodePosition = true;

        //! Agent's max acceleration
        float m_maxAccel = 0.f;

        //! Agent's max velocity
        float m_maxSpeed = 0.f;

        //! Agent's radius, if 0 the navigation mesh's setting will be used.
        float m_radius = 0.f;

        //! Agent's height, if 0 the navigation mesh's setting will be used.
        float m_height = 0.f;

        //! Agent's query filter type, it is an index to the query filter buffer configured in Detour crowd manager.
        uint32_t m_queryFilterType = 0;

        //! Agent's obstacle avoidance type, it is an index to the obstacle avoidance array configured in Detour crowd manager. It is ignored when agent's navigation quality is not set to "NAVIGATIONQUALITY_HIGH".
        uint32_t m_obstacleAvoidanceType = 0;

        //! Agent's navigation quality. The higher the setting, the higher the CPU usage during crowd simulation.
        NavQuality m_navQuality = NavQuality::HIGH;

        //! Agent's navigation pushiness. The higher the setting, the stronger the agent pushes its colliding neighbors around.
        NavPushiness m_navPushiness = NavPushiness::MEDIUM;

        //! Agent's previous position used to check for position changes.
        Vec3 m_previousPosition = {0.f, 0.f, 0.f};

        //! Requested target type
        RequestedTarget m_requestedTargetType = RequestedTarget::NONE;

        //! Agent's previous target state used to check for state changes.
        TargetState m_previousTargetState = TargetState::NONE;

        //! Agent's previous agent state used to check for state changes.
        AgentState m_previousAgentState = AgentState::WALKING;

        //! Internal flag to ignore transform changes because it came from us, used in OnCrowdAgentReposition().
        bool m_bIgnoreTransformChanges = false;

        //! Cache activated status
        bool m_bIsActivated = false;

        //! On created event
        static Event<NavAgent *> m_onCreatedEvent;

        //! On destroyed event
        static Event<NavAgent *> m_onDestroyedEvent;

        //! On activated event
        static Event<NavAgent *> m_onActivatedEvent;

        //! On deactivated event
        static Event<NavAgent *> m_onDeactivatedEvent;
    };
} // namespace ige::scene
