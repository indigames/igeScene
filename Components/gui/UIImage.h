#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/SpriteComponent.h"

namespace ige::scene
{
    //! UIImage
    class UIImage : public SpriteComponent
    {
    public:
        //! Constructor
        UIImage(const std::shared_ptr<SceneObject>& owner, const std::string& texture = "", const Vec2& size = {128.f, 128.f});

        //! Destructor
        virtual ~UIImage();

        //! Get component name
        virtual std::string getName() const override { return "UIImage"; }

        //! Update
        virtual void onUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

    };
}
