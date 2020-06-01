#include <algorithm>

#include "Component.h"
#include "SceneObject.h"

namespace ige::scene
{
    //! Constructor
    SceneObject::SceneObject(int id, std::string name, std::shared_ptr<SceneObject> parent) 
                    : m_id(id), m_name(name), m_parent(parent) 
    {

    }

    //! Copy Constructor
    SceneObject::SceneObject(const SceneObject& other)
    {
        m_id = other.getId();
    }

    //! Destructor
    SceneObject::~SceneObject()
    {
        m_components.clear();        
        removeChildren();
    }

    //! Set parent
    void SceneObject::setParent(std::shared_ptr<SceneObject> parent)
    {
        m_parent = parent;
    }

    // Get parent
    std::shared_ptr<SceneObject> SceneObject::getParent() const
    {
        return m_parent.lock();
    }

    //! Adds a child.
    void SceneObject::addChild(std::shared_ptr<SceneObject> child)
    {
        child->setParent(shared_from_this());

        // Insert to the first available slot.
        for (auto& current : m_children)
        {
            if (!current)
            {
                current = child;
                return;
            }
        }

        // Add at back of vector.        
        m_children.push_back(child);
    }

    //! Remove a childs.
    bool SceneObject::removeChild(std::shared_ptr<SceneObject> child)
    {
        if (!child)
            return false;

        // Remove from vector by nullptr assignment
        for (auto& current : m_children)
        {
            if (current == child)
            {
                current->setParent(nullptr);
                current = nullptr;
                return true;
            }
        }
        
        // Not found, return false
        return false;
    }

    //! Get all children
    std::vector<std::shared_ptr<SceneObject>> SceneObject::getChildren() const
    {
        return m_children;
    }

    //! Count children
    size_t SceneObject::getChildrenCount() const
    {
        return m_children.size();
    }

    //! Remove children
    void SceneObject::removeChildren()
    {
        std::for_each(m_children.begin(), m_children.end(), [](auto child) {
            child->setParent(nullptr);
            child = nullptr;
        });
        m_children.clear();
    }

    //! Add a component
    void SceneObject::addComponent(std::shared_ptr<Component> component)
    {
        m_components.push_back(component);
    }

    //! Remove a component
    bool SceneObject::removeComponent(std::shared_ptr<Component> component)
    {
        auto it = std::find(m_components.begin(), m_components.end(), component);
        if(it != m_components.end())
        {
            m_components.erase(it);
            return true;
        }
        return false;
    }

    //! Update function
    void SceneObject::onUpdate(float dt)
    {        
        if (isActive())
        {
            std::for_each(m_components.begin(), m_components.end(), [&](auto element) { element->onUpdate(dt); });
        }
    }

    //! Update function
    void SceneObject::onFixedUpdate(float dt)
    {        
        if (isActive())
        {
            std::for_each(m_components.begin(), m_components.end(), [&](auto element) { element->onFixedUpdate(dt); });
        }
    }

    //! Update function
    void SceneObject::onLateUpdate(float dt)
    {
        if (isActive())
        {
            std::for_each(m_components.begin(), m_components.end(), [&](auto element) { element->onLateUpdate(dt); });
        }
    }
    
    /**
    * Enable or disable the actor
    * @param isActive
    */
    void SceneObject::setActive(bool isActive)
    {
        m_isActive = isActive;
    }

    /**
    * Check active
    * @return true/false
    */
    bool SceneObject::isActive() const
    {
        return m_isActive;
    }
}