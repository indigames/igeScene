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
    //! Dof6SpringConstraint
    class Dof6SpringConstraint : public PhysicConstraint
    {
    public:
        //! Constructor
        Dof6SpringConstraint(Rigidbody &owner);

        //! Destructor
        virtual ~Dof6SpringConstraint();

        //! Lower linear limit
        const btVector3 &getLinearLowerLimit() const { return m_linearLowerLimit; }
        void setLinearLowerLimit(const btVector3 &value_entW);

        //! Upper linear limit
        const btVector3 &getLinearUpperLimit() const { return m_linearUpperLimit; }
        void setLinearUpperLimit(const btVector3 &val);

        //! Lower angular limit
        const btVector3 &getAngularLowerLimit() const { return m_angularLowerLimit; }
        void setAngularLowerLimit(const btVector3 &val);

        //! Upper angular limit
        const btVector3 &getAngularUpperLimit() const { return m_angularUpperLimit; }
        void setAngularUpperLimit(const btVector3 &val);

        //! Target linear velocity
        const btVector3 &getLinearTargetVelocity() const { return m_linearTargetVelocity; }
        void setLinearTargetVelocity(const btVector3 &val);

        //! Target angular velocity
        const btVector3 &getAngularTargetVelocity() const { return m_angularTargetVelocity; }
        void setAngularTargetVelocity(const btVector3 &val);

        //! Linear bounce
        const btVector3 &getLinearBounce() const { return m_linearBounce; }
        void setLinearBounce(const btVector3 &val);

        //! Angular bounce
        const btVector3 &getAngularBounce() const { return m_angularBounce; }
        void setAngularBounce(const btVector3 &val);

        //! Enable linear spring
        const btVector3 &getLinearSpringEnabled() const { return m_linearSpringEnabled; }
        void setLinearSpringEnabled(const btVector3 &val);

        //! Linear stiffness
        const btVector3 &getLinearStiffness() const { return m_linearStiffness; }
        void setLinearStiffness(const btVector3 &val);

        //! Enable angular spring
        const btVector3 &getAngularSpringEnabled() const { return m_angularSpringEnabled; }
        void setAngularSpringEnabled(const btVector3 &val);

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
        const btVector3 &getLinearMotorEnabled() const { return m_linearMotorEnabled; }
        void setLinearMotorEnabled(const btVector3 &val);

        //! Linear max motor force
        const btVector3 &getLinearMaxMotorForce() const { return m_linearMaxMotorForce; }
        void setLinearMaxMotorForce(const btVector3 &val);

        //! Enable angular motor
        const btVector3 &getAngularMotorEnabled() const { return m_angularMotorEnabled; }
        void setAngularMotorEnabled(const btVector3 &val);

        //! Angular max motor force
        const btVector3 &getAngularMaxMotorForce() const { return m_angularMaxMotorForce; }
        void setAngularMaxMotorForce(const btVector3 &val);

        //! Enable linear servo
        const btVector3 &getLinearServoEnabled() const { return m_linearServoEnabled; }
        void setLinearServoEnabled(const btVector3 &val);

        //! Linear servo target
        const btVector3 &getLinearServoTarget() const { return m_linearServoTarget; }
        void setLinearServoTarget(const btVector3 &val);

        //! Enable angular servo
        const btVector3 &getAngularServoEnabled() const { return m_angularServoEnabled; }
        void setAngularServoEnabled(const btVector3 &val);

        //! Angular servo target
        const btVector3 &getAngularServoTarget() const { return m_angularServoTarget; }
        void setAngularServoTarget(const btVector3 &val);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json &j) const override;
        virtual void from_json(const json& j) override;

        //! Create physic constraint
        virtual void create() override;

    protected:
        //! Lower linear limit (lower higher than upper mean no limit)
        btVector3 m_linearLowerLimit = {1.f, 1.f, 1.f};

        //! Upper linear limit
        btVector3 m_linearUpperLimit = {0.f, 0.f, 0.f};

        //! Lower angular limit (lower higher than upper mean no limit)
        btVector3 m_angularLowerLimit = {1.f, 1.f, 1.f};

        //! Upper angular limit
        btVector3 m_angularUpperLimit = {0.f, 0.f, 0.f};

        //! Target linear velocity
        btVector3 m_linearTargetVelocity = {0.f, 0.f, 0.f};

        //! Target angular velocity
        btVector3 m_angularTargetVelocity = {0.f, 0.f, 0.f};

        //! Linear bounce
        btVector3 m_linearBounce = {0.f, 0.f, 0.f};

        //! Angular bounce
        btVector3 m_angularBounce = {0.f, 0.f, 0.f};

        //! Enable linear spring
        btVector3 m_linearSpringEnabled = {0.f, 0.f, 0.f};

        //! Linear stiffness
        btVector3 m_linearStiffness = {0.f, 0.f, 0.f};

        //! Enable angular spring
        btVector3 m_angularSpringEnabled = {0.f, 0.f, 0.f};

        //! Angular stiffness
        btVector3 m_angularStiffness = {0.f, 0.f, 0.f};

        //! Linear damping
        btVector3 m_linearDamping = {0.f, 0.f, 0.f};

        //! Angular damping
        btVector3 m_angularDamping = {0.f, 0.f, 0.f};

        //! Enable linear motor
        btVector3 m_linearMotorEnabled = {0.f, 0.f, 0.f};

        //! Linear max motor force
        btVector3 m_linearMaxMotorForce = {0.f, 0.f, 0.f};

        //! Enable angular motor
        btVector3 m_angularMotorEnabled = {0.f, 0.f, 0.f};

        //! Angular max motor force
        btVector3 m_angularMaxMotorForce = {0.f, 0.f, 0.f};

        //! Enable linear servo
        btVector3 m_linearServoEnabled = {0.f, 0.f, 0.f};

        //! Linear servo target
        btVector3 m_linearServoTarget = {0.f, 0.f, 0.f};

        //! Enable angular servo
        btVector3 m_angularServoEnabled = {0.f, 0.f, 0.f};

        //! Angular servo target
        btVector3 m_angularServoTarget = {0.f, 0.f, 0.f};
    };
} // namespace ige::scene
