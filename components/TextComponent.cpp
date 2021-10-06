#include "components/TextComponent.h"
#include "components/TransformComponent.h"
#include "components/gui/RectTransform.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/GraphicsHelper.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Constructor
    TextComponent::TextComponent(SceneObject &owner, const std::string& text, const std::string& fontPath, int fontSize, const Vec4& color,  bool isBillboard, bool isGUI)
        : Component(owner), m_bIsGUI(isGUI)
    {
        m_textData = text;
        m_fontPath = fontPath;
        m_color = color;
        m_fontSize = fontSize;
        m_fontType = 0;

        generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);

        setBillboard(isBillboard);
    }

    //! Destructor
    TextComponent::~TextComponent()
    {
        if (m_text != nullptr && m_text->getFigure() && getOwner()->getScene()) {
            onRemoveFigure(m_text->getFigure());
        }
        m_text = nullptr;

        if (getOwner()->getTransform())
            getOwner()->getTransform()->makeDirty();
    }

    //! Enable
    void TextComponent::onEnable()
    {
        if (!getOwner()->isActive() || !isEnabled()) return;
        Component::onEnable();
        if (m_text != nullptr && m_text->getFigure() && getOwner()->getScene()) {
            onCreateFigure(m_text->getFigure());
        }
    }

    //! Disable
    void TextComponent::onDisable()
    {
        if (m_text != nullptr && m_text->getFigure() && getOwner()->getScene()) {
            onRemoveFigure(m_text->getFigure());
        }
        Component::onDisable();
    }

    //! Update
    void TextComponent::onUpdate(float dt)
    {
        if (getFigure() == nullptr)
            return;

        // Update transform from transform component
        auto transform = getOwner()->getTransform();
        getFigure()->SetPosition(transform->getWorldPosition());
        getFigure()->SetRotation(transform->getWorldRotation());
        getFigure()->SetScale(transform->getWorldScale());

        // Update
        getFigure()->Pose();
    }

    //! Update
    void TextComponent::onRender()
    {
    }

    void TextComponent::generateText(const std::string& text, const std::string& fontPath, int fontSize, const Vec4& color, int fontType) {
        if (m_text != nullptr)
        {
            //!Release old
            auto oldFigure = m_text->getFigure();
            onRemoveFigure(oldFigure);
        }
        m_text = std::make_shared<Text>(text, fontPath, fontSize, color, fontType);
        if (m_text != nullptr) {
            auto newFigure = m_text->getFigure();
            onCreateFigure(newFigure);
        }
    }

    //! Set Text
    void TextComponent::setText(const std::string& text)
    {
        m_textData = text;
        if (m_text == nullptr) {
            generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);
        }
        else {
            auto oldFigure = m_text->getFigure();
            m_text->setText(m_textData);
            auto newFigure = m_text->getFigure();
            if (oldFigure != newFigure)
            {
                if (oldFigure)
                    onRemoveFigure(oldFigure);
                if (newFigure)
                    onCreateFigure(newFigure);
            }
        }
        getOwner()->getTransform()->makeDirty();
    }

    //! Set path
    void TextComponent::setFontPath(const std::string &path)
    {
        m_fontPath = path;
        if (m_text == nullptr) {
            generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);
        }
        else
        {
            auto oldFigure = m_text->getFigure();
            m_text->setFontPath(path);
            auto newFigure = m_text->getFigure();
            if (oldFigure != newFigure)
            {
                if (oldFigure)
                    onRemoveFigure(oldFigure);
                if (newFigure)
                    onCreateFigure(newFigure);
            }
        }
        getOwner()->getTransform()->makeDirty();
    }

    //! Set size
    void TextComponent::setFontSize(int size)
    {
        m_fontSize = size;
        if (m_text == nullptr) {
            generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);
        }
        else
        {
            auto oldFigure = m_text->getFigure();
            m_text->setFontSize(size);
            auto newFigure = m_text->getFigure();
            if (oldFigure != newFigure)
            {
                if (oldFigure)
                    onRemoveFigure(oldFigure);
                if (newFigure)
                    onCreateFigure(newFigure);
            }
        }
        getOwner()->getTransform()->makeDirty();
    }

    //! Set billboard
    void TextComponent::setBillboard(bool isBillboard)
    {
        if (m_bIsBillboard != isBillboard)
        {
            m_bIsBillboard = isBillboard;
            if (m_text) {
                auto figure = m_text->getFigure();
                if (figure)
                {
                    auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                    shaderDesc->SetValue(figure->GetShaderName(0));
                    shaderDesc->SetBillboard(m_bIsBillboard);
                    figure->SetShaderName(0, shaderDesc->GetValue());
                }
            }
        }
    }

    //! Set Alpha
    void TextComponent::setAlpha(float value)
    {
        if (m_text) {
            auto color = m_text->getColor();
            color[3] = value;
            setColor(color);
        }
    }

    const float TextComponent::getAlpha() const
    {
        if(m_text)
            return m_text->getColor()[3];
        return 0;
    }

    //! Set Color
    void TextComponent::setColor(const Vec4& value)
    {
        if (m_color != value) {
            m_color = value;
            if (m_text == nullptr) {
                generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);
            }
            else
                m_text->setColor(m_color);
        }
    }

    void TextComponent::onCreateFigure(EditableFigure* fig) {
        if (m_bIsGUI)
            getOwner()->getScene()->getUIResourceAddedEvent().invoke(fig);
        else
            getOwner()->getScene()->getResourceAddedEvent().invoke(fig);
    }

    void TextComponent::onRemoveFigure(EditableFigure* fig) {
        if (m_bIsGUI)
            getOwner()->getScene()->getUIResourceRemovedEvent().invoke(fig);
        else
            getOwner()->getScene()->getResourceRemovedEvent().invoke(fig);
    }

    //! Serialize
    void TextComponent::to_json(json &j) const
    {
        Component::to_json(j);
        j["text"] = getText();
        j["font"] = getFontPath();
        j["size"] = getFontSize();
        j["color"] = getColor();
        j["billboard"] = isBillboard();
    }

    //! Deserialize
    void TextComponent::from_json(const json &j)
    {
        setText(j.at("text"));
        setFontPath(j.at("font"));
        setFontSize(j.at("size"));
        setColor(j.at("color"));
        setBillboard(j.value("billboard", false));
        Component::from_json(j);
    }

    //! Update property by key value
    void TextComponent::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("text") == 0)
        {
            setText(val);
        }
        else if (key.compare("font") == 0)
        {
            setFontPath(val);
        }
        else if (key.compare("size") == 0)
        {
            setFontSize(val);
        }
        else if (key.compare("color") == 0)
        {
            setColor(val);
        }
        else if (key.compare("billboard") == 0)
        {
            setBillboard(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }
} // namespace ige::scene
