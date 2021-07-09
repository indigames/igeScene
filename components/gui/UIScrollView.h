#ifndef __UI_SCROLL_VIEW_H__
#define __UI_SCROLL_VIEW_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

#include "components/gui/UIImage.h"
#include "components/gui/RectTransform.h"
#include "event/Event.h"

USING_NS_PYXIE

NS_IGE_SCENE_BEGIN

class UIEventContext;
class EventContext;
class Tweener;
class UIScrollBar;

class UIScrollView : public UIImage
{
public:
	enum class MovementType 
	{
		UnRestricted,
		Elastic,
		Clamped,
	};

	UIScrollView(SceneObject& owner, const std::string& texture = "", const Vec2& size = { 256.f, 256.f }, const bool isSliced = false, const Vec4& border = { 0.f, 0.f, 0.f, 0.f });
	virtual ~UIScrollView();

	//! Get component name
	virtual std::string getName() const override { return "UIScrollView"; }

	//! Returns the type of the component
	virtual Type getType() const override { return Type::UIScrollView; }

	//! Update
	virtual void onUpdate(float dt) override;

	//! Render
	virtual void onRender() override;

	void setContent(std::shared_ptr<SceneObject> obj);
	void setViewport(std::shared_ptr<SceneObject> obj);

	bool enableHorizontal() const { return m_bHorizontal; };
	void setEnableHorizontal(bool active);
	bool enableVertical() const { return m_bVertical; };
	void setEnableVertical(bool active);

	void setHorizontalScrollBar(std::shared_ptr<UIScrollBar> obj);
	std::shared_ptr<UIScrollBar> getHorizontalScrollBar() const { return m_horizontalScrollBar; }
	void setVerticalScrollBar(std::shared_ptr<UIScrollBar> obj);
	std::shared_ptr<UIScrollBar> getVerticalScrollBar() const { return m_verticalScrollBar; }
	
	MovementType getMovementType() const { return m_movementType; }
	void setMovementType(MovementType type);
	void setMovementType(int type);

	Vec2 getElasticExtra() const { return m_elasticExtra; }
	void setElasticExtra(Vec2 value);

	float getElasticity() const { return m_elasticity; }
	void setElasticity(float value);

	bool isInertia() const { return m_bInertia; }
	void setInertia(bool value) { m_bInertia = value; }

	float getDecelerationRate() const { return m_decelerationRate; }
	void setDecelerationRate(float value);

	void scrollTo(Vec2 delta);

	virtual void onSerializeFinished(Scene* scene) override;
	
	//! Update property by key value
	virtual void setProperty(const std::string& key, const json& val) override;

protected:
	virtual void _onTouchPress(EventContext* context) override;
	virtual void _onTouchRelease(EventContext* context) override;
	virtual void _onTouchDrag(EventContext* context) override;

	void _updateSize(bool forced = false);
	void _updateScrollBars(Vec2 offset);
	void _scroll(Vec2 delta);
	void _scrollTo(Vec2 delta, bool updateScrollBar = true);

	//! Inertia Update
	float _updateInertiaDuration(float pos, int axis);
	void _startTween(int type);
	void _killTween();
	void _tweenUpdate(float dt);
	float _runTween(int axis, float dt);

	float _getHorizontalNormalizedPosition();
	void _setHorizontalNormalizedPosition(float value);
	float _getVerticalNormalizedPosition();
	void _setVerticalNormalizedPosition(float value);
	void _setNormalizedPosition(float value, int axis);

protected:
	bool m_bIsSelectable = false;
	bool m_bIsDragging = false;

	MovementType m_movementType;

	bool m_bHorizontal = true;
	bool m_bVertical = true;

	Vec2 m_clickedPos;
	Vec2 m_deltaPos;

	Vec4 m_boundContent;
	Vec4 m_boundView;

	//! Stored current Content Size and recheck when start drag
	Vec2 m_currentContentSize;
	Vec2 m_currentViewportSize;
	
	Vec2 m_elasticExtra;
	float m_elasticity;

	bool m_bInertia;
	float m_decelerationRate;

	Vec2 m_velocity;
	clock_t m_lastMoveTime;
	float m_deltaTime;
	int m_tweening;
	Vec2 m_tweenStart;
	Vec2 m_tweenChange;

	bool m_dirtySetObj;
	std::shared_ptr<RectTransform> m_rectContent;
	std::string m_contentUUID;
	std::shared_ptr<RectTransform> m_rectViewport;
	std::string m_viewportUUID;

	std::shared_ptr<UIScrollBar> m_horizontalScrollBar;
	std::string m_horizontalUUID;
	std::shared_ptr<UIScrollBar> m_verticalScrollBar;
	std::string m_verticalUUID;

	uint64_t m_eventHorizontalScrollBarId;
	uint64_t m_eventVerticalScrollBarId;

protected:
	//! Serialize
	virtual void to_json(json& j) const override;

	//! Deserialize
	virtual void from_json(const json& j) override;
};

NS_IGE_SCENE_END


#endif //__UI_SCROLL_VIEW_H__