#include "components/physic/Rigidbody.h"
#include "components/physic/collider/MeshCollider.h"
#include "components/physic/PhysicManager.h"
#include "components/physic/PhysicConstraint.h"
#include "components/physic/constraint/FixedConstraint.h"
#include "components/physic/constraint/HingeConstraint.h"
#include "components/physic/constraint/SliderConstraint.h"
#include "components/physic/constraint/SpringConstraint.h"
#include "components/physic/constraint/Dof6SpringConstraint.h"
#include "components/TransformComponent.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Initialize static members
    Event<Rigidbody *> Rigidbody::m_onCreatedEvent;
    Event<Rigidbody *> Rigidbody::m_onDestroyedEvent;
    Event<Rigidbody *> Rigidbody::m_onActivatedEvent;
    Event<Rigidbody *> Rigidbody::m_onDeactivatedEvent;

    //! Constructor
    Rigidbody::Rigidbody(SceneObject &owner)
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
        m_transformEventId = getOwner()->getTransformChangedEvent().addListener(std::bind(&Rigidbody::onTransformChanged, this, std::placeholders::_1));
    }

    //! Destructor
    Rigidbody::~Rigidbody()
    {
        if (m_transformEventId != (uint64_t)-1) {
            getOwner()->getTransformChangedEvent().removeListener(m_transformEventId);
            m_transformEventId = (uint64_t)-1;
        }
        destroy();
        m_manager.reset();
    }

    //! Initialization
    bool Rigidbody::init()
    {
        getOnCreatedEvent().invoke(this);
        createBody();
        return true;
    }

    //! Initialization
    bool Rigidbody::destroy()
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

    //! Transform changed: update transform for kinematic object
    void Rigidbody::onTransformChanged(SceneObject& object) {
        if (m_body && isKinematic()) {
            updateBtTransform();
            m_body->setInterpolationWorldTransform(m_body->getWorldTransform());
            if (getBody() && getBody()->getMotionState()) {
                getBody()->getMotionState()->setWorldTransform(m_body->getWorldTransform());
            }
        }
#if EDITOR_MODE
        getOwner()->updateAabb();
#endif
    }

    //! Add constraint
    void Rigidbody::addConstraint(const std::shared_ptr<PhysicConstraint> &constraint)
    {
        m_constraints.push_back(constraint);
    }

    //! Add constraint
    std::shared_ptr<PhysicConstraint> Rigidbody::addConstraint(int type)
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
    void Rigidbody::removeConstraint(const std::shared_ptr<PhysicConstraint> &constraint)
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
    void Rigidbody::removeConstraint(PhysicConstraint *constraint)
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
    void Rigidbody::removeAllConstraints()
    {
        for (auto &constraint : m_constraints)
            constraint = nullptr;
        m_constraints.clear();
    }

    //! Set enable
    void Rigidbody::setEnabled(bool enable)
    {
        Component::setEnabled(enable);
        if (isEnabled())
            activate();
        else
            deactivate();
    }

    //! Set enable
    void Rigidbody::setCCD(bool isCCD)
    {
        m_bIsCCD = isCCD;
        if (!m_body) return;
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
    void Rigidbody::setMass(float mass)
    {
        if (m_bIsDirty || m_mass != mass)
        {
            m_mass = mass;
            if (!m_body) return;
            btVector3 inertia = {0.f, 0.f, 0.f};
            if (m_mass != 0.0f) {
                if (!m_collider.expired() && m_collider.lock()->getShape()) {
                    m_collider.lock()->getShape()->calculateLocalInertia(m_mass, inertia);
                }
            }
            getBody()->setMassProps(mass, inertia);
            getBody()->updateInertiaTensor();
        }
    }

    void Rigidbody::setFriction(float friction)
    {
        if (m_bIsDirty || m_friction != friction)
        {
            m_friction = friction;
            if (!m_body) return;
            m_body->setFriction(m_friction);
        }
    }

    void Rigidbody::setRestitution(float restitution)
    {
        if (m_bIsDirty || m_restitution != restitution)
        {
            m_restitution = restitution;
            if (!m_body) return;
            m_body->setRestitution(m_restitution);
        }
    }

    void Rigidbody::setLinearVelocity(const btVector3 &velocity)
    {
        if (m_bIsDirty || m_linearVelocity != velocity)
        {
            m_linearVelocity = velocity;
            if (!m_body) return;
            getBody()->setLinearVelocity(m_linearVelocity);
        }
    }

    void Rigidbody::setAngularVelocity(const btVector3 &velocity)
    {
        if (m_bIsDirty || m_angularVelocity != velocity)
        {
            m_angularVelocity = velocity;
            if (!m_body) return;
            getBody()->setAngularVelocity(m_angularVelocity);
        }
    }

    void Rigidbody::setLinearFactor(const btVector3 &factor)
    {
        if (m_bIsDirty || m_linearFactor != factor)
        {
            m_linearFactor = factor;
            if (getBody())
                getBody()->setLinearFactor(m_linearFactor);
        }
    }

    void Rigidbody::setAngularFactor(const btVector3 &factor)
    {
        if (m_bIsDirty || m_angularFactor != factor)
        {
            m_angularFactor = factor;
            if (getBody())
                getBody()->setAngularFactor(m_angularFactor);
        }
    }

    void Rigidbody::setLinearSleepingThreshold(float value) {
        if (m_bIsDirty || m_linearSleepingThreshold != value)
        {
            m_linearSleepingThreshold = value;
            if (getBody())
                getBody()->setSleepingThresholds(m_linearSleepingThreshold, m_angularSleepingThreshold);
        }
    }

    void Rigidbody::setAngularSleepingThreshold(float value) {
        if (m_bIsDirty || m_angularSleepingThreshold != value)
        {
            m_angularSleepingThreshold = value;
            if (getBody())
                getBody()->setSleepingThresholds(m_linearSleepingThreshold, m_angularSleepingThreshold);
        }
    }

    int Rigidbody::getActivationState() const {
        return m_activeState;
    }

    void Rigidbody::setActivationState(int state) {
        m_activeState = MATH_CLAMP(state, 0, 5);
        if (getBody())
            getBody()->setActivationState(m_activeState);
    }

    void Rigidbody::setPositionOffset(const Vec3& offset)
    {
        if(m_bIsDirty || m_positionOffset != offset) {
            m_positionOffset = offset;
            updateBtTransform();
            getOwner()->updateAabb();
        }
    }

    //! Set is trigger
    void Rigidbody::setIsTrigger(bool isTrigger)
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
    void Rigidbody::setIsKinematic(bool isKinematic)
    {
        if (m_bIsDirty || m_bIsKinematic != isKinematic)
        {
            if (m_bIsKinematic != isKinematic)
            {
                m_bIsKinematic = isKinematic;

                // Dynamic body -> force convex mesh collider
                if (!this->isKinematic()) {
                    auto meshCollider = getOwner()->getComponent<MeshCollider>();
                    if (meshCollider && !meshCollider->isConvex()) {
                        meshCollider->setConvex(true);
                    }
                }
                recreateBody();
            }
            if (m_bIsKinematic)
            {
                setMass(0.f);
                clearForces();
                setLinearVelocity({0.f, 0.f, 0.f});
                setAngularVelocity({0.f, 0.f, 0.f});
                addCollisionFlag(btCollisionObject::CF_KINEMATIC_OBJECT);
                setActivationState(DISABLE_DEACTIVATION);
            }
            else
            {
                removeCollisionFlag(btCollisionObject::CF_KINEMATIC_OBJECT);
            }

            if (m_body && m_body->getBroadphaseHandle())
            {
                m_body->getBroadphaseHandle()->m_collisionFilterGroup = m_collisionFilterGroup;
                m_body->getBroadphaseHandle()->m_collisionFilterMask = m_collisionFilterMask;
            }
        }
    }

    //! Create physic body
    void Rigidbody::createBody()
    {
        destroyBody();

        m_collider = getOwner()->getComponent<Collider>();
        if (m_collider.expired() ||! m_collider.lock()->getShape())
            return;

        m_motion = std::make_unique<btDefaultMotionState>(PhysicHelper::to_btTransform(getOwner()->getTransform()->getLocalRotation(), getOwner()->getTransform()->getLocalPosition()));
        m_body = std::make_unique<btRigidBody>(btRigidBody::btRigidBodyConstructionInfo{0.0f, m_motion.get(),  m_collider.lock()->getShape().get(), btVector3(0.0f, 0.0f, 0.0f)});
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

        // Update transform
        updateBtTransform();

        m_bIsDirty = false;

        if (isEnabled())
            activate();
    }

    //! Create physic body
    void Rigidbody::destroyBody()
    {
        deactivate();
        if (m_body)
            m_body.reset();
        if (m_motion)
            m_motion.reset();
    }

    //! Calculate and apply inertia
    void Rigidbody::applyInertia()
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
                if (m_mass != 0.0f) {
                    if (!m_collider.expired() && m_collider.lock()->getShape()) {
                        m_collider.lock()->getShape()->calculateLocalInertia(m_mass, inertia);
                    }
                }
                getBody()->setMassProps(std::max(0.0000001f, m_mass), inertia);
            }
        }
    }

    //! Collision filter group
    void Rigidbody::setCollisionFilterGroup(int group)
    {
        if (m_collisionFilterGroup != group)
        {
            m_collisionFilterGroup = group;
            if (m_body && m_body->getBroadphaseHandle())
                m_body->getBroadphaseHandle()->m_collisionFilterGroup = m_collisionFilterGroup;
        }
    }

    //! Collision filter mask
    void Rigidbody::setCollisionFilterMask(int mask)
    {
        if (m_collisionFilterMask != mask)
        {
            m_collisionFilterMask = mask;
            if (m_body && m_body->getBroadphaseHandle())
                m_body->getBroadphaseHandle()->m_collisionFilterMask = m_collisionFilterMask;
        }
    }

    //! Activate
    void Rigidbody::activate()
    {
        if (!m_bIsActivated)
        {
            getOnActivatedEvent().invoke(this);
            if(m_body) m_body->activate(true);
            m_bIsActivated = true;
        }
    }

    //! Deactivate
    void Rigidbody::deactivate()
    {
        if (m_bIsActivated)
        {
            getOnDeactivatedEvent().invoke(this);
            if(m_body) m_body->activate(false);
            m_bIsActivated = false;
        }
    }

    //! Update Bullet transform
    void Rigidbody::updateBtTransform()
    {
        if (!m_body) return;
        auto offset = Vec3();
        const auto& transform = getOwner()->getTransform();
        if (m_positionOffset.LengthSqr() > 0) {
            auto matrix = Mat4::IdentityMat();
            vmath_mat_from_quat(transform->getRotation().P(), 4, matrix.P());
            vmath_mat_appendScale(matrix.P(), transform->getScale().P(), 4, 4, matrix.P());
            offset = matrix * m_positionOffset;
        }
        m_body->setWorldTransform(PhysicHelper::to_btTransform(transform->getRotation(), transform->getPosition() + offset));
        Vec3 scale = transform->getScale();
        Vec3 dScale = {scale[0] - m_previousScale[0], scale[1] - m_previousScale[1], scale[2] - m_previousScale[2]};
        float scaleDelta = vmath_lengthSqr(dScale.P(), 3);
        if (scaleDelta >= 0.01f) {            
            if (!m_collider.expired()) {
                m_collider.lock()->setScale({ std::abs(scale[0]), std::abs(scale[1]), std::abs(scale[2]) });
                m_previousScale = m_collider.lock()->getScale();
            }
        }
    }

    //! Update IGE transform
    void Rigidbody::updateIgeTransform()
    {
        if (!m_body || isKinematic()) return;
        auto transform = getOwner()->getTransform();
        const auto &result = m_body->getWorldTransform();
        transform->setPosition(PhysicHelper::from_btVector3(result.getOrigin()));
        transform->setRotation(PhysicHelper::from_btQuaternion(result.getRotation()));
        if (m_positionOffset.LengthSqr() > 0) {
            auto matrix = Mat4::IdentityMat();
            vmath_mat_from_quat(transform->getRotation().P(), 4, matrix.P());
            vmath_mat_appendScale(matrix.P(), transform->getScale().P(), 4, 4, matrix.P());
            auto offset = matrix * m_positionOffset;
            transform->translate(-offset);
        }
    }

    //! Get AABB
    AABBox Rigidbody::getAABB()
    {
        if (!getBody()) return {};
        btVector3 aabbMin, aabbMax;
        getBody()->getAabb(aabbMin, aabbMax);
        auto box = AABBox(PhysicHelper::from_btVector3(aabbMin), PhysicHelper::from_btVector3(aabbMax));
        box = box.Transform(getOwner()->getTransform()->getWorldMatrix().Inverse());
        return box;
    }

    //! Serialize
    void Rigidbody::to_json(json &j) const
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
        j["group"] = getCollisionFilterGroup();
        j["mask"] = getCollisionFilterMask();
        j["ccd"] = isCCD();
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
    void Rigidbody::from_json(const json &j)
    {
        Component::from_json(j);
        m_json = json(j); // copy data
    }

    void Rigidbody::onSerializeFinished(Scene* scene) {

        init();
        auto j = m_json;
        setMass(j.value("mass", 1.f));
        setRestitution(j.value("restitution", 1.f));
        setFriction(j.value("friction", 0.5f));
        setLinearVelocity(PhysicHelper::to_btVector3(j.value("linearVelocity", Vec3())));
        setAngularVelocity(PhysicHelper::to_btVector3(j.value("angularVelocity", Vec3())));
        setLinearFactor(PhysicHelper::to_btVector3(j.value("linearFactor", Vec3(1.f, 1.f, 1.f))));
        setAngularFactor(PhysicHelper::to_btVector3(j.value("angularFactor", Vec3(1.f, 1.f, 1.f))));
        setIsKinematic(j.value("isKinematic", false));
        setIsTrigger(j.value("isTrigger", false));
        setCollisionFilterGroup(j.value("group", isKinematic() ? 2 : 1));
        setCollisionFilterMask(j.value("mask", isKinematic() ? 3 : -1));
        setCCD(j.value("ccd", false));
        setLinearSleepingThreshold(j.value("linearSleepingThreshold", 0.8f));
        setAngularSleepingThreshold(j.value("angularSleepingThreshold", 1.0f));
        setActivationState(j.value("activeState", 1));
        setPositionOffset(j.value("offset", Vec3(0.f, 0.f, 0.f)));

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
        m_json.clear();
    }

    //! Update property by key value
    void Rigidbody::setProperty(const std::string& key, const json& val)
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
        else if (key.compare("group") == 0)
            setCollisionFilterGroup(val);
        else if (key.compare("mask") == 0)
            setCollisionFilterMask(val);
        else if (key.compare("ccd") == 0)
            setCCD(val);
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