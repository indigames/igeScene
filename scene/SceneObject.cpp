#include <algorithm>

#include "scene/SceneObject.h"
#include "scene/Scene.h"

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
    SceneObject::SceneObject(Scene* scene, uint64_t id, std::string name, SceneObject *parent, bool isGui, const Vec2& size)
        : m_scene(scene), m_id(id), m_name(name), m_bIsGui(isGui), m_isActive(true), m_isSelected(false), m_transform(nullptr)
    {
        // Invoke created event
        getCreatedEvent().invoke(*this);

        // Create and add transform component
        if (isGui)
            m_transform = addComponent<RectTransform>(Vec3(0.f, 0.f, 0.f), size);
        else
            m_transform = addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f));

        // Update parent
        setParent(parent);
    }

    //! Destructor
    SceneObject::~SceneObject()
    {
        if (m_parent)
            m_parent->removeChild(this);
        m_parent = nullptr;
        m_pid = (uint64_t)-1;

        setCanvas(nullptr);
        
        removeAllComponents();
        removeChildren();

        getTransform()->setParent(nullptr);
        m_transform = nullptr;

        getDestroyedEvent().invoke(*this);
    }

    //! Set Name
    void SceneObject::setName(const std::string& name)
    {
        if (m_name != name)
        {
            m_name = name;
            getNameChangedEvent().invoke(*this);
        }
    }

    //! Set parent
    void SceneObject::setParent(SceneObject *parent)
    {
        if (parent)
        {
            m_parent = parent;
            m_pid = m_parent->getId();
            m_parent->addChild(this);
            getTransform()->setParent(m_parent->getTransform().get());
            setCanvas(m_parent->getCanvas());
            getAttachedEvent().invoke(*this);
        }
        else
        {
            getDetachedEvent().invoke(*this);
            if (m_parent)
                m_parent->removeChild(this);
            getTransform()->setParent(nullptr);
            setCanvas(nullptr);
            m_parent = nullptr;
            m_pid = (uint64_t)-1;
        }
    }

    // Get parent
    SceneObject *SceneObject::getParent() const
    {
        return m_parent;
    }


    //! Get all children
    const std::vector<SceneObject*> &SceneObject::getChildren() const
    {
        return m_children;
    }

    //! Add child
    void SceneObject::addChild(SceneObject* child)
    {
        auto found = std::find_if(m_children.begin(), m_children.end(), [&](auto elem) { return elem == child; });
        if (found == m_children.end())
            m_children.push_back(child);
    }

    //! Removes child
    void SceneObject::removeChild(SceneObject* child)
    {
        if (child == nullptr)
            return;
        auto itr = std::find_if(m_children.begin(), m_children.end(), [&](auto elem) { return elem && (elem->getId() == child->getId()); });
        if(itr != m_children.end())
            m_children.erase(itr);
    }

    //! Remove children
    void SceneObject::removeChildren()
    {
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
            {"pid", m_pid},
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
    }

    //! Deserialize
    void SceneObject::from_json(const json &j)
    {
        j.at("id").get_to(m_id);
        setName(j.value("name", ""));
        setActive(j.value("active", false));
        m_bIsGui = j.value("gui", false);
        m_pid = j.value("pid", (uint64_t)-1);

        auto jComps = j.at("comps");
        for (auto it : jComps)
        {
            auto key = it.at(0);
            auto val = it.at(1);
            std::shared_ptr<Component> comp = nullptr;
            if (key == "TransformComponent")
            {
                if(getTransform())
                    comp = getTransform();
                else
                    comp = addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f));
            }
            else if (key == "RectTransform")
            {
                if (getRectTransform())
                    comp = getRectTransform();
                else
                    comp = addComponent<RectTransform>(Vec3(0.f, 0.f, 0.f));
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
    }
} // namespace ige::scene
