#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
	class Object {
	public:
		virtual std::string getType() const = 0;
	};
}