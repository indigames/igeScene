#include "components/physic/PhysicConstraint.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Initialize static members
    Event<PhysicConstraint *> PhysicConstraint::m_onActivatedEvent;
    Event<PhysicConstraint *> PhysicConstraint::m_onDeactivatedEvent;

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
        if (m_constraint)
        {
            getOnActivatedEvent().invoke(this);
        }
        m_bIsDirty = false;
    }

    //! Destroy
    void PhysicConstraint::destroy()
    {
        m_bIsDirty = true;
        if (m_constraint)
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
    PhysicObject *PhysicConstraint::getOther()
    {
        return m_other;
    }

    //! Set other object
    void PhysicConstraint::setOther(PhysicObject *other)
    {
        if(m_other != other)
        {
            m_other = other;
            recreate();
        }
    }

    // Get other body object UUID
    const std::string& PhysicConstraint::getOtherUUID() const
    {
        return m_other ? m_other->getOwner()->getUUID() : std::string();
    }

    // Set other body object by UUID
    void PhysicConstraint::setOtherUUID(const std::string &otherUUID)
    {
        if (otherUUID != getOwner()->getOwner()->getUUID())
        {
            auto otherObject = getOwner()->getOwner()->getScene()->findObjectByUUID(otherUUID);
            auto other = otherObject ? otherObject->getComponent<PhysicObject>().get() : nullptr;
            setOther(other);
        }
    }

    //! Enable collision between bodies
    void PhysicConstraint::setEnableCollisionBetweenBodies(bool enable)
    {
        if (m_bEnableCollisionBetweenBodies != enable)
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
            {"type", (int)getType()},
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
    void PhysicConstraint::onSerializeFinished(Scene &scene)
    {
        setType((ConstraintType)m_json.value("type", -1));
        setEnableCollisionBetweenBodies(m_json.value("enableCol", true));
        setOtherUUID(m_json.value("otherId", std::string()));
        setEnabled(m_json.value("enable", true));
        setBreakingImpulseThreshold(m_json.value("breakTs", std::numeric_limits<float>().max()));
    }
} // namespace ige::scene
