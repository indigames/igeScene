#include "components/tween/TweenManager.h"
#include "components/tween/Tweener.h"
#include "scene/SceneObject.h"
NS_IGE_SCENE_BEGIN

TweenManager::TweenManager() : 
	m_totalActiveTweens(0),
	m_inited(false)
{

}

TweenManager::~TweenManager()
{
	if(m_inited)
		clean();
}

std::shared_ptr<Tweener> TweenManager::createTween()
{
	if (!m_inited) init();

	std::shared_ptr<Tweener> tweener;
	int cnt = (int)m_tweenerPool.size();
	if (cnt > 0)
	{
		tweener = m_tweenerPool[cnt - 1];
		m_tweenerPool.pop_back();
	}
	else
		tweener = std::make_shared<Tweener>();
	tweener->_init();
	if (m_activeTweens.size() == m_totalActiveTweens)
		m_activeTweens.push_back(tweener);
	else
		m_activeTweens[m_totalActiveTweens] = tweener;
	m_totalActiveTweens++;
	return tweener;
}

bool TweenManager::isTweening(SceneObject* target, TweenPropType propType) 
{
	if (target == nullptr) return false;

	bool anyType = propType == TweenPropType::None;
	for (int i = 0; i < m_totalActiveTweens; i++) {
		auto tweener = m_activeTweens[i];
		if (tweener != nullptr && tweener->m_target == target && !tweener->m_killed && (anyType || tweener->m_propType == propType))
			return true;
	}
	return false;
}

bool TweenManager::isTweening(SceneObject* target, uint64_t referenceID, TweenPropType propType)
{
	if (target == nullptr) return false;

	bool anyType = propType == TweenPropType::None;
	for (int i = 0; i < m_totalActiveTweens; i++) {
		auto tweener = m_activeTweens[i];
		if (tweener != nullptr && tweener->m_target == target && !tweener->m_killed && tweener->m_referenceId == referenceID && (anyType || tweener->m_propType == propType))
			return true;
	}
	return false;
}

bool TweenManager::killTweens(SceneObject* target, TweenPropType propType, bool completed)
{
	if (target == nullptr) return false;
	bool flag = false;
	int cnt = m_totalActiveTweens;
	bool anyType = propType == TweenPropType::None;
	for (int i = 0; i < cnt; i++)
	{
		auto tweener = m_activeTweens[i];
		if (tweener != nullptr && tweener->m_target == target && !tweener->m_killed && (anyType || tweener->m_propType == propType))
		{
			tweener->kill(completed);
			flag = true;
		}
	}

	return flag;
}

bool TweenManager::killTweens(SceneObject* target, uint64_t referenceID,TweenPropType propType, bool completed)
{
	if (target == nullptr) return false;
	bool flag = false;
	int cnt = m_totalActiveTweens;
	bool anyType = propType == TweenPropType::None;
	for (int i = 0; i < cnt; i++)
	{
		auto tweener = m_activeTweens[i];
		if (tweener != nullptr && tweener->m_target == target && !tweener->m_killed && tweener->m_referenceId == referenceID && (anyType || tweener->m_propType == propType))
		{
			tweener->kill(completed);
			flag = true;
		}
	}

	return flag;
}

std::shared_ptr<Tweener> TweenManager::getTween(SceneObject* target, TweenPropType propType)
{
	if (target == nullptr)
		return nullptr;

	int cnt = m_totalActiveTweens;
	bool anyType = propType == TweenPropType::None;
	for (int i = 0; i < cnt; i++)
	{
		auto tweener = m_activeTweens[i];
		if (tweener != nullptr && tweener->m_target == target && !tweener->m_killed && (anyType || tweener->m_propType == propType))
		{
			return tweener;
		}
	}

	return nullptr;
}

std::shared_ptr<Tweener> TweenManager::getTween(SceneObject* target, uint64_t referenceID,TweenPropType propType)
{
	if (target == nullptr)
		return nullptr;

	int cnt = m_totalActiveTweens;
	bool anyType = propType == TweenPropType::None;
	for (int i = 0; i < cnt; i++)
	{
		auto tweener = m_activeTweens[i];
		if (tweener != nullptr && tweener->m_target == target && !tweener->m_killed && tweener->m_referenceId == referenceID && (anyType || tweener->m_propType == propType))
		{
			return tweener;
		}
	}

	return nullptr;
}

void TweenManager::update(float dt)
{
	int cnt = m_totalActiveTweens;
	int freePosStart = -1;
	for (int i = 0; i < cnt; i++)
	{
		auto tweener = m_activeTweens[i];
		if (tweener == nullptr)
		{
			if (freePosStart == -1)
				freePosStart = i;
		}
		else if (tweener->m_killed)
		{
			tweener->_reset();
			m_tweenerPool.push_back(tweener);
			m_activeTweens[i] = nullptr;

			if (freePosStart == -1)
				freePosStart = i;
		}
		else
		{
			//! if target is not available, cancel it
			if (tweener->m_target != nullptr &&  tweener->m_referenceId != -1 && tweener->m_target->getComponent(tweener->m_referenceId) == nullptr)
				tweener->m_killed = true;
			else if (!tweener->m_paused)
				tweener->_update(dt);

			if (freePosStart != -1)
			{
				m_activeTweens[freePosStart] = tweener;
				m_activeTweens[i] = nullptr;
				freePosStart++;
			}
		}
	}

	if (freePosStart >= 0)
	{
		if (m_totalActiveTweens != cnt) //new tweens added
		{
			int j = cnt;
			cnt = m_totalActiveTweens - cnt;
			for (int i = 0; i < cnt; i++)
				m_activeTweens[freePosStart++] = m_activeTweens[j++];
		}
		m_totalActiveTweens = freePosStart;
	}
}

void TweenManager::clean()
{
	reset();

	//! Clear Active Tween
	for (auto it = m_activeTweens.begin(); it != m_activeTweens.end(); it++) {
		if(*it != nullptr)
			(*it)->release();
	}
	m_activeTweens.clear();


	//! Clear Pool Tween
	//! 
	for (auto it = m_tweenerPool.begin(); it != m_tweenerPool.end(); it++)
		(*it)->release();
	m_tweenerPool.clear();
}


void TweenManager::init()
{
	m_inited = true;
	
}

void TweenManager::reset()
{
	int cnt = m_totalActiveTweens;
	for (int i = 0; i < cnt; i++) {
		auto tweener = m_activeTweens[i];
		if (tweener != nullptr) {
			tweener->_reset();
			m_tweenerPool.push_back(tweener);
			m_activeTweens[i] = nullptr;
		}
	}

	m_totalActiveTweens = 0;
	m_inited = false;
}

NS_IGE_SCENE_END