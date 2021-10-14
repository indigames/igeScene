#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftBody.h>

#include "event/Event.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"

namespace ige::scene
{
    class PhysicConstraint;
    class PhysicManager;

    //! PhysicObject
    class PhysicObject : public Component
    {
    public:
        //! Constructor
        PhysicObject(SceneObject &owner);

        //! Destructor
        virtual ~PhysicObject();

        //! Cache PhysicManager
        std::shared_ptr<PhysicManager> getManager() const { return m_manager.expired() ? nullptr : m_manager.lock(); }
        void setManager(std::shared_ptr<PhysicManager> manager) { m_manager = manager; }

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

        //! Linear Sleeping Threshold
        virtual float getLinearSleepingThreshold() const { return m_linearSleepingThreshold; }
        virtual void setLinearSleepingThreshold(float value);

        //! Angular Sleeping Threshold
        virtual float getAngularSleepingThreshold() const { return m_angularSleepingThreshold; }
        virtual void setAngularSleepingThreshold(float value);

        virtual int getActivationState();
        virtual void setActivationState(int state);

        //! Indicate object is a trigger object
        virtual bool isTrigger() const { return m_bIsTrigger; }
        virtual void setIsTrigger(bool isTrigger = true);

        //! Indicate object is a kinematic object
        virtual bool isKinematic() const { return m_bIsKinematic; }
        virtual void setIsKinematic(bool isKinematic = true);

        //! Enable/Disable physic component
        virtual void setEnabled(bool enable = true) override;

        //! Enable/Disable continuos collision detection
        virtual bool isCCD() const { return m_bIsCCD; }
        virtual void setCCD(bool isCCD = true);

        //! Get AABB
        virtual AABBox getAABB();

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

        //! Get contraints vector
        const std::vector<std::shared_ptr<PhysicConstraint>>& getContraints() const { return m_constraints; }

        //! Add constraint
        void addConstraint(const std::shared_ptr<PhysicConstraint>& constraint);

        //! Add constraint by type
        std::shared_ptr<PhysicConstraint> addConstraint(int type);

        //! Remove constraint
        void removeConstraint(const std::shared_ptr<PhysicConstraint>& constraint);

        //! Remove constraint by raw pointer
        void removeConstraint(PhysicConstraint* constraint);

        //! Remove all constraints
        void removeAllConstraints();

        //! Add constraint by type
        template <typename T, typename... Args>
        std::shared_ptr<T> addConstraint(Args &&... args);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

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

        //! Get onActivatedEvent
        static Event<PhysicObject*> &getOnActivatedEvent() { return m_onActivatedEvent; }

        //! Get onDeactivatedEvent
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

        //! Linear sleeping threshold
        float m_linearSleepingThreshold = 0.8f;

        //! Angular sleeping threshold
        float m_angularSleepingThreshold = 1.0f;

        //! isKinematic
        bool m_bIsKinematic = false;

        //! isTrigger
        bool m_bIsTrigger = false;

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

        //! Associated constraints
        std::vector<std::shared_ptr<PhysicConstraint>> m_constraints;

        //! Cache previous scale value
        Vec3 m_previousScale = {1.f, 1.f, 1.f};

        //! Cache isSoftBody state
        bool m_bIsSoftBody = false;

        //! Cache dirty state
        bool m_bIsDirty = false;

        //! Cache PhysicManager
        std::weak_ptr<PhysicManager> m_manager;
    };

    //! Add constraint by type
    template <typename T, typename... Args>
    inline std::shared_ptr<T> PhysicObject::addConstraint(Args &&... args)
    {
        static_assert(std::is_base_of<PhysicConstraint, T>::value, "T should derive from PhysicConstraint");
        auto instance = std::make_shared<T>(*this, args...);
        m_constraints.push_back(instance);
        return instance;
    }

} // namespace ige::scene
