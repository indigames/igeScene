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
    //! HingeConstraint
    class HingeConstraint : public PhysicConstraint
    {
    public:
        //! Constructor
        HingeConstraint(Rigidbody &owner);

        //! Destructor
        virtual ~HingeConstraint();

        //! Get constraint
        btHinge2Constraint *getHingeConstraint() const { return (btHinge2Constraint *)m_constraint.get(); }

        //! Anchor
        const btVector3 &getAnchor() const { return getHingeConstraint()->getAnchor(); }
        void setAnchor(const btVector3 &anchor);

        //! Other anchor
        const btVector3 &getOtherAnchor() const { return getHingeConstraint()->getAnchor2(); }

        //! Axis
        const btVector3 &getAxis1() const { return getHingeConstraint()->getAxis1(); }
        void setAxis1(const btVector3 &axis);

        //! Other axis
        const btVector3 &getAxis2() const { return getHingeConstraint()->getAxis2(); }
        void setAxis2(const btVector3 &axis);

        //! Get angle
        float getAngle() const { return getHingeConstraint()->getAngle1(); }
        float getOtherAngle() const { return getHingeConstraint()->getAngle2(); }

        //! Lower angle limit
        float getLowerLimit() const { return m_lowerLimit; }
        void setLowerLimit(float angleMin);

        //! Upper angle limit
        float getUpperLimit() const { return m_upperLimit; }
        void setUpperLimit(float angleMax);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json &j) const override;
        virtual void from_json(const json& j) override;

        //! Create physic constraint
        virtual void create() override;

    protected:
        //! Anchor point
        btVector3 m_anchor = {0.f, 0.f, 0.f};

        //! Axis
        btVector3 m_axis1 = {0.f, 1.f, 0.f};

        //! Other Axis
        btVector3 m_axis2 = {1.f, 0.f, 0.f};

        //! Lower limit
        float m_lowerLimit = -SIMD_PI;

        //! Upper limit
        float m_upperLimit = SIMD_PI;
    };
} // namespace ige::scene
