#include "components/gui/UIText.h"
#include "components/gui/RectTransform.h"
#include "components/gui/UIMask.h"

#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

#include "event/EventContext.h"

namespace ige::scene
{
    //! Constructor

    UIText::UIText(SceneObject& owner, const std::string& text, const std::string& fontPath, int fontSize, const Vec4& color)
        : Component(owner), UIMaskable()
    {
        m_textData = text;
        m_fontPath = fontPath;
        m_color = color;
        m_fontSize = fontSize;
        m_fontType = 0;

        generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);
        setStencilMask(getOwner()->isInMask() ? 1 : -1);

        getOwner()->addEventListener((int)EventType::SetParent, [this](auto vol) {
            this->onSetParent(vol);
            }, m_instanceId);
    }

    UIText::UIText(SceneObject& owner, const std::string &text, const std::string &fontPath, int fontSize, const Vec4 &color, int fontType)
        : Component(owner), UIMaskable()
    {
        m_textData = text;
        m_fontPath = fontPath;
        m_color = color;
        m_fontSize = fontSize;
        m_fontType = fontType;

        generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);
        setStencilMask(getOwner()->isInMask() ? 1 : -1);

        getOwner()->addEventListener((int)EventType::SetParent, [this](auto vol) {
            this->onSetParent(vol);
            }, m_instanceId);
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
        
        updatePosition();
    }

    //! Render
    void UIText::onRender()
    {
    }

    void UIText::updatePosition()
    {
        auto transform = getOwner()->getRectTransform();
        auto size = m_text->getSize();
        auto containerSize = transform->getSize();

        auto margin = (0.1f * size[1]);
        auto rectSize = Vec2(std::max(size[0] + (0.6f * size[1]), 32.f), std::max(size[1] + (0.6f * size[1]), 32.f));

        if (isRectAutoScale()) {
            transform->setSize(rectSize);
            containerSize = rectSize;
        }

        auto center = Vec3(0, 0, 0);
        switch (m_textAlignHorizontal) {
        case 0:
            center[0] -= (containerSize[0] - size[0]) * 0.5f - margin;
            break;
        case 1:
            break;
        case 2:
            center[0] += (containerSize[0] - size[0]) * 0.5f - margin;
            break;
        }

        switch (m_textAlignVertical) {
        case 0:
            center[1] += (containerSize[1] - size[1]) * 0.5f - margin;
            break;
        case 1:
            break;
        case 2:
            center[1] -= (containerSize[1] - size[1]) * 0.5f - margin;
            break;
        }

        // Update figure
        if (getFigure()) {
            getFigure()->SetRotation(transform->getRotation());
            getFigure()->SetScale(transform->getScale());
            getFigure()->SetPosition(transform->localToGlobal(center));
            getFigure()->Pose();
        }
    }

    void UIText::setTextAlignHorizontal(int val)
    {
        auto _val = MATH_CLAMP(val, 0, 2);
        if (m_textAlignHorizontal != _val) {
            m_textAlignHorizontal = _val;
            updatePosition();
        }
    }

    void UIText::setTextAlignVertical(int val)
    {
        auto _val = MATH_CLAMP(val, 0, 2);
        if (m_textAlignVertical != _val) {
            m_textAlignVertical = _val;
            updatePosition();
        }
    }

    void UIText::onResourceAdded(Resource* res) {
        if (m_bResAdded || res == nullptr) return;
        if (getOwner() && getOwner()->getScene()) {
            getOwner()->getScene()->getUIResourceAddedEvent().invoke(res);
            m_bResAdded = true;
        }
        if (res != nullptr) {
            setStencilMask(getOwner()->isInMask() ? 1 : -1);
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
        j["alignhorizontal"] = getTextAlignHorizontal();
        j["alignvertical"] = getTextAlignVertical();
        j["rectScale"] = isRectAutoScale();
    }

    //! Deserialize
    void UIText::from_json(const json &j)
    {
        m_textAlignHorizontal = (j.value("alignhorizontal", 0));
        m_textAlignVertical = (j.value("alignvertical", 0));
        setText(j.value("text", std::string()));
        setFontPath(j.value("font", "fonts/Manjari-Regular.ttf"));
        setFontSize(j.value("size", 12));
        setColor(j.value("color", Vec4(0.5f, 0.5f, 0.5f, 1.0f)));
        setRectAutoScale(j.value("rectScale", false));
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
            if (oldFigure)
                onResourceRemoved(oldFigure);
            m_text->setText(text, m_fontType);
            auto newFigure = m_text->getFigure();
            if (newFigure)
                onResourceAdded(newFigure);
        }
        getOwner()->getTransform()->makeDirty();
    }

    //! Font Path
    void UIText::setFontPath(const std::string &path)
    {
        auto tmpPath = path;
        std::replace(tmpPath.begin(), tmpPath.end(), '\\', '/');
        auto ext = fs::path(tmpPath).extension().string();
        m_fontType = (ext.compare(".pybm") == 0) ? 1 : 0;
        if (m_fontPath.compare(tmpPath) != 0) {
            m_fontPath = tmpPath;
            if (m_text == nullptr) {
                generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);
            }
            else {
                auto oldFigure = m_text->getFigure();
                if (oldFigure)
                    onResourceRemoved(oldFigure);
                m_text->setFontPath(m_fontPath);
                auto newFigure = m_text->getFigure();
                if (newFigure)
                    onResourceAdded(newFigure);
            }
            getOwner()->getTransform()->makeDirty();
        }
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
            if (oldFigure)
                onResourceRemoved(oldFigure);
            m_text->setFontSize(size);
            auto newFigure = m_text->getFigure();
            if (newFigure)
                onResourceAdded(newFigure);
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
        else if (key.compare("alignhorizontal") == 0)
        {
            setTextAlignHorizontal(val);
        }
        else if (key.compare("alignvertical") == 0)
        {
            setTextAlignVertical(val);
        }
        else if (key.compare("rectScale") == 0)
        {
            setRectAutoScale(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }
} // namespace ige::scene
