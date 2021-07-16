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
        RuntimeComponent(SceneObject &owner);

        //! Destructor
        virtual ~RuntimeComponent();

        //! Enable
        virtual void onEnable() override;

        //! Disable
        virtual void onDisable() override;

        //! Update functions
        virtual void onUpdate(float dt) override;
        virtual void onFixedUpdate(float dt) override;
        virtual void onLateUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

        //! Destroyed
        virtual void onDestroy() override;

        //! Click
        virtual void onClick() override;

        //! Suspend
        virtual void onSuspend() override;

        //! Resume
        virtual void onResume() override;

    private:
        void onRunEditor(EventContext* context);
        void onPauseEditor(EventContext* context);
        void onStopEditor(EventContext* context);

    protected:
        //! Active when start runtime
        virtual void Initialize();

        //! Active when stop runtime
        virtual void Clear();

        bool isRunning() const { return m_bIsInit; }

    private:
        bool m_bIsInit = false;


    };
} // namespace ige::scene

#endif