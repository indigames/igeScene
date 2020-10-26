#pragma once

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include "event/Event.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"
#include "components/TransformComponent.h"

namespace ige::scene
{
    //! PhysicBase
    class PhysicBase : public Component
    {
    public:
        //! Constructor
        PhysicBase(SceneObject &owner);

        //! Destructor
        virtual ~PhysicBase();

        //! Initialization
        bool init();

        //! Apply torque
        void applyTorque(const btVector3 &torque);

        //! Apply force
        void applyForce(const btVector3 &force);
        void applyForce(const btVector3 &force, const btVector3 &pos);

        //! Apply impulse
        void applyImpulse(const btVector3 &impulse);
        void applyImpulse(const btVector3 &impulse, const btVector3 &pos);

        //! Clear forces
        void clearForces();

        //! Add collision flag
        void addCollisionFlag(btCollisionObject::CollisionFlags flag);

        //! Remove collision flag
        void removeCollisionFlag(btCollisionObject::CollisionFlags flag);

        //! Update Bullet transform
        void updateBtTransform();

        //! Update IGE transform
        void updateIgeTransform();

        //! Body
        btRigidBody &getBody() const { return *m_body; };

        //! Mass
        float getMass() const { return m_mass; };
        void setMass(float mass);

        //! Friction
        float getFriction() const { return m_body->getFriction(); }
        void setFriction(float friction)
        {
            m_friction = friction;
            m_body->setFriction(friction);
        }

        //! Restitution
        float getRestitution() const { return m_body->getRestitution(); }
        void setRestitution(float restitution)
        {
            m_restitution = restitution;
            m_body->setRestitution(restitution);
        }

        //! Linear velocity
        const btVector3 &getLinearVelocity() const { return m_linearVelocity; }
        void setLinearVelocity(const btVector3 &velocity);

        //! Angular velocity
        const btVector3 &getAngularVelocity() const { return m_angularVelocity; }
        void setAngularVelocity(const btVector3 &velocity);

        //! Linear factor
        const btVector3 &getLinearFactor() const { return m_linearFactor; }
        void setLinearFactor(const btVector3 &factor);

        //! Angular factor
        const btVector3 &getAngularFactor() const { return m_angularFactor; }
        void setAngularFactor(const btVector3 &factor);

        //! Indicate object is a trigger object
        bool isTrigger() const { return m_bIsTrigger; }
        void setIsTrigger(bool isTrigger = true);

        //! Indicate object is a kinematic object
        bool isKinematic() const { return m_bIsKinematic; }
        void setIsKinematic(bool isKinematic = true);

        //! Enable/Disable physic component
        bool isEnabled() const { return m_bIsEnabled; }
        void setEnabled(bool enable = true);

        //! Enable/Disable continous collision detection
        bool isCCD() const { return m_bIsCCD; }
        void setCCD(bool isCCD = true);

        //! Position offset
        const Vec3 &getPositionOffset() const { return m_positionOffset; }
        void setPositionOffset(const Vec3 &offset) { m_positionOffset = offset; }

        //! Get AABB
        void getAABB(btVector3 &aabbMin, btVector3 aabbMax);

        //! Collision filter group
        int getCollisionFilterGroup() { return m_collisionFilterGroup; }
        void setCollisionFilterGroup(int group);

        //! Collision filter mask
        int getCollisionFilterMask() { return m_collisionFilterMask; }
        void setCollisionFilterMask(int mask);

        //! Get onCreatedEvent
        static Event<PhysicBase &> &getOnCreatedEvent() { return m_onCreatedEvent; }

        //! Get onDestroyedEvent
        static Event<PhysicBase &> &getOnDestroyedEvent() { return m_onDestroyedEvent; }

        //! Get onDestroyedEvent
        static Event<PhysicBase &> &getOnActivatedEvent() { return m_onActivatedEvent; }

        //! Get onDestroyedEvent
        static Event<PhysicBase &> &getOnDeactivatedEvent() { return m_onDeactivatedEvent; }

        //! Collision events
        Event<PhysicBase &> &getCollisionStartEvent() { return m_collisionStartEvent; }
        Event<PhysicBase &> &getCollisionStayEvent() { return m_collisionStayEvent; }
        Event<PhysicBase &> &getCollisionStopEvent() { return m_collisionStopEvent; }

        //! Trigger events
        Event<PhysicBase &> &getTriggerStartEvent() { return m_triggerStartEvent; }
        Event<PhysicBase &> &getTriggerStayEvent() { return m_triggerStayEvent; }
        Event<PhysicBase &> &getTriggerStopEvent() { return m_triggerStopEvent; }

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Create physic body
        void createBody();

        //! Destroy physic body
        void destroyBody();

        //! Calculate and apply inertia
        void applyInertia();

        //! Activate
        void activate();

        //! Deactivate
        void deactivate();

        //! Recreate Body
        void recreateBody();

        //! Set local scale
        virtual void setLocalScale(const Vec3 &scale) = 0;

    protected:
        //! On created event
        static Event<PhysicBase &> m_onCreatedEvent;

        //! On destroyed event
        static Event<PhysicBase &> m_onDestroyedEvent;

        //! On activated event
        static Event<PhysicBase &> m_onActivatedEvent;

        //! On deactivated event
        static Event<PhysicBase &> m_onDeactivatedEvent;

        //! Collision events
        Event<PhysicBase &> m_collisionStartEvent;
        Event<PhysicBase &> m_collisionStayEvent;
        Event<PhysicBase &> m_collisionStopEvent;

        //! Trigger events
        Event<PhysicBase &> m_triggerStartEvent;
        Event<PhysicBase &> m_triggerStayEvent;
        Event<PhysicBase &> m_triggerStopEvent;

    protected:
        //! Collision shape
        std::unique_ptr<btCollisionShape> m_shape;

        //! Rigid Body
        std::unique_ptr<btRigidBody> m_body;

        //! Motion State
        std::unique_ptr<btMotionState> m_motion;

        //! Mass
        float m_mass = 1.f;

        //! Restuitution
        float m_restitution = 1.f;

        //! Friction
        float m_friction = 0.5f;

        //! Linear velocity
        btVector3 m_linearVelocity = {0.f, 0.f, 0.f};

        //! Angular velocity
        btVector3 m_angularVelocity = {0.f, 0.f, 0.f};

        //! Linear factor
        btVector3 m_linearFactor = {1.f, 1.f, 0.f};

        //! Angular factor
        btVector3 m_angularFactor = {0.f, 0.f, 0.f};

        //! isKinematic
        bool m_bIsKinematic = false;

        //! isTrigger
        bool m_bIsTrigger = false;

        //! Enable physic
        bool m_bIsEnabled = true;

        //! Continuous collision detection
        bool m_bIsCCD = false;

        //! Collision filter group
        int m_collisionFilterGroup = 1;

        //! Collisuin filter mask
        int m_collisionFilterMask = -1;

        //! Cache activated status
        bool m_bIsActivated = false;

        //! Cache transform
        TransformComponent *m_transform;

        //! Position offsets
        Vec3 m_positionOffset = {0.f, 0.f, 0.f};

        //! Cache previous scale value
        Vec3 m_previousScale = {1.f, 1.f, 1.f};
    };
} // namespace ige::scene
