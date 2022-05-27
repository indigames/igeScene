#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/Collider.h"

namespace ige::scene
{
    //! SphereCollider
    class SphereCollider : public Collider
    {
    public:
        //! Constructor
        SphereCollider(SceneObject &owner, float radius = 1.f);

        //! Destructor
        virtual ~SphereCollider();

        //! Get name
        std::string getName() const override { return "SphereCollider"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::SphereCollider; }

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
        virtual void createShape() override;

        //! Set local scale of the box
        virtual void setScale(const Vec3 &scale) override;

    protected:
        float m_radius;
    };
} // namespace ige::scene