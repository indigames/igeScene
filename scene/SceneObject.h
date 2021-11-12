#pragma once

#include <memory>
#include <vector>
#include <string>

#include "event/Event.h"
#include "event/EventContext.h"
#include "event/InputEventContext.h"

#include "components/Component.h"
#include "components/TransformComponent.h"
#include "components/ScriptComponent.h"
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    class Scene;
    /**
    * SceneObject represents an object in scene hierarchy
    */
    class SceneObject
    {
    public:
        //! Constructor
        SceneObject(Scene* scene, uint64_t id, std::string name = "", bool isGui = false, const Vec2& size = { 64.f, 64.f }, const std::string& prefabId = std::string(),
            const Vec3& pos = Vec3(), const Quat& rot = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));

        //! Destructor
        virtual ~SceneObject();

        //! Get ID
        inline virtual uint64_t getId() const { return m_id; }

        //! Get UUID
        inline virtual std::string getUUID() const { return m_uuid; }
        inline void setUUID(const std::string& uuid) { m_uuid = uuid; }

        //! Get Name
        inline virtual std::string getName() const { return m_name; }

        //! Set Name
        virtual void setName(const std::string& name);

        //! Set parent
        virtual void setParent(std::shared_ptr<SceneObject> parent);

        // Get shared_ptr
        virtual std::shared_ptr<SceneObject> getSharedPtr();

        // Get parent
        virtual std::shared_ptr<SceneObject> getParent() const;

        // Check relative recursive
        virtual bool isRelative(uint64_t id);

        //! Get children list
        virtual std::vector<std::weak_ptr<SceneObject>>& getChildren();

        //! Add child
        virtual void addChild(std::shared_ptr<SceneObject> child);

        //! Add child by id
        virtual void addChildById(uint64_t id);

        //! Removes child
        virtual void removeChild(std::shared_ptr<SceneObject> child);

        //! Removes child by id
        virtual void removeChildById(uint64_t id);

        //! Removes all children
        virtual void removeChildren();

        //! Find first child by name
        virtual std::shared_ptr<SceneObject> findChildByName(const std::string& name);

        //! Find child
        virtual std::shared_ptr<SceneObject> findChild(const std::string& uuid);

        //! Find child by id
        virtual std::shared_ptr<SceneObject> findChildById(uint64_t id);

        //! Create a component by name
        virtual std::shared_ptr<Component> createComponent(const std::string& name);

        //! Add a component
        virtual void addComponent(const std::shared_ptr<Component> &component);

        //! Remove a component
        virtual bool removeComponent(const std::shared_ptr<Component> &component);
        virtual bool removeComponent(uint64_t id);

        //! Remove a component by name
        virtual bool removeComponent(const std::string &name);

        //! Remove all components
        virtual bool removeAllComponents();

        //! Get components list
        virtual std::vector<std::shared_ptr<Component>> &getComponents();

        //! Get component by name
        std::shared_ptr<Component> getComponent(const std::string& name) const;

        //! Get component by id
        std::shared_ptr<Component> getComponent(const uint64_t id) const;

        //! Get components by type recursively
        void getComponentsRecursive(std::vector<Component*>& components, const std::string& type) const;

        //! Get first component in hierarchy
        std::shared_ptr<Component> getFirstComponentRecursive(const std::string& type) const;

        //! Get components count
        virtual size_t getComponentsCount();

        //! Check component by type
        template <typename T>
        inline bool hasComponent();

        //! Get component by type
        template <typename T>
        inline std::shared_ptr<T> getComponent();

        //! Add component by type
        template <typename T, typename... Args>
        std::shared_ptr<T> addComponent(Args &&... args);

        //! Update functions
        virtual void onUpdate(float dt);
        virtual void onFixedUpdate(float dt);
        virtual void onLateUpdate(float dt);
        virtual void onPhysicUpdate(float dt);

        //! Render function
        virtual void onRender();

        //! Enable or disable the actor
        virtual void setActive(bool isActive, bool recursive = false);

        //! Check active
        virtual bool isActive(bool recursive = false) const;

        // Set selected
        void setSelected(bool select, bool recursive = false);

        //! Check selected
        bool isSelected() const;

        //!ReloadScript
        void reloadScripts(bool includeChild = true);

        //! Internal event
        Event<SceneObject &> &getNameChangedEvent() { return m_nameChangedEvent; }
        Event<SceneObject &> &getTransformChangedEvent() { return m_transformChangedEvent; }

        //! Public events: Created, Destroyed, Attached, Detached
        static Event<SceneObject &> &getCreatedEvent() { return s_createdEvent; }
        static Event<SceneObject &> &getDestroyedEvent() { return s_destroyedEvent; }
        static Event<SceneObject &> &getAttachedEvent() { return s_attachedEvent; }
        static Event<SceneObject &> &getDetachedEvent() { return s_detachedEvent; }
        static Event<SceneObject &> &getSelectedEvent() { return s_selectedEvent; }
        static Event<SceneObject&>& getDeselectedEvent() { return s_deselectedEvent; }

        //! Helper to generate UUID
        static std::string generateUUID(unsigned int len = 16);

        //! Serialize
        virtual void to_json(json &j);

        //! Deserialize
        virtual void from_json(const json &j);

        //! Check whether it's a GUI object
        bool isGUIObject() const { return m_bIsGui; }

        //! Get transform component
        std::shared_ptr<TransformComponent> &getTransform() { return m_transform; }

        //! Get RectTransform component (GUI only)
        std::shared_ptr<RectTransform> getRectTransform() { return std::dynamic_pointer_cast<RectTransform>(m_transform); }

        //! Set transform component
        void setTransform(const std::shared_ptr<TransformComponent> &transform) { m_transform = transform; }

        //! Get canvas
        std::shared_ptr<Canvas>& getCanvas() { return m_canvas; }

        //! Set canvas
        void setCanvas(const std::shared_ptr<Canvas>& canvas) { m_canvas = canvas; }

        //! Get scene
        Scene* getScene() const { return m_scene; }

        //! Get scene root
        std::shared_ptr<SceneObject> getRoot();

        void setIsRaycastTarget(bool value) { m_bIsRaycastTarget = value; }
        bool isRaycastTarget() const { return m_bIsRaycastTarget; }

        void setIsInteractable(bool value) { m_bIsInteractable = value; }
        bool isInteractable() const { return m_bIsInteractable; }

        //! Event 
        void addEventListener(int eventType, const EventCallback& callback, const uint64_t tag = 0);
        void removeEventListener(int eventType, const uint64_t tag = 0);
        void removeEventListeners();
        bool hasEventListener(int eventType, const uint64_t tag = 0) const;
        bool dispatchEvent(int eventType, const Value& dataValue = Value::Null);
        bool dispatchEventIncludeChild(int eventType, const Value& dataValue = Value::Null);

        bool bubbleEvent(int eventType, const Value& dataValue = Value::Null);
        bool dispatchInputEvent(int eventType, const Value& dataValue = Value::Null);
        bool bubbleInputEvent(int eventType, const Value& dataValue = Value::Null);

        //! Aabb
        const AABBox& getAABB() const { return m_aabb; }

        //! AABB in world space
        AABBox getWorldAABB() { return m_aabb.Transform(getTransform()->getWorldMatrix()); }

        //! Get Frame AABB in world space (use for )
        AABBox getFrameAABB() { return m_bLockedFrameAABB ? m_aabb : getWorldAABB(); }

        //! Set locked frame AABB
        void setLockedFrameAABB(bool locked = true) { m_bLockedFrameAABB = locked; }

        //! Update AABB
        virtual void updateAabb();

        //! Set prefab Id
        virtual bool hasPrefab() const { return (!m_prefabIdsLinked.empty()); }
        virtual bool isPrefab() const;
        virtual bool isInPrefab() const;

        //! PrefabID
        virtual std::string getPrefabRootId();
        virtual std::shared_ptr<SceneObject> getPrefabRoot();
        virtual std::string getPrefabId();
        virtual bool unpackPrefab();
        void setPrefabId(const std::string& id);

        //! Mask state
        bool isInMask() const { return m_bIsInMask; }
        void setInMask(bool value);

        std::shared_ptr<ScriptComponent> getScript(const std::string& path) const;
    protected:
        //! Event
        void doDispatch(int eventType, EventContext* context, bool includeChild = false);
        void doBubble(int eventType, EventContext* context);

        //! Active children components
        void activeChildren(bool active);

    protected:
        //! Node ID
        uint64_t m_id;

        //! Node UUID
        std::string m_uuid;

        //! Node Name
        std::string m_name;

        //! Active/Inactive
        bool m_isActive;

        //! Selected/Unselected
        bool m_isSelected;

        //! Cache pointer to current scene
        Scene* m_scene = nullptr;

        //! Pointer to parent, use weak_ptr avoid dangling issue
        std::weak_ptr<SceneObject> m_parent;

        //! Cached children vector
        std::vector<std::weak_ptr<SceneObject>> m_children;

        //! Components vector
        std::vector<std::shared_ptr<Component>> m_components;

        //! Internal events
        Event<SceneObject &> m_nameChangedEvent;
        Event<SceneObject &> m_transformChangedEvent;

        //! Public events
        static Event<SceneObject&> s_destroyedEvent;
        static Event<SceneObject&> s_createdEvent;
        static Event<SceneObject&> s_attachedEvent;
        static Event<SceneObject&> s_detachedEvent;
        static Event<SceneObject&> s_selectedEvent;
        static Event<SceneObject&> s_deselectedEvent;

        //! Cache transform component
        std::shared_ptr<TransformComponent> m_transform = nullptr;

        //! Cache Canvas component (GUI)
        std::shared_ptr<Canvas> m_canvas = nullptr;

        //! Cache isGui
        bool m_bIsGui = false;

        //! Cached aabb
        AABBox m_aabb;

        //! Obj will not update frame AABB
        bool m_bLockedFrameAABB = false;

        //! Define if object will cast with UI
        bool m_bIsRaycastTarget = false;

        //! Define if object is interactable
        bool m_bIsInteractable = false;

        //! Linked Prefab IDs (in the last non-prefab node)
        std::vector<std::string> m_prefabIdsLinked;

        //! Prefab ID (In the root node of prefab)
        std::string m_prefabId;

        int m_aabbDirty = 2;

        //Event Dispatch
        struct EventCallbackItem
        {
            EventCallback callback;
            int eventType;
            uint64_t tag; //! Component Ref ID
            int dispatching;
        };

        std::vector<EventCallbackItem*> m_callbacks;
        int m_dispatching;
        bool m_bIsInMask;
    };

    //! Check component by type
    template <typename T>
    inline bool SceneObject::hasComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T should derive from Component");
        if (m_components.size() == 0) return false;
        for (auto it = m_components.begin(); it != m_components.end(); ++it)
        {
            auto result = std::dynamic_pointer_cast<T>(*it);
            if (result) return true;
        }
        return false;
    }

    //! Get component by type
    template <typename T>
    inline std::shared_ptr<T> SceneObject::getComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T should derive from Component");
        for (auto it = m_components.begin(); it != m_components.end(); ++it) {
            auto result = std::dynamic_pointer_cast<T>(*it);
            if (result)
                return result;
        }
        return nullptr;
    }

    //! Add component by type
    template <typename T, typename... Args>
    inline std::shared_ptr<T> SceneObject::addComponent(Args &&... args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T should derive from Component");
        auto instance = std::make_shared<T>(*this, args...);
        addComponent(instance);
        return instance;
    }
} // namespace ige::scene
