#include "components/physic/PhysicConstraint.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Initialize static members
    Event<PhysicConstraint*> PhysicConstraint::m_onActivatedEvent;
    Event<PhysicConstraint*> PhysicConstraint::m_onDeactivatedEvent;

    //! Constructor
    PhysicConstraint::PhysicConstraint(PhysicObject &owner)
        : m_owner(owner), m_other(nullptr)
    {
        m_serializeEventId = getOwner()->getOwner()->getScene()->getSerializeFinishedEvent().addListener(std::bind(&PhysicConstraint::onSerializeFinished, this, std::placeholders::_1));
    }

    //! Destructor
    PhysicConstraint::~PhysicConstraint()
    {
        getOwner()->getOwner()->getScene()->getSerializeFinishedEvent().removeListener(m_serializeEventId);
        destroy();
    }

    //! Create
    void PhysicConstraint::create()
    {
        if(m_constraint)
        {
            getOnActivatedEvent().invoke(this);
        }
    }

    //! Destroy
    void PhysicConstraint::destroy()
    {
        if(m_constraint)
        {
            getOnDeactivatedEvent().invoke(this);
            m_constraint.reset();
        }
    }

    //! Recreate
    void PhysicConstraint::recreate()
    {
        destroy();
        create();
    }

    //! Get other object
    PhysicObject* PhysicConstraint::getOther()
    {
        return m_other;
    }

    // Set other body object
    void PhysicConstraint::setOtherUUID(const std::string& otherUUID)
    {
        if(m_otherUUID != otherUUID)
        {
            m_otherUUID = otherUUID;
            auto otherObject = SceneManager::getInstance()->getCurrentScene()->findObjectByUUID(m_otherUUID);
            m_other = otherObject ? otherObject->getComponent<PhysicObject>().get() : nullptr;
            recreate();
        }
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
            {"enableCol", isEnableCollisionBetweenBodies()},
            {"otherId", getOtherUUID()},
            {"enable", isEnabled()},
            {"breakTs", getBreakingImpulseThreshold()},
        };
    }

    //! Deserialize
    void PhysicConstraint::from_json(const json &j)
    {
        m_json = j;
    }

    //! Serialization finished event
    void PhysicConstraint::onSerializeFinished(Scene& scene)
    {
        setEnableCollisionBetweenBodies(m_json.value("enableCol", true));
        setOtherUUID(m_json.value("otherId", std::string()));
        setEnabled(m_json.value("enable", true));
        setBreakingImpulseThreshold(m_json.value("breakTs", std::numeric_limits<float>().max()));
    }
}
