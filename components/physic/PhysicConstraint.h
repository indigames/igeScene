#pragma once

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletSoftBody/btSoftBody.h>

#include "event/Event.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/PhysicObject.h"

namespace ige::scene
{
    //! PhysicConstraint
    class PhysicConstraint
    {
    public:
        //! Constructor
        PhysicConstraint(PhysicObject &owner);

        //! Destructor
        virtual ~PhysicConstraint();

        //! Initialization
        virtual bool init();

        //! Deinitialization
        virtual bool destroy();

        //! Get Owner
        PhysicObject *getOwner() const { return &m_owner; }

        //! Get Owner RigidBody
        virtual btRigidBody *getOwnerBody() const { return getOwner()->getBody(); }

        //! Get Other
        PhysicObject *getOther() const { return m_other; }

        //! Get Other RigidBody
        virtual btRigidBody *getOtherBody() const { return m_other ? m_other->getBody() : nullptr; }

        //! Bullet generic constraint
        btGeneric6DofConstraint* getConstraint() const { return m_constraint; }

        //! Enable collision between bodies
        bool isEnableCollisionBetweenBodies() const { return m_bEnableCollisionBetweenBodies; }
        void setEnableCollisionBetweenBodies(bool enable = true);

    public:
        //! Get onCreatedEvent
        static Event<PhysicConstraint *> &getOnActivatedEvent() { return m_onActivatedEvent; }

        //! Get onDestroyedEvent
        static Event<PhysicConstraint *> &getOnDeactivatedEvent() { return m_onDeactivatedEvent; }

    protected:
        //! Serialize
        virtual void to_json(json &j) const;

        //! Deserialize
        virtual void from_json(const json &j);

        //! Create physic constraint
        virtual void createConstraint() = 0;

        //! Destroy physic constraint
        virtual void destroyConstraint() = 0;

    protected:
        //! On created event
        static Event<PhysicConstraint *> m_onActivatedEvent;

        //! On destroyed event
        static Event<PhysicConstraint *> m_onDeactivatedEvent;

    protected:
        //! Reference to owner object
        PhysicObject &m_owner;

        //! Pointer to depend object
        PhysicObject *m_other = nullptr;

        //! Bullet generic constraint
        btGeneric6DofConstraint* m_constraint;

        //! Enable collision between linked body
        bool m_bEnableCollisionBetweenBodies = true;
    };
} // namespace ige::scene
