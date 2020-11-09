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
    //! SpringConstraint
    class SpringConstraint : public PhysicConstraint
    {
    public:
        //! Constructor
        SpringConstraint(PhysicObject &owner);

        //! Destructor
        virtual ~SpringConstraint();

        //! Lower angle limit
        const btVector3& getLowerLimit() const { return m_lowerLimit; }
        void setLowerLimit(const btVector3& angleMin);

        //! Upper angle limit
        const btVector3& getUpperLimit() const { return m_upperLimit; }
        void setUpperLimit(const btVector3& angleMax);

        //! Stiffness
        float getStiffness() const { return m_stiffness; }
        void setStiffness(float val);

        //! Damping
        float getDamping() const { return m_damping; }
        void setDamping(float val);

    protected:
        //! Serialize
        virtual void to_json(json &j) const override;

        //! Serialize finished event
        virtual void onSerializeFinished(Scene &scene);

        //! Create physic constraint
        virtual void create() override;

    protected:
        //! Lower limit (lower higher than upper mean no limit)
        btVector3 m_lowerLimit = {1.f, 1.f, 1.f};

        //! Upper limit
        btVector3 m_upperLimit = {0.f, 0.f, 0.f};

        //! Stiffness
        float m_stiffness = 10.f;

        //! Damping
        float m_damping = 0.5f;
    };
} // namespace ige::scene
