#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftBody.h>

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
        FixedConstraint(Rigidbody &owner);

        //! Destructor
        virtual ~FixedConstraint();

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Create physic constraint
        virtual void create() override;
    };
} // namespace ige::scene
