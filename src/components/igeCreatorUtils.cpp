#include "components/igeCreatorUtils.h"

namespace ige::scene {

#if IGE_PLATFORM == IGE_PLATFORM_EDITOR

	igeInspectorDispatchSystem* igeInspectorDispatchSystem::m_instance = nullptr;

	igeInspectorDispatchSystem* igeInspectorDispatchSystem::getInstance() { return m_instance; }

	void igeInspectorDispatchSystem::setInstance(std::shared_ptr<igeInspectorDispatchSystem> instance)
	{
		m_instance = instance.get();
	}

	void igeInspectorDispatchSystem::release()
	{ 
		for (auto obj : m_instance->m_map) {
			for (auto ee : obj.second) {
				ee.second.removeAllListeners();
			}
		}
		m_instance = nullptr; 
	}

	igeInspectorDispatchSystem::igeInspectorDispatchSystem() {}

	igeInspectorDispatchSystem::~igeInspectorDispatchSystem() {
		for (auto obj : m_map) {
			for (auto ee : obj.second) {
				ee.second.removeAllListeners();
			}
		}
	}

	void igeInspectorDispatchSystem::dispatchProperty(uint64_t componentId, std::string propertyName)
	{
		m_map[componentId][propertyName].invoke();
	}

	void igeInspectorDispatchSystem::registerProperty(uint64_t componentId, std::string propertyName, std::function<void()> callback) {
		m_map[componentId][propertyName].addListener(callback);
	}

#endif
}