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
    //! SpringConstraint
    class SpringConstraint : public PhysicConstraint
    {
    public:
        //! Constructor
        SpringConstraint(Rigidbody &owner);

        //! Destructor
        virtual ~SpringConstraint();

        //! Lower angle limit
        const btVector3 &getLowerLimit() const { return m_lowerLimit; }
        void setLowerLimit(const btVector3 &angleMin);

        //! Upper angle limit
        const btVector3 &getUpperLimit() const { return m_upperLimit; }
        void setUpperLimit(const btVector3 &angleMax);

        //! Enable
        const btVector3 &getEnable() const { return m_enable; }
        void setEnable(const btVector3 &val);

        //! Stiffness
        const btVector3 &getStiffness() const { return m_stiffness; }
        void setStiffness(const btVector3 &val);

        //! Damping
        const btVector3 &getDamping() const { return m_damping; }
        void setDamping(const btVector3 &val);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json &j) const override;
        virtual void from_json(const json& j) override;

        //! Create physic constraint
        virtual void create() override;

    protected:
        //! Lower limit (lower higher than upper mean no limit)
        btVector3 m_lowerLimit = {1.f, 1.f, 1.f};

        //! Upper limit
        btVector3 m_upperLimit = {0.f, 0.f, 0.f};

        //! Enable spring
        btVector3 m_enable = {1.f, 0.f, 0.f};

        //! Stiffness
        btVector3 m_stiffness = {10.f, 0.f, 0.f};

        //! Damping
        btVector3 m_damping = {0.5f, 0.f, 0.f};
    };
} // namespace ige::scene
