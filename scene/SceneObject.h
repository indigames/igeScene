#pragma once

#include <memory>
#include <vector>
#include <string>

#include "json/json.hpp"
using json = nlohmann::json;

#include "event/Event.h"
#include "components/Component.h"

namespace ige::scene
{
    /**
    * SceneObject represents an object in scene hierarchy
    */
    class SceneObject: public std::enable_shared_from_this<SceneObject>
    {
    public:
        //! Constructor
        SceneObject(uint64_t id, std::string name = "", SceneObject* parent = nullptr);

        //! Destructor
        virtual ~SceneObject();

        //! Get ID
        inline uint64_t getId() const { return m_id; }

        //! Get Name
        inline const std::string& getName() { return m_name; }

        //! Set Name
        inline void setName(const std::string& name) 
        {
            if (m_name != name)
            {
                m_name = name;
                s_nameChangedEvent.invoke(*this);
            }
        }

        //! Set parent
        virtual void setParent(SceneObject* parent);

        // Has parent
        virtual bool hasParent() const { return getParent() != nullptr; };

        // Get parent
        virtual SceneObject* getParent() const;

        //! Adds a child.
        virtual void addChild(const std::shared_ptr<SceneObject>& child);

        //! Remove a childs.
        virtual bool removeChild(const std::shared_ptr<SceneObject>& child);

        //! Get children list
        virtual std::vector<std::shared_ptr<SceneObject>>& getChildren();

        //! Get children count
        virtual size_t getChildrenCount() const;

        //! Removes all children
        virtual void removeChildren();

        //! Add a component
        virtual void addComponent(const std::shared_ptr<Component>& component);

        //! Remove a component
        virtual bool removeComponent(const std::shared_ptr<Component>& component);

        //! Remove all components
        virtual bool removeAllComponents();

        //! Get components list
        virtual std::vector<std::shared_ptr<Component>>& getComponents();

        //! Get components count
        virtual size_t getComponentsCount();

        //! Get component by type
        template<typename T>
        inline std::shared_ptr<T> getComponent();

        //! Add component by type
        template<typename T, typename ... Args>
        std::shared_ptr<T> addComponent(Args&&... args);

        //! Remove component by type
        template<typename T>
        bool removeComponent();

        //! Find object by id
        std::shared_ptr<SceneObject> SceneObject::findObjectById(uint64_t id) const;

        //! Find object by name
        std::shared_ptr<SceneObject> SceneObject::findObjectByName(std::string name) const;

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

        //! Serialize
        void to_json(json& j, const SceneObject& obj);

        //! Deserialize 
        void from_json(const json& j, SceneObject& obj);
        
        //! Internal event
        Event<std::shared_ptr<Component>>& getComponentAddedEvent() { return m_componentAddedEvent; }
        Event<std::shared_ptr<Component>>& getComponentRemovedEvent() { return m_componentRemovedEvent; }
        
        //! Public events: Created, Destroyed, Attached, Detached
        static Event<SceneObject&>& getCreatedEvent() { return s_createdEvent; }
        static Event<SceneObject&>& getDestroyedEvent() { return s_destroyedEvent; }
        static Event<SceneObject&>& getAttachedEvent() { return s_attachedEvent; }
        static Event<SceneObject&>& getDetachedEvent() { return s_detachedEvent; }
        static Event<SceneObject&>& getNameChangedEvent() { return s_nameChangedEvent; }

    protected:
        //! Node ID
        uint64_t m_id;

        //! Node Name
        std::string m_name;

        //! Active/Inactive
        bool m_isActive;

        //! Pointer to parent, use weak_ptr avoid dangling issue
        SceneObject* m_parent;

        //! Children vector
        std::vector<std::shared_ptr<SceneObject>> m_children;

        //! Components vector
        std::vector<std::shared_ptr<Component>> m_components;

        //! Internal events
        Event<std::shared_ptr<Component>> m_componentAddedEvent;
        Event<std::shared_ptr<Component>> m_componentRemovedEvent;

        //! Public events
        static Event<SceneObject&> s_destroyedEvent;
        static Event<SceneObject&> s_createdEvent;
        static Event<SceneObject&> s_attachedEvent;
        static Event<SceneObject&> s_detachedEvent;
        static Event<SceneObject&> s_nameChangedEvent;
    };

    //! Get component by type
    template<typename T>
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
    template<typename T, typename ...Args>
    inline std::shared_ptr<T> SceneObject::addComponent(Args&& ...args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T should derive from Component");
        auto found = getComponent<T>();
        if (found) return found;
        auto instance = std::make_shared<T>(shared_from_this(), args...);
        m_components.push_back(instance);
        m_componentAddedEvent.invoke(instance);
        return instance;
    }

    //! Remove component by type
    template<typename T>
    inline bool SceneObject::removeComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T should derive from Component");
        std::shared_ptr<T> result(nullptr);
        for (auto it = m_components.begin(); it != m_components.end(); ++it)
        {
            result = std::dynamic_pointer_cast<T>(*it);
            if (result)
            {
                m_componentRemovedEvent.invoke(result);
                m_components.erase(it);
                return true;
            }
        }

        return false;
    }
}
