#include "components/physic/FixedConstraint.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    FixedConstraint::FixedConstraint(Rigidbody &owner)
        : PhysicConstraint(owner)
    {
        setType(ConstraintType::Fixed);

        // Create constraint
        create();
    }

    //! Destructor
    FixedConstraint::~FixedConstraint()
    {
    }

    // Create constraint
    void FixedConstraint::create()
    {
        // Destroy old constrain
        destroy();

        // Create constraint
        auto body = getOwnerBody();
        auto transform = PhysicHelper::to_btTransform(getOwner()->getOwner()->getTransform()->getRotation(), getOwner()->getOwner()->getTransform()->getPosition());

        auto otherBody = getOtherBody() ? getOtherBody() : &btGeneric6DofConstraint::getFixedBody();
        auto otherTransform = getOther() ? PhysicHelper::to_btTransform(getOther()->getOwner()->getTransform()->getRotation(), getOther()->getOwner()->getTransform()->getPosition()) : btTransform::getIdentity();
        m_constraint = std::make_unique<btFixedConstraint>(*body, *otherBody, transform, otherTransform);

        // Call parent create function to register this constraint to world
        PhysicConstraint::create();
    }

    //! Serialize
    void FixedConstraint::to_json(json &j) const
    {
        PhysicConstraint::to_json(j);
    }

    //! Deserialize
    void FixedConstraint::onSerializeFinished(Scene *scene)
    {
        PhysicConstraint::onSerializeFinished(scene);
    }

    //! Update property by key value
    void FixedConstraint::setProperty(const std::string& key, const json& val)
    {
        PhysicConstraint::setProperty(key, val);
    }
} // namespace ige::scene
