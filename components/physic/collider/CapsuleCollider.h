#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/Collider.h"

namespace ige::scene
{
    //! CapsuleCollider
    class CapsuleCollider : public Collider
    {
    public:
        //! Constructor
        CapsuleCollider(SceneObject& owner);

        //! Destructor
        virtual ~CapsuleCollider();

        //! Get name
        std::string getName() const override { return "CapsuleCollider"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::CapsuleCollider; }

        //! Get radius
        float getRadius() const;

        //! Set radius
        void setRadius(float radius);

        //! Get height
        float getHeight() const;

        //! Set height
        void setHeight(float radius);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;
        virtual void from_json(const json& j) override;

        //! Create collision shape
        virtual void createShape() override;

    protected:
        //! Radius
        float m_radius = 0.f;

        //! Height
        float m_height = 0.f;
    };
}