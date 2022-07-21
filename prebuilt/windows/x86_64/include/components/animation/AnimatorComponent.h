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
        enum class UpdateMode {
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

        //! Update mode
        UpdateMode getUpdateMode() const { return m_updateMode; }
        void setUpdateMode(UpdateMode mode) { m_updateMode = mode; }

        //! Override update functions
        virtual void onUpdate(float dt) override;

        //! Override update functions
        virtual void onFixedUpdate(float dt) override;

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    public:
        //! Animator controller instance
        std::shared_ptr<AnimatorController> controller = nullptr;

        //! Update mode
        UpdateMode m_updateMode = UpdateMode::Normal;

        //! Controller path
        std::string m_controllerPath = {};
    };
} // namespace ige::scene
