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
        setType(ConstraintType::Dof6Spring);

        // Create constraint
        create();
    }

    //! Destructor
    Dof6SpringConstraint::~Dof6SpringConstraint()
    {
    }

    //! Set lower limit
    void Dof6SpringConstraint::setLinearLowerLimit(const btVector3 &limit)
    {
        if (m_bIsDirty || m_linearLowerLimit != limit)
        {
            m_linearLowerLimit = limit;
            getConstraint()->setLinearLowerLimit(m_linearLowerLimit);
        }
    }

    //! Set upper limit
    void Dof6SpringConstraint::setLinearUpperLimit(const btVector3 &limit)
    {
        if (m_bIsDirty || m_linearUpperLimit != limit)
        {
            m_linearUpperLimit = limit;
            getConstraint()->setLinearUpperLimit(m_linearUpperLimit);
        }
    }

    //! Set lower limit
    void Dof6SpringConstraint::setAngularLowerLimit(const btVector3 &limit)
    {
        if (m_bIsDirty || m_angularLowerLimit != limit)
        {
            m_angularLowerLimit = limit;
            getConstraint()->setAngularLowerLimit(m_angularLowerLimit);
        }
    }

    //! Set upper limit
    void Dof6SpringConstraint::setAngularUpperLimit(const btVector3 &limit)
    {
        if (m_bIsDirty || m_angularUpperLimit != limit)
        {
            m_angularUpperLimit = limit;
            getConstraint()->setAngularUpperLimit(m_angularUpperLimit);
        }
    }

    //! Target linear velocity
    void Dof6SpringConstraint::setLinearTargetVelocity(const btVector3 &val)
    {
        if (m_bIsDirty || m_linearTargetVelocity != val)
        {
            m_linearTargetVelocity = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setTargetVelocity(i, m_linearTargetVelocity[i]);
            }
        }
    }

    //! Target angular velocity
    void Dof6SpringConstraint::setAngularTargetVelocity(const btVector3 &val)
    {
        if (m_bIsDirty || m_angularTargetVelocity != val)
        {
            m_angularTargetVelocity = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setTargetVelocity(i + 3, m_angularTargetVelocity[i]);
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
    void Dof6SpringConstraint::setLinearSpringEnabled(const btVector3 &val)
    {
        if (m_bIsDirty || m_linearSpringEnabled != val)
        {
            m_linearSpringEnabled = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->enableSpring(i, m_linearSpringEnabled[i]);
                if (m_linearSpringEnabled[i])
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
    void Dof6SpringConstraint::setAngularSpringEnabled(const btVector3 &val)
    {
        if (m_bIsDirty || m_angularSpringEnabled != val)
        {
            m_angularSpringEnabled = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->enableSpring(i + 3, m_angularSpringEnabled[i]);
                if (m_angularSpringEnabled[i])
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
    void Dof6SpringConstraint::setLinearMotorEnabled(const btVector3 &val)
    {
        if (m_bIsDirty || m_linearMotorEnabled != val)
        {
            m_linearMotorEnabled = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->enableMotor(i, m_linearMotorEnabled[i]);
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
    void Dof6SpringConstraint::setAngularMotorEnabled(const btVector3 &val)
    {
        if (m_bIsDirty || m_angularMotorEnabled != val)
        {
            m_angularMotorEnabled = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->enableMotor(i + 3, m_angularMotorEnabled[i]);
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
    void Dof6SpringConstraint::setLinearServoEnabled(const btVector3 &val)
    {
        if (m_bIsDirty || m_linearServoEnabled != val)
        {
            m_linearServoEnabled = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setServo(i, m_linearServoEnabled[i]);
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
    void Dof6SpringConstraint::setAngularServoEnabled(const btVector3 &val)
    {
        if (m_bIsDirty || m_angularServoEnabled != val)
        {
            m_angularServoEnabled = val;

            for (int i = 0; i < 3; ++i)
            {
                getConstraint()->setServo(i + 3, m_angularServoEnabled[i]);
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
        setLinearLowerLimit(m_linearLowerLimit);
        setLinearUpperLimit(m_linearUpperLimit);
        setLinearTargetVelocity(m_linearTargetVelocity);

        // Angular limit
        setAngularLowerLimit(m_angularLowerLimit);
        setAngularUpperLimit(m_angularUpperLimit);
        setAngularTargetVelocity(m_angularTargetVelocity);

        // Bounce
        setLinearBounce(m_linearBounce);
        setAngularBounce(m_angularBounce);

        // Linear spring
        setLinearSpringEnabled(m_linearSpringEnabled);
        setLinearStiffness(m_linearStiffness);
        setLinearDamping(m_linearDamping);

        // Angular spring
        setAngularSpringEnabled(m_angularSpringEnabled);
        setAngularStiffness(m_angularStiffness);
        setAngularDamping(m_angularDamping);

        // Linear Motor
        setLinearMotorEnabled(m_linearMotorEnabled);
        setLinearMaxMotorForce(m_linearMaxMotorForce);
        setLinearServoEnabled(m_linearServoEnabled);
        setLinearServoTarget(m_linearServoTarget);

        // Angular Motor
        setAngularMotorEnabled(m_angularMotorEnabled);
        setAngularMaxMotorForce(m_angularMaxMotorForce);
        setAngularServoEnabled(m_angularServoEnabled);
        setAngularServoTarget(m_angularServoTarget);

        // Call parent create function to register this constraint to world
        PhysicConstraint::create();
    }

    //! Serialize
    void Dof6SpringConstraint::to_json(json &j) const
    {
        PhysicConstraint::to_json(j);
        j["lLow"] = PhysicHelper::from_btVector3(getLinearLowerLimit());
        j["lUp"] = PhysicHelper::from_btVector3(getLinearUpperLimit());
        j["lVel"] = PhysicHelper::from_btVector3(getLinearTargetVelocity());
        j["aLow"] = PhysicHelper::from_btVector3(getAngularLowerLimit());
        j["aUp"] = PhysicHelper::from_btVector3(getAngularUpperLimit());
        j["aVel"] = PhysicHelper::from_btVector3(getAngularTargetVelocity());
        j["lBounce"] = PhysicHelper::from_btVector3(getLinearBounce());
        j["aBounce"] = PhysicHelper::from_btVector3(getAngularBounce());
        j["lSpring"] = PhysicHelper::from_btVector3(getLinearSpringEnabled());
        j["lStiff"] = PhysicHelper::from_btVector3(getLinearStiffness());
        j["lDamp"] = PhysicHelper::from_btVector3(getLinearDamping());
        j["aSpring"] = PhysicHelper::from_btVector3(getAngularSpringEnabled());
        j["aStiff"] = PhysicHelper::from_btVector3(getAngularStiffness());
        j["aDamp"] = PhysicHelper::from_btVector3(getAngularDamping());
        j["lMotor"] = PhysicHelper::from_btVector3(getLinearMotorEnabled());
        j["lMForce"] = PhysicHelper::from_btVector3(getLinearMaxMotorForce());
        j["lServo"] = PhysicHelper::from_btVector3(getLinearServoEnabled());
        j["lSTarget"] = PhysicHelper::from_btVector3(getLinearServoTarget());
        j["aMotor"] = PhysicHelper::from_btVector3(getAngularMotorEnabled());
        j["aMForce"] = PhysicHelper::from_btVector3(getAngularMaxMotorForce());
        j["aServo"] = PhysicHelper::from_btVector3(getAngularServoEnabled());
        j["aSTarget"] = PhysicHelper::from_btVector3(getAngularServoTarget());
    }

    //! Deserialize
    void Dof6SpringConstraint::onSerializeFinished(Scene *scene)
    {
        // Linear limit
        setLinearLowerLimit(PhysicHelper::to_btVector3(m_json.value("lLow", Vec3(0.f, 0.f, 0.f))));
        setLinearUpperLimit(PhysicHelper::to_btVector3(m_json.value("lUp", Vec3(0.f, 0.f, 0.f))));
        setLinearTargetVelocity(PhysicHelper::to_btVector3(m_json.value("lVel", Vec3(0.f, 0.f, 0.f))));

        // Angular limit
        setAngularLowerLimit(PhysicHelper::to_btVector3(m_json.value("aLow", Vec3(0.f, 0.f, 0.f))));
        setAngularUpperLimit(PhysicHelper::to_btVector3(m_json.value("aUp", Vec3(0.f, 0.f, 0.f))));
        setAngularTargetVelocity(PhysicHelper::to_btVector3(m_json.value("aVel", Vec3(0.f, 0.f, 0.f))));

        // Bounce
        setLinearBounce(PhysicHelper::to_btVector3(m_json.value("lBounce", Vec3(0.f, 0.f, 0.f))));
        setAngularBounce(PhysicHelper::to_btVector3(m_json.value("aBounce", Vec3(0.f, 0.f, 0.f))));

        // Linear spring
        setLinearSpringEnabled(PhysicHelper::to_btVector3(m_json.value("lSpring", Vec3(0.f, 0.f, 0.f))));
        setLinearStiffness(PhysicHelper::to_btVector3(m_json.value("lStiff", Vec3(0.f, 0.f, 0.f))));
        setLinearDamping(PhysicHelper::to_btVector3(m_json.value("lDamp", Vec3(0.f, 0.f, 0.f))));

        // Angular spring
        setAngularSpringEnabled(PhysicHelper::to_btVector3(m_json.value("aSpring", Vec3(0.f, 0.f, 0.f))));
        setAngularStiffness(PhysicHelper::to_btVector3(m_json.value("aStiff", Vec3(0.f, 0.f, 0.f))));
        setAngularDamping(PhysicHelper::to_btVector3(m_json.value("aDamp", Vec3(0.f, 0.f, 0.f))));

        // Linear Motor
        setLinearMotorEnabled(PhysicHelper::to_btVector3(m_json.value("lMotor", Vec3(0.f, 0.f, 0.f))));
        setLinearMaxMotorForce(PhysicHelper::to_btVector3(m_json.value("lMForce", Vec3(0.f, 0.f, 0.f))));
        setLinearServoEnabled(PhysicHelper::to_btVector3(m_json.value("lServo", Vec3(0.f, 0.f, 0.f))));
        setLinearServoTarget(PhysicHelper::to_btVector3(m_json.value("lSTarget", Vec3(0.f, 0.f, 0.f))));

        // Angular Motor
        setAngularMotorEnabled(PhysicHelper::to_btVector3(m_json.value("aMotor", Vec3(0.f, 0.f, 0.f))));
        setAngularMaxMotorForce(PhysicHelper::to_btVector3(m_json.value("aMForce", Vec3(0.f, 0.f, 0.f))));
        setAngularServoEnabled(PhysicHelper::to_btVector3(m_json.value("aServo", Vec3(0.f, 0.f, 0.f))));
        setAngularServoTarget(PhysicHelper::to_btVector3(m_json.value("aSTarget", Vec3(0.f, 0.f, 0.f))));

        // Serialization done
        PhysicConstraint::onSerializeFinished(scene);
    }

    //! Update property by key value
    void Dof6SpringConstraint::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("lLow") == 0)
            setLinearLowerLimit(PhysicHelper::to_btVector3(val));
        else if (key.compare("lUp") == 0)
            setLinearUpperLimit(PhysicHelper::to_btVector3(val));
        else if (key.compare("lVel") == 0)
            setLinearTargetVelocity(PhysicHelper::to_btVector3(val));
        else if (key.compare("aLow") == 0)
            setAngularLowerLimit(PhysicHelper::to_btVector3(val));
        else if (key.compare("aUp") == 0)
            setAngularUpperLimit(PhysicHelper::to_btVector3(val));
        else if (key.compare("aVel") == 0)
            setAngularTargetVelocity(PhysicHelper::to_btVector3(val));
        else if (key.compare("lBounce") == 0)
            setLinearBounce(PhysicHelper::to_btVector3(val));
        else if (key.compare("aBounce") == 0)
            setAngularBounce(PhysicHelper::to_btVector3(val));
        else if (key.compare("lSpring") == 0)
            setLinearSpringEnabled(PhysicHelper::to_btVector3(val));
        else if (key.compare("lStiff") == 0)
            setLinearStiffness(PhysicHelper::to_btVector3(val));
        else if (key.compare("lDamp") == 0)
            setLinearDamping(PhysicHelper::to_btVector3(val));
        else if (key.compare("aSpring") == 0)
            setAngularSpringEnabled(PhysicHelper::to_btVector3(val));
        else if (key.compare("aStiff") == 0)
            setAngularStiffness(PhysicHelper::to_btVector3(val));
        else if (key.compare("aDamp") == 0)
            setAngularDamping(PhysicHelper::to_btVector3(val));
        else if (key.compare("lMotor") == 0)
            setLinearMotorEnabled(PhysicHelper::to_btVector3(val));
        else if (key.compare("lMForce") == 0)
            setLinearMaxMotorForce(PhysicHelper::to_btVector3(val));
        else if (key.compare("lServo") == 0)
            setLinearServoEnabled(PhysicHelper::to_btVector3(val));
        else if (key.compare("lSTarget") == 0)
            setLinearServoTarget(PhysicHelper::to_btVector3(val));
        else if (key.compare("aMotor") == 0)
            setAngularMotorEnabled(PhysicHelper::to_btVector3(val));
        else if (key.compare("aMForce") == 0)
            setAngularMaxMotorForce(PhysicHelper::to_btVector3(val));
        else if (key.compare("aServo") == 0)
            setAngularServoEnabled(PhysicHelper::to_btVector3(val));
        else if (key.compare("aSTarget") == 0)
            setAngularServoTarget(PhysicHelper::to_btVector3(val));
        else
            PhysicConstraint::setProperty(key, val);
    }
} // namespace ige::scene
