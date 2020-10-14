#include <algorithm>

#include "scene/SceneObject.h"

#include "components/Component.h"
#include "components/TransformComponent.h"
#include "components/CameraComponent.h"
#include "components/EnvironmentComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "components/ScriptComponent.h"
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"
#include "components/gui/UIImage.h"
#include "components/gui/UIText.h"
#include "components/gui/UITextField.h"
#include "components/physic/PhysicBox.h"
#include "components/physic/PhysicCapsule.h"
#include "components/physic/PhysicSphere.h"
#include "components/audio/AudioSource.h"
#include "components/audio/AudioListener.h"

namespace ige::scene
{
    //! Static member initialization
    Event<SceneObject &, const std::shared_ptr<Component> &> SceneObject::m_componentAddedEvent;
    Event<SceneObject &, const std::shared_ptr<Component> &> SceneObject::m_componentRemovedEvent;

    //! Events
    Event<SceneObject &> SceneObject::s_destroyedEvent;
    Event<SceneObject &> SceneObject::s_createdEvent;
    Event<SceneObject &> SceneObject::s_attachedEvent;
    Event<SceneObject &> SceneObject::s_detachedEvent;
    Event<SceneObject &> SceneObject::s_selectedEvent;

    //! Constructor
    SceneObject::SceneObject(uint64_t id, std::string name, SceneObject *parent, bool isGui)
        : m_id(id), m_name(name), m_parent(parent), m_bIsGui(isGui), m_isActive(true), m_isSelected(false), m_transform(nullptr), m_showcase(nullptr)
    {
        // Cache root item
        m_root = (parent == nullptr) ? this : parent->getRoot();

        // Only create showcase for root objects
        if (parent == nullptr)
        {
            m_showcase = ResourceCreator::Instance().NewShowcase((m_name + "_" + std::to_string(m_id) + "_showcase").c_str());
            getResourceAddedEvent().addListener(std::bind(&SceneObject::onResourceAdded, this, std::placeholders::_1));
            getResourceRemovedEvent().addListener(std::bind(&SceneObject::onResourceRemoved, this, std::placeholders::_1));
        }
        else
        {
            setCanvas(parent->getCanvas());
        }


        // Invoke created event
        getCreatedEvent().invoke(*this);
    }

    //! Destructor
    SceneObject::~SceneObject()
    {
        for (auto &comp : m_components)
            comp->onDestroy();

        removeChildren();
        setParent(nullptr);

        removeAllComponents();
        m_transform = nullptr;

        if (m_showcase)
        {
            getResourceAddedEvent().removeAllListeners();
            getResourceRemovedEvent().removeAllListeners();

            m_showcase->Clear();
            m_showcase->DecReference();
            m_showcase = nullptr;
        }

        getDestroyedEvent().invoke(*this);
    }

    //! Set parent
    void SceneObject::setParent(SceneObject *parent)
    {
        if (parent)
        {
            m_parent = parent;
            getTransform()->setParent(parent->getTransform().get());
            getAttachedEvent().invoke(*this);
        }
        else
        {
            getDetachedEvent().invoke(*this);
            getTransform()->setParent(nullptr);
            m_parent = nullptr;
        }
    }

    // Get parent
    SceneObject *SceneObject::getParent() const
    {
        return m_parent;
    }

    Showcase *SceneObject::getShowcase()
    {
        if (m_root == this)
            return m_showcase;
        return m_root->getShowcase();
    }

    //! Adds a child.
    void SceneObject::addChild(const std::shared_ptr<SceneObject> &child)
    {
        child->setParent(this);

        // Insert to the first available slot.
        for (auto &currObject : m_children)
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
    bool SceneObject::removeChild(const std::shared_ptr<SceneObject> &child)
    {
        if (!child)
            return false;

        // Remove from vector by nullptr assignment
        for (auto &currObject : m_children)
        {
            if (currObject != nullptr && currObject == child)
            {
                currObject = nullptr;
                return true;
            }
        }

        // Recursive remove child
        for (auto &currObject : m_children)
        {
            if (currObject && currObject->removeChild(child))
                return true;
        }

        // Not found, return false
        return false;
    }

    //! Get all children
    std::vector<std::shared_ptr<SceneObject>> &SceneObject::getChildren()
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
        for (auto &child : m_children)
        {
            if (child)
                child->setParent(nullptr);
            child = nullptr;
        }
        m_children.clear();
    }

    //! Add a component
    void SceneObject::addComponent(const std::shared_ptr<Component> &component)
    {
        m_components.push_back(component);
    }

    //! Remove a component
    bool SceneObject::removeComponent(const std::shared_ptr<Component> &component)
    {
        auto it = std::find(m_components.begin(), m_components.end(), component);
        if (it != m_components.end())
        {
            auto result = std::dynamic_pointer_cast<Component>(*it);
            m_componentRemovedEvent.invoke(*this, result);
            m_components.erase(it);
            return true;
        }
        return false;
    }

    //! Remove a component by name
    bool SceneObject::removeComponent(const std::string &name)
    {
        auto found = std::find_if(m_components.begin(), m_components.end(), [&name](auto element) {
            return name.compare(element->getName()) == 0;
        });

        if (found != m_components.end())
        {
            auto result = std::dynamic_pointer_cast<Component>(*found);
            m_componentRemovedEvent.invoke(*this, result);
            m_components.erase(found);
            return true;
        }
        return false;
    }

    //! Add a component by raw pointer
    void SceneObject::addComponent(Component *component)
    {
        m_components.push_back(std::shared_ptr<Component>(component));
    }

    //! Remove a component by raw pointer
    bool SceneObject::removeComponent(Component *component)
    {
        auto it = std::find_if(m_components.begin(), m_components.end(), [&](const auto &comp) {
            return comp.get() == component;
        });
        if (it != m_components.end())
        {
            auto result = std::dynamic_pointer_cast<Component>(*it);
            m_componentRemovedEvent.invoke(*this, result);
            m_components.erase(it);
            return true;
        }
        return false;
    }

    //! Resource added event
    void SceneObject::onResourceAdded(Resource *resource)
    {
        if (m_showcase != nullptr)
        {
            if (resource)
                m_showcase->Add(resource);
        }
    }

    //! Resource removed event
    void SceneObject::onResourceRemoved(Resource *resource)
    {
        if (m_showcase != nullptr)
        {
            if (resource)
                m_showcase->Remove(resource);
        }
    }

    //! Remove all component
    bool SceneObject::removeAllComponents()
    {
        for (auto &comp : m_components)
        {
            auto result = std::dynamic_pointer_cast<Component>(comp);
            m_componentRemovedEvent.invoke(*this, result);
            comp = nullptr;
        }
        m_components.clear();
        return true;
    }

    //! Get components list
    std::vector<std::shared_ptr<Component>> &SceneObject::getComponents()
    {
        return m_components;
    }

    //! Get component count
    size_t SceneObject::getComponentsCount()
    {
        return m_components.size();
    }

    //! Get component by name
    std::shared_ptr<Component> SceneObject::getComponent(std::string name) const
    {
        for (int i = 0; i < m_components.size(); ++i)
        {
            if (m_components[i]->getName().compare(name) == 0)
                return m_components[i];
        }
        return nullptr;
    }

    //! Find object by id
    std::shared_ptr<SceneObject> SceneObject::findObjectById(uint64_t id) const
    {
        for (int i = 0; i < m_children.size(); ++i)
        {
            if (m_children[i] == nullptr)
                continue;
            if (m_children[i]->getId() == id)
                return m_children[i];
            auto ret = m_children[i]->findObjectById(id);
            if (ret)
                return ret;
        }
        return nullptr;
    }

    //! Find object by name
    std::shared_ptr<SceneObject> SceneObject::findObjectByName(std::string name) const
    {
        for (int i = 0; i < m_children.size(); ++i)
        {
            if (m_children[i] == nullptr)
                continue;
            if (m_children[i]->getName() == name)
                return m_children[i];
            auto ret = m_children[i]->findObjectByName(name);
            if (ret)
                return ret;
        }
        return nullptr;
    }

    //! Update function
    void SceneObject::onUpdate(float dt)
    {
        if (isActive())
        {
            for (auto &comp : m_components)
            {
                // Camera updated before other objects
                if (comp->getName() != "CameraComponent")
                    comp->onUpdate(dt);
            }

            for (auto &obj : m_children)
            {
                if (obj != nullptr)
                    obj->onUpdate(dt);
            }
        }
    }

    //! Update function
    void SceneObject::onFixedUpdate(float dt)
    {
        if (isActive())
        {
            for (auto &comp : m_components)
            {
                comp->onFixedUpdate(dt);
            }

            for (auto &obj : m_children)
            {
                if (obj != nullptr)
                    obj->onFixedUpdate(dt);
            }
        }
    }

    //! Update function
    void SceneObject::onLateUpdate(float dt)
    {
        if (isActive())
        {
            for (auto &comp : m_components)
            {
                comp->onLateUpdate(dt);
            }

            for (auto &obj : m_children)
            {
                if (obj != nullptr)
                    obj->onLateUpdate(dt);
            }
        }
    }

    //! Render
    void SceneObject::onRender()
    {
        if (isActive())
        {
            for (auto &comp : m_components)
            {
                // Camera rendered before other objects
                if (comp->getName() != "CameraComponent")
                    comp->onRender();
            }

            for (auto &obj : m_children)
            {
                if (obj != nullptr)
                    obj->onRender();
            }
        }
    }

    //! Enable or disable the actor
    void SceneObject::setActive(bool isActive)
    {
        if (m_isActive != isActive)
        {
            m_isActive = isActive;
            if (m_isActive)
            {
                for (auto &comp : m_components)
                    comp->onEnable();
            }
            else
            {
                for (auto &comp : m_components)
                    comp->onDisable();
            }
        }
    }

    //! Check active
    bool SceneObject::isActive() const
    {
        return m_isActive;
    }

    //! Enable or disable the actor
    void SceneObject::setSelected(bool select)
    {
        if (m_isSelected != select)
        {
            m_isSelected = select;
            if (m_isSelected)
            {
                getSelectedEvent().invoke(*this);
            }
        }

        if (m_isSelected)
        {
            for (auto &comp : m_components)
                comp->onClick();
        }
    }

    //! Check active
    bool SceneObject::isSelected() const
    {
        return m_isSelected;
    }

    //! Serialize
    void SceneObject::to_json(json &j)
    {
        j = json{
            {"id", m_id},
            {"name", m_name},
            {"active", m_isActive},
            {"gui", m_bIsGui},
        };

        auto jComponents = json::array();
        for (const auto &comp : m_components)
        {
            json jCmp;
            comp->to_json(jCmp);
            json jPairCmp = {comp->getName(), jCmp};
            jComponents.push_back(jPairCmp);
        }
        j["comps"] = jComponents;

        auto jChildren = json::array();
        for (const auto &child : m_children)
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
    void SceneObject::from_json(const json &j)
    {
        j.at("id").get_to(m_id);
        setName(j.value("name", ""));
        setActive(j.value("active", false));
        m_bIsGui = j.value("gui", false);

        auto jComps = j.at("comps");
        for (auto it : jComps)
        {
            auto key = it.at(0);
            auto val = it.at(1);
            std::shared_ptr<Component> comp = nullptr;
            if (key == "TransformComponent")
            {
                auto transform = addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f));
                setTransform(transform);
                comp = transform;
            }
            else if (key == "RectTransform")
            {
                auto transform = addComponent<RectTransform>(Vec3(0.f, 0.f, 0.f));
                setTransform(transform);
                comp = transform;
            }
            else if (key == "CameraComponent")
                comp = addComponent<CameraComponent>(val.at("name"));
            else if (key == "EnvironmentComponent")
                comp = addComponent<EnvironmentComponent>(val.at("name"));
            else if (key == "FigureComponent")
                comp = addComponent<FigureComponent>(val.at("path"));
            else if (key == "SpriteComponent")
                comp = addComponent<SpriteComponent>(val.at("size"), val.at("path"));
            else if (key == "ScriptComponent")
                comp = addComponent<ScriptComponent>();
            else if (key == "PhysicBox")
                comp = addComponent<PhysicBox>();
            else if (key == "PhysicSphere")
                comp = addComponent<PhysicSphere>();
            else if (key == "PhysicCapsule")
                comp = addComponent<PhysicCapsule>();
            else if (key == "Canvas")
                comp = addComponent<Canvas>();
            else if (key == "UIImage")
                comp = addComponent<UIImage>();
            else if (key == "UIText")
                comp = addComponent<UIText>();
            else if (key == "UITextField")
                comp = addComponent<UITextField>();
            else if (key == "AudioSource")
                comp = addComponent<AudioSource>();
            else if (key == "AudioListener")
                comp = addComponent<AudioListener>();
            if (comp)
                comp->from_json(val);
        }

        auto jChildren = j.at("children");
        for (auto it : jChildren)
        {
            auto child = std::make_shared<SceneObject>(it.at("id"), it.at("name"), this, it.value("gui", false));
            child->from_json(it);
            addChild(child);
        }
    }
} // namespace ige::scene
