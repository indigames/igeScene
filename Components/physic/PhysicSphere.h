#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/PhysicBase.h"

namespace ige::scene
{
    //! PhysicSphere
    class PhysicSphere : public PhysicBase
    {
    public:
        //! Constructor
        PhysicSphere(const std::shared_ptr<SceneObject> &owner, float radius = 1.f);

        //! Destructor
        virtual ~PhysicSphere();

        //! Get name
        std::string getName() const override { return "PhysicSphere"; }

        //! Get radius
        float getRadius();

        //! Set radius
        void setRadius(float radius);

    protected:
        //! Create collision shape
        void createCollisionShape(float radius);

        //! Recreate collision shape once radius changed
        void recreateCollisionShape(float radius);

        //! Set local scale of the box
        virtual void setLocalScale(const Vec3& scale) override;

    protected:
        float m_radius;
    };
}