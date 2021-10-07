#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/Component.h"
#include "components/TextComponent.h"
#include "core/Text.h"

namespace ige::scene
{
    //! TextBitmapComponent
    class TextBitmapComponent : public TextComponent
    {
    public:
        //! Constructor
        TextBitmapComponent(SceneObject &owner, const std::string& text = "", const std::string& fontPath = "fontBitmaps/dejavu-sans_0.pybm", int fontSize = 12, const Vec4& color = { 1.f, 1.f, 1.f, 1.f }, bool isBillboard = false, bool isGUI = false);

        //! Destructor
        virtual ~TextBitmapComponent();

        //! Get component name
        virtual std::string getName() const override { return "TextBitmap"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::TextBitmap; }

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;
    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

    };
} // namespace ige::scene
