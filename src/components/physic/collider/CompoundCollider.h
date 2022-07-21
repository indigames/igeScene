#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/Collider.h"

namespace ige::scene
{
    //! CapsuleCollider
    class CompoundCollider : public Collider
    {
    public:
        //! Constructor
        CompoundCollider(SceneObject& owner);

        //! Destructor
        virtual ~CompoundCollider();

        //! Get name
        std::string getName() const override { return "CompoundCollider"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::CompoundCollider; }

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:

        //! Create collision shape
        virtual void createShape() override;
    };
}