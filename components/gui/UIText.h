#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/SpriteComponent.h"
#include "core/Text.h"
#include "components/gui/UIMaskable.h"

namespace ige::scene
{
    class UIEventContext;
    class EventContext;

    //! UIText
    class UIText : public Component, UIMaskable
    {
    public:
        //! Constructor
        UIText(SceneObject &owner, const std::string &text = "", const std::string &fontPath = "fonts/Manjari-Regular.ttf", int fontSize = 12, const Vec4 &color = {1.f, 1.f, 1.f, 1.f});

        //! Destructor
        virtual ~UIText();

        //! Get component name
        virtual std::string getName() const override { return "UIText"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::UIText; }

        //! Update
        virtual void onUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

        //! Text
        const std::string getText() const { return m_text->getText(); }
        void setText(const std::string &text);

        //! Font Path
        const std::string &getFontPath() const { return m_text->getFontPath(); }
        void setFontPath(const std::string &path);

        //! Font Size
        int getFontSize() const { return m_text->getFontSize(); }
        void setFontSize(int size);

        //! Color
        const Vec4 &getColor() const { return m_text->getColor(); }
        void setColor(const Vec4 &color);

        //! Figure
        EditableFigure *getFigure() { return m_text->getFigure(); }

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

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
