#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/PhysicBase.h"

namespace ige::scene
{
    //! PhysicCapsule
    class PhysicCapsule : public PhysicBase
    {
    public:
        //! Constructor
        PhysicCapsule(const std::shared_ptr<SceneObject> &owner, float radius = 1.f, float height = 1.f);

        //! Destructor
        virtual ~PhysicCapsule();

        //! Get name
        std::string getName() const override { return "PhysicCapsule"; }

        //! Serialize
        virtual void to_json(json &j) const;

        //! Deserialize
        virtual void from_json(const json &j);

        //! Get radius
        float getRadius() const;

        //! Set radius
        void setRadius(float radius);

        //! Get height
        float getHeight() const;

        //! Set height
        void setHeight(float radius);

    protected:
        //! Create collision shape
        void createCollisionShape(float radius, float height);

        //! Recreate collision shape once size changed
        void recreateCollisionShape(float radius, float height);

        //! Set local scale of the box
        virtual void setLocalScale(const Vec3& scale) override;

    protected:
        //! Radius
        float m_radius;

        //! Height
        float m_height;
    };
}