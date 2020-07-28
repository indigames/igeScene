#include <algorithm>

#include "scene/SceneObject.h"

#include "components/Component.h"
#include "components/TransformComponent.h"
#include "components/CameraComponent.h"
#include "components/EnvironmentComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "components/ScriptComponent.h"
#include "components/RectTransform.h"

namespace ige::scene
{
    Event<SceneObject&, const std::shared_ptr<Component>&> SceneObject::m_componentAddedEvent;
    Event<SceneObject&, const std::shared_ptr<Component>&> SceneObject::m_componentRemovedEvent;

    //! Static member initialization
    Event<SceneObject&> SceneObject::s_destroyedEvent;
    Event<SceneObject&> SceneObject::s_createdEvent;
    Event<SceneObject&> SceneObject::s_attachedEvent;
    Event<SceneObject&> SceneObject::s_detachedEvent;
    Event<SceneObject&> SceneObject::s_nameChangedEvent;
    Event<SceneObject&> SceneObject::s_selectedEvent;

    //! Constructor
    SceneObject::SceneObject(uint64_t id, std::string name, SceneObject* parent, bool isGui)
        : m_id(id), m_name(name), m_parent(parent), m_bIsGui(isGui), m_isActive(true), m_isSelected(false), m_transform(nullptr), m_showcase(nullptr)
    {
        // Cache root item
        m_root = (parent == nullptr) ? this : parent->getRoot();

        // Only create showcase for root objects
        if (parent == nullptr)
        {
            m_showcase = ResourceCreator::Instance().NewShowcase((m_name + "_" + std::to_string(m_id) + "_showcase").c_str());            
        }

        // Invoke created event
        getCreatedEvent().invoke(*this);
    }

    //! Destructor
    SceneObject::~SceneObject()
    {
        m_transform = nullptr;

        if (m_showcase)
        {
            m_showcase->Clear();
            m_showcase->DecReference();
            m_showcase = nullptr;
        }

        removeAllComponents();
        removeChildren();
        setParent(nullptr);

        getDestroyedEvent().invoke(*this);
        ResourceManager::Instance().DeleteDaemon();
    }

    //! Set parent
    void SceneObject::setParent(SceneObject* parent)
    {
        if (parent)
        {
            m_parent = parent;
            getAttachedEvent().invoke(*this);
        }
        else
        {
            m_parent = nullptr;
            getDetachedEvent().invoke(*this);
        }        
    }

    // Get parent
    SceneObject* SceneObject::getParent() const
    {
        return m_parent;
    }

    //! Adds a child.
    void SceneObject::addChild(const std::shared_ptr<SceneObject>& child)
    {
        child->setParent(this);

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
    bool SceneObject::removeChild(const std::shared_ptr<SceneObject>& child)
    {
        if (!child)
            return false;

        // Remove from vector by nullptr assignment
        for (auto& currObject : m_children)
        {
            if (currObject != nullptr && currObject == child)
            {
                currObject = nullptr;
                return true;
            }
        }
        
        // Recursive remove child
        for (auto& currObject : m_children)
        {
            if (currObject && currObject->removeChild(child))
                return true;
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
       for(auto& child: m_children) {
            if(child != nullptr) child->setParent(nullptr);
            child = nullptr;
        }
        m_children.clear();
    }

    //! Add a component
    void SceneObject::addComponent(const std::shared_ptr<Component>& component)
    {
        m_components.push_back(component);
    }
  
    //! Remove a component
    bool SceneObject::removeComponent(const std::shared_ptr<Component>& component)
    {
        auto it = std::find(m_components.begin(), m_components.end(), component);
        if(it != m_components.end())
        {
            auto result = std::dynamic_pointer_cast<Component>(*it);
            m_componentRemovedEvent.invoke(*this, result);
            onComponentRemoved(*this, result);
            m_components.erase(it);
            return true;
        }
        return false;
    }

    //! Component added event
    void SceneObject::onComponentAdded(SceneObject& obj, const std::shared_ptr<Component>& component)
    {        
        if(m_showcase != nullptr)
        {
            if (component->getName() == "FigureComponent")
            {
                auto figureComponent = std::dynamic_pointer_cast<FigureComponent>(component);
                auto figure = figureComponent->getFigure();
                if (figure) m_showcase->Add(figure);

                figureComponent->getOnFigureCreatedEvent().addListener([this](auto figure) {
                    if (figure) m_showcase->Add(figure);
                });

                figureComponent->getOnFigureDestroyedEvent().addListener([this](auto figure) {
                    if (figure) m_showcase->Remove(figure);
                });
            }
            else if (component->getName() == "SpriteComponent")
            {
                auto eFigComp = std::dynamic_pointer_cast<SpriteComponent>(component);
                auto figure = eFigComp->getFigure();
                if (figure) m_showcase->Add(figure);

                eFigComp->getOnFigureCreatedEvent().addListener([this](auto figure) {
                    if (figure) m_showcase->Add(figure);
                    });

                eFigComp->getOnFigureDestroyedEvent().addListener([this](auto figure) {
                    if (figure) m_showcase->Remove(figure);
                    });
            }
            else if (component->getName() == "EnvironmentComponent")
            {
                auto envComp = std::dynamic_pointer_cast<EnvironmentComponent>(component);
                auto env = envComp->getEnvironment();
                if (env) m_showcase->Add(env);
            }
        }

        if (hasParent())
        {
            getParent()->onComponentAdded(obj, component);
        }
    }

    //! Component removed event
    void SceneObject::onComponentRemoved(SceneObject& obj, const std::shared_ptr<Component>& component)
    {
        if(m_showcase != nullptr)
        {
            if (component->getName() == "FigureComponent")
            {
                auto figComp = std::dynamic_pointer_cast<FigureComponent>(component);
                auto figure = figComp->getFigure();
                if (figure) m_showcase->Remove(figure);

                figComp->getOnFigureCreatedEvent().removeAllListeners();
                figComp->getOnFigureDestroyedEvent().removeAllListeners();
            }
            else if (component->getName() == "SpriteComponent")
            {
                auto eFigComp = std::dynamic_pointer_cast<SpriteComponent>(component);
                auto figure = eFigComp->getFigure();
                if (figure) m_showcase->Remove(figure);

                eFigComp->getOnFigureCreatedEvent().removeAllListeners();
                eFigComp->getOnFigureDestroyedEvent().removeAllListeners();
            }
            else if (component->getName() == "EnvironmentComponent")
            {
                auto envComp = std::dynamic_pointer_cast<EnvironmentComponent>(component);
                auto env = envComp->getEnvironment();
                if (env) m_showcase->Remove(env);
            }
        }

        if (hasParent())
        {
            getParent()->onComponentRemoved(obj, component);
        }
    }

    //! Remove all component
    bool SceneObject::removeAllComponents()
    {
        auto it = m_components.begin();
        while (it != m_components.end())
        {
            auto result = std::dynamic_pointer_cast<Component>(*it);
            m_componentRemovedEvent.invoke(*this, result);
            onComponentRemoved(*this, result);
            it = m_components.erase(it);
        }
        return true;
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
        
    //! Find object by id
    std::shared_ptr<SceneObject> SceneObject::findObjectById(uint64_t id) const
    {
        for(int i = 0; i < m_children.size(); ++i)
        {
            if(m_children[i] == nullptr)
                continue;
            if (m_children[i]->getId() == id)
                return m_children[i];
            auto ret = m_children[i]->findObjectById(id);
            if (ret) return ret;
        }
        return nullptr;
    }

    //! Find object by name
    std::shared_ptr<SceneObject> SceneObject::findObjectByName(std::string name) const
    {
        for(int i = 0; i < m_children.size(); ++i)
        {
            if(m_children[i] == nullptr)
                continue;
            if (m_children[i]->getName() == name)
                return m_children[i];
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
            for (auto& comp : m_components)
            {
                // Camera updated before other objects
                if(comp->getName() != "CameraComponent")
                    comp->onUpdate(dt);
            }
            
            for (auto& obj : m_children)
            {
                if(obj != nullptr) obj->onUpdate(dt);
            }
        }
    }

    //! Update function
    void SceneObject::onFixedUpdate(float dt)
    {
        if (isActive())
        {
            for (auto& comp : m_components)
            {
                comp->onFixedUpdate(dt);
            }

            for (auto& obj : m_children)
            {
                if (obj != nullptr) obj->onFixedUpdate(dt);
            }
        }
    }

    //! Update function
    void SceneObject::onLateUpdate(float dt)
    {
        if (isActive())
        {
            for (auto& comp : m_components)
            {
                comp->onLateUpdate(dt);
            }

            for (auto& obj : m_children)
            {
                if (obj != nullptr) obj->onLateUpdate(dt);
            }
        }
    }
    
    //! Render
    void SceneObject::onRender()
    {
        if (isActive())
        {
            for (auto& comp : m_components)
            {
                // Camera rendered before other objects
                if (comp->getName() != "CameraComponent")
                    comp->onRender();
            }

            for (auto& obj : m_children)
            {
                if (obj != nullptr) obj->onRender();
            }
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

    //! Enable or disable the actor
    void SceneObject::setSelected(bool select)
    {
        if(m_isSelected != select)
        {
            m_isSelected = select;
            if(m_isSelected)
            {
                getSelectedEvent().invoke(*this);
            }
        }
    }

    //! Check active
    bool SceneObject::isSelected() const
    {
        return m_isSelected;
    }

    //! Serialize
    void SceneObject::to_json(json& j)
    {
        j = json {
            {"id", m_id},
            {"name", m_name},
            {"active", m_isActive},
            {"select", m_isSelected},
        };

        auto jComponents = json::array();
        for (const auto& comp : m_components)
        {
            json jCmp = json{
                {comp->getName(), *comp.get()}
            };
            jComponents.push_back(jCmp);
        }
        j["comps"] = jComponents;

        auto jChildren = json::array();
        for (const auto& child : m_children)
        {
            if (child)
            {
                json jChild;
                child->to_json(jChild);
                jChildren.push_back(jChild);
            }            
        }
        j["children"] = jChildren;
    }

    //! Deserialize 
    void SceneObject::from_json(const json& j)
    {
        j.at("id").get_to(m_id);
        j.at("name").get_to(m_name);
        j.at("active").get_to(m_isActive);
        j.at("select").get_to(m_isSelected);

        auto jComps = j.at("comps");
        for (auto it : jComps)
        {
            auto key = it.begin().key();
            auto val = it.begin().value();
            std::shared_ptr<Component> comp = nullptr;
            if (key == "TransformComponent") comp = addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f));
            else if (key == "CameraComponent") comp = addComponent<CameraComponent>(val.at("name"));
            else if (key == "EnvironmentComponent") comp = addComponent<EnvironmentComponent>(val.at("name"));
            else if (key == "FigureComponent") comp = addComponent<FigureComponent>(val.at("path"));
            else if (key == "SpriteComponent") comp = addComponent<SpriteComponent>(val.at("size"), val.at("path"));
            else if (key == "ScriptComponent") comp = addComponent<ScriptComponent>(val.at("path"));
            if (comp) comp->from_json(val);
        }

        auto jChildren = j.at("children");
        for (auto it : jChildren)
        {
            auto child = std::make_shared<SceneObject>(it.at("id"), it.at("name"), this);
            child->from_json(it);
            addChild(child);
        }
    }
}
