#include "components/physic/SpringConstraint.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    SpringConstraint::SpringConstraint(PhysicObject &owner)
        : PhysicConstraint(owner)
    {
    }

    //! Destructor
    SpringConstraint::~SpringConstraint()
    {
    }

    //! Set lower limit
    void SpringConstraint::setLowerLimit(const btVector3 &limit)
    {
        if (m_bIsDirty || m_lowerLimit != limit)
        {
            m_lowerLimit = limit;
            getConstraint()->setLinearLowerLimit(m_lowerLimit);
        }
    }

    //! Set upper limit
    void SpringConstraint::setUpperLimit(const btVector3 &limit)
    {
        if (m_bIsDirty || m_upperLimit != limit)
        {
            m_upperLimit = limit;
            getConstraint()->setLinearUpperLimit(m_upperLimit);
        }
    }

    void SpringConstraint::setEnable(const btVector3 &val)
    {
        if (m_bIsDirty || m_enable != val)
        {
            m_enable = val;
            for (int i = 0; i < 3; ++i)
            {
                m_constraint->enableSpring(i, m_enable[i]);
                if (m_enable[i])
                    m_constraint->setEquilibriumPoint(i);
                else
                    m_constraint->setEquilibriumPoint(i, 0.f);
            }
        }
    }

    void SpringConstraint::setStiffness(const btVector3 &val)
    {
        if (m_bIsDirty || m_stiffness != val)
        {
            m_stiffness = val;
            for (int i = 0; i < 3; ++i)
            {
                m_constraint->setStiffness(i, m_stiffness[i]);
            }
        }
    }

    void SpringConstraint::setDamping(const btVector3 &val)
    {
        if (m_bIsDirty || m_damping != val)
        {
            m_damping = val;
            for (int i = 0; i < 3; ++i)
            {
                m_constraint->setDamping(i, m_damping[i]);
            }
        }
    }

    // Create constraint
    void SpringConstraint::create()
    {
        // Destroy old constrain
        destroy();

        // Create constraint
        auto body = getOwnerBody();
        auto transform = PhysicHelper::to_btTransform(getOwner()->getOwner()->getTransform()->getWorldRotation(), getOwner()->getOwner()->getTransform()->getWorldPosition());

        auto otherBody = getOtherBody() ? getOtherBody() : &btGeneric6DofConstraint::getFixedBody();
        auto otherTransform = getOther() ? PhysicHelper::to_btTransform(getOther()->getOwner()->getTransform()->getWorldRotation(), getOther()->getOwner()->getTransform()->getWorldPosition()) : btTransform::getIdentity();
        m_constraint = std::make_unique<btGeneric6DofSpring2Constraint>(*body, *otherBody, transform, otherTransform);

        // Apply constraint
        setLowerLimit(m_lowerLimit);
        setUpperLimit(m_upperLimit);
        setEnable(m_enable);
        setStiffness(m_stiffness);
        setDamping(m_damping);

        // Call parent create function to register this constraint to world
        PhysicConstraint::create();
    }

    //! Serialize
    void SpringConstraint::to_json(json &j) const
    {
        PhysicConstraint::to_json(j);
        j["low"] = PhysicHelper::from_btVector3(getLowerLimit());
        j["up"] = PhysicHelper::from_btVector3(getUpperLimit());
        j["enable"] = PhysicHelper::from_btVector3(getEnable());
        j["stiff"] = PhysicHelper::from_btVector3(getStiffness());
        j["damp"] = PhysicHelper::from_btVector3(getDamping());
    }

    //! Deserialize
    void SpringConstraint::onSerializeFinished(Scene &scene)
    {
        PhysicConstraint::onSerializeFinished(scene);
        setLowerLimit(PhysicHelper::to_btVector3(m_json.value("low", Vec3(1.f, 1.f, 1.f))));
        setUpperLimit(PhysicHelper::to_btVector3(m_json.value("up", Vec3(0.f, 0.f, 0.f))));
        setEnable(PhysicHelper::to_btVector3(m_json.value("enable", Vec3(1.f, 0.f, 0.f))));
        setStiffness(PhysicHelper::to_btVector3(m_json.value("stiff", Vec3(10.f, 0.f, 0.f))));
        setDamping(PhysicHelper::to_btVector3(m_json.value("damp", Vec3(0.5f, 0.f, 0.f))));

        // Serialization done, clear json
        m_json.clear();
    }
} // namespace ige::scene
