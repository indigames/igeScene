#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/SpriteComponent.h"
#include "core/Text.h"
#include "components/gui/UIMaskable.h"
#include "components/gui/UIText.h"

namespace ige::scene
{
    class UIEventContext;
    class EventContext;

    //! UIText
    class UITextBitmap : public UIText
    {
    public:
        //! Constructor
        UITextBitmap(SceneObject &owner, const std::string &text = "", const std::string &fontTexturePath = "fontBitmaps/dejavu-sans_0.pybm", int fontSize = 12, const Vec4 &color = {1.f, 1.f, 1.f, 1.f});

        //! Destructor
        virtual ~UITextBitmap();

        //! Get component name
        virtual std::string getName() const override { return "UITextBitmap"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::UITextBitmap; }

    protected:
        //! Overide function UIMaskable
        virtual EditableFigure* getCurrentFigure() override;
        virtual SceneObject* getSceneObjectOwner() override;

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Text
        std::shared_ptr<Text> m_text;

        bool m_flagMask = false;
    };
} // namespace ige::scene
