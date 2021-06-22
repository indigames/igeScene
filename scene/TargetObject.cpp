
#include "scene/TargetObject.h"
#include "scene/Scene.h"


namespace ige::scene
{    
    //! Constructor
    TargetObject::TargetObject(Scene* scene)
        : SceneObject::SceneObject(scene, (uint64_t)-1)
    {       
    }

    //! Destructor
    TargetObject::~TargetObject()
    {

    }

    //! Get ID
    uint64_t TargetObject::getId() const 
    {
        if(m_objects.size() == 1)
            return m_objects[0]->getId();
        return (uint64_t)-1;
    }

    //! Get UUID
    std::string TargetObject::getUUID() const 
    {
        if(m_objects.size() == 1)
            return m_objects[0]->getUUID();
        return "FFFFFFFFFFFFFFFF";
    }

    //! Get Name
    const std::string& TargetObject::getName() const 
    {
        if(m_objects.size() == 1)
            return m_objects[0]->getName();
        return "Multiple Objects";
    }

    // Get parent
    SceneObject* TargetObject::getParent() const 
    {
        return SceneObject::getParent();
    }

    //! Add a component
    void TargetObject::addComponent(const std::shared_ptr<Component> &component) 
    {
        auto found = std::find_if(m_components.begin(), m_components.end(), [&component](auto element) {
            return component->getName().compare(element->getName()) == 0;
        });

        if (found == m_components.end())
        {
            m_components.push_back(component);

            for(auto& object: m_objects)
            {
                object->addComponent(component);
            }
        }
    }

    //! Remove a component
    bool TargetObject::removeComponent(const std::shared_ptr<Component> &component) 
    {
        auto found = std::find_if(m_components.begin(), m_components.end(), [&component](auto element) {
            return component->getName().compare(element->getName()) == 0;
        });

        if (found != m_components.end())
        {
            for(auto& object: m_objects)
            {
                object->removeComponent(component);
            }
            m_components.erase(found);
            return true;
        }
        return false;
    }

    //! Get components list
    std::vector<std::shared_ptr<Component>>& TargetObject::getComponents()
    {
        return m_components;
    }

    //! Check active
    bool TargetObject::isActive() const
    {
        if(m_objects.size() < 0)
            return false;
        return m_objects[0]->isActive();
    }

    //! Enable or disable the actor
    void TargetObject::setActive(bool isActive)
    {
        for(auto& obj : m_objects)
        {
            obj->setActive(isActive);
        }
    }
    
    //! Utils to collect shared components
    void TargetObject::collectSharedComponents()
    {
        m_components.clear();
        if(m_objects.size() < 0)
            return;

        for(const auto& comp: m_objects[0]->getComponents())
        {
            if(comp->canMultiEdit())
            {
                m_components.push_back(comp);
            }
        }

        for(int i = 1; i < m_objects.size(); ++i)
        {
            for(const auto& comp: m_components)
            {
                const auto& object = m_objects[i];
                const auto& components = object->getComponents();
                auto itr = std::find(components.begin(), components.end(), comp);
                
                if(itr == components.end())
                {
                    m_components.erase(std::find(m_components.begin(), m_components.end(), comp));
                }
            }
        }
    }
    
    //! Add object
    void TargetObject::add(SceneObject* object)
    {
        if(object != nullptr)
        {
            m_objects.push_back(object);
            collectSharedComponents();
        }
    }

    //! Remove object
    void TargetObject::remove(SceneObject* object)
    {
        if(object != nullptr)
        {
            auto itr = std::find(m_objects.begin(), m_objects.end(), object);
            if(itr != m_objects.end())
            {
                m_objects.erase(itr);
                collectSharedComponents();
            }            
        }
    }
} // namespace ige::scene
