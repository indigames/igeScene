#pragma once

#include "components/Component.h"
#include "event/Event.h"

namespace ige::scene
{
    class AnimatorController;

    /**
     * Class Animator
     */
    class AnimatorComponent : public Component
    {
    public:
        enum class UpdateMode: int {
            Normal = 0, // sync with onUpdate 
            AnimatePhysic, // sync with onFixedUpdate 
            UnscaledTime, // sync with onUpdate, but ignore timeScale factor
        };

        AnimatorComponent(SceneObject& owner);
        virtual ~AnimatorComponent();

        //! Get name
        virtual std::string getName() const override { return "Animator"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::Animator; }

        //! Override update functions
        virtual void onUpdate(float dt) override;

        //! Override update functions
        virtual void onFixedUpdate(float dt) override;

    public:
        //! Animator controller instance
        std::shared_ptr<AnimatorController> controller;

        //! Update mode
        int m_updateMode = UpdateMode::Normal;
    };
} // namespace ige::scene
