#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/Component.h"
#include "core/Text.h"

namespace ige::scene
{
    //! TextComponent
    class TextComponent : public Component
    {
    public:
        //! Constructor
        TextComponent(SceneObject& owner, const std::string& text = "", const std::string& fontPath = "fonts/Manjari-Regular.ttf", int fontSize = 12, const Vec4& color = { 1.f, 1.f, 1.f, 1.f }, bool isBillboard = false, bool isGUI = false);

        //! Destructor
        virtual ~TextComponent();

        //! Get component name
        virtual std::string getName() const override { return "Text"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::Text; }

        //! Enable
        virtual void onEnable() override;

        //! Disable
        virtual void onDisable() override;

        //! Update
        virtual void onUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

        //! Figure
        EditableFigure* getFigure() { if (m_text) return m_text->getFigure(); return nullptr; }

        //! Text
        const std::string getText() const { return m_textData; }
        virtual void setText(const std::string& text);

        //! Font Path
        virtual const std::string& getFontPath() const { return m_fontPath; }
        virtual void setFontPath(const std::string& path);

        //! Font Size
        virtual int getFontSize() const { return m_fontSize; }
        virtual void setFontSize(int size);

        //! Color
        virtual const Vec4& getColor() const { return m_color; }
        virtual void setColor(const Vec4& color);

        //! Billboard
        void setBillboard(bool isBillboard = true);
        const bool isBillboard() const { return m_bIsBillboard; }

        //! Alpha
        virtual void setAlpha(float value);
        virtual const float getAlpha() const;

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;
    protected:
        TextComponent(SceneObject& owner, const std::string& text, const std::string& fontPath, int fontSize, const Vec4& color, bool isBillboard, bool isGUI, int fontType);

        //! Utils to add/remove resource from showcase
        void onResourceAdded(Resource* res);
        void onResourceRemoved(Resource* res);
        bool m_bResAdded = false;

        virtual void generateText(const std::string& text, const std::string& fontPath, int fontSize, const Vec4& color, int fontType);

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

    protected:

        //! Text
        std::shared_ptr<Text> m_text;

        //! Billboard setting
        bool m_bIsBillboard = false;
        bool m_bIsGUI = false;
        std::string m_textData = "";
        std::string m_fontPath = "";
        int m_fontSize = 11;
        int m_fontType = 0;
        Vec4 m_color = { 1.f, 1.f, 1.f, 1.f };
    };
} // namespace ige::scene
