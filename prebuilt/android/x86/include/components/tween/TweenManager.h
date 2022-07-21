#ifndef __TWEEN_MANAGER_H__
#define __TWEEN_MANAGER_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"
#include "components/tween/TweenPropType.h"

USING_NS_PYXIE
NS_IGE_SCENE_BEGIN

class Tweener;

class TweenManager
{
public:
	std::shared_ptr<Tweener> createTween();
	bool isTweening(SceneObject* target, TweenPropType type);
	bool isTweening(SceneObject* target, uint64_t referenceID, TweenPropType type);
	bool killTweens(SceneObject* target, TweenPropType type, bool completed);
	bool killTweens(SceneObject* target, uint64_t referenceID, TweenPropType type, bool completed);
	std::shared_ptr<Tweener> getTween(SceneObject* target, TweenPropType type);
	std::shared_ptr<Tweener> getTween(SceneObject* target, uint64_t referenceID, TweenPropType type);

	TweenManager();
	~TweenManager();

	void update(float dt);
	void clean();
	void init();
protected:
	void reset();
protected:
	std::vector<std::shared_ptr<Tweener>> m_activeTweens;
	std::vector<std::shared_ptr<Tweener>> m_tweenerPool;
	int m_totalActiveTweens = 0;
	bool m_inited = false;

};

NS_IGE_SCENE_END

#endif