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
    PhysicBase::PhysicBase(SceneObject &owner)
        : Component(owner)
    {
        m_transform = getOwner()->getTransform().get();
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

    //! Set enable
    void PhysicBase::setCCD(bool isCCD)
    {
        if (m_bIsCCD != isCCD)
        {
            m_bIsCCD = isCCD;
            if (m_bIsCCD)
            {
                // Continous detection
                m_body->setCcdMotionThreshold(static_cast<btScalar>(0.001f));
                m_body->setCcdSweptSphereRadius(0.5f);
            }
            else
            {
                // Discrete detection
                m_body->setCcdMotionThreshold(std::numeric_limits<float>::max());
                m_body->setCcdSweptSphereRadius(0.0f);
            }
        }
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

        // Apply collision filter group and mask
        if (m_bIsKinematic)
        {
            addCollisionFlag(btCollisionObject::CF_KINEMATIC_OBJECT);
            m_collisionFilterGroup = 2;
            m_collisionFilterMask = 3;
        }
        else
        {
            m_collisionFilterGroup = 1;
            m_collisionFilterMask = -1;
        }

        if (m_body->getBroadphaseHandle())
        {
            m_body->getBroadphaseHandle()->m_collisionFilterGroup = m_collisionFilterGroup;
            m_body->getBroadphaseHandle()->m_collisionFilterMask = m_collisionFilterMask;
        }

        // Continous detection mode
        if (m_bIsCCD)
        {
            // Continous detection
            m_body->setCcdMotionThreshold(static_cast<btScalar>(0.001f));
            m_body->setCcdSweptSphereRadius(0.5f);
        }
        else
        {
            // Discrete detection
            m_body->setCcdMotionThreshold(std::numeric_limits<float>::max());
            m_body->setCcdSweptSphereRadius(0.0f);
        }

        // Apply inertia
        applyInertia();

        // Add custom material callback for collision events
        addCollisionFlag(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        // Add trigger flag
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
        if (m_bIsKinematic)
        {
            m_body->setMassProps(0.0f, btVector3(0.0f, 0.0f, 0.0f));
        }
        else
        {
            btVector3 inertia = { 0.f, 0.f, 0.f };
            if (m_mass != 0.0f)
                m_shape->calculateLocalInertia(m_mass, inertia);
            m_body->setMassProps(std::max(0.0000001f, m_mass), inertia);
        }
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

    //! Collision filter group
    void PhysicBase::setCollisionFilterGroup(int group)
    {
        if (m_collisionFilterGroup != group)
        {
            m_collisionFilterGroup = group;
            if (m_body && m_body->getBroadphaseHandle())
                m_body->getBroadphaseHandle()->m_collisionFilterGroup = m_collisionFilterGroup;
        }
    }

    //! Collision filter mask
    void PhysicBase::setCollisionFilterMask(int mask)
    {
        if (m_collisionFilterMask != mask)
        {
            m_collisionFilterMask = mask;
            if (m_body && m_body->getBroadphaseHandle())
                m_body->getBroadphaseHandle()->m_collisionFilterMask = m_collisionFilterMask;
        }
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
        m_body->setWorldTransform(PhysicHelper::to_btTransform(m_transform->getWorldRotation(), m_transform->getWorldPosition() + m_positionOffset));

        Vec3 scale = m_transform->getWorldScale();
        Vec3 dScale = {scale[0] - m_previousScale[0], scale[1] - m_previousScale[1], scale[2] - m_previousScale[2]};
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
            m_transform->setPosition(PhysicHelper::from_btVector3(result.getOrigin()) - m_positionOffset);
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
            {"offset", m_positionOffset},
            {"isKinematic", m_bIsKinematic},
            {"isTrigger", m_bIsTrigger},
            {"isEnabled", m_bIsEnabled},
            {"scale", m_previousScale},
            {"group", m_collisionFilterGroup},
            {"mask", m_collisionFilterMask},
            {"ccd", m_bIsCCD},
        };
    }

    //! Deserialize
    void PhysicBase::from_json(const json &j)
    {
        setMass(j.value("mass", 1.f));
        setRestitution(j.value("restitution", 1.f));
        setFriction(j.value("friction", 0.5f));
        setLinearVelocity(PhysicHelper::to_btVector3(j.value("linearVelocity", Vec3())));
        setAngularVelocity(PhysicHelper::to_btVector3(j.value("angularVelocity", Vec3())));
        setLinearFactor(PhysicHelper::to_btVector3(j.value("linearFactor", Vec3(1.f, 1.f, 1.f))));
        setAngularFactor(PhysicHelper::to_btVector3(j.value("angularFactor", Vec3(1.f, 1.f, 1.f))));
        setPositionOffset(j.value("offset", Vec3()));
        setIsKinematic(j.value("isKinematic", false));
        setIsTrigger(j.value("isTrigger", false));
        setEnabled(j.value("isEnabled", true));
        setLocalScale(j.value("scale", Vec3(1.f, 1.f, 1.f)));
        setCollisionFilterGroup(j.value("group", isKinematic() ? 2 : 1));
        setCollisionFilterMask(j.value("mask", isKinematic() ? 3 : -1));
        setCCD(j.value("ccd", false));
    }
} // namespace ige::scene