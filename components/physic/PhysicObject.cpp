#include "components/physic/PhysicObject.h"
#include "components/physic/PhysicManager.h"
#include "components/physic/PhysicConstraint.h"
#include "components/physic/FixedConstraint.h"
#include "components/physic/HingeConstraint.h"
#include "components/physic/SliderConstraint.h"
#include "components/physic/SpringConstraint.h"
#include "components/physic/Dof6SpringConstraint.h"
#include "components/TransformComponent.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Initialize static members
    Event<PhysicObject *> PhysicObject::m_onCreatedEvent;
    Event<PhysicObject *> PhysicObject::m_onDestroyedEvent;
    Event<PhysicObject *> PhysicObject::m_onActivatedEvent;
    Event<PhysicObject *> PhysicObject::m_onDeactivatedEvent;

    //! Constructor
    PhysicObject::PhysicObject(SceneObject &owner)
        : Component(owner)
    {
        // Register manager
        if (!getOwner()->getScene()->isPrefab() && getOwner()->getRoot())
        {
            auto manager = getOwner()->getRoot()->getComponent<PhysicManager>();
            if (manager == nullptr)
                manager = getOwner()->getRoot()->addComponent<PhysicManager>();
            setManager(manager);
        }
    }

    //! Destructor
    PhysicObject::~PhysicObject()
    {
        destroy();
        m_manager.reset();
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
        // Deactivate
        deactivate();

        // Notify destroyed
        getOnDestroyedEvent().invoke(this);

        // Remove all constraints
        removeAllConstraints();

        // Destroy body
        destroyBody();

        return true;
    }

    //! Add constraint
    void PhysicObject::addConstraint(const std::shared_ptr<PhysicConstraint> &constraint)
    {
        m_constraints.push_back(constraint);
    }

    //! Add constraint
    std::shared_ptr<PhysicConstraint> PhysicObject::addConstraint(int type)
    {
        switch (type)
        {
        case (int)PhysicConstraint::ConstraintType::Fixed:
            return addConstraint<FixedConstraint>();
        case (int)PhysicConstraint::ConstraintType::Hinge:
            return addConstraint<HingeConstraint>();
        case (int)PhysicConstraint::ConstraintType::Slider:
            return addConstraint<SliderConstraint>();
        case (int)PhysicConstraint::ConstraintType::Spring:
            return addConstraint<SpringConstraint>();
        case (int)PhysicConstraint::ConstraintType::Dof6Spring:
            return addConstraint<Dof6SpringConstraint>();
        }
        return nullptr;
    }

    //! Remove constraint
    void PhysicObject::removeConstraint(const std::shared_ptr<PhysicConstraint> &constraint)
    {
        auto found = std::find_if(m_constraints.begin(), m_constraints.end(), [&constraint](const auto &element) {
            return constraint == element;
        });

        if (found != m_constraints.end())
        {
            m_constraints.erase(found);
        }
    }

    //! Remove constraint
    void PhysicObject::removeConstraint(PhysicConstraint *constraint)
    {
        auto found = std::find_if(m_constraints.begin(), m_constraints.end(), [&constraint](const auto &element) {
            return constraint == element.get();
        });

        if (found != m_constraints.end())
        {
            m_constraints.erase(found);
        }
    }

    //! Remove all constraints
    void PhysicObject::removeAllConstraints()
    {
        for (auto &constraint : m_constraints)
            constraint = nullptr;
        m_constraints.clear();
    }

    //! Set enable
    void PhysicObject::setEnabled(bool enable)
    {
        Component::setEnabled(enable);
        if (isEnabled())
            activate();
        else
            deactivate();
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
            btVector3 inertia = {0.f, 0.f, 0.f};
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

    void PhysicObject::setLinearVelocity(const btVector3 &velocity)
    {
        if (m_bIsDirty || m_linearVelocity != velocity)
        {
            m_linearVelocity = velocity;
            getBody()->setLinearVelocity(m_linearVelocity);
        }
    }

    void PhysicObject::setAngularVelocity(const btVector3 &velocity)
    {
        if (m_bIsDirty || m_angularVelocity != velocity)
        {
            m_angularVelocity = velocity;
            getBody()->setAngularVelocity(m_angularVelocity);
        }
    }

    void PhysicObject::setLinearFactor(const btVector3 &factor)
    {
        if (m_bIsDirty || m_linearFactor != factor)
        {
            m_linearFactor = factor;
            if (getBody())
                getBody()->setLinearFactor(m_linearFactor);
        }
    }

    void PhysicObject::setAngularFactor(const btVector3 &factor)
    {
        if (m_bIsDirty || m_angularFactor != factor)
        {
            m_angularFactor = factor;
            if (getBody())
                getBody()->setAngularFactor(m_angularFactor);
        }
    }

    void PhysicObject::setLinearSleepingThreshold(float value) {
        if (m_bIsDirty || m_linearSleepingThreshold != value)
        {
            m_linearSleepingThreshold = value;
            if (getBody())
                getBody()->setSleepingThresholds(m_linearSleepingThreshold, m_angularSleepingThreshold);
        }
    }

    void PhysicObject::setAngularSleepingThreshold(float value) {
        if (m_bIsDirty || m_angularSleepingThreshold != value)
        {
            m_angularSleepingThreshold = value;
            if (getBody())
                getBody()->setSleepingThresholds(m_linearSleepingThreshold, m_angularSleepingThreshold);
        }
    }

    int PhysicObject::getActivationState() const {
        return m_activeState;
    }

    void PhysicObject::setActivationState(int state) {
        m_activeState = MATH_CLAMP(state, 0, 5);
        if (getBody())
            getBody()->setActivationState(m_activeState);
    }

    void PhysicObject::setCollisionMargin(float margin)
    {
        if (m_bIsDirty || m_collisionMargin != margin)
        {
            m_collisionMargin = margin;
            m_body->getCollisionShape()->setMargin(m_collisionMargin);
        }
    }

    void PhysicObject::setPositionOffset(const Vec3& offset)
    {
        if(m_positionOffset != offset) {
            m_positionOffset = offset;
            updateBtTransform();
            getOwner()->updateAabb();
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
        if (m_bIsDirty || m_bIsKinematic != isKinematic)
        {
            if (m_bIsKinematic != isKinematic)
            {
                m_bIsKinematic = isKinematic;
                recreateBody();
            }
            if (m_bIsKinematic)
            {
                setMass(0.f);
                clearForces();
                setLinearVelocity({0.f, 0.f, 0.f});
                setAngularVelocity({0.f, 0.f, 0.f});

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
    void PhysicObject::setLocalScale(const Vec3 &scale)
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

        // Apply pre-configured values
        setMass(getMass());
        setFriction(getFriction());
        setRestitution(getRestitution());
        setLinearVelocity(getLinearVelocity());
        setAngularVelocity(getAngularVelocity());
        setLinearFactor(getLinearFactor());
        setAngularFactor(getAngularFactor());
        setIsKinematic(isKinematic());
        setIsTrigger(isTrigger());
        setCollisionFilterGroup(getCollisionFilterGroup());
        setCollisionFilterMask(getCollisionFilterMask());
        setCCD(isCCD());
        setCollisionMargin(getCollisionMargin());
        setLinearSleepingThreshold(getLinearSleepingThreshold());
        setAngularSleepingThreshold(getAngularSleepingThreshold());
        setActivationState(getActivationState());

        // Apply inertia
        applyInertia();

        // Add custom material callback for collision events
        addCollisionFlag(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        // Add trigger flag
        if (isTrigger())
            addCollisionFlag(btCollisionObject::CF_NO_CONTACT_RESPONSE);

        m_bIsDirty = false;

        if (isEnabled())
            activate();
    }

    //! Create physic body
    void PhysicObject::destroyBody()
    {
        deactivate();
        if (m_body)
            m_body.reset();
        if (m_motion)
            m_motion.reset();
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
                btVector3 inertia = {0.f, 0.f, 0.f};
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
        m_body->setWorldTransform(PhysicHelper::to_btTransform(getOwner()->getTransform()->getRotation(), getOwner()->getTransform()->getPosition() + m_positionOffset));

        Vec3 scale = getOwner()->getTransform()->getScale();
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
            getOwner()->getTransform()->setPosition(PhysicHelper::from_btVector3(result.getOrigin()) - m_positionOffset);
            getOwner()->getTransform()->setRotation(PhysicHelper::from_btQuaternion(result.getRotation()));
        }
    }

    //! Get AABB
    AABBox PhysicObject::getAABB()
    {
        if (!getBody()) return {};
        btVector3 aabbMin, aabbMax;
        getBody()->getAabb(aabbMin, aabbMax);
        auto box = AABBox(PhysicHelper::from_btVector3(aabbMin), PhysicHelper::from_btVector3(aabbMax));
        return box.Transform(getOwner()->getTransform()->getWorldMatrix().Inverse());
    }

    //! Serialize
    void PhysicObject::to_json(json &j) const
    {
        Component::to_json(j);
        j["mass"] = getMass();
        j["restitution"] = getRestitution();
        j["friction"] = getFriction();
        j["linearVelocity"] = PhysicHelper::from_btVector3(getLinearVelocity());
        j["angularVelocity"] = PhysicHelper::from_btVector3(getAngularVelocity());
        j["linearFactor"] = PhysicHelper::from_btVector3(getLinearFactor());
        j["angularFactor"] = PhysicHelper::from_btVector3(getAngularFactor());
        j["isKinematic"] = isKinematic();
        j["isTrigger"] = isTrigger();
        j["scale"] = getLocalScale();
        j["group"] = getCollisionFilterGroup();
        j["mask"] = getCollisionFilterMask();
        j["ccd"] = isCCD();
        j["margin"] = getCollisionMargin();
        j["linearSleepingThreshold"] = getLinearSleepingThreshold();
        j["angularSleepingThreshold"] = getAngularSleepingThreshold();
        j["activeState"] = getActivationState();
        j["offset"] = getPositionOffset();

        auto jConstraints = json::array();
        for (const auto &constraint : m_constraints)
            jConstraints.push_back({(int)constraint->getType(), json(*constraint.get())});
        j["consts"] = jConstraints;
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
        setLocalScale(j.value("scale", Vec3(1.f, 1.f, 1.f)));
        setCollisionFilterGroup(j.value("group", isKinematic() ? 2 : 1));
        setCollisionFilterMask(j.value("mask", isKinematic() ? 3 : -1));
        setCCD(j.value("ccd", false));
        setCollisionMargin(j.value("margin", 0.025f));
        setLinearSleepingThreshold(j.value("linearSleepingThreshold", 0.8f));
        setAngularSleepingThreshold(j.value("angularSleepingThreshold", 1.0f));
        setActivationState(j.value("activeState", 1));
        setPositionOffset(j.value("offset", Vec3(0.f, 0.f, 0.f)));
        Component::from_json(j);

        auto jConstraints = j.value("consts", json());
        for (auto it : jConstraints)
        {
            auto key = (int)it.at(0);
            auto val = it.at(1);
            std::shared_ptr<PhysicConstraint> constraint = nullptr;
            switch (key)
            {
            case (int)PhysicConstraint::ConstraintType::Fixed:
                constraint = addConstraint<FixedConstraint>();
                break;
            case (int)PhysicConstraint::ConstraintType::Hinge:
                constraint = addConstraint<HingeConstraint>();
                break;
            case (int)PhysicConstraint::ConstraintType::Slider:
                constraint = addConstraint<SliderConstraint>();
                break;
            case (int)PhysicConstraint::ConstraintType::Spring:
                constraint = addConstraint<SpringConstraint>();
                break;
            case (int)PhysicConstraint::ConstraintType::Dof6Spring:
                constraint = addConstraint<Dof6SpringConstraint>();
                break;
            }
            if (constraint)
                val.get_to(*constraint);
        }
    }

    //! Update property by key value
    void PhysicObject::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("mass") == 0)
            setMass(val);
        else if (key.compare("restitution") == 0)
            setRestitution(val);
        else if (key.compare("friction") == 0)
            setFriction(val);
        else if (key.compare("linearVelocity") == 0)
            setLinearVelocity(PhysicHelper::to_btVector3(val));
        else if (key.compare("angularVelocity") == 0)
            setAngularVelocity(PhysicHelper::to_btVector3(val));
        else if (key.compare("linearFactor") == 0)
            setLinearFactor(PhysicHelper::to_btVector3(val));
        else if (key.compare("angularFactor") == 0)
            setAngularFactor(PhysicHelper::to_btVector3(val));
        else if (key.compare("isKinematic") == 0)
            setIsKinematic(val);
        else if (key.compare("isTrigger") == 0)
            setIsTrigger(val);
        else if (key.compare("scale") == 0)
            setLocalScale(val);
        else if (key.compare("group") == 0)
            setCollisionFilterGroup(val);
        else if (key.compare("mask") == 0)
            setCollisionFilterMask(val);
        else if (key.compare("ccd") == 0)
            setCCD(val);
        else if (key.compare("margin") == 0)
            setCollisionMargin(val);
        else if (key.compare("linearSleepingThreshold") == 0)
            setLinearSleepingThreshold(val);
        else if (key.compare("angularSleepingThreshold") == 0)
            setAngularSleepingThreshold(val);
        else if (key.compare("activeState") == 0)
            setActivationState(val);
        else if (key.compare("offset") == 0)
            setPositionOffset(val);
        else
            Component::setProperty(key, val);
    }
} // namespace ige::scene