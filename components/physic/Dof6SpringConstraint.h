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
    //! Dof6SpringConstraint
    class Dof6SpringConstraint : public PhysicConstraint
    {
    public:
        //! Constructor
        Dof6SpringConstraint(PhysicObject &owner);

        //! Destructor
        virtual ~Dof6SpringConstraint();

        //! Lower linear limit
        const btVector3 &getLowerLinearLimit() const { return m_lowerLinearLimit; }
        void setLowerLinearLimit(const btVector3 &value_entW);

        //! Upper linear limit
        const btVector3 &getUpperLinearLimit() const { return m_upperLinearLimit; }
        void setUpperLinearLimit(const btVector3 &val);

        //! Lower angular limit
        const btVector3 &getLowerAngularLimit() const { return m_lowerAngularLimit; }
        void setLowerAngularLimit(const btVector3 &val);

        //! Upper angular limit
        const btVector3 &getUpperAngularLimit() const { return m_upperAngularLimit; }
        void setUpperAngularLimit(const btVector3 &val);

        //! Target linear velocity
        const btVector3 &getTargetLinearVelocity() const { return m_targetLinearVelocity; }
        void setTargetLinearVelocity(const btVector3 &val);

        //! Target angular velocity
        const btVector3 &getTargetAngularVelocity() const { return m_targetAngularVelocity; }
        void setTargetAngularVelocity(const btVector3 &val);

        //! Linear bounce
        const btVector3 &getLinearBounce() const { return m_linearBounce; }
        void setLinearBounce(const btVector3 &val);

        //! Angular bounce
        const btVector3 &getAngularBounce() const { return m_angularBounce; }
        void setAngularBounce(const btVector3 &val);

        //! Enable linear spring
        const btVector3 & getEnableLinearSpring() const { return m_bEnableLinearSpring; }
        void setEnableLinearSpring(const btVector3 &val);

        //! Linear stiffness
        const btVector3 &getLinearStiffness() const { return m_linearStiffness; }
        void setLinearStiffness(const btVector3 &val);

        //! Enable angular spring
        const btVector3 & getEnableAngularSpring() const { return m_bEnableAngularSpring; }
        void setEnableAngularSpring(const btVector3 &val);

        //! Angular stiffness
        const btVector3 &getAngularStiffness() const { return m_angularStiffness; }
        void setAngularStiffness(const btVector3 &val);

        //! Linear damping
        const btVector3 &getLinearDamping() const { return m_linearDamping; }
        void setLinearDamping(const btVector3 &val);

        //! Angular damping
        const btVector3 &getAngularDamping() const { return m_angularDamping; }
        void setAngularDamping(const btVector3 &val);

        //! Enable linear motor
        const btVector3 & getEnabledLinearMotor() const { return m_bEnableLinearMotor; }
        void setEnabledLinearMotor(const btVector3 &val);

        //! Linear max motor force
        const btVector3 &getLinearMaxMotorForce() const { return m_linearMaxMotorForce; }
        void setLinearMaxMotorForce(const btVector3 &val);

        //! Enable angular motor
        const btVector3 & getEnabledAngularMotor() const { return m_bEnableAngularMotor; }
        void setEnabledAngularMotor(const btVector3 &val);

        //! Angular max motor force
        const btVector3 &getAngularMaxMotorForce() const { return m_angularMaxMotorForce; }
        void setAngularMaxMotorForce(const btVector3 &val);

        //! Enable linear servo
        const btVector3 & getEnableLinearServo() const { return m_bEnableLinearServo; }
        void setEnableLinearServo(const btVector3 &val);

        //! Linear servo target
        const btVector3 &getLinearServoTarget() const { return m_linearServoTarget; }
        void setLinearServoTarget(const btVector3 &val);

        //! Enable angular servo
        const btVector3 & getEnableAngularServo() const { return m_bEnableAngularServo; }
        void setEnableAngularServo(const btVector3 &val);

        //! Angular servo target
        const btVector3 &getAngularServoTarget() const { return m_angularServoTarget; }
        void setAngularServoTarget(const btVector3 &val);

    protected:
        //! Serialize
        virtual void to_json(json &j) const override;

        //! Serialize finished event
        virtual void onSerializeFinished(Scene &scene);

        //! Create physic constraint
        virtual void create() override;

    protected:
        //! Lower linear limit (lower higher than upper mean no limit)
        btVector3 m_lowerLinearLimit = {1.f, 1.f, 1.f};

        //! Upper linear limit
        btVector3 m_upperLinearLimit = {0.f, 0.f, 0.f};

        //! Lower angular limit (lower higher than upper mean no limit)
        btVector3 m_lowerAngularLimit = {1.f, 1.f, 1.f};

        //! Upper angular limit
        btVector3 m_upperAngularLimit = {0.f, 0.f, 0.f};

        //! Target linear velocity
        btVector3 m_targetLinearVelocity = {0.f, 0.f, 0.f};

        //! Target angular velocity
        btVector3 m_targetAngularVelocity = {0.f, 0.f, 0.f};

        //! Linear bounce
        btVector3 m_linearBounce = {0.f, 0.f, 0.f};

        //! Angular bounce
        btVector3 m_angularBounce = {0.f, 0.f, 0.f};

        //! Enable linear spring
        btVector3 m_bEnableLinearSpring = {0.f, 0.f, 0.f};

        //! Linear stiffness
        btVector3 m_linearStiffness = {0.f, 0.f, 0.f};

        //! Enable angular spring
        btVector3 m_bEnableAngularSpring = {0.f, 0.f, 0.f};

        //! Angular stiffness
        btVector3 m_angularStiffness = {0.f, 0.f, 0.f};

        //! Linear damping
        btVector3 m_linearDamping = {0.f, 0.f, 0.f};

        //! Angular damping
        btVector3 m_angularDamping = {0.f, 0.f, 0.f};

        //! Enable linear motor
        btVector3 m_bEnableLinearMotor = {0.f, 0.f, 0.f};

        //! Linear max motor force
        btVector3 m_linearMaxMotorForce = {0.f, 0.f, 0.f};

        //! Enable angular motor
        btVector3 m_bEnableAngularMotor = {0.f, 0.f, 0.f};

        //! Angular max motor force
        btVector3 m_angularMaxMotorForce = {0.f, 0.f, 0.f};

        //! Enable linear servo
        btVector3 m_bEnableLinearServo = {0.f, 0.f, 0.f};

        //! Linear servo target
        btVector3 m_linearServoTarget = {0.f, 0.f, 0.f};

        //! Enable angular servo
        btVector3 m_bEnableAngularServo = {0.f, 0.f, 0.f};

        //! Angular servo target
        btVector3 m_angularServoTarget = {0.f, 0.f, 0.f};

        //! Dirty flag
        bool m_bIsDirty = true;
    };
} // namespace ige::scene
