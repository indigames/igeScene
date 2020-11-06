#pragma once
#include <string>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletSoftBody/btSoftBody.h>

#include "event/Event.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/PhysicObject.h"
#include "scene/Scene.h"

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

        //! Create physic constraint
        virtual void create();

        //! Deinitialization
        virtual void destroy();

        //! Recreate physic constraint
        virtual void recreate();

        //! Get Owner
        PhysicObject *getOwner() const { return &m_owner; }

        //! Get Owner RigidBody
        virtual btRigidBody *getOwnerBody() const { return getOwner()->getBody(); }

        //! Get Other UUID
        const std::string& getOtherUUID() const { return m_otherUUID; }
        void setOtherUUID(const std::string& other);

        //! Get other object
        PhysicObject* getOther();

        //! Get Other RigidBody
        virtual btRigidBody *getOtherBody() { return getOther() ? getOther()->getBody() : nullptr; }

        //! Bullet generic constraint
        btGeneric6DofSpring2Constraint* getConstraint() const { return m_constraint.get(); }

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

        //! Serialize finished event
        void onSerializeFinished(Scene& scene);

    protected:
        //! On created event
        static Event<PhysicConstraint *> m_onActivatedEvent;

        //! On destroyed event
        static Event<PhysicConstraint *> m_onDeactivatedEvent;

    protected:
        //! Reference to owner object
        PhysicObject &m_owner;

        //! Pointer to depend object
        std::string m_otherUUID = {};

        //! Bullet generic constraint
        std::unique_ptr<btGeneric6DofSpring2Constraint> m_constraint = nullptr;

        //! Enable collision between linked body
        bool m_bEnableCollisionBetweenBodies = true;

        //! Cache other object
        PhysicObject *m_other = nullptr;

        //! Cache json to serialize
        json m_json = {};

        //! Cache serialize event id
        uint64_t m_serializeEventId = (uint64_t)(-1);
    };
} // namespace ige::scene
