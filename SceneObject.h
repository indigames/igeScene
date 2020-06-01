#pragma once

#include <memory>
#include <vector>
#include <string>

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
        SceneObject(int id, std::string name = "", std::shared_ptr<SceneObject> parent = nullptr);

        //! Copy Constructor
        SceneObject(const SceneObject& other);

        //! Destructor
        virtual ~SceneObject();

        inline int getId() const { return m_id; }

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
        virtual std::vector<std::shared_ptr<SceneObject>> getChildren() const;

        //! Get children list
        virtual size_t getChildrenCount() const;

        //! Removes all children
        virtual void removeChildren();

        //! Add a component
        virtual void addComponent(std::shared_ptr<Component> component);

        //! Remove a component
        virtual bool removeComponent(std::shared_ptr<Component> component);

        //! Update functions
        virtual void onUpdate(float dt);
        virtual void onFixedUpdate(float dt);
        virtual void onLateUpdate(float dt);
        
        /**
		* Enable or disable the actor
		* @param isActive
		*/
		inline void setActive(bool isActive);

		/**
		* Check active
        * @return true/false
		*/
		inline bool isActive() const;

    protected:
        //! Node ID
        int m_id;

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
