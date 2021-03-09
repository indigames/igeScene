#include <algorithm>
#include <sstream>
#include <random>

#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"

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
#include "components/physic/PhysicManager.h"
#include "components/physic/PhysicBox.h"
#include "components/physic/PhysicCapsule.h"
#include "components/physic/PhysicSphere.h"
#include "components/physic/PhysicMesh.h"
#include "components/physic/PhysicSoftBody.h"
#include "components/audio/AudioManager.h"
#include "components/audio/AudioSource.h"
#include "components/audio/AudioListener.h"
#include "components/light/AmbientLight.h"
#include "components/light/DirectionalLight.h"
#include "components/light/PointLight.h"
#include "components/particle/ParticleManager.h"
#include "components/particle/Particle.h"
#include "components/navigation/DynamicNavMesh.h"
#include "components/navigation/NavAgent.h"
#include "components/navigation/NavAgentManager.h"
#include "components/navigation/Navigable.h"
#include "components/navigation/NavMesh.h"
#include "components/navigation/NavObstacle.h"
#include "components/navigation/OffMeshLink.h"

namespace ige::scene
{
    //! Events
    Event<SceneObject &> SceneObject::s_destroyedEvent;
    Event<SceneObject &> SceneObject::s_createdEvent;
    Event<SceneObject &> SceneObject::s_attachedEvent;
    Event<SceneObject &> SceneObject::s_detachedEvent;
    Event<SceneObject &> SceneObject::s_selectedEvent;
    Event<SceneObject&> SceneObject::s_deselectedEvent;

    //! Constructor
    SceneObject::SceneObject(Scene *scene, uint64_t id, std::string name, SceneObject *parent, bool isGui, const Vec2 &size, bool isCanvas)
        : m_scene(scene), m_id(id), m_name(name), m_bIsGui(isGui), m_bIsCanvas(isCanvas), m_isActive(true), m_isSelected(false), m_transform(nullptr), m_parent(nullptr)
    {
        // Generate new UUID
        m_uuid = generateUUID();

        // Invoke created event
        getCreatedEvent().invoke(*this);

        // Update parent
        setParent(parent);

        // Create and add transform component
        if (isGui)
            m_transform = addComponent<RectTransform>(Vec3(0.f, 0.f, 0.f), size);
        else
            m_transform = addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f));

        // Update parent transform
        if (getParent())
            m_transform->setParent(getParent()->getTransform().get());

        // Set AABB to default
        m_aabb = AABBox({ 0.f, 0.f, 0.f }, { -1.f, -1.f, -1.f });
        m_frameAABB = AABBox({ -5.f, -5.f, -5.f }, { 5.f, 5.f, 5.f });
        getTransformChangedEvent().addListener(std::bind(&SceneObject::onTransformChanged, this, std::placeholders::_1));
    }

    //! Destructor
    SceneObject::~SceneObject()
    {
        getTransformChangedEvent().removeAllListeners();

        setParent(nullptr);
        setCanvas(nullptr);

        removeAllComponents();
        removeChildren();

        getTransform()->setParent(nullptr);
        m_transform = nullptr;

        getDestroyedEvent().invoke(*this);
    }

    //! Generate UUID
    std::string SceneObject::generateUUID(unsigned int len)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        std::stringstream ss;
        for (auto i = 0; i < len; i++)
            ss << std::hex << dis(gen);
        return ss.str();
    }

    //! Set Name
    void SceneObject::setName(const std::string &name)
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
        if (m_parent)
        {
            m_parent->removeChild(this);

            if (getTransform())
                getTransform()->setParent(nullptr);
            getDetachedEvent().invoke(*this);
            setCanvas(nullptr);
            m_parent = nullptr;
        }

        if (parent)
        {
            m_parent = parent;
            m_parent->addChild(this);
            if (getTransform())
                getTransform()->setParent(m_parent->getTransform().get());
            setCanvas(m_parent->getCanvas());
            getAttachedEvent().invoke(*this);
        }
    }

    // Get parent
    SceneObject *SceneObject::getParent() const
    {
        return m_parent;
    }

    //! Get all children
    const std::vector<SceneObject *> &SceneObject::getChildren() const
    {
        return m_children;
    }

    //! Add child
    void SceneObject::addChild(SceneObject *child)
    {
        auto found = std::find_if(m_children.begin(), m_children.end(), [&](auto elem) { return elem == child; });
        if (found == m_children.end())
            m_children.push_back(child);
    }

    //! Removes child
    void SceneObject::removeChild(SceneObject *child)
    {
        if (child == nullptr)
            return;
        auto itr = std::find_if(m_children.begin(), m_children.end(), [&](auto elem) { return elem && (elem->getId() == child->getId()); });
        if (itr != m_children.end())
            m_children.erase(itr);
    }

    //! Remove children
    void SceneObject::removeChildren()
    {
        for (auto &child : m_children)
            child = nullptr;
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
            m_components.erase(found);
            return true;
        }
        return false;
    }

    //! Remove all component
    bool SceneObject::removeAllComponents()
    {
        for (auto &comp : m_components)
        {
            if(comp) comp = nullptr;
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
    std::shared_ptr<Component> SceneObject::getComponent(const std::string& name) const
    {
        for (int i = 0; i < m_components.size(); ++i)
        {
            if (m_components[i]->getName().compare(name) == 0)
                return m_components[i];
        }
        return nullptr;
    }

    //! Get components by type recursively
    void SceneObject::getComponentsRecursive(std::vector<Component*>& components, const std::string& type) const
    {
        auto comp = getComponent(type);
        if(comp != nullptr)
            components.push_back(comp.get());

        for (const auto &child : m_children)
        {
            if (child)
                child->getComponentsRecursive(components, type);
        }
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
            else
            {
                getDeselectedEvent().invoke(*this);
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

    //! Get scene root
    SceneObject* SceneObject::getRoot()
    {
        return m_scene ? m_scene->getRoot().get() : nullptr;
    }

    //! Transform changed event
    void SceneObject::onTransformChanged(SceneObject& sceneObject)
    {
        updateAabb();
    }

    //! Update AABB
    void SceneObject::updateAabb()
    {
        // Ignore canvas object
        if (getComponent<Canvas>() != nullptr || getParent() == nullptr)
        {
            m_aabb = AABBox({ 0.f, 0.f, 0.f }, { -1.f, -1.f, -1.f });
            m_aabbWorld = m_aabb.Transform(m_transform->getWorldMatrix());
            return;
        }

        auto figureComp = getComponent<FigureComponent>();
        if (figureComp && figureComp->getFigure())
        {
            figureComp->onUpdate(0.33f);
            Vec3 aabbMin, aabbMax;
            figureComp->getFigure()->CalcAABBox(0, aabbMin.P(), aabbMax.P(), LocalSpace);
            m_aabb = { aabbMin, aabbMax };
            m_aabbWorld = m_aabb.Transform(m_transform->getWorldMatrix());
            m_frameAABB = m_bLockedFrameAABB ? m_frameAABB : m_aabbWorld;
            return;
        }

        auto spriteComp = getComponent<SpriteComponent>();
        if (spriteComp && spriteComp->getFigure())
        {
            spriteComp->onUpdate(0.33f);
            Vec3 aabbMin, aabbMax;
            spriteComp->getFigure()->CalcAABBox(0, aabbMin.P(), aabbMax.P());
            m_aabb = { aabbMin, aabbMax };
            m_aabbWorld = m_aabb.Transform(m_transform->getWorldMatrix());
            m_frameAABB = m_bLockedFrameAABB ? m_frameAABB : m_aabbWorld;
            return;
        }

        auto uiText = getComponent<UIText>();
        if (uiText && uiText->getFigure())
        {
            uiText->onUpdate(0.33f);
            Vec3 aabbMin, aabbMax;
            uiText->getFigure()->CalcAABBox(0, aabbMin.P(), aabbMax.P());
            m_aabb = { aabbMin, aabbMax };
            m_aabbWorld = m_aabb.Transform(m_transform->getWorldMatrix());
            m_frameAABB = m_bLockedFrameAABB ? m_frameAABB : m_aabbWorld;
            return;
        }

        m_aabb = AABBox({ 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f });
        m_aabbWorld = m_aabb.Transform(m_transform->getWorldMatrix());
        return;
    }

    //! Serialize
    void SceneObject::to_json(json &j)
    {
        j = json{
            {"uuid", m_uuid},
            {"name", m_name},
            {"active", m_isActive},
            {"gui", m_bIsGui},
            {"cvs", m_bIsCanvas},
        };

        auto jComponents = json::array();
        for (const auto &comp : m_components)
        {
            if (!comp->isSkipSerialize())
            {
                jComponents.push_back({comp->getName(), json(*comp.get())});
            }
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
        j["childs"] = jChildren;
    }

    //! Deserialize
    void SceneObject::from_json(const json &j)
    {
        setName(j.value("name", ""));
        setUUID(j.value("uuid", getUUID()));
        setActive(j.value("active", false));
        m_bIsGui = j.value("gui", false);
        m_bIsCanvas = j.value("cvs", false);

        auto jComps = j.at("comps");
        for (auto it : jComps)
        {
            auto key = it.at(0);
            auto val = it.at(1);
            std::shared_ptr<Component> comp = nullptr;
            if (key == "TransformComponent")
            {
                if (getTransform())
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
                comp = addComponent<EnvironmentComponent>();
            else if (key == "FigureComponent")
                comp = addComponent<FigureComponent>(val.at("path"));
            else if (key == "SpriteComponent")
                comp = addComponent<SpriteComponent>(val.at("path"), val.at("size"));
            else if (key == "ScriptComponent")
                comp = addComponent<ScriptComponent>();
            else if (key == "PhysicManager")
                comp = addComponent<PhysicManager>();
            else if (key == "PhysicBox")
                comp = addComponent<PhysicBox>();
            else if (key == "PhysicSphere")
                comp = addComponent<PhysicSphere>();
            else if (key == "PhysicCapsule")
                comp = addComponent<PhysicCapsule>();
            else if (key == "PhysicMesh")
                comp = addComponent<PhysicMesh>();
            else if (key == "PhysicSoftBody")
                comp = addComponent<PhysicSoftBody>();
            else if (key == "Canvas")
            {
                comp = addComponent<Canvas>();
                setCanvas(getComponent<Canvas>());
            }
            else if (key == "UIImage")
                comp = addComponent<UIImage>();
            else if (key == "UIText")
                comp = addComponent<UIText>();
            else if (key == "UITextField")
                comp = addComponent<UITextField>();
            else if (key == "AudioManager")
                comp = addComponent<AudioManager>();
            else if (key == "AudioSource")
                comp = addComponent<AudioSource>();
            else if (key == "AudioListener")
                comp = addComponent<AudioListener>();
            else if (key == "AmbientLight")
                comp = addComponent<AmbientLight>();
            else if (key == "DirectionalLight")
                comp = addComponent<DirectionalLight>();
            else if (key == "PointLight")
                comp = addComponent<PointLight>();
            else if (key == "ParticleManager")
                comp = addComponent<ParticleManager>();
            else if (key == "Particle")
                comp = addComponent<Particle>();
            else if (key == "DynamicNavMesh")
                comp = addComponent<DynamicNavMesh>();
            else if (key == "NavAgent")
                comp = addComponent<NavAgent>();
            else if (key == "NavAgentManager")
                comp = addComponent<NavAgentManager>();
            else if (key == "Navigable")
                comp = addComponent<Navigable>();
            else if (key == "NavMesh")
                comp = addComponent<NavMesh>();
            else if (key == "NavObstacle")
                comp = addComponent<NavObstacle>();
            else if (key == "OffMeshLink")
                comp = addComponent<OffMeshLink>();
            if (comp)
                val.get_to(*comp);
        }

        // Add editor camera figure debug
        if (SceneManager::getInstance()->isEditor())
        {
            if (auto camera = getComponent<CameraComponent>())
            {
                if (!getComponent<FigureComponent>())
                    addComponent<FigureComponent>("figure/camera.pyxf")->setSkipSerialize(true);
            }

            if (auto canvas = getComponent<Canvas>())
            {
                if (!getComponent<UIImage>())
                    addComponent<UIImage>("sprite/rect")->setSkipSerialize(true);
            }

            if (auto directionalLight = getComponent<DirectionalLight>())
            {
                if (!getComponent<FigureComponent>() && !getComponent<SpriteComponent>())
                    addComponent<SpriteComponent>("sprite/sun", Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
            }

            if (auto pointLight = getComponent<PointLight>())
            {
                if (!getComponent<FigureComponent>() && !getComponent<SpriteComponent>())
                    addComponent<SpriteComponent>("sprite/light", Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
            }
        }

        auto jChildren = j.at("childs");
        auto thisObj = getScene()->findObjectById(getId());
        for (auto it : jChildren)
        {
            auto child = getScene()->createObject(it.at("name"), thisObj, it.value("gui", false), {}, it.value("cvs", false));
            child->from_json(it);
        }
    }
} // namespace ige::scene
