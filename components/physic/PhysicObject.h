#pragma once

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletSoftBody/btSoftBody.h>

#include "event/Event.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"

namespace ige::scene
{
    //! PhysicObject
    class PhysicObject : public Component
    {
    public:
        //! Constructor
        PhysicObject(SceneObject &owner);

        //! Destructor
        virtual ~PhysicObject();

        //! Initialization
        virtual bool init();

        //! Deinitialization
        virtual bool destroy();

        //! Get RigidBody
        virtual btRigidBody* getBody() const {
            if (isSoftBody())
                return nullptr;
            return (btRigidBody*)m_body.get();
        }

        //! Get SoftBody
        virtual btSoftBody* getSoftBody() const {
            if(!isSoftBody())
                return nullptr;
            return (btSoftBody*)m_body.get();
        }

        //! Mass
        virtual float getMass() const { return m_mass; };
        virtual void setMass(float mass);

        //! Friction
        virtual float getFriction() const { return m_friction; }
        virtual void setFriction(float friction);

        //! Restitution
        virtual float getRestitution() const { return m_restitution; }
        virtual void setRestitution(float restitution);

        //! Linear velocity
        virtual const btVector3 &getLinearVelocity() const { return m_linearVelocity; }
        virtual void setLinearVelocity(const btVector3& velocity);

        //! Angular velocity
        virtual const btVector3 &getAngularVelocity() const { return m_angularVelocity; }
        virtual void setAngularVelocity(const btVector3& velocity);

        //! Linear factor
        virtual const btVector3 &getLinearFactor() const { return m_linearFactor; }
        virtual void setLinearFactor(const btVector3& factor);

        //! Angular factor
        virtual const btVector3 &getAngularFactor() const { return m_angularFactor; }
        virtual void setAngularFactor(const btVector3& factor);

        //! Indicate object is a trigger object
        virtual bool isTrigger() const { return m_bIsTrigger; }
        virtual void setIsTrigger(bool isTrigger = true);

        //! Indicate object is a kinematic object
        virtual bool isKinematic() const { return m_bIsKinematic; }
        virtual void setIsKinematic(bool isKinematic = true);

        //! Enable/Disable physic component
        virtual bool isEnabled() const { return m_bIsEnabled; }
        virtual void setEnabled(bool enable = true);

        //! Enable/Disable continuos collision detection
        virtual bool isCCD() const { return m_bIsCCD; }
        virtual void setCCD(bool isCCD = true);

        //! Get AABB
        virtual void getAABB(btVector3 &aabbMin, btVector3 aabbMax);

        //! Add collision flag
        virtual void addCollisionFlag(btCollisionObject::CollisionFlags flag) {
            m_body->setCollisionFlags(m_body->getCollisionFlags() | flag);
        }

        //! Remove collision flag
        virtual void removeCollisionFlag(btCollisionObject::CollisionFlags flag) {
            m_body->setCollisionFlags(m_body->getCollisionFlags() & ~flag);
        }

        //! Collision filter group
        virtual int getCollisionFilterGroup() const { return m_collisionFilterGroup; }
        virtual void setCollisionFilterGroup(int group);

        //! Collision filter mask
        virtual int getCollisionFilterMask() const { return m_collisionFilterMask; }
        virtual void setCollisionFilterMask(int mask);

        //! Collision Margin
        float getCollisionMargin() const { return m_collisionMargin; }
        void setCollisionMargin(float margin);

    public:
        //! Apply torque
        virtual void applyTorque(const btVector3& torque) {
            if (getBody()) {
                getBody()->applyTorque(torque);
            }
        }

        //! Apply force
        virtual void applyForce(const btVector3& force) { getBody()->applyCentralForce(force); }
        virtual void applyForce(const btVector3& force, const btVector3& pos) { getBody()->applyForce(force, pos); }

        //! Apply impulse
        virtual void applyImpulse(const btVector3& impulse) { getBody()->applyCentralImpulse(impulse); }
        virtual void applyImpulse(const btVector3& impulse, const btVector3& pos) { getBody()->applyImpulse(impulse, pos); }

        //! Clear forces
        virtual void clearForces() { if (getBody()) getBody()->clearForces(); }

        //! Update Bullet transform
        virtual void updateBtTransform();

        //! Update IGE transform
        virtual void updateIgeTransform();

        //! Get onCreatedEvent
        static Event<PhysicObject*> &getOnCreatedEvent() { return m_onCreatedEvent; }

        //! Get onDestroyedEvent
        static Event<PhysicObject*> &getOnDestroyedEvent() { return m_onDestroyedEvent; }

        //! Get onDestroyedEvent
        static Event<PhysicObject*> &getOnActivatedEvent() { return m_onActivatedEvent; }

        //! Get onDestroyedEvent
        static Event<PhysicObject*> &getOnDeactivatedEvent() { return m_onDeactivatedEvent; }

        //! Collision events
        Event<PhysicObject*> &getCollisionStartEvent() { return m_collisionStartEvent; }
        Event<PhysicObject*> &getCollisionStayEvent() { return m_collisionStayEvent; }
        Event<PhysicObject*> &getCollisionStopEvent() { return m_collisionStopEvent; }

        //! Trigger events
        Event<PhysicObject*> &getTriggerStartEvent() { return m_triggerStartEvent; }
        Event<PhysicObject*> &getTriggerStayEvent() { return m_triggerStayEvent; }
        Event<PhysicObject*> &getTriggerStopEvent() { return m_triggerStopEvent; }

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Create physic body
        virtual void createBody();

        //! Destroy physic body
        virtual void destroyBody();

        //! Calculate and apply inertia
        virtual void applyInertia();

        //! Activate
        virtual void activate();

        //! Deactivate
        virtual void deactivate();

        //! Recreate Body
        virtual void recreateBody();

        //! Local scale
        virtual const Vec3& getLocalScale() const { return m_previousScale; }
        virtual void setLocalScale(const Vec3& scale);

        //! Is SoftBody
        bool isSoftBody() const { return m_bIsSoftBody; }
        void setSoftBody(bool isSoftBody) { m_bIsSoftBody = isSoftBody; }

    protected:
        //! On created event
        static Event<PhysicObject*> m_onCreatedEvent;

        //! On destroyed event
        static Event<PhysicObject*> m_onDestroyedEvent;

        //! On activated event
        static Event<PhysicObject*> m_onActivatedEvent;

        //! On deactivated event
        static Event<PhysicObject*> m_onDeactivatedEvent;

        //! Collision events
        Event<PhysicObject*> m_collisionStartEvent;
        Event<PhysicObject*> m_collisionStayEvent;
        Event<PhysicObject*> m_collisionStopEvent;

        //! Trigger events
        Event<PhysicObject*> m_triggerStartEvent;
        Event<PhysicObject*> m_triggerStayEvent;
        Event<PhysicObject*> m_triggerStopEvent;

    protected:
        //! Collision shape
        std::unique_ptr<btCollisionShape> m_shape;

        //! Rigid Body
        std::unique_ptr<btCollisionObject> m_body;

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
        btVector3 m_linearFactor = {1.f, 1.f, 1.f};

        //! Angular factor
        btVector3 m_angularFactor = { 1.f, 1.f, 1.f };

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

        //! Collision filter mask
        int m_collisionFilterMask = -1;

        //! Cache activated status
        bool m_bIsActivated = false;

        //! Collision margin
        float m_collisionMargin = 0.025f;

        //! Cache previous scale value
        Vec3 m_previousScale = {1.f, 1.f, 1.f};

        //! Cache isSoftBody state
        bool m_bIsSoftBody = false;

        //! Cache dirty state
        bool m_bIsDirty = false;
    };
} // namespace ige::scene
