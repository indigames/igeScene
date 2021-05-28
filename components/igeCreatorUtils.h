#pragma once

#include "core/igeSceneMacros.h"
#include "event/Event.h"

#include <map>
#include <vector>
#include <string>

namespace ige::scene {

#if IGE_PLATFORM == IGE_PLATFORM_EDITOR

	class igeInspectorDispatchSystem {
	public:
		static igeInspectorDispatchSystem* getInstance();
		static void setInstance(std::shared_ptr<igeInspectorDispatchSystem> instance);
		static void release();

		igeInspectorDispatchSystem();
		~igeInspectorDispatchSystem();
		void dispatchProperty(uint64_t componentId, std::string propertyName);
		void registerProperty(uint64_t componentId, std::string propertyName, std::function<void()> callback);
	protected:
		static igeInspectorDispatchSystem* m_instance;
		std::map<uint64_t, std::map<std::string, ige::scene::Event<>>> m_map;
	};

#define INSPECTOR_PROPERTY_DISPATCH(COMPONENT_ID, VALUE)  \
if(igeInspectorDispatchSystem::getInstance() != nullptr)	\
	igeInspectorDispatchSystem::getInstance()->dispatchProperty(COMPONENT_ID, #VALUE); \


#define INSPECTOR_PROPERTY_REGISTER(COMPONENT_ID, VALUE, CALLBACK) if (igeInspectorDispatchSystem::getInstance() != nullptr) \
		igeInspectorDispatchSystem::getInstance()->registerProperty(COMPONENT_ID, VALUE, CALLBACK); \

#else

#define INSPECTOR_PROPERTY_DISPATCH(COMPONENT_ID, VALUE)
#define INSPECTOR_PROPERTY_REGISTER(COMPONENT_ID, VALUE, CALLBACK)

#endif



#if  IGE_PLATFORM == IGE_PLATFORM_EDITOR

#define INSPECTOR_PROPERTY(_TYPE_, VALUE) \
protected: \
	_TYPE_ _##VALUE; \
public: \
	_TYPE_ get##VALUE() { return _##VALUE; } \
	void set##VALUE(_TYPE_ V)  \
	{ \
		_##VALUE = V; \
		INSPECTOR_PROPERTY_DISPATCH(getInstanceId(), VALUE) \
	} \
	void setDirect_##VALUE(_TYPE_ V)  \
	{ \
		_##VALUE = V; \
	} \
	


#else 

#define INSPECTOR_PROPERTY(_TYPE_, VALUE) \
protected: \
	_TYPE_ _##VALUE; \
public: \
	_TYPE_ get##VALUE() { return _##VALUE; } \
	void set##VALUE(_TYPE_ V)  \
	{ \
		_##VALUE = V; \
	} \

#endif 

}