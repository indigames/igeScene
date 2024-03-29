#pragma once

#include <memory>
#include <vector>
#include <string>

#include "utils/Serialize.h"

namespace ige::scene
{
    class Scene;
    class SceneObject;

    /**
    * Component is the base class of any component
    */
    class Component
    {
    public:
        enum class Type
        {
            Camera = 0,
            Environment,
            Figure,
            EditableFigure,
            Sprite,
            Text,
            TextBitmap,
            BoneTransform,
            Script,
            AmbientLight,
            DirectionalLight,
            PointLight,
            SpotLight,
            Canvas,
            UIImage,
            UIText,
            UITextField,
            UIButton,
            UISlider,
            UIScrollView,
            UIScrollBar,
            UIMask,
            PhysicManager,
            BoxCollider,
            SphereCollider,
            CapsuleCollider,
            MeshCollider,
            CompoundCollider,
            Rigidbody,
            Softbody,
            AudioManager,
            AudioSource,
            AudioListener,
            ParticleManager,
            Particle,
            Navigable,
            NavMesh,
            NavAgent,
            NavAgentManager,
            DynamicNavMesh,
            NavObstacle,
            NavArea,
            OffMeshLink,
            Transform,
            RectTransform,
            Compound,
            Animator
        };

    public:
        //! Constructor
        Component(SceneObject& owner);

        //! Destructor
        virtual ~Component();

        //! Get owner object
        virtual SceneObject* getOwner() const { return &m_owner; }

        //! Returns the name of the component
        virtual std::string getName() const = 0;

        //! Returns the type of the component
        virtual Type getType() const = 0;

        //! Enable/Disable
        virtual bool isEnabled() const { return m_bIsEnabled; }
        virtual void setEnabled(bool enable = true);

        //! Skip serialize
        bool isSkipSerialize() const { return m_bSkipSerialize; }
        void setSkipSerialize(bool skip = true) { m_bSkipSerialize = skip; }

        //! Can attach multiple instance to one object?
        inline virtual bool canMultiInstance() { return false; }

        //! Can multiple edit?
        inline virtual bool canMultiEdit() { return false; }

        //! Should always update
        inline virtual bool shouldAlwaysUpdate() { return false; }

        //! Enable
        virtual void onEnable();

        //! Disable
        virtual void onDisable();

        //! Update functions
        virtual void onUpdate(float dt);
        virtual void onFixedUpdate(float dt);
        virtual void onLateUpdate(float dt);
        virtual void onPhysicUpdate(float dt);

        //! Render
        virtual void onRender();
        virtual void onRenderUI();

        //! Destroyed
        virtual void onDestroy();

        //! Click
        virtual void onClick();

        //! Suspend
        virtual void onSuspend();

        //! Resume
        virtual void onResume();

        //! Serialize
        friend void to_json(json &j, const Component &obj);

        //! Deserialize
        friend void from_json(const json &j, Component &obj);

        uint64_t getInstanceId() const { return m_instanceId; }

        //! Serialize
        virtual void to_json(json& j) const;

        //! Deserialize
        virtual void from_json(const json& j);

        //! Get property value by key via JSON, it's slow so don't overuse it
        template <typename T>
        T getProperty(const std::string& key, const T& defaultVal);

        //! Update json value
        virtual void setProperty(const std::string& key, const json& val);

        //! Serialize finished event
        virtual void onSerializeFinished();

    protected:
        //! Reference to owner object
        SceneObject& m_owner;

        //! Enable/Disable
        bool m_bIsEnabled = true;

        //! Skip serialize (editor properties)
        bool m_bSkipSerialize = false;

        //! Cache instance id
        uint64_t m_instanceId;
        static std::atomic<uint64_t> s_instanceID;
    };

    //! Serialize
    void to_json(json &j, const Component &obj);

    //! Deserialize
    void from_json(const json &j, Component &obj);

    //! Get property value by key
    template <typename T>
    T Component::getProperty(const std::string& key, const T& defaultVal)
    {
        auto jComp = json{};
        to_json(jComp);
        return (jComp.contains(key) && !jComp.at(key).is_null()) ? jComp.value(key, defaultVal) : defaultVal;
    }
} // namespace ige::scene
