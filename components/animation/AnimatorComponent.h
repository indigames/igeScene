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

        //! Initialize
        void initialize();

        //! Clear
        void clear();

        //! Get name
        virtual std::string getName() const override { return "Animator"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::Animator; }

        //! Controller Path
        const std::string& getControllerPath() const { return m_controllerPath; }
        void setControllerPath(const std::string& path);

        //! Controller
        std::shared_ptr<AnimatorController> getController() { return controller; }
        void setController(const std::shared_ptr<AnimatorController>& controller);

        //! Override update functions
        virtual void onUpdate(float dt) override;

        //! Override update functions
        virtual void onFixedUpdate(float dt) override;

    public:
        //! Animator controller instance
        std::shared_ptr<AnimatorController> controller = nullptr;

        //! Update mode
        int m_updateMode = UpdateMode::Normal;

        //! Controller path
        std::string m_controllerPath = {};
    };
} // namespace ige::scene
