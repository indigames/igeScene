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
    class UIText : public Component, protected UIMaskable
    {
    public:
        //! Constructor
        UIText(SceneObject &owner, const std::string &text = "", const std::string &fontPath = "fonts/Manjari-Regular.ttf", int fontSize = 12, const Vec4 &color = {0.5f, 0.5f, 0.5f, 1.f});

        //! Destructor
        virtual ~UIText();

        //! Get component name
        virtual std::string getName() const override { return "UIText"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::UIText; }

        //! Enable
        virtual void onEnable() override;

        //! Disable
        virtual void onDisable() override;

        //! Update
        virtual void onUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

        //! Text
        const std::string getText() const { return m_text->getText(); }
        virtual void setText(const std::string &text);

        //! Font Path
        const std::string &getFontPath() const { return m_text->getFontPath(); }
        void setFontPath(const std::string &path);

        //! Font Size
        int getFontSize() const { return m_text->getFontSize(); }
        void setFontSize(int size);

        //! Color
        const Vec4 &getColor() const { return m_text->getColor(); }
        void setColor(const Vec4 &color);


        //Text Align Horizontal
        int getTextAlignHorizontal() const { return m_textAlignHorizontal; }
        void setTextAlignHorizontal(int val);

        //Text Align Vertical
        int getTextAlignVertical() const { return m_textAlignVertical; }
        void setTextAlignVertical(int val);

        //! Get/Set rect scale
        void setRectAutoScale(bool scale = true) { m_bRectAutoScale = scale; }
        bool isRectAutoScale() const { return m_bRectAutoScale; }

        //! Figure
        EditableFigure *getFigure() { return m_text->getFigure(); }

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        UIText(SceneObject& owner, const std::string& text, const std::string& fontPath, int fontSize, const Vec4& color, int fontType);

        //! Overide function UIMaskable
        virtual EditableFigure* getCurrentFigure() override;
        virtual SceneObject* getSceneObjectOwner() override;

        virtual void generateText(const std::string& text, const std::string& fontPath, int fontSize, const Vec4& color, int fontType);

        virtual void onResourceAdded(Resource* res);
        virtual void onResourceRemoved(Resource* res);

        virtual void updatePosition();

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Text
        std::shared_ptr<Text> m_text;
        std::string m_textData = "";
        std::string m_fontPath = "";
        int m_fontSize = 11;
        int m_fontType = 0;
        Vec4 m_color = { 0.5f, 0.5f, 0.5f, 1.f };
        bool m_bResAdded = false;
        int m_textAlignHorizontal = 1;
        int m_textAlignVertical = 1;
        bool m_bRectAutoScale = false;
    };
} // namespace ige::scene
