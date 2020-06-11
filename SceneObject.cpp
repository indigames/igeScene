#include <algorithm>

#include "SceneObject.h"
#include "components/Component.h"

namespace ige::scene
{
    //! Constructor
    SceneObject::SceneObject(uint64_t id, std::string name, std::shared_ptr<SceneObject> parent) 
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
        setParent(nullptr);       
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
        for (auto& currObject : m_children)
        {
            if (!currObject)
            {
                currObject = child;
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
        for (auto& currObject : m_children)
        {
            if (currObject == child)
            {
                currObject->setParent(nullptr);
                currObject = nullptr;
                return true;
            }
        }
        
        // Not found, return false
        return false;
    }

    //! Get all children
    std::vector<std::shared_ptr<SceneObject>>& SceneObject::getChildren()
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

    //! Get components list
    std::vector<std::shared_ptr<Component>>& SceneObject::getComponents()
    {
        return m_components;
    }

    //! Get component count
    size_t SceneObject::getComponentsCount()
    {
        return m_components.size();
    }

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

    //! Find object by id
    std::shared_ptr<SceneObject> SceneObject::findObjectById(uint64_t id) const
    {
        if(m_id == id) return std::const_pointer_cast<SceneObject>(shared_from_this());
        for(int i = 0; i < m_children.size(); ++i)
        {
            if(m_children[i] == nullptr) continue;
            auto ret = m_children[i]->findObjectById(id);
            if (ret) return ret;
        }
        return nullptr;
    }

    //! Find object by name
    std::shared_ptr<SceneObject> SceneObject::findObjectByName(std::string name) const
    {
        if(m_name == name) return std::const_pointer_cast<SceneObject>(shared_from_this());
        for(int i = 0; i < m_children.size(); ++i)
        {
            if(m_children[i] == nullptr) continue;
            auto ret = m_children[i]->findObjectByName(name);
            if (ret) return ret;
        }
        return nullptr;
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
    
    //! Enable or disable the actor    
    void SceneObject::setActive(bool isActive)
    {
        m_isActive = isActive;
    }

    //! Check active    
    bool SceneObject::isActive() const
    {
        return m_isActive;
    }

    //! Serialize
    void SceneObject::to_json(json& j, const SceneObject& obj)
    {
        
    }

    //! Deserialize 
    void SceneObject::from_json(const json& j, SceneObject& obj)
    {

    }
}
