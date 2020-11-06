#include "components/physic/FixedConstraint.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    FixedConstraint::FixedConstraint(PhysicObject &owner)
        : PhysicConstraint(owner)
    {}

    //! Destructor
    FixedConstraint::~FixedConstraint()
    {}

    // Create constraint
    void FixedConstraint::create()
    {
        // Destroy old constrain
        destroy();

        auto body = getOwnerBody();
        auto transform = PhysicHelper::to_btTransform(getOwner()->getOwner()->getTransform()->getWorldRotation(), getOwner()->getOwner()->getTransform()->getWorldPosition());

        auto otherBody = getOtherBody() ? getOtherBody() : &btGeneric6DofConstraint::getFixedBody();
        auto otherTransform = btTransform::getIdentity();
        if(getOther())
        {
            otherTransform = PhysicHelper::to_btTransform(getOther()->getOwner()->getTransform()->getWorldRotation(), getOther()->getOwner()->getTransform()->getWorldPosition());
        }

        m_constraint = std::make_unique<btFixedConstraint>(getOwnerBody(), getOtherBody(), transform, otherTransform);

        // Call parent create function to register this constraint to world
        PhysicConstraint::create();
    }

    //! Serialize
    void FixedConstraint::to_json(json &j) const
    {
        PhysicConstraint::to_json(j);
    }

    //! Deserialize
    void FixedConstraint::from_json(const json &j)
    {
        PhysicConstraint::from_json(j);
    }
}
