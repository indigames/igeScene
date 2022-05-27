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
    //! SliderConstraint
    class SliderConstraint : public PhysicConstraint
    {
    public:
        //! Constructor
        SliderConstraint(Rigidbody &owner);

        //! Destructor
        virtual ~SliderConstraint();

        //! Lower angle limit
        const btVector3 &getLowerLimit() const { return m_lowerLimit; }
        void setLowerLimit(const btVector3 &angleMin);

        //! Upper angle limit
        const btVector3 &getUpperLimit() const { return m_upperLimit; }
        void setUpperLimit(const btVector3 &angleMax);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json &j) const override;

        //! Serialize finished event
        virtual void onSerializeFinished(Scene *scene) override;

        //! Create physic constraint
        virtual void create() override;

    protected:
        //! Lower limit
        btVector3 m_lowerLimit = {0.f, 0.f, 0.f};

        //! Upper limit
        btVector3 m_upperLimit = {0.f, 0.f, 0.f};
    };
} // namespace ige::scene
