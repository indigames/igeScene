#ifndef __UI_BUTTON_H__
#define __UI_BUTTON_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

#include "components/gui/UIImage.h"
#include "event/Event.h"

USING_NS_PYXIE

NS_IGE_SCENE_BEGIN

class UIEventContext;
class EventContext;
class Tweener;

enum class TransitionMode {
	ColorTint,
	SpriteSwap,
	Custom
};

enum class AnimationMode {
	None,
	Elastic,
	Bounce,
};

enum class ButtonState {
	NORMAL = 0,
	PRESSED = 1,
	SELECTED = 2,
	DISABLE = -1
};

class UIButton : public UIImage
{
public:

	UIButton(SceneObject& owner, const std::string& texture = "", const Vec2& size = { 128.f, 128.f }, const bool isSliced = false, const Vec4& border = { 0.f, 0.f, 0.f, 0.f });
	virtual ~UIButton();

	//! Get component name
	virtual std::string getName() const override { return "UIButton"; }

	bool isSelected() const { return m_btnState == ButtonState::SELECTED; }
	
	TransitionMode getTransitionMode() const { return m_transitionMode; }
	void setTransitionMode(TransitionMode value);

	AnimationMode getAnimationMode() const { return m_animationMode; }
	void setAnimationMode(AnimationMode value);

	void setTexturePath(const std::string& texturePath, ButtonState setState);

	const std::string& getPressedPath() const { return m_pressedPath; }
	const std::string& getSelectedPath() const { return m_selectedPath; }
	const std::string& getDisabledPath() const { return m_disabledPath; }

	//! NormalColor
	void setColor(float r, float g, float b, float a = 1) override;
	void setColor(const Vec4& value) override;
	const Vec4 getColor()  const override { return m_normalColor; }

	//! PresssedColor
	void setPressedColor(float r, float g, float b, float a = 1);
	void setPressedColor(const Vec4& value);
	const Vec4 getPressedColor()  const { return m_pressedColor; }

	//! SelectedColor
	void setSelectedColor(float r, float g, float b, float a = 1);
	void setSelectedColor(const Vec4& value);
	const Vec4 getSelectedColor()  const { return m_selectedColor; }

	//! DisabledColor
	void setDisabledColor(float r, float g, float b, float a = 1);
	void setDisabledColor(const Vec4& value);
	const Vec4 getDisabledColor()  const { return m_disableColor; }

	void setFadeDuration(float value) { m_fadeDuration = value; }
	const float getFadeDuration() const { return m_fadeDuration; }
protected:

	virtual void _onTouchPress(EventContext* context) override;
	virtual void _onTouchRelease(EventContext* context) override;
	virtual void _onSelected(EventContext* context) override;
	virtual void _onClick(EventContext* context) override;
	virtual void _onExit(EventContext* context) override;
	
	virtual void onClick() override;

	void changeState(ButtonState state, bool forced = false);
	Texture* getTextureByState(ButtonState state) const;
	Vec4 getColorByState(ButtonState state) const;

protected:
	TransitionMode m_transitionMode;
	AnimationMode m_animationMode;

	//! Color Tint
	Vec4 m_normalColor;
	Vec4 m_pressedColor;
	Vec4 m_selectedColor;
	Vec4 m_disableColor;
	float m_fadeDuration;

	//! Sprite Swap
	Texture* m_pressedTexture;
	Texture* m_selectedTexture;
	Texture* m_disabledTexture;

	std::string m_pressedPath;
	std::string m_selectedPath;
	std::string m_disabledPath;

	ButtonState m_btnState;

	bool m_bIsSelectable = false;

	std::shared_ptr<Tweener> tween = nullptr;

protected:
	//! Serialize
	virtual void to_json(json& j) const override;

	//! Deserialize
	virtual void from_json(const json& j) override;
};

NS_IGE_SCENE_END


#endif //__UI_BUTTON_H__