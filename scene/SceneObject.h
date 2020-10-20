#pragma once

#include <memory>
#include <vector>
#include <string>

#include "event/Event.h"
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
    class SceneObject
    {
    public:
        //! Constructor
        SceneObject(Scene* scene, uint64_t id, std::string name = "", SceneObject* parent = nullptr, bool isGui = false, const Vec2& size = {64.f, 64.f});

        //! Destructor
        virtual ~SceneObject();

        //! Get ID
        inline uint64_t getId() const { return m_id; }

        //! Get Name
        inline const std::string &getName() const { return m_name; }

        //! Set Name
        inline void setName(const std::string& name);

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

        //! Add a component
        virtual void addComponent(const std::shared_ptr<Component> &component);

        //! Remove a component
        virtual bool removeComponent(const std::shared_ptr<Component> &component);

        //! Remove a component by name
        virtual bool removeComponent(const std::string &name);

        //! Add a component by raw pointer
        virtual void addComponent(Component *component);

        //! Remove a component by raw pointer
        virtual bool removeComponent(Component *component);

        //! Remove all components
        virtual bool removeAllComponents();

        //! Get components list
        virtual std::vector<std::shared_ptr<Component>> &getComponents();

        //! Get component by name
        std::shared_ptr<Component> getComponent(std::string name) const;

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

        // Component related events
        static Event<SceneObject &, const std::shared_ptr<Component> &> &getComponentAddedEvent() { return m_componentAddedEvent; }
        static Event<SceneObject &, const std::shared_ptr<Component> &> &getComponentRemovedEvent() { return m_componentRemovedEvent; }

        //! Serialize
        void to_json(json &j);

        //! Deserialize
        void from_json(const json &j);

        //! Check whether it's a GUI object
        bool isGUIObject() const { return m_bIsGui; }

        //! Get transform component
        std::shared_ptr<TransformComponent> &getTransform() { return m_transform; }

        //! Get RectRransform component (GUI only)
        std::shared_ptr<RectTransform> getRectTransform() { return std::dynamic_pointer_cast<RectTransform>(m_transform); }

        //! Set transform component
        void setTransform(const std::shared_ptr<TransformComponent> &transform) { m_transform = transform; }

        //! Get canvas
        std::shared_ptr<Canvas>& getCanvas() { return m_canvas; }

        //! Set canvas
        void setCanvas(const std::shared_ptr<Canvas>& canvas) { m_canvas = canvas; }

        //! Get scene
        Scene* getScene() { return m_scene; }

    protected:
        //! Node ID
        uint64_t m_id;

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
        static Event<SceneObject &> s_destroyedEvent;
        static Event<SceneObject &> s_createdEvent;
        static Event<SceneObject &> s_attachedEvent;
        static Event<SceneObject &> s_detachedEvent;
        static Event<SceneObject &> s_selectedEvent;

        //! Component related event
        static Event<SceneObject &, const std::shared_ptr<Component> &> m_componentAddedEvent;
        static Event<SceneObject &, const std::shared_ptr<Component> &> m_componentRemovedEvent;

        //! Cache transform component
        std::shared_ptr<TransformComponent> m_transform = nullptr;

        //! Cache Canvas component (GUI)
        std::shared_ptr<Canvas> m_canvas = nullptr;

        //! Cache isGui
        bool m_bIsGui = false;
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
        m_components.push_back(instance);
        m_componentAddedEvent.invoke(*this, instance);
        return instance;
    }
} // namespace ige::scene
