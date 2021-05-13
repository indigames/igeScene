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
        BoneTransform(SceneObject &owner);

        //! Destructor
        virtual ~BoneTransform();

        //! Get component name
        virtual std::string getName() const override { return "BoneTransform"; }

        //! Update
        void onUpdate(float dt) override;

        //! Render
        void onRender() override;

        //! Initialize
        bool initialize();

        //! Get list of joint objects
        const std::unordered_map<std::string, SceneObject*>& getJointObjects() const { return m_jointObjects; }

        //! Trigger when select joint in the editor
        void onJointObjectSelected(const std::string& name, bool selected);

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Serialize finished event
        virtual void onSerializeFinished(Scene* scene) override;

        //! SceneObject deleted event
        void onSceneObjectDeleted(SceneObject& sceneObject);

    protected:
        //! Store all object created by BoneTransform
        std::unordered_map<std::string, SceneObject*> m_jointObjects = {};

        //! Cache the figure object
        Figure* m_figure = nullptr;

        //! SceneObject deleted event ID
        uint64_t m_sceneObjectDeleteEventId = (uint64_t)-1;
    };
} // namespace ige::scene
