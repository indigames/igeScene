#pragma once

#include <memory>
#include <vector>
#include <string>

#include "event/Event.h"
#include "event/EventContext.h"
#include "event/InputEventContext.h"

#include "components/Component.h"
#include "components/TransformComponent.h"
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
    class SceneObject : std::enable_shared_from_this<SceneObject>
    {
    public:
        //! Constructor
        SceneObject(Scene* scene, uint64_t id, std::string name = "", SceneObject* parent = nullptr, bool isGui = false, const Vec2& size = {64.f, 64.f}, bool isCanvas = false);

        //! Destructor
        virtual ~SceneObject();

        //! Get ID
        inline uint64_t getId() const { return m_id; }

        //! Get UUID
        inline std::string getUUID() const { return m_uuid; }

        //! Get Name
        inline const std::string &getName() const { return m_name; }

        //! Set Name
        void setName(const std::string& name);

        //! Set parent
        virtual void setParent(SceneObject *parent);

        // Get parent
        virtual SceneObject *getParent() const;

        //! Get children list
        virtual const std::vector<SceneObject*> &getChildren() const;

        //! Add child
        virtual void addChild(SceneObject* child);

        //! Removes child
        virtual void removeChild(SceneObject* child);

        //! Removes all children
        virtual void removeChildren();

        //! Find child
        virtual SceneObject* findChild(std::string uuid);

        //! Add a component
        virtual void addComponent(const std::shared_ptr<Component> &component);

        //! Remove a component
        virtual bool removeComponent(const std::shared_ptr<Component> &component);

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

        //! Get components count
        virtual size_t getComponentsCount();

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

        //! Render function
        virtual void onRender();

        //! Enable or disable the actor
        void setActive(bool isActive);

        //! Check active
        bool isActive() const;

        // Set selected
        void setSelected(bool select);

        //! Check selected
        bool isSelected() const;

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

        //! Serialize
        void to_json(json &j);

        //! Deserialize
        void from_json(const json &j);

        //! Check whether it's a GUI object
        bool isGUIObject() const { return m_bIsGui; }

        //! Check whether it's a Canvas object
        bool isCanvasObject() const { return m_bIsCanvas; }

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
        Scene* getScene() { return m_scene; }

        //! Get scene root
        SceneObject* getRoot();

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
        bool bubbleEvent(int eventType, const Value& dataValue = Value::Null);
        bool dispatchInputEvent(int eventType, const Value& dataValue = Value::Null);
        bool bubbleInputEvent(int eventType, const Value& dataValue = Value::Null);

        //! Aabb
        const AABBox& getAABB() const { return m_aabb; }

        //! AABB in world space
        const AABBox& getWorldAABB() const { return m_aabbWorld; }

        //! Get Frame AABB in world space (use for )
        const AABBox& getFrameAABB() const { return m_frameAABB; }

        //! Set locked frame AABB
        void setLockedFrameAABB(bool locked = true) { m_bLockedFrameAABB = locked; }

        bool isInMask() const { return m_bIsInMask; }
        void setInMask(bool value);

    protected:
        //! Helper to generate UUID
        std::string generateUUID(unsigned int len = 16);

        //! Set UUID
        inline void setUUID(const std::string& uuid) { m_uuid = uuid; }

        //! Event
        void doDispatch(int eventType, EventContext* context);
        void doBubble(int eventType, EventContext* context);

        //! Transform changed event
        void onTransformChanged(SceneObject& sceneObject);

        //! Update AABB
        virtual void updateAabb();

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
        SceneObject *m_parent = nullptr;

        //! Cached children vector
        std::vector<SceneObject*> m_children;

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

        //! Cache isCanvas
        bool m_bIsCanvas = false;

        //! Cached aabb
        AABBox m_aabb;

        //! Cached world aabb
        AABBox m_aabbWorld;

        //! Cached frame aabb
        AABBox m_frameAABB;

        //! Obj will not update frame AABB
        bool m_bLockedFrameAABB = false;

        //! Define if object will cast with UI
        bool m_bIsRaycastTarget = false;
        //! Define if object is interactable
        bool m_bIsInteractable = false;

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

    //! Get component by type
    template <typename T>
    inline std::shared_ptr<T> SceneObject::getComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T should derive from Component");

        for (auto it = m_components.begin(); it != m_components.end(); ++it)
        {
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
        auto found = getComponent<T>();
        if (found)
            return found;
        auto instance = std::make_shared<T>(*this, args...);
        addComponent(instance);
        return instance;
    }
} // namespace ige::scene
