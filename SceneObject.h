#pragma once

#include <memory>
#include <vector>
#include <string>

#include "json/json.hpp"
using json = nlohmann::json;

namespace ige::scene
{
    class Component;

    /**
    * SceneObject represents an object in scene hierarchy
    */
    class SceneObject : std::enable_shared_from_this<SceneObject>
    {
    public:
        //! Constructor
        SceneObject(uint64_t id, std::string name = "", std::shared_ptr<SceneObject> parent = nullptr);

        //! Copy Constructor
        SceneObject(const SceneObject& other);

        //! Destructor
        virtual ~SceneObject();

        //! Get ID
        inline uint64_t getId() const { return m_id; }

        //! Get Name
        inline std::string getName() const { return m_name; }

        //! Set parent
        virtual void setParent(std::shared_ptr<SceneObject> parent);

        // Get parent
        virtual std::shared_ptr<SceneObject> getParent() const;

        //! Adds a child.
        virtual void addChild(std::shared_ptr<SceneObject> child);

        //! Remove a childs.
        virtual bool removeChild(std::shared_ptr<SceneObject> child);

        //! Get children list
        virtual std::vector<std::shared_ptr<SceneObject>>& getChildren();

        //! Get children count
        virtual size_t getChildrenCount() const;

        //! Removes all children
        virtual void removeChildren();

        //! Add a component
        virtual void addComponent(std::shared_ptr<Component> component);

        //! Remove a component
        virtual bool removeComponent(std::shared_ptr<Component> component);

        //! Get components list
        virtual std::vector<std::shared_ptr<Component>>& getComponents();

        //! Get components count
        virtual size_t getComponentsCount();

        //! Get component by type
        template<typename T>
	    inline std::shared_ptr<T> getComponent();

        //! Find object by id
        std::shared_ptr<SceneObject> SceneObject::findObjectById(uint64_t id) const;

        //! Find object by name
        std::shared_ptr<SceneObject> SceneObject::findObjectByName(std::string name) const;

        //! Update functions
        virtual void onUpdate(float dt);
        virtual void onFixedUpdate(float dt);
        virtual void onLateUpdate(float dt);
        
        //! Enable or disable the actor		
        inline void setActive(bool isActive);

        //! Check active
        inline bool isActive() const;

        //! Serialize
        void to_json(json& j, const SceneObject& obj);

        //! Deserialize 
        void from_json(const json& j, SceneObject& obj);
        
    protected:
        //! Node ID
        uint64_t m_id;

        //! Node Name
        std::string m_name;

        //! Active/Inactive
        bool m_isActive;

        //! Pointer to parent, use weak_ptr avoid dangling issue
        std::weak_ptr<SceneObject> m_parent;

        //! Children vector
        std::vector<std::shared_ptr<SceneObject>> m_children;

        //! Components vector
        std::vector<std::shared_ptr<Component>> m_components;
    };
}
