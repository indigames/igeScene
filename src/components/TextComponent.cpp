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
    TextComponent::TextComponent(SceneObject& owner, const std::string& text, const std::string& fontPath, int fontSize, const Vec4& color,  bool isBillboard, bool isGUI)
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

    TextComponent::TextComponent(SceneObject& owner, const std::string& text, const std::string& fontPath, int fontSize, const Vec4& color, bool isBillboard, bool isGUI, int fontType)
        : Component(owner), m_bIsGUI(isGUI)
    {
        m_textData = text;
        m_fontPath = fontPath;
        m_color = color;
        m_fontSize = fontSize;
        m_fontType = fontType;

        generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);

        setBillboard(isBillboard);
    }

    //! Destructor
    TextComponent::~TextComponent()
    {
        if (m_text != nullptr && m_text->getFigure() && getOwner()->getScene()) {
            onResourceRemoved(m_text->getFigure());
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
            onResourceAdded(m_text->getFigure());
        }
    }

    //! Disable
    void TextComponent::onDisable()
    {
        if (m_text != nullptr && m_text->getFigure() && getOwner()->getScene()) {
            onResourceRemoved(m_text->getFigure());
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
        getFigure()->SetPosition(transform->getPosition());
        getFigure()->SetRotation(transform->getRotation());
        getFigure()->SetScale(transform->getScale());

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
            onResourceRemoved(oldFigure);
        }
        m_text = std::make_shared<Text>(text, fontPath, fontSize, color, fontType, true);
        if (m_text != nullptr) {
            auto newFigure = m_text->getFigure();
            onResourceAdded(newFigure);
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
            m_text->setText(m_textData, m_fontType);
            auto newFigure = m_text->getFigure();
            if (oldFigure != newFigure)
            {
                if (oldFigure)
                    onResourceRemoved(oldFigure);
                if (newFigure)
                    onResourceAdded(newFigure);
            }
        }
        getOwner()->getTransform()->makeDirty();
    }

    //! Set path
    void TextComponent::setFontPath(const std::string &path)
    {
        auto tmpPath = path;
        std::replace(tmpPath.begin(), tmpPath.end(), '\\', '/');
        m_fontPath = tmpPath;

        if (m_text == nullptr) {
            generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);
        }
        else {
            auto oldFigure = m_text->getFigure();
            m_text->setFontPath(m_fontPath);
            auto newFigure = m_text->getFigure();
            if (oldFigure != newFigure)
            {
                if (oldFigure)
                    onResourceRemoved(oldFigure);
                if (newFigure)
                    onResourceAdded(newFigure);
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
            if (oldFigure != newFigure) {
                if (oldFigure)
                    onResourceRemoved(oldFigure);
                if (newFigure)
                    onResourceAdded(newFigure);
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
            {
                auto oldFigure = m_text->getFigure();
                m_text->setColor(m_color);
                auto newFigure = m_text->getFigure();
                if (oldFigure != newFigure)
                {
                    if (oldFigure)
                        onResourceRemoved(oldFigure);
                    if (newFigure)
                        onResourceAdded(newFigure);
                }
            }
        }
    }

    void TextComponent::onResourceAdded(Resource* res) {
        if (m_bResAdded || res == nullptr) return;
        if (getOwner()->isActive(true) && getOwner()->getScene()) {
            if (m_bIsGUI)
                getOwner()->getScene()->getUIResourceAddedEvent().invoke(res);
            else
                getOwner()->getScene()->getResourceAddedEvent().invoke(res);
            m_bResAdded = true;
        }
    }

    void TextComponent::onResourceRemoved(Resource* res) {
        if (!m_bResAdded || res == nullptr) return;
        if (getOwner()->getScene()) {
            if (m_bIsGUI)
                getOwner()->getScene()->getUIResourceRemovedEvent().invoke(res);
            else
                getOwner()->getScene()->getResourceRemovedEvent().invoke(res);
            m_bResAdded = false;
        }
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
        m_fontType = 0;
        m_textData = j.value("text", "");
        m_fontPath = j.value("font", "fonts/Manjari-Regular.ttf");
        m_fontSize = j.value("size", 12);
        m_color = j.value("color", Vec4(1.f, 1.f, 1.f, 1.f));
        generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);
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
