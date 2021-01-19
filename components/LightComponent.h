#pragma once
#ifndef __LIGHT_COMPONENT_H
#define __LIGHT_COMPONENT_H

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"

namespace ige::scene
{
    //! AmbientLight
    class LightComponent : public Component
    {
    public:
        //! Constructor
        LightComponent(SceneObject& owner);

        //! Destructor
        virtual ~LightComponent();
    };
} // namespace ige::scene

#endif