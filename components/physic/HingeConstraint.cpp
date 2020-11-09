#include "components/physic/HingeConstraint.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    HingeConstraint::HingeConstraint(PhysicObject &owner)
        : PhysicConstraint(owner)
    {
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
    void HingeConstraint::setAxis(const btVector3 &axis)
    {
        if (m_bIsDirty || m_axis != axis)
        {
            m_axis = axis;
            recreate();
        }
    }

    //! Set other axis
    void HingeConstraint::setOtherAxis(const btVector3 &axis)
    {
        if (m_bIsDirty || m_otherAxis != axis)
        {
            m_otherAxis = axis;
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
        m_constraint = std::make_unique<btHinge2Constraint>(*getOwnerBody(), *otherBody, m_anchor, m_axis, m_otherAxis);

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
        j["axis"] = PhysicHelper::from_btVector3(getAxis());
        j["axis2"] = PhysicHelper::from_btVector3(getOtherAxis());
        j["low"] = getLowerLimit();
        j["up"] = getUpperLimit();
    }

    //! Deserialize
    void HingeConstraint::onSerializeFinished(Scene &scene)
    {
        PhysicConstraint::onSerializeFinished(scene);
        setAnchor(PhysicHelper::to_btVector3(m_json.value("anchor", Vec3(0.f, 0.f, 0.f))));
        setAxis(PhysicHelper::to_btVector3(m_json.value("axis", Vec3(0.f, 0.f, 0.f))));
        setOtherAxis(PhysicHelper::to_btVector3(m_json.value("axis2", Vec3(0.f, 0.f, 0.f))));
        setLowerLimit(m_json.value("low", 0.f));
        setUpperLimit(m_json.value("up", 0.f));

        // Serialization done, clear json
        m_json.clear();
    }
} // namespace ige::scene
