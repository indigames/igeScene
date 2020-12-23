#pragma once

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletSoftBody/btSoftBody.h>

#include "event/Event.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/PhysicConstraint.h"

namespace ige::scene
{
    //! FixedConstraint
    class FixedConstraint : public PhysicConstraint
    {
    public:
        //! Constructor
        FixedConstraint(PhysicObject &owner);

        //! Destructor
        virtual ~FixedConstraint();

    protected:
        //! Serialize
        virtual void to_json(json &j) const override;

        //! Serialize finished event
        virtual void onSerializeFinished(Scene *scene) override;

        //! Create physic constraint
        virtual void create() override;
    };
} // namespace ige::scene
