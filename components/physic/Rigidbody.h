#pragma once

#include "event/Event.h"
#include "components/Component.h"
#include "components/physic/Collider.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftBody.h>

namespace ige::scene
{
    class PhysicConstraint;
    class PhysicManager;

    //! Rigidbody
    class Rigidbody : public Component
    {
    public:
        //! Constructor
        Rigidbody(SceneObject& owner);

        //! Destructor
        virtual ~Rigidbody();

        //! Get name
        std::string getName() const override { return "Rigidbody"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::Rigidbody; }

        //! Cache PhysicManager
        std::shared_ptr<PhysicManager> getManager() const { return m_manager.expired() ? nullptr : m_manager.lock(); }
        void setManager(std::shared_ptr<PhysicManager> manager) { m_manager = manager; }

        //! Initialization
        virtual bool init();

        //! Deinitialization
        virtual bool destroy();

        //! Transform changed event
        virtual void onTransformChanged();

        //! Get RigidBody
        virtual btRigidBody* getBody() const {
            if (!m_body || isSoftBody())
                return nullptr;
            return (btRigidBody*)m_body.get();
        }

        //! Get SoftBody
        virtual btSoftBody* getSoftBody() const {
            if (!m_body || !isSoftBody())
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
        virtual const btVector3& getLinearVelocity() const { return m_linearVelocity; }
        virtual void setLinearVelocity(const btVector3& velocity);

        //! Angular velocity
        virtual const btVector3& getAngularVelocity() const { return m_angularVelocity; }
        virtual void setAngularVelocity(const btVector3& velocity);

        //! Linear factor
        virtual const btVector3& getLinearFactor() const { return m_linearFactor; }
        virtual void setLinearFactor(const btVector3& factor);

        //! Angular factor
        virtual const btVector3& getAngularFactor() const { return m_angularFactor; }
        virtual void setAngularFactor(const btVector3& factor);

        //! Linear Sleeping Threshold
        virtual float getLinearSleepingThreshold() const { return m_linearSleepingThreshold; }
        virtual void setLinearSleepingThreshold(float value);

        //! Angular Sleeping Threshold
        virtual float getAngularSleepingThreshold() const { return m_angularSleepingThreshold; }
        virtual void setAngularSleepingThreshold(float value);

        virtual int getActivationState() const;
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

        //! Enable/Disable gravity
        virtual bool isGravityEnabled() const { return m_bEnableGravity; }
        virtual void setGravityEnabled(bool enable = true);

        //! Get AABB
        virtual AABBox getAABB();

        //! Add collision flag
        virtual void addCollisionFlag(int flag) {
            if(m_body) m_body->setCollisionFlags(m_body->getCollisionFlags() | flag);
        }

        //! Remove collision flag
        virtual void removeCollisionFlag(int flag) {
            if (m_body) m_body->setCollisionFlags(m_body->getCollisionFlags() & ~flag);
        }

        //! Collision filter group
        virtual int getCollisionFilterGroup() const { return m_collisionFilterGroup; }
        virtual void setCollisionFilterGroup(int group);

        //! Collision filter mask
        virtual int getCollisionFilterMask() const { return m_collisionFilterMask; }
        virtual void setCollisionFilterMask(int mask);

        //! Position offset
        const Vec3& getPositionOffset() const { return m_positionOffset; }
        void setPositionOffset(const Vec3& offset);

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

        //! Create physic body
        virtual void createBody();

        //! Destroy physic body
        virtual void destroyBody();

        //! Recreate Body
        virtual void recreateBody() { createBody(); }

    public:
        //! Apply torque
        virtual void applyTorque(const btVector3& torque) { if (getBody()) getBody()->applyTorque(torque); }

        //! Apply Torque Impulse
        virtual void applyTorqueImpulse(const btVector3& torque) { if (getBody()) getBody()->applyTorqueImpulse(torque); }

        //! Apply force
        virtual void applyForce(const btVector3& force) { if (getBody()) getBody()->applyCentralForce(force); }
        virtual void applyForce(const btVector3& force, const btVector3& pos) { getBody()->applyForce(force, pos); }

        //! Apply impulse
        virtual void applyImpulse(const btVector3& impulse) { if (getBody()) getBody()->applyCentralImpulse(impulse); }
        virtual void applyImpulse(const btVector3& impulse, const btVector3& pos) { if (getBody()) getBody()->applyImpulse(impulse, pos); }

        //! Move Ridgidbody
        virtual void movePosition(const btVector3& pos);
        virtual void moveRotation(const btQuaternion& quat);

        //! Clear forces
        virtual void clearForces() { if (getBody()) getBody()->clearForces(); }

        //! Update Bullet transform
        virtual void updateBtTransform();

        //! Update IGE transform
        virtual void updateIgeTransform();

        //! Get onCreatedEvent
        static Event<Rigidbody&>& getOnCreatedEvent() { return m_onCreatedEvent; }
        static Event<Rigidbody&>& getOnDestroyedEvent() { return m_onDestroyedEvent; }
        static Event<Rigidbody&>& getOnActivatedEvent() { return m_onActivatedEvent; }
        static Event<Rigidbody&>& getOnDeactivatedEvent() { return m_onDeactivatedEvent; }

        //! Collision events
        Event<Rigidbody*>& getCollisionStartEvent() { return m_collisionStartEvent; }
        Event<Rigidbody*>& getCollisionStayEvent() { return m_collisionStayEvent; }
        Event<Rigidbody*>& getCollisionStopEvent() { return m_collisionStopEvent; }

        //! Trigger events
        Event<Rigidbody*>& getTriggerStartEvent() { return m_triggerStartEvent; }
        Event<Rigidbody*>& getTriggerStayEvent() { return m_triggerStayEvent; }
        Event<Rigidbody*>& getTriggerStopEvent() { return m_triggerStopEvent; }

        //! Serialize finished event
        virtual void onSerializeFinished() override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Calculate and apply inertia
        virtual void applyInertia();

        //! Activate
        virtual void activate();

        //! Deactivate
        virtual void deactivate();

        //! Is SoftBody
        bool isSoftBody() const { return m_bIsSoftBody; }
        void setSoftBody(bool isSoftBody) { m_bIsSoftBody = isSoftBody; }

    protected:
        //! Events
        static Event<Rigidbody&> m_onCreatedEvent;
        static Event<Rigidbody&> m_onDestroyedEvent;
        static Event<Rigidbody&> m_onActivatedEvent;
        static Event<Rigidbody&> m_onDeactivatedEvent;

        //! Collision events
        Event<Rigidbody*> m_collisionStartEvent;
        Event<Rigidbody*> m_collisionStayEvent;
        Event<Rigidbody*> m_collisionStopEvent;

        //! Trigger events
        Event<Rigidbody*> m_triggerStartEvent;
        Event<Rigidbody*> m_triggerStayEvent;
        Event<Rigidbody*> m_triggerStopEvent;

    protected:
        //! Collision shape
        std::weak_ptr<Collider> m_collider;

        //! Rigid Body
        std::unique_ptr<btCollisionObject> m_body;

        //! Motion State
        std::unique_ptr<btMotionState> m_motion;

        //! Mass
        float m_mass = 1.f;

        //! Restuitution
        float m_restitution = 0.f;

        //! Friction
        float m_friction = 0.2f;

        //! Linear velocity
        btVector3 m_linearVelocity = {0.f, 0.f, 0.f};

        //! Angular velocity
        btVector3 m_angularVelocity = {0.f, 0.f, 0.f};

        //! Linear factor
        btVector3 m_linearFactor = {1.f, 1.f, 1.f};

        //! Angular factor
        btVector3 m_angularFactor = { 1.f, 1.f, 1.f };

        //! Linear sleeping threshold
        float m_linearSleepingThreshold = 0.f;

        //! Angular sleeping threshold
        float m_angularSleepingThreshold = 0.f;

        //! isKinematic
        bool m_bIsKinematic = false;

        //! isTrigger
        bool m_bIsTrigger = false;

        //! Continuous collision detection
        bool m_bIsCCD = true;

        //! Collision filter group
        int m_collisionFilterGroup = 1;

        //! Collision filter mask
        int m_collisionFilterMask = -1;

        //! Cache activated status
        bool m_bIsActivated = false;

        //! Associated constraints
        std::vector<std::shared_ptr<PhysicConstraint>> m_constraints;

        //! Cache previous scale value
        Vec3 m_previousScale = {1.f, 1.f, 1.f};

        //! Position offsets
        Vec3 m_positionOffset = { 0.f, 0.f, 0.f };

        //! Cache isSoftBody state
        bool m_bIsSoftBody = false;

        //! Cache dirty state
        bool m_bIsDirty = false;

        //! Cache activeState
        int m_activeState = 1;

        //! Enable gravity
        bool m_bEnableGravity = true;

        //! Cache PhysicManager
        std::weak_ptr<PhysicManager> m_manager;

        //! Cache serialize data
        json m_json;
    };

    //! Add constraint by type
    template <typename T, typename... Args>
    inline std::shared_ptr<T> Rigidbody::addConstraint(Args &&... args)
    {
        static_assert(std::is_base_of<PhysicConstraint, T>::value, "T should derive from PhysicConstraint");
        auto instance = std::make_shared<T>(*this, args...);
        m_constraints.push_back(instance);
        return instance;
    }

} // namespace ige::scene
