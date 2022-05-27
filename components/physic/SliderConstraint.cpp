#include "components/physic/SliderConstraint.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    SliderConstraint::SliderConstraint(Rigidbody &owner)
        : PhysicConstraint(owner)
    {
        setType(ConstraintType::Slider);

        // Create constraint
        create();
    }

    //! Destructor
    SliderConstraint::~SliderConstraint()
    {
    }

    //! Set lower limit
    void SliderConstraint::setLowerLimit(const btVector3 &limit)
    {
        if (m_bIsDirty || m_lowerLimit != limit)
        {
            m_lowerLimit = limit;
            getConstraint()->setLinearLowerLimit(m_lowerLimit);
        }
    }

    //! Set upper limit
    void SliderConstraint::setUpperLimit(const btVector3 &limit)
    {
        if (m_bIsDirty || m_upperLimit != limit)
        {
            m_upperLimit = limit;
            getConstraint()->setLinearUpperLimit(m_upperLimit);
        }
    }

    // Create constraint
    void SliderConstraint::create()
    {
        // Destroy old constrain
        destroy();

        // Create constraint
        auto body = getOwnerBody();
        auto transform = PhysicHelper::to_btTransform(getOwner()->getOwner()->getTransform()->getRotation(), getOwner()->getOwner()->getTransform()->getPosition());

        auto otherBody = getOtherBody() ? getOtherBody() : &btGeneric6DofConstraint::getFixedBody();
        auto otherTransform = getOther() ? PhysicHelper::to_btTransform(getOther()->getOwner()->getTransform()->getRotation(), getOther()->getOwner()->getTransform()->getPosition()) : btTransform::getIdentity();
        m_constraint = std::make_unique<btGeneric6DofSpring2Constraint>(*body, *otherBody, transform, otherTransform);
        setLowerLimit(m_lowerLimit);
        setUpperLimit(m_upperLimit);

        // Call parent create function to register this constraint to world
        PhysicConstraint::create();
    }

    //! Serialize
    void SliderConstraint::to_json(json &j) const
    {
        PhysicConstraint::to_json(j);
        j["low"] = PhysicHelper::from_btVector3(getLowerLimit());
        j["up"] = PhysicHelper::from_btVector3(getUpperLimit());
    }

    //! Deserialize
    void SliderConstraint::onSerializeFinished(Scene *scene)
    {
        setLowerLimit(PhysicHelper::to_btVector3(m_json.value("low", Vec3(0.f, 0.f, 0.f))));
        setUpperLimit(PhysicHelper::to_btVector3(m_json.value("up", Vec3(0.f, 0.f, 0.f))));

        // Serialization done, clear json
        PhysicConstraint::onSerializeFinished(scene);
    }

    //! Update property by key value
    void SliderConstraint::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("low") == 0)
            setLowerLimit(PhysicHelper::to_btVector3(val));
        else if (key.compare("up") == 0)
            setUpperLimit(PhysicHelper::to_btVector3(val));
        else
            PhysicConstraint::setProperty(key, val);
    }
} // namespace ige::scene
