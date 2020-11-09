#include "components/physic/PhysicObject.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Initialize static members
    Event<PhysicObject*> PhysicObject::m_onCreatedEvent;
    Event<PhysicObject*> PhysicObject::m_onDestroyedEvent;
    Event<PhysicObject*> PhysicObject::m_onActivatedEvent;
    Event<PhysicObject*> PhysicObject::m_onDeactivatedEvent;

    //! Constructor
    PhysicObject::PhysicObject(SceneObject &owner)
        : Component(owner)
    {}

    //! Destructor
    PhysicObject::~PhysicObject()
    {
        destroy();
    }

    //! Initialization
    bool PhysicObject::init()
    {
        getOnCreatedEvent().invoke(this);
        createBody();
        return true;
    }

    //! Initialization
    bool PhysicObject::destroy()
    {
        destroyBody();
        getOnDestroyedEvent().invoke(this);
        return true;
    }

    //! Add constraint
    void PhysicObject::addConstraint(const std::shared_ptr<PhysicConstraint>& constraint)
    {
        m_constraints.push_back(constraint);
    }

    //! Remove constraint
    void PhysicObject::removeConstraint(const std::shared_ptr<PhysicConstraint>& constraint)
    {
        auto found = std::find_if(m_constraints.begin(), m_constraints.end(), [&constraint](const auto& element)
        {
            return constraint == element;
        });

        if (found != m_constraints.end())
        {
            m_constraints.erase(found);
        }
    }

    //! Set enable
    void PhysicObject::setEnabled(bool enable)
    {
        if (m_bIsEnabled != enable)
        {
            m_bIsEnabled = enable;
            if (m_bIsEnabled)
                activate();
            else
                deactivate();
        }
    }

    //! Set enable
    void PhysicObject::setCCD(bool isCCD)
    {
        m_bIsCCD = isCCD;
        if (m_bIsCCD)
        {
            // Continuos detection
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

    //! Set mass
    void PhysicObject::setMass(float mass)
    {
        if (m_bIsDirty || m_mass != mass)
        {
            m_mass = mass;
            btVector3 inertia = { 0.f, 0.f, 0.f };
            if (m_mass != 0.0f)
                m_shape->calculateLocalInertia(m_mass, inertia);
            getBody()->setMassProps(mass, inertia);
            getBody()->updateInertiaTensor();
        }
    }

    void PhysicObject::setFriction(float friction)
    {
        if (m_bIsDirty || m_friction != friction)
        {
            m_friction = friction;
            m_body->setFriction(m_friction);
        }
    }

    void PhysicObject::setRestitution(float restitution)
    {
        if (m_bIsDirty || m_restitution != restitution)
        {
            m_restitution = restitution;
            m_body->setRestitution(m_restitution);
        }
    }

    void PhysicObject::setLinearVelocity(const btVector3& velocity)
    {
        if (m_bIsDirty || m_linearVelocity != velocity)
        {
            m_linearVelocity = velocity;
            getBody()->setLinearVelocity(m_linearVelocity);
        }
    }

    void PhysicObject::setAngularVelocity(const btVector3& velocity)
    {
        if (m_bIsDirty || m_angularVelocity != velocity)
        {
            m_angularVelocity = velocity;
            getBody()->setAngularVelocity(m_angularVelocity);
        }
    }

    void PhysicObject::setLinearFactor(const btVector3& factor)
    {
        if (m_bIsDirty || m_linearFactor != factor)
        {
            m_linearFactor = factor;
            if (getBody())
                getBody()->setLinearFactor(m_linearFactor);
        }
    }

    void PhysicObject::setAngularFactor(const btVector3& factor)
    {
        if (m_bIsDirty || m_angularFactor != factor)
        {
            m_angularFactor = factor;
            if (getBody())
                getBody()->setAngularFactor(m_angularFactor);
        }
    }

    void PhysicObject::setCollisionMargin(float margin)
    {
        if (m_bIsDirty || m_collisionMargin != margin)
        {
            m_collisionMargin = margin;
            m_body->getCollisionShape()->setMargin(m_collisionMargin);
        }
    }

    //! Set is trigger
    void PhysicObject::setIsTrigger(bool isTrigger)
    {
        if (m_bIsDirty || m_bIsTrigger != isTrigger)
        {
            m_bIsTrigger = isTrigger;
            if (m_bIsTrigger)
                addCollisionFlag(btCollisionObject::CF_NO_CONTACT_RESPONSE);
            else
                removeCollisionFlag(btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
    }

    //! Set is kinematic
    void PhysicObject::setIsKinematic(bool isKinematic)
    {
        if(m_bIsDirty || m_bIsKinematic != isKinematic)
        {
            m_bIsKinematic = isKinematic;
            if (m_bIsKinematic)
            {
                setMass(0.f);
                clearForces();
                setLinearVelocity({ 0.f, 0.f, 0.f });
                setAngularVelocity({ 0.f, 0.f, 0.f });

                addCollisionFlag(btCollisionObject::CF_KINEMATIC_OBJECT);
                m_collisionFilterGroup = 2;
                m_collisionFilterMask = 3;
            }
            else
            {
                removeCollisionFlag(btCollisionObject::CF_KINEMATIC_OBJECT);
                m_collisionFilterGroup = 1;
                m_collisionFilterMask = -1;
            }

            if (m_body->getBroadphaseHandle())
            {
                m_body->getBroadphaseHandle()->m_collisionFilterGroup = m_collisionFilterGroup;
                m_body->getBroadphaseHandle()->m_collisionFilterMask = m_collisionFilterMask;
            }
        }
    }

    //! Set local scale
    void PhysicObject::setLocalScale(const Vec3& scale)
    {
        if (m_bIsDirty || m_previousScale != scale)
        {
            m_previousScale = scale;
            if (m_shape)
                m_shape->setLocalScaling(PhysicHelper::to_btVector3(m_previousScale));
        }
    }

    //! Create physic body
    void PhysicObject::createBody()
    {
        m_motion = std::make_unique<btDefaultMotionState>(PhysicHelper::to_btTransform(*(getOwner()->getTransform())));
        m_body = std::make_unique<btRigidBody>(btRigidBody::btRigidBodyConstructionInfo{0.0f, m_motion.get(), m_shape.get(), btVector3(0.0f, 0.0f, 0.0f)});
        m_body->setUserPointer(this);

        m_bIsDirty = true;

        // Apply pre-configurated values
        setMass(m_mass);
        setFriction(m_friction);
        setRestitution(m_restitution);
        setLinearVelocity(m_linearVelocity);
        setAngularVelocity(m_angularVelocity);
        setLinearFactor(m_linearFactor);
        setAngularFactor(m_angularFactor);
        setCollisionMargin(m_collisionMargin);

        // Continuos detection mode
        setCCD(m_bIsCCD);

        // Apply inertia
        applyInertia();

        // Add custom material callback for collision events
        addCollisionFlag(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        // Add trigger flag
        if (m_bIsTrigger)
            addCollisionFlag(btCollisionObject::CF_NO_CONTACT_RESPONSE);

        m_bIsDirty = false;

        if (m_bIsEnabled)
            activate();
    }

    //! Create physic body
    void PhysicObject::destroyBody()
    {
        deactivate();
        if(m_body) m_body.reset();
        if(m_motion) m_motion.reset();
    }

    //! Calculate and apply inertia
    void PhysicObject::applyInertia()
    {
        if (getBody())
        {
            if (m_bIsKinematic)
            {
                getBody()->setMassProps(0.0f, btVector3(0.0f, 0.0f, 0.0f));
            }
            else
            {
                btVector3 inertia = { 0.f, 0.f, 0.f };
                if (m_mass != 0.0f)
                    m_shape->calculateLocalInertia(m_mass, inertia);
                getBody()->setMassProps(std::max(0.0000001f, m_mass), inertia);
            }
        }
    }

    //! Collision filter group
    void PhysicObject::setCollisionFilterGroup(int group)
    {
        if (m_collisionFilterGroup != group)
        {
            m_collisionFilterGroup = group;
            if (m_body->getBroadphaseHandle())
                m_body->getBroadphaseHandle()->m_collisionFilterGroup = m_collisionFilterGroup;
        }
    }

    //! Collision filter mask
    void PhysicObject::setCollisionFilterMask(int mask)
    {
        if (m_collisionFilterMask != mask)
        {
            m_collisionFilterMask = mask;
            if (m_body->getBroadphaseHandle())
                m_body->getBroadphaseHandle()->m_collisionFilterMask = m_collisionFilterMask;
        }
    }

    //! Activate
    void PhysicObject::activate()
    {
        if (!m_bIsActivated)
        {
            getOnActivatedEvent().invoke(this);
            m_body->activate(true);
            m_bIsActivated = true;
        }
    }

    //! Deactivate
    void PhysicObject::deactivate()
    {
        if (m_bIsActivated)
        {
            getOnDeactivatedEvent().invoke(this);
            m_body->activate(false);
            m_bIsActivated = false;
        }
    }

    //! Recreate Body
    void PhysicObject::recreateBody()
    {
        destroyBody();
        createBody();
    }

    //! Update Bullet transform
    void PhysicObject::updateBtTransform()
    {
        m_body->setWorldTransform(PhysicHelper::to_btTransform(getOwner()->getTransform()->getWorldRotation(), getOwner()->getTransform()->getWorldPosition()));

        Vec3 scale = getOwner()->getTransform()->getWorldScale();
        Vec3 dScale = {scale[0] - m_previousScale[0], scale[1] - m_previousScale[1], scale[2] - m_previousScale[2]};
        float scaleDelta = vmath_lengthSqr(dScale.P(), 3);
        if (scaleDelta >= 0.01f)
        {
            setLocalScale({std::abs(scale[0]), std::abs(scale[1]), std::abs(scale[2])});
            recreateBody();
        }
    }

    //! Update IGE transform
    void PhysicObject::updateIgeTransform()
    {
        if (!m_bIsKinematic)
        {
            const btTransform &result = m_body->getWorldTransform();
            getOwner()->getTransform()->setPosition(PhysicHelper::from_btVector3(result.getOrigin()));
            getOwner()->getTransform()->setRotation(PhysicHelper::from_btQuaternion(result.getRotation()));
            getOwner()->getTransform()->onUpdate(0.f);
        }
    }

    //! Get AABB
    void PhysicObject::getAABB(btVector3 &aabbMin, btVector3 aabbMax)
    {
        getBody()->getAabb(aabbMin, aabbMax);
    }

    //! Serialize
    void PhysicObject::to_json(json &j) const
    {
        j = json{
            {"mass", getMass()},
            {"restitution", getRestitution()},
            {"friction", getFriction()},
            {"linearVelocity", PhysicHelper::from_btVector3(getLinearVelocity())},
            {"angularVelocity", PhysicHelper::from_btVector3(getAngularVelocity())},
            {"linearFactor", PhysicHelper::from_btVector3(getLinearFactor())},
            {"angularFactor", PhysicHelper::from_btVector3(getAngularFactor())},
            {"isKinematic", isKinematic()},
            {"isTrigger", isTrigger()},
            {"isEnabled", isEnabled()},
            {"scale", getLocalScale()},
            {"group", getCollisionFilterGroup()},
            {"mask", getCollisionFilterMask()},
            {"ccd", isCCD()},
            {"margin", getCollisionMargin()},
        };
    }

    //! Deserialize
    void PhysicObject::from_json(const json &j)
    {
        setMass(j.value("mass", 1.f));
        setRestitution(j.value("restitution", 1.f));
        setFriction(j.value("friction", 0.5f));
        setLinearVelocity(PhysicHelper::to_btVector3(j.value("linearVelocity", Vec3())));
        setAngularVelocity(PhysicHelper::to_btVector3(j.value("angularVelocity", Vec3())));
        setLinearFactor(PhysicHelper::to_btVector3(j.value("linearFactor", Vec3(1.f, 1.f, 1.f))));
        setAngularFactor(PhysicHelper::to_btVector3(j.value("angularFactor", Vec3(1.f, 1.f, 1.f))));
        setIsKinematic(j.value("isKinematic", false));
        setIsTrigger(j.value("isTrigger", false));
        setEnabled(j.value("isEnabled", true));
        setLocalScale(j.value("scale", Vec3(1.f, 1.f, 1.f)));
        setCollisionFilterGroup(j.value("group", isKinematic() ? 2 : 1));
        setCollisionFilterMask(j.value("mask", isKinematic() ? 3 : -1));
        setCCD(j.value("ccd", false));
        setCollisionMargin(j.value("margin", 0.025f));
    }
} // namespace ige::scene