#include "components/physic/PhysicBase.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Initialize static members
    Event<PhysicBase &> PhysicBase::m_onCreatedEvent;
    Event<PhysicBase &> PhysicBase::m_onDestroyedEvent;
    Event<PhysicBase &> PhysicBase::m_onActivatedEvent;
    Event<PhysicBase &> PhysicBase::m_onDeactivatedEvent;

    //! Constructor
    PhysicBase::PhysicBase(const std::shared_ptr<SceneObject> &owner)
        : Component(owner)
    {
        m_transform = owner->getTransform().get();
    }

    //! Destructor
    PhysicBase::~PhysicBase()
    {
        destroyBody();
        getOnDestroyedEvent().invoke(*this);
        m_transform = nullptr;
    }

    //! Initialization
    bool PhysicBase::init()
    {
        getOnCreatedEvent().invoke(*this);
        createBody();
        return true;
    }

    //! Set enable
    void PhysicBase::setEnabled(bool enable)
    {
        m_bIsEnabled = enable;
        m_body->clearForces();

        if (m_bIsEnabled)
            activate();
        else
            deactivate();
    }

    //! Set mass
    void PhysicBase::setMass(float mass)
    {
        m_mass = mass;
        btVector3 inertia = {0.f, 0.f, 0.f};
        if (m_mass != 0.0f)
            m_shape->calculateLocalInertia(m_mass, inertia);
        m_body->setMassProps(mass, inertia);
        m_body->updateInertiaTensor();
    }

    //! Set linear velocity
    void PhysicBase::setLinearVelocity(const btVector3 &velocity)
    {
        m_linearVelocity = velocity;
        m_body->setLinearFactor(m_linearVelocity);
    }

    //! Set angular velocity
    void PhysicBase::setAngularVelocity(const btVector3 &velocity)
    {
        m_angularVelocity = velocity;
        m_body->setAngularVelocity(m_angularVelocity);
    }

    //! Set linear factor
    void PhysicBase::setLinearFactor(const btVector3 &factor)
    {
        m_linearFactor = factor;
        m_body->setLinearFactor(m_linearFactor);
    }

    //! Set linear factor
    void PhysicBase::setAngularFactor(const btVector3 &factor)
    {
        m_angularFactor = factor;
        m_body->setAngularFactor(m_angularFactor);
    }

    //! Set is trigger
    void PhysicBase::setIsTrigger(bool isTrigger)
    {
        m_bIsTrigger = isTrigger;
        if (m_bIsTrigger)
            addCollisionFlag(btCollisionObject::CF_NO_CONTACT_RESPONSE);
        else
            removeCollisionFlag(btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    //! Set is kinematic
    void PhysicBase::setIsKinematic(bool isKinematic)
    {
        m_bIsKinematic = isKinematic;
        if (m_bIsKinematic)
        {
            clearForces();
            setLinearVelocity({0.f, 0.f, 0.f});
            setAngularVelocity({0.f, 0.f, 0.f});
        }
        recreateBody();
    }

    //! Apply torque
    void PhysicBase::applyTorque(const btVector3 &torque)
    {
        m_body->applyTorque(torque);
    }

    //! Apply force
    void PhysicBase::applyForce(const btVector3 &force)
    {
        m_body->applyCentralForce(force);
    }

    void PhysicBase::applyForce(const btVector3 &force, const btVector3 &pos)
    {
        m_body->applyForce(force, pos);
    }

    //! Apply impulse
    void PhysicBase::applyImpulse(const btVector3 &impulse)
    {
        m_body->applyCentralImpulse(impulse);
    }

    void PhysicBase::applyImpulse(const btVector3 &impulse, const btVector3 &pos)
    {
        m_body->applyImpulse(impulse, pos);
    }

    //! Clear forces
    void PhysicBase::clearForces()
    {
        m_body->clearForces();
    }

    //! Create physic body
    void PhysicBase::createBody()
    {
        m_motion = std::make_unique<btDefaultMotionState>(PhysicHelper::to_btTransform(*m_transform));
        m_body = std::make_unique<btRigidBody>(btRigidBody::btRigidBodyConstructionInfo{0.0f, m_motion.get(), m_shape.get(), btVector3(0.0f, 0.0f, 0.0f)});
        m_body->setRestitution(m_restitution);
        m_body->setFriction(m_friction);
        m_body->setLinearVelocity(m_linearVelocity);
        m_body->setAngularVelocity(m_angularVelocity);
        m_body->setLinearFactor(m_linearFactor);
        m_body->setAngularFactor(m_angularFactor);
        m_body->setUserPointer(this);

        applyInertia();

        if (m_bIsTrigger)
            addCollisionFlag(btCollisionObject::CF_NO_CONTACT_RESPONSE);

        if (m_bIsEnabled)
            activate();
    }

    //! Create physic body
    void PhysicBase::destroyBody()
    {
        deactivate();
        m_body.reset();
        m_motion.reset();
    }

    //! Calculate and apply inertia
    void PhysicBase::applyInertia()
    {
        btVector3 inertia = {0.f, 0.f, 0.f};
        if (m_mass != 0.0f)
            m_shape->calculateLocalInertia(m_mass, inertia);
        m_body->setMassProps(m_bIsKinematic ? 0.0f : std::max(0.0000001f, m_mass), m_bIsKinematic ? btVector3(0.0f, 0.0f, 0.0f) : inertia);
    }

    //! Add collision flag
    void PhysicBase::addCollisionFlag(btCollisionObject::CollisionFlags flag)
    {
        m_body->setCollisionFlags(m_body->getCollisionFlags() | flag);
    }

    //! Remove collision flag
    void PhysicBase::removeCollisionFlag(btCollisionObject::CollisionFlags flag)
    {
        m_body->setCollisionFlags(m_body->getCollisionFlags() & ~flag);
    }

    //! Activate
    void PhysicBase::activate()
    {
        if (!m_bIsActivated)
        {
            m_body->activate(true);
            getOnActivatedEvent().invoke(*this);
            m_bIsActivated = true;
        }
    }

    //! Deactivate
    void PhysicBase::deactivate()
    {
        if (m_bIsActivated)
        {
            getOnDeactivatedEvent().invoke(*this);
            m_body->activate(false);
            m_bIsActivated = false;
        }
    }

    //! Recreate Body
    void PhysicBase::recreateBody()
    {
        destroyBody();
        createBody();
    }

    //! Update Bullet transform
    void PhysicBase::updateBtTransform()
    {
        m_body->setWorldTransform(PhysicHelper::to_btTransform(*m_transform));

        Vec3 scale = m_transform->getWorldScale();
        Vec3 dScale = { scale[0] - m_previousScale[0], scale[1] - m_previousScale[1], scale[2] - m_previousScale[2] };
        float scaleDelta = vmath_lengthSqr(dScale.P(), 3);
        if (scaleDelta >= 0.01f)
        {
            m_previousScale = scale;
            setLocalScale({std::abs(scale[0]), std::abs(scale[1]), std::abs(scale[2])});
            recreateBody();
        }
    }

    //! Update IGE transform
    void PhysicBase::updateIgeTransform()
    {
        if (!m_bIsKinematic)
        {
            const btTransform &result = m_body->getWorldTransform();
            m_transform->setPosition(PhysicHelper::from_btVector3(result.getOrigin()));
            m_transform->setRotation(PhysicHelper::from_btQuaternion(result.getRotation()));
        }
    }

    //! Get AABB
    void PhysicBase::getAABB(btVector3 &aabbMin, btVector3 aabbMax)
    {
        m_body->getAabb(aabbMin, aabbMax);
    }

    //! Serialize
    void PhysicBase::to_json(json &j) const
    {
        j = json{
            {"mass", m_mass},
            {"restitution", m_restitution},
            {"friction", m_friction},
            {"linearVelocity", PhysicHelper::from_btVector3(m_linearVelocity)},
            {"angularVelocity", PhysicHelper::from_btVector3(m_angularVelocity)},
            {"linearFactor", PhysicHelper::from_btVector3(m_linearFactor)},
            {"angularFactor", PhysicHelper::from_btVector3(m_angularFactor)},
            {"isKinematic", m_bIsKinematic},
            {"isTrigger", m_bIsTrigger},
            {"isEnabled", m_bIsEnabled},
            {"scale", m_previousScale},
        };
    }

    //! Deserialize
    void PhysicBase::from_json(const json &j)
    {
        setMass(j.at("mass"));
        setRestitution(j.at("restitution"));
        setFriction(j.at("friction"));
        setLinearVelocity(PhysicHelper::to_btVector3(j.at("linearVelocity")));
        setAngularVelocity(PhysicHelper::to_btVector3(j.at("angularVelocity")));
        setLinearFactor(PhysicHelper::to_btVector3(j.at("linearFactor")));
        setAngularFactor(PhysicHelper::to_btVector3(j.at("angularFactor")));
        setIsKinematic(j.at("isKinematic"));
        setIsTrigger(j.at("isTrigger"));
        setEnabled(j.at("isEnabled"));
    }
} // namespace ige::scene