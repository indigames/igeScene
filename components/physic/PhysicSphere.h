#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/PhysicObject.h"

namespace ige::scene
{
    //! PhysicSphere
    class PhysicSphere : public PhysicObject
    {
    public:
        //! Constructor
        PhysicSphere(SceneObject &owner, float radius = 1.f);

        //! Destructor
        virtual ~PhysicSphere();

        //! Get name
        std::string getName() const override { return "PhysicSphere"; }

        //! Get radius
        float getRadius() const;

        //! Set radius
        void setRadius(float radius);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Create collision shape
        void createCollisionShape(float radius);

        //! Recreate collision shape once radius changed
        void recreateCollisionShape(float radius);

        //! Set local scale of the box
        virtual void setLocalScale(const Vec3 &scale) override;

    protected:
        float m_radius;
    };
} // namespace ige::scene