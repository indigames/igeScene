#include "components/gui/UIButton.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/GraphicsHelper.h"

#include "utils/filesystem.h"

#include "event/EventContext.h"
namespace fs = ghc::filesystem;

NS_IGE_SCENE_BEGIN

UIButton::UIButton(SceneObject& owner, const std::string& texture, const Vec2& size) : 
	UIImage(owner, texture, size),
    m_normalTexture(nullptr), m_highlightedTexture(nullptr), m_pressedTexture(nullptr), m_selectedTexture(nullptr), m_disabledTexture(nullptr),
	m_normalColor(0, 0, 0, 1), m_highlightedColor(0, 0, 0, 1), m_pressedColor(0, 0, 0, 1), m_disableColor(0, 0, 0, 1),
	m_fadeDuration(0.1f)
{
    m_btnState = ButtonState::NORMAL;
    if(m_sprite)
        m_normalTexture = m_sprite->getTexture();

    getOwner()->addEventListener((int)EventType::TouchBegin, std::bind(&UIButton::_onTouchPress, this, std::placeholders::_1));
    getOwner()->addEventListener((int)EventType::Click, std::bind(&UIButton::_onTouchRelease, this, std::placeholders::_1));
}

UIButton::~UIButton()
{

}

Texture* UIButton::_loadTexture(std::string path)
{
	if (path.empty())
		return nullptr;

    auto fsPath = fs::path(path);
    auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
    std::replace(relPath.begin(), relPath.end(), '\\', '/');

    if (strcmp(relPath.c_str(), path.c_str()) != 0)
    {
        auto m_path = relPath;
        std::replace(m_path.begin(), m_path.end(), '\\', '/');

        if (m_path.length() > 0)
        {
            auto texture = ResourceCreator::Instance().NewTexture(m_path.c_str());
            return texture;
        }
    }
    return nullptr;
}

void UIButton::_releaseTexture(std::string path) {
    // Free old texture
    if (!path.empty())
    {
        auto texture = (Texture*)ResourceManager::Instance().GetResource(path.c_str(), TEXTURETYPE);
        if (texture)
        {
            texture->DecReference();
            texture = nullptr;
        }
    }
}

void UIButton::_onTouchPress(EventContext* context)
{
    changeState(ButtonState::PRESSED);
}

void UIButton::_onTouchRelease(EventContext* context)
{
    changeState(ButtonState::NORMAL);
    onClick();
}

void UIButton::_onSelected(EventContext* context)
{

}

void UIButton::_onExit(EventContext* context)
{

}


void UIButton::changeState(ButtonState state) 
{
    if (m_btnState != state) {
        m_btnState = state;
    }
}

void UIButton::setTransitionMode(TransitionMode value)
{
    if (m_transitionMode != value) 
    {
        m_transitionMode == value;
    }
}

void UIButton::setAnimationMode(AnimationMode value)
{
    if (m_animationMode != value)
    {
        m_animationMode == value;
    }
}

//! Json 
void UIButton::to_json(json& j) const
{
    Component::to_json(j);
    j["path"] = getPath();
    j["size"] = getSize();
    j["fillmethod"] = getFillMethod();
    j["fillorigin"] = getFillOrigin();
    j["fillamount"] = getFillAmount();
    j["alpha"] = getAlpha();
}

//! Deserialize
void UIButton::from_json(const json& j)
{
    setSize(j.at("size"));
    setFillMethod(j.at("fillmethod"));
    setFillOrigin(j.at("fillorigin"));
    setFillAmount(j.at("fillamount"));
    setAlpha(j.at("alpha"));
    setPath(j.at("path"));
    Component::from_json(j);
}

NS_IGE_SCENE_END