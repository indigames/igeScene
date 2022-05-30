#include "components/physic/constraint/HingeConstraint.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    HingeConstraint::HingeConstraint(Rigidbody &owner)
        : PhysicConstraint(owner)
    {
        setType(ConstraintType::Hinge);

        // Create constraint
        create();
    }

    //! Destructor
    HingeConstraint::~HingeConstraint()
    {
    }

    //! Set anchor
    void HingeConstraint::setAnchor(const btVector3 &anchor)
    {
        if (m_bIsDirty || m_anchor != anchor)
        {
            m_anchor = anchor;
            recreate();
        }
    }

    //! Set axis
    void HingeConstraint::setAxis1(const btVector3 &axis)
    {
        if (m_bIsDirty || m_axis1 != axis)
        {
            m_axis1 = axis;
            recreate();
        }
    }

    //! Set other axis
    void HingeConstraint::setAxis2(const btVector3 &axis)
    {
        if (m_bIsDirty || m_axis2 != axis)
        {
            m_axis2 = axis;
            recreate();
        }
    }

    //! Set lower limit
    void HingeConstraint::setLowerLimit(float angle)
    {
        if (m_bIsDirty || m_lowerLimit != angle)
        {
            m_lowerLimit = angle;
            getHingeConstraint()->setLowerLimit(m_lowerLimit);
        }
    }

    //! Set upper limit
    void HingeConstraint::setUpperLimit(float angle)
    {
        if (m_bIsDirty || m_upperLimit != angle)
        {
            m_upperLimit = angle;
            getHingeConstraint()->setLowerLimit(m_upperLimit);
        }
    }

    // Create constraint
    void HingeConstraint::create()
    {
        // Destroy old constrain
        destroy();

        // Create constraint
        auto otherBody = getOtherBody() ? getOtherBody() : &btGeneric6DofConstraint::getFixedBody();
        m_constraint = std::make_unique<btHinge2Constraint>(*getOwnerBody(), *otherBody, m_anchor, m_axis1, m_axis2);

        // Apply constraint
        setLowerLimit(m_lowerLimit);
        setUpperLimit(m_upperLimit);

        // Call parent create function to register this constraint to world
        PhysicConstraint::create();
    }

    //! Serialize
    void HingeConstraint::to_json(json &j) const
    {
        PhysicConstraint::to_json(j);
        j["anchor"] = PhysicHelper::from_btVector3(getAnchor());
        j["axis1"] = PhysicHelper::from_btVector3(getAxis1());
        j["axis2"] = PhysicHelper::from_btVector3(getAxis2());
        j["low"] = getLowerLimit();
        j["up"] = getUpperLimit();
    }

    //! Deserialize
    void HingeConstraint::onSerializeFinished(Scene *scene)
    {
        setAnchor(PhysicHelper::to_btVector3(m_json.value("anchor", Vec3(0.f, 0.f, 0.f))));
        setAxis1(PhysicHelper::to_btVector3(m_json.value("axis1", Vec3(0.f, 1.f, 0.f))));
        setAxis2(PhysicHelper::to_btVector3(m_json.value("axis2", Vec3(1.f, 0.f, 0.f))));
        setLowerLimit(m_json.value("low", -SIMD_PI));
        setUpperLimit(m_json.value("up", SIMD_PI));
        PhysicConstraint::onSerializeFinished(scene);
    }

    //! Update property by key value
    void HingeConstraint::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("anchor") == 0)
            setAnchor(PhysicHelper::to_btVector3(val));
        else if (key.compare("axis1") == 0)
            setAxis1(PhysicHelper::to_btVector3(val));
        else if (key.compare("axis2") == 0)
            setAxis2(PhysicHelper::to_btVector3(val));
        else if (key.compare("low") == 0)
            setLowerLimit(val);
        else if (key.compare("up") == 0)
            setUpperLimit(val);
        else
            PhysicConstraint::setProperty(key, val);
    }
} // namespace ige::scene
