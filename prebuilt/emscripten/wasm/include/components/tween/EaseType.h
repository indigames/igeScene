#ifndef __EASE_TYPE_H__
#define __EASE_TYPE_H__

#include "core/igeSceneMacros.h"

namespace ige::scene {

	enum class EaseType
	{
		Linear,
		SineIn,
		SineOut,
		SineInOut,
		QuadIn,
		QuadOut,
		QuadInOut,

		Custom
	};


}

#endif //__EASE_TYPE_H__
