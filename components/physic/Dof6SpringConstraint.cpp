#include "components/physic/Dof6SpringConstraint.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    Dof6SpringConstraint::Dof6SpringConstraint(PhysicObject &owner)
        : PhysicConstraint(owner)
    {
    }

    //! Destructor
    Dof6SpringConstraint::~Dof6SpringConstraint()
    {
    }

    //! Set lower limit
    void Dof6SpringConstraint::setLowerLinearLimit(const btVector3 &limit)
    {
        if (m_bIsDirty || m_lowerLinearLimit != limit)
        {
            m_lowerLinearLimit = limit;
            getConstraint()->setLinearLowerLimit(m_lowerLinearLimit);
        }
    }

    //! Set upper limit
    void Dof6SpringConstraint::setUpperLinearLimit(const btVector3 &limit)
    {
        if (m_bIsDirty || m_upperLinearLimit != limit)
        {
            m_upperLinearLimit = limit;
            getConstraint()->setLinearUpperLimit(m_upperLinearLimit);
        }
    }

    //! Set lower limit
    void Dof6SpringConstraint::setLowerAngularLimit(const btVector3 &limit)
    {
        if (m_bIsDirty || m_lowerAngularLimit != limit)
        {
            m_lowerAngularLimit = limit;
            getConstraint()->setAngularLowerLimit(m_lowerAngularLimit);
        }
    }

    //! Set upper limit
    void Dof6SpringConstraint::setUpperAngularLimit(const btVector3 &limit)
    {
        if (m_bIsDirty || m_upperAngularLimit != limit)
        {
            m_upperAngularLimit = limit;
            getConstraint()->setAngularUpperLimit(m_upperAngularLimit);
        }
    }

    //! Target linear velocity
    void Dof6SpringConstraint::setTargetLinearVelocity(const btVector3 &val)
    {
        if (m_bIsDirty || m_targetLinearVelocity != val)
        {
            m_targetLinearVelocity = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setTargetVelocity(i, m_targetLinearVelocity[i]);
            }
        }
    }

    //! Target angular velocity
    void Dof6SpringConstraint::setTargetAngularVelocity(const btVector3 &val)
    {
        if (m_bIsDirty || m_targetAngularVelocity != val)
        {
            m_targetAngularVelocity = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setTargetVelocity(i + 3, m_targetAngularVelocity[i]);
            }
        }
    }

    //! Linear bounce
    void Dof6SpringConstraint::setLinearBounce(const btVector3 &val)
    {
        if (m_bIsDirty || m_linearBounce != val)
        {
            m_linearBounce = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setBounce(i, m_linearBounce[i]);
            }
        }
    }

    //! Angular bounce
    void Dof6SpringConstraint::setAngularBounce(const btVector3 &val)
    {
        if (m_bIsDirty || m_angularBounce != val)
        {
            m_angularBounce = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setBounce(i + 3, m_angularBounce[i]);
            }
        }
    }

    //! Enable linear spring
    void Dof6SpringConstraint::setEnableLinearSpring(const btVector3 &val)
    {
        if (m_bIsDirty || m_bEnableLinearSpring != val)
        {
            m_bEnableLinearSpring = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->enableSpring(i, m_bEnableLinearSpring[i]);
                if (m_bEnableLinearSpring[i])
                    getConstraint()->setEquilibriumPoint(i);
                else
                    getConstraint()->setEquilibriumPoint(i, 0.f);
            }
        }
    }

    //! Linear stiffness
    void Dof6SpringConstraint::setLinearStiffness(const btVector3 &val)
    {
        if (m_bIsDirty || m_linearStiffness != val)
        {
            m_linearStiffness = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setStiffness(i, m_linearStiffness[i]);
            }
        }
    }

    //! Enable angular spring
    void Dof6SpringConstraint::setEnableAngularSpring(const btVector3 &val)
    {
        if (m_bIsDirty || m_bEnableAngularSpring != val)
        {
            m_bEnableAngularSpring = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->enableSpring(i + 3, m_bEnableAngularSpring[i]);
                if (m_bEnableAngularSpring[i])
                    getConstraint()->setEquilibriumPoint(i + 3);
                else
                    getConstraint()->setEquilibriumPoint(i + 3, 0.f);
            }
        }
    }

    //! Angular stiffness
    void Dof6SpringConstraint::setAngularStiffness(const btVector3 &val)
    {
        if (m_bIsDirty || m_angularStiffness != val)
        {
            m_angularStiffness = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setStiffness(i + 3, m_angularStiffness[i]);
            }
        }
    }

    //! Linear damping
    void Dof6SpringConstraint::setLinearDamping(const btVector3 &val)
    {
        if (m_bIsDirty || m_linearDamping != val)
        {
            m_linearDamping = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setDamping(i, m_linearDamping[i]);
            }
        }
    }

    //! Angular damping
    void Dof6SpringConstraint::setAngularDamping(const btVector3 &val)
    {
        if (m_bIsDirty || m_angularDamping != val)
        {
            m_angularDamping = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setDamping(i + 3, m_angularDamping[i]);
            }
        }
    }

    //! Enable linear motor
    void Dof6SpringConstraint::setEnabledLinearMotor(const btVector3 &val)
    {
        if (m_bIsDirty || m_bEnableLinearMotor != val)
        {
            m_bEnableLinearMotor = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->enableMotor(i, m_bEnableLinearMotor[i]);
            }
        }
    }

    //! Linear max motor force
    void Dof6SpringConstraint::setLinearMaxMotorForce(const btVector3 &val)
    {
        if (m_bIsDirty || m_linearMaxMotorForce != val)
        {
            m_linearMaxMotorForce = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setMaxMotorForce(i, m_linearMaxMotorForce[i]);
            }
        }
    }

    //! Enable angular motor
    void Dof6SpringConstraint::setEnabledAngularMotor(const btVector3 &val)
    {
        if (m_bIsDirty || m_bEnableAngularMotor != val)
        {
            m_bEnableAngularMotor = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->enableMotor(i + 3, m_bEnableAngularMotor[i]);
            }
        }
    }

    //! Angular max motor force
    void Dof6SpringConstraint::setAngularMaxMotorForce(const btVector3 &val)
    {
        if (m_bIsDirty || m_angularMaxMotorForce != val)
        {
            m_angularMaxMotorForce = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setMaxMotorForce(i + 3, m_angularMaxMotorForce[i]);
            }
        }
    }

    //! Enable linear servo
    void Dof6SpringConstraint::setEnableLinearServo(const btVector3 &val)
    {
        if (m_bIsDirty || m_bEnableLinearServo != val)
        {
            m_bEnableLinearServo = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setServo(i, m_bEnableLinearServo[i]);
            }
        }
    }

    //! Linear servo target
    void Dof6SpringConstraint::setLinearServoTarget(const btVector3 &val)
    {
        if (m_bIsDirty || m_linearServoTarget != val)
        {
            m_linearServoTarget = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setServoTarget(i, m_linearServoTarget[i]);
            }
        }
    }

    //! Enable angular servo
    void Dof6SpringConstraint::setEnableAngularServo(const btVector3 &val)
    {
        if (m_bIsDirty || m_bEnableAngularServo != val)
        {
            m_bEnableAngularServo = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setServo(i + 3, m_bEnableAngularServo[i]);
            }
        }
    }

    //! Angular servo target
    void Dof6SpringConstraint::setAngularServoTarget(const btVector3 &val)
    {
        if (m_bIsDirty || m_angularServoTarget != val)
        {
            m_angularServoTarget = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setServoTarget(i + 3, m_angularServoTarget[i]);
            }
        }
    }

    // Create constraint
    void Dof6SpringConstraint::create()
    {
        // Destroy old constrain
        destroy();

        // Create constraint
        auto body = getOwnerBody();
        auto transform = PhysicHelper::to_btTransform(getOwner()->getOwner()->getTransform()->getWorldRotation(), getOwner()->getOwner()->getTransform()->getWorldPosition());

        auto otherBody = getOtherBody() ? getOtherBody() : &btGeneric6DofConstraint::getFixedBody();
        auto otherTransform = getOther() ? PhysicHelper::to_btTransform(getOther()->getOwner()->getTransform()->getWorldRotation(), getOther()->getOwner()->getTransform()->getWorldPosition()) : btTransform::getIdentity();
        m_constraint = std::make_unique<btGeneric6DofSpring2Constraint>(*body, *otherBody, transform, otherTransform);

        // Linear limit
        setLowerLinearLimit(m_lowerLinearLimit);
        setUpperLinearLimit(m_upperLinearLimit);
        setTargetLinearVelocity(m_targetLinearVelocity);

        // Angular limit
        setLowerAngularLimit(m_lowerAngularLimit);
        setUpperAngularLimit(m_upperAngularLimit);
        setTargetAngularVelocity(m_targetAngularVelocity);

        // Bounce
        setLinearBounce(m_linearBounce);
        setAngularBounce(m_angularBounce);

        // Linear spring
        setEnableLinearSpring(m_bEnableLinearSpring);
        setLinearStiffness(m_linearStiffness);
        setLinearDamping(m_linearDamping);

        // Angular spring
        setEnableAngularSpring(m_bEnableAngularSpring);
        setAngularStiffness(m_angularStiffness);
        setAngularDamping(m_angularDamping);

        // Linear Motor
        setEnabledLinearMotor(m_bEnableLinearMotor);
        setLinearMaxMotorForce(m_linearMaxMotorForce);
        setEnableLinearServo(m_bEnableLinearServo);
        setLinearServoTarget(m_linearServoTarget);

        // Angular Motor
        setEnabledAngularMotor(m_bEnableAngularMotor);
        setAngularMaxMotorForce(m_angularMaxMotorForce);
        setEnableAngularServo(m_bEnableAngularServo);
        setAngularServoTarget(m_angularServoTarget);

        // Call parent create function to register this constraint to world
        PhysicConstraint::create();
    }

    //! Serialize
    void Dof6SpringConstraint::to_json(json &j) const
    {
        PhysicConstraint::to_json(j);
        j["lLow"] = PhysicHelper::from_btVector3(getLowerLinearLimit());
        j["lUp"] = PhysicHelper::from_btVector3(getUpperLinearLimit());
        j["lVel"] = PhysicHelper::from_btVector3(getTargetLinearVelocity());
        j["aLow"] = PhysicHelper::from_btVector3(getLowerAngularLimit());
        j["aUp"] = PhysicHelper::from_btVector3(getUpperAngularLimit());
        j["aVel"] = PhysicHelper::from_btVector3(getTargetAngularVelocity());
        j["lBounce"] = PhysicHelper::from_btVector3(getLinearBounce());
        j["aBounce"] = PhysicHelper::from_btVector3(getAngularBounce());
        j["lSpring"] = PhysicHelper::from_btVector3(getEnableLinearSpring());
        j["lStiff"] = PhysicHelper::from_btVector3(getLinearStiffness());
        j["lDamp"] = PhysicHelper::from_btVector3(getLinearDamping());
        j["aSpring"] = PhysicHelper::from_btVector3(getEnableAngularSpring());
        j["aStiff"] = PhysicHelper::from_btVector3(getAngularStiffness());
        j["aDamp"] = PhysicHelper::from_btVector3(getAngularDamping());
        j["lMotor"] = PhysicHelper::from_btVector3(getEnabledLinearMotor());
        j["lMForce"] = PhysicHelper::from_btVector3(getLinearMaxMotorForce());
        j["lServo"] = PhysicHelper::from_btVector3(getEnableLinearServo());
        j["lSTarget"] = PhysicHelper::from_btVector3(getLinearServoTarget());
        j["aMotor"] = PhysicHelper::from_btVector3(getEnabledAngularMotor());
        j["aMForce"] = PhysicHelper::from_btVector3(getAngularMaxMotorForce());
        j["aServo"] = PhysicHelper::from_btVector3(getEnableAngularServo());
        j["aSTarget"] = PhysicHelper::from_btVector3(getAngularServoTarget());
    }

    //! Deserialize
    void Dof6SpringConstraint::onSerializeFinished(Scene &scene)
    {
        PhysicConstraint::onSerializeFinished(scene);

        // Linear limit
        setLowerLinearLimit(PhysicHelper::to_btVector3(m_json.value("lLow", Vec3(0.f, 0.f, 0.f))));
        setUpperLinearLimit(PhysicHelper::to_btVector3(m_json.value("lUp", Vec3(0.f, 0.f, 0.f))));
        setTargetLinearVelocity(PhysicHelper::to_btVector3(m_json.value("lVel", Vec3(0.f, 0.f, 0.f))));

        // Angular limit
        setLowerAngularLimit(PhysicHelper::to_btVector3(m_json.value("aLow", Vec3(0.f, 0.f, 0.f))));
        setUpperAngularLimit(PhysicHelper::to_btVector3(m_json.value("aUp", Vec3(0.f, 0.f, 0.f))));
        setTargetAngularVelocity(PhysicHelper::to_btVector3(m_json.value("aVel", Vec3(0.f, 0.f, 0.f))));

        // Bounce
        setLinearBounce(PhysicHelper::to_btVector3(m_json.value("lBounce", Vec3(0.f, 0.f, 0.f))));
        setAngularBounce(PhysicHelper::to_btVector3(m_json.value("aBounce", Vec3(0.f, 0.f, 0.f))));

        // Linear spring
        setEnableLinearSpring(PhysicHelper::to_btVector3(m_json.value("lSpring", Vec3(0.f, 0.f, 0.f))));
        setLinearStiffness(PhysicHelper::to_btVector3(m_json.value("lStiff", Vec3(0.f, 0.f, 0.f))));
        setLinearDamping(PhysicHelper::to_btVector3(m_json.value("lDamp", Vec3(0.f, 0.f, 0.f))));

        // Angular spring
        setEnableAngularSpring(PhysicHelper::to_btVector3(m_json.value("aSpring", Vec3(0.f, 0.f, 0.f))));
        setAngularStiffness(PhysicHelper::to_btVector3(m_json.value("aStiff", Vec3(0.f, 0.f, 0.f))));
        setAngularDamping(PhysicHelper::to_btVector3(m_json.value("aDamp", Vec3(0.f, 0.f, 0.f))));

        // Linear Motor
        setEnabledLinearMotor(PhysicHelper::to_btVector3(m_json.value("lMotor", Vec3(0.f, 0.f, 0.f))));
        setLinearMaxMotorForce(PhysicHelper::to_btVector3(m_json.value("lMForce", Vec3(0.f, 0.f, 0.f))));
        setEnableLinearServo(PhysicHelper::to_btVector3(m_json.value("lServo", Vec3(0.f, 0.f, 0.f))));
        setLinearServoTarget(PhysicHelper::to_btVector3(m_json.value("lSTarget", Vec3(0.f, 0.f, 0.f))));

        // Angular Motor
        setEnabledAngularMotor(PhysicHelper::to_btVector3(m_json.value("aMotor", Vec3(0.f, 0.f, 0.f))));
        setAngularMaxMotorForce(PhysicHelper::to_btVector3(m_json.value("aMForce", Vec3(0.f, 0.f, 0.f))));
        setEnableAngularServo(PhysicHelper::to_btVector3(m_json.value("aServo", Vec3(0.f, 0.f, 0.f))));
        setAngularServoTarget(PhysicHelper::to_btVector3(m_json.value("aSTarget", Vec3(0.f, 0.f, 0.f))));

        // Serialization done, clear json
        m_json.clear();
    }
} // namespace ige::scene
