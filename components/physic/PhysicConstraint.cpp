#include "components/physic/PhysicConstraint.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Initialize static members
    Event<PhysicConstraint*> PhysicConstraint::m_onActivatedEvent;
    Event<PhysicConstraint*> PhysicConstraint::m_onDeactivatedEvent;

    //! Constructor
    PhysicConstraint::PhysicConstraint(PhysicObject &owner)
        : m_owner(owner), m_other(nullptr)
    {}

    //! Destructor
    PhysicConstraint::~PhysicConstraint()
    {
        destroy();
    }

    //! Initialization
    bool PhysicConstraint::init()
    {
        createConstraint();
        getOnActivatedEvent().invoke(this);
        return true;
    }

    //! Initialization
    bool PhysicConstraint::destroy()
    {
        getOnDeactivatedEvent().invoke(this);
        destroyConstraint();
        return true;
    }

    //! Enable collision between bodies
    void PhysicConstraint::setEnableCollisionBetweenBodies(bool enable)
    {
        if(m_bEnableCollisionBetweenBodies != enable)
        {
            // Remove old constraint
            getOnDeactivatedEvent().invoke(this);

            // Update new value
            m_bEnableCollisionBetweenBodies = enable;

            // Add new constraint
            getOnActivatedEvent().invoke(this);
        }
    }

    //! Serialize
    void PhysicConstraint::to_json(json &j) const
    {
        j = json{
            {"colBody", isEnableCollisionBetweenBodies()},
        };
    }

    //! Deserialize
    void PhysicConstraint::from_json(const json &j)
    {
        setEnableCollisionBetweenBodies(j.value("colBody", true));
    }
}
