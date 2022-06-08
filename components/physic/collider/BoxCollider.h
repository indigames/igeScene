#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/Collider.h"

namespace ige::scene
{
    //! BoxCollider
    class BoxCollider : public Collider
    {
    public:
        //! Constructor
        BoxCollider(SceneObject &owner);

        //! Destructor
        virtual ~BoxCollider();

        //! Get name
        std::string getName() const override { return "BoxCollider"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::BoxCollider; }

        //! Size
        const Vec3 &getSize() const;
        void setSize(const Vec3 &size);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json &j) const override;

        //! Serialize finished event
        virtual void onSerializeFinished(Scene* scene) override;

        //! Create shape
        virtual void createShape() override;

    protected:
        //! Box size
        Vec3 m_size = { 0.f, 0.f, 0.f };
    };
} // namespace ige::scene