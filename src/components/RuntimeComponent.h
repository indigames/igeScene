#ifndef __RUNTIME_COMPONENT_H__
#define __RUNTIME_COMPONENT_H__
#pragma once

#include <memory>
#include <vector>
#include <string>

#include "utils/Serialize.h"

#include "components/Component.h"
#include "event/Event.h"

namespace ige::scene
{
    class EventContext;

    /**
    * RuntimeComponent is only active when running, not in editor
    */
    class RuntimeComponent : public Component
    {
    public:
        //! Constructor
        RuntimeComponent(SceneObject& owner);

        //! Destructor
        virtual ~RuntimeComponent();

        //! Update functions
        virtual void onUpdate(float dt) override;
        virtual void onFixedUpdate(float dt) override;
        virtual void onLateUpdate(float dt) override;

        //! Runtime functions
        virtual void onRuntimeUpdate(float dt) = 0;
        virtual void onRuntimeFixedUpdate(float dt) = 0;
        virtual void onRuntimeLateUpdate(float dt) = 0;

        bool isInitialized() const { return m_bIsInit; }
    protected:
        //! Active when start runtime
        virtual void Initialize();

        //! Active when stop runtime
        virtual void Clear();

    private:
        bool m_bIsInit = false;
    };
} // namespace ige::scene

#endif