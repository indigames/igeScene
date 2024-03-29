#pragma once

#include <unordered_map>

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/Component.h"

namespace ige::scene
{
    //! BoneTransform
    class BoneTransform : public Component
    {
    public:
        //! Constructor
        BoneTransform(SceneObject& owner);

        //! Destructor
        virtual ~BoneTransform();

        //! Get component name
        virtual std::string getName() const override { return "BoneTransform"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::BoneTransform; }

        //! Update
        void onUpdate(float dt) override;

        //! Render
        void onRender() override;

        //! Enable
        virtual void onEnable() override;

        //! Disable
        virtual void onDisable() override;

        //! Initialize
        bool initialize();

        //! Clear
        void clear();

        //! Get list of joint objects
        const std::unordered_map<std::string, std::shared_ptr<SceneObject>>& getJointObjects() const { return m_jointObjects; }

        //! Trigger when select joint in the editor
        void onJointObjectSelected(const std::string& name, bool selected);

        //! Can multiple edit
        inline virtual bool canMultiEdit() override { return false; }

        //! Serialize finished event
        virtual void onSerializeFinished() override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! SceneObject deleted event
        void onSceneObjectDeleted(SceneObject& sceneObject);

    protected:
        //! Store all object created by BoneTransform
        std::unordered_map<std::string, std::shared_ptr<SceneObject>> m_jointObjects = {};

        //! Cache the figure object
        Figure* m_figure = nullptr;

        //! SceneObject deleted event ID
        uint64_t m_sceneObjectDeleteEventId = (uint64_t)-1;

        //! Initialized indicator
        bool m_bIsInitialized = false;
    };
} // namespace ige::scene
