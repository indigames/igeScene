#ifndef __UI_BUTTON_H__
#define __UI_BUTTON_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

#include "components/gui/UIImage.h"
#include "event/Event.h"

USING_NS_PYXIE

NS_IGE_SCENE_BEGIN

class UIImage;
class UIEventContext;
class EventContext;

enum TransitionMode {
	ColorTint,
	SpriteSwap,
	Custom
};

enum AnimationMode {
	None,
	Elastic,
	Bounce,
};

enum class ButtonState {
	NORMAL = 0,
	HIGHLIGHT = 1,
	PRESSED = 2,
	SELECTED = 3,
	DISABLE = -1
};

class UIButton : public UIImage
{
public:

	UIButton(SceneObject& owner, const std::string& texture = "", const Vec2& size = { 128.f, 128.f });
	virtual ~UIButton();

	//! Get component name
	virtual std::string getName() const override { return "UIButton"; }

	bool isSelected() const { return m_btnState == ButtonState::SELECTED; }
	
	TransitionMode getTransitionMode() const { return m_transitionMode; }
	void setTransitionMode(TransitionMode value);

	AnimationMode getAnimationMode() const { return m_animationMode; }
	void setAnimationMode(AnimationMode value);

protected:

	void _onTouchPress(EventContext* context);
	void _onTouchRelease(EventContext* context);
	void _onSelected(EventContext* context);
	void _onExit(EventContext* context);
		
	Texture* _loadTexture(std::string path);
	void _releaseTexture(std::string path);

	void changeState(ButtonState state);

protected:
	TransitionMode m_transitionMode;
	AnimationMode m_animationMode;

	//! Events
	Event<UIEventContext*> m_onClickedEvent;
	Event<UIEventContext*> m_onPressedEvent;
	Event<UIEventContext*> m_onReleasedEvent;
	Event<UIEventContext*> m_onSelectedEvent;

	//! Color Tint
	Vec4 m_normalColor;
	Vec4 m_highlightedColor;
	Vec4 m_pressedColor;
	Vec4 m_selectedColor;
	Vec4 m_disableColor;
	float m_fadeDuration;

	//! Sprite Swap
	Texture* m_normalTexture;
	Texture* m_highlightedTexture;
	Texture* m_pressedTexture;
	Texture* m_selectedTexture;
	Texture* m_disabledTexture;

	ButtonState m_btnState;

	bool m_bIsSelectable = false;

protected:
	//! Serialize
	virtual void to_json(json& j) const override;

	//! Deserialize
	virtual void from_json(const json& j) override;
};

NS_IGE_SCENE_END


#endif //__UI_BUTTON_H__