#include "components/gui/UIText.h"
#include "components/gui/RectTransform.h"
#include "components/gui/UIMask.h"

#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "event/EventContext.h"

namespace ige::scene
{
    //! Constructor

    UIText::UIText(SceneObject& owner, const std::string& text, const std::string& fontPath, int fontSize, const Vec4& color)
        : Component(owner), UIMaskable(), m_flagMask(false)
    {
        m_bResAdded = false;
        m_textData = text;
        m_fontPath = fontPath;
        m_color = color;
        m_fontSize = fontSize;
        m_fontType = 0;

        generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);

        getOwner()->addEventListener((int)EventType::SetParent, [this](auto vol) {
            this->onSetParent(vol);
            }, m_instanceId);

        if (getOwner()->isInMask())
        {
            if (getFigure() == nullptr) m_flagMask = true;
            setStencilMask(1);
        }
    }

    UIText::UIText(SceneObject &owner, const std::string &text, const std::string &fontPath, int fontSize, const Vec4 &color, int fontType)
        : Component(owner), UIMaskable(), m_flagMask(false)
    {
        m_bResAdded = false;
        m_textData = text;
        m_fontPath = fontPath;
        m_color = color;
        m_fontSize = fontSize;
        m_fontType = fontType;

        generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);

        getOwner()->addEventListener((int)EventType::SetParent, [this](auto vol) {
            this->onSetParent(vol);
            }, m_instanceId);

        if (getOwner()->isInMask())
        {
            if (getFigure() == nullptr) m_flagMask = true;
            setStencilMask(1);
        }
    }

    //! Destructor
    UIText::~UIText()
    {
        if (m_text != nullptr && m_text->getFigure() && getOwner()->getScene()) {
            onResourceRemoved(m_text->getFigure());
        }
        m_text = nullptr;
        
        getOwner()->removeEventListener((int)EventType::SetParent, m_instanceId);

        if(getOwner()->getTransform())
            getOwner()->getTransform()->makeDirty();
    }

    //! Enable
    void UIText::onEnable()
    {
        if (!getOwner()->isActive() || !isEnabled()) return;
        Component::onEnable();
        if (m_text != nullptr && m_text->getFigure() && getOwner()->getScene()) {
            onResourceAdded(m_text->getFigure());
        }
    }

    //! Disable
    void UIText::onDisable()
    {
        if (m_text != nullptr && m_text->getFigure() && getOwner()->getScene()) {
            onResourceRemoved(m_text->getFigure());
        }
        Component::onDisable();
    }

    //! Update
    void UIText::onUpdate(float dt)
    {
        if (getFigure() == nullptr)
            return;

        if (m_flagMask) {
            setStencilMask(1);
            m_flagMask = false;
        }
        
        // Update transform from transform component
        auto transform = getOwner()->getRectTransform();

        // Scale container to fit text size
        auto containerSize = transform->getSize();
        auto size = m_text->getSize();
        auto sizeChanged = false;
        if (size.X() > containerSize.X())
        {
            containerSize.X(size.X() + 1.f);
            sizeChanged = true;
        }
        if (size.Y() > containerSize.Y())
        {
            containerSize.Y(size.Y() + 1.f);
            sizeChanged = true;
        }
        if (sizeChanged)
            transform->setSize(containerSize);

        getFigure()->SetPosition(transform->getPosition());
        getFigure()->SetRotation(transform->getRotation());
        getFigure()->SetScale(transform->getScale());

        // Update
        getFigure()->Pose();
    }

    //! Render
    void UIText::onRender()
    {
        /*if (getFigure() == nullptr)
            return;
        getFigure()->Render();*/
    }

    void UIText::onResourceAdded(Resource* res) {
        if (m_bResAdded || res == nullptr) return;
        if (getOwner() && getOwner()->isActive(true) && getOwner()->getScene()) {
            getOwner()->getScene()->getUIResourceAddedEvent().invoke(res);
            m_bResAdded = true;
        }
    }

    void UIText::onResourceRemoved(Resource* res) {
        if (!m_bResAdded || res == nullptr) return;
        if (getOwner() && getOwner()->getScene()) {
            getOwner()->getScene()->getUIResourceRemovedEvent().invoke(res);
            m_bResAdded = false;
        }
    }

    void UIText::generateText(const std::string& text, const std::string& fontPath, int fontSize, const Vec4& color, int fontType) {
        if (m_text != nullptr)
        {
            //!Release old
            auto oldFigure = m_text->getFigure();
            onResourceRemoved(oldFigure);
        }
        m_text = std::make_shared<Text>(text, fontPath, fontSize, color, fontType);
        if (m_text != nullptr) {
            auto newFigure = m_text->getFigure();
            onResourceAdded(newFigure);
        }
    }
    
    //! Serialize
    void UIText::to_json(json &j) const
    {
        Component::to_json(j);
        j["text"] = getText();
        j["font"] = getFontPath();
        j["size"] = getFontSize();
        j["color"] = getColor();
    }

    //! Deserialize
    void UIText::from_json(const json &j)
    {
        setText(j.at("text"));
        setFontPath(j.at("font"));
        setFontSize(j.at("size"));
        setColor(j.at("color"));
        Component::from_json(j);
    }

    //! Text
    void UIText::setText(const std::string &text)
    {
        m_textData = text;
        if (m_text == nullptr) {
            generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);
        }
        else {
            auto oldFigure = m_text->getFigure();
            m_text->setText(text);
            auto newFigure = m_text->getFigure();
            if (oldFigure != newFigure)
            {
                if (oldFigure) {
                    onResourceRemoved(oldFigure);
                }
                if (newFigure)
                    onResourceAdded(newFigure);
            }
        }
        getOwner()->getTransform()->makeDirty();
    }

    //! Font Path
    void UIText::setFontPath(const std::string &path)
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
            if (oldFigure != newFigure) {
                if (oldFigure)
                    onResourceRemoved(oldFigure);
                if (newFigure)
                    onResourceAdded(newFigure);
            }
        }
        getOwner()->getTransform()->makeDirty();
    }

    //! Font Size
    void UIText::setFontSize(int size)
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
                    onResourceRemoved(oldFigure);
                if (newFigure)
                    onResourceAdded(newFigure);
            }
        }
        getOwner()->getTransform()->makeDirty();
    }

    //! Color
    void UIText::setColor(const Vec4 &color)
    {
        if (m_color != color) {
            m_color = color;
            if (m_text == nullptr) {
                generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);
            }
            else
                m_text->setColor(color);
        }
    }

    EditableFigure* UIText::getCurrentFigure() { return getFigure(); }
    SceneObject* UIText::getSceneObjectOwner() { return getOwner(); }

    //! Update property by key value
    void UIText::setProperty(const std::string& key, const json& val)
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
        else
        {
            Component::setProperty(key, val);
        }
    }
} // namespace ige::scene
