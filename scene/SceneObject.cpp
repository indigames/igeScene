#include <algorithm>
#include <sstream>
#include <random>

#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"

#include "event/InputProcessor.h"

#include "components/Component.h"
#include "components/TransformComponent.h"
#include "components/CameraComponent.h"
#include "components/EnvironmentComponent.h"
#include "components/FigureComponent.h"
#include "components/BoneTransform.h"
#include "components/SpriteComponent.h"
#include "components/ScriptComponent.h"
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"
#include "components/gui/UIImage.h"
#include "components/gui/UIText.h"
#include "components/gui/UITextField.h"
#include "components/gui/UIButton.h"
#include "components/gui/UISlider.h"
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
        : m_scene(scene), m_id(id), m_name(name), m_bIsGui(isGui), m_bIsCanvas(isCanvas), m_isActive(true), m_isSelected(false), m_transform(nullptr), m_parent(nullptr),
        m_dispatching(0), m_aabbDirty(2)
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
        removeChildren();

        dispatchEvent((int)EventType::Delete);
        getTransformChangedEvent().removeAllListeners();

        setParent(nullptr);
        setCanvas(nullptr);

        getDestroyedEvent().invoke(*this);

        m_dispatching = 0;
        removeEventListeners();

        getTransform()->setParent(nullptr);
        m_transform = nullptr;

        removeAllComponents();
        m_scene = nullptr;
    }

    //! Generate UUID
    std::string SceneObject::generateUUID(unsigned int len)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 15);
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

    // Check relative recursive
    bool SceneObject::isRelative(uint64_t id)
    {
        if (m_id == id) return true;
        if (m_parent == nullptr) return false;
        if (m_parent->getId() == id) return true;
        return m_parent->isRelative(id);
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
            comp = nullptr;
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

    //! Get component by id
    std::shared_ptr<Component> SceneObject::getComponent(const uint64_t id) const
    {
        for (int i = 0; i < m_components.size(); ++i)
        {
            if (m_components[i]->getInstanceId() == id)
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
        if (m_aabbDirty > 0) {
            updateAabb();
            m_aabbDirty--;
        }

        if (isActive())
        {
            for (auto &comp : m_components)
            {
                // Camera updated before other objects
                if (comp->getName() != "Camera")
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
                if (comp->getName() != "Camera")
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

    //! Event Dispatch System 
    void SceneObject::addEventListener(int eventType, const EventCallback& callback, const uint64_t tag)
    {
        if (tag != 0)
        {
            for (auto it = m_callbacks.begin(); it != m_callbacks.end(); it++)
            {
                if ((*it)->eventType == eventType && (*it)->tag == tag)
                {
                    (*it)->callback = callback;
                    return;
                }
            }
        }

        EventCallbackItem* item = new EventCallbackItem();
        item->callback = callback;
        item->eventType = eventType;
        item->dispatching = 0;
        item->tag = tag;
        m_callbacks.push_back(item);
    }

    void SceneObject::removeEventListener(int eventType, const uint64_t tag)
    {
        if (m_callbacks.empty())
            return;

        for (auto it = m_callbacks.begin(); it != m_callbacks.end(); )
        {
            if ((*it)->eventType == eventType && (((*it)->tag == tag || tag == 0)))
            {
                if (m_dispatching > 0)
                {
                    (*it)->callback = nullptr;
                    it++;
                }
                else
                {
                    delete (*it);
                    it = m_callbacks.erase(it);
                }
            }
            else
                it++;
        }
    }

    void SceneObject::removeEventListeners()
    {
        if (m_callbacks.empty())
            return;

        if (m_dispatching > 0)
        {
            for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
                (*it)->callback = nullptr;
        }
        else
        {
            for (auto it = m_callbacks.begin(); it != m_callbacks.end(); it++)
                delete (*it);
            m_callbacks.clear();
        }
    }

    bool SceneObject::hasEventListener(int eventType, const uint64_t tag) const
    {
        if (m_callbacks.empty())
            return false;

        for (auto it = m_callbacks.cbegin(); it != m_callbacks.cend(); ++it)
        {
            if ((*it)->eventType == eventType && (((*it)->tag == tag || tag == 0)) && (*it)->callback != nullptr)
                return true;
        }
        return false;
    }

    bool SceneObject::dispatchEvent(int eventType, const Value& dataValue)
    {
        if (m_callbacks.size() == 0)
            return false;

        EventContext context;
        if (InputProcessor::getInstance())
            context.m_inputEvent = InputProcessor::getInstance()->getRecentInput();
        context.m_sender = this;
        context.m_type = eventType;
        context.m_dataValue = dataValue;

        doDispatch(eventType, &context);

        return context.isDefaultPrevented();
    }

    bool SceneObject::bubbleEvent(int eventType, const Value& dataValue)
    {
        EventContext context;
        if (InputProcessor::getInstance())
            context.m_inputEvent = InputProcessor::getInstance()->getRecentInput();
        context.m_type = eventType;
        context.m_dataValue = dataValue;

        doBubble(eventType, &context);

        return context.m_defaultPrevented;
    }


    void SceneObject::doDispatch(int eventType, EventContext* context)
    {
        m_dispatching++;
        context->m_sender = this;
        bool hasDeletedItems = false;

        size_t cnt = m_callbacks.size(); //dont use iterator, because new item would be added in callback.
        for (size_t i = 0; i < cnt; i++)
        {
            EventCallbackItem* ci = m_callbacks[i];
            if (ci->callback == nullptr)
            {
                hasDeletedItems = true;
                continue;
            }
            if (ci->eventType == eventType)
            {
                ci->dispatching++;
                context->m_touchCapture = 0;
                ci->callback(context);
                ci->dispatching--;
                if (context->m_touchCapture != 0 && dynamic_cast<SceneObject*>(this))
                {
                    if (context->m_touchCapture == 1 && eventType == (int)EventType::TouchBegin)
                        context->getInput()->getProcessor()->addTouchMonitor(context->getInput()->getTouchId(), this);
                    else if (context->m_touchCapture == 2)
                        context->getInput()->getProcessor()->removeTouchMonitor(this);
                }
            }
        }

        m_dispatching--;
        if (hasDeletedItems && m_dispatching == 0)
        {
            for (auto it = m_callbacks.begin(); it != m_callbacks.end(); )
            {
                if ((*it)->callback == nullptr)
                {
                    delete (*it);
                    it = m_callbacks.erase(it);
                }
                else
                    it++;
            }
        }
    }
    
    void SceneObject::doBubble(int eventType, EventContext* context)
    {
        //parent maybe disposed in callbacks
        auto p = this->getParent();

        if (!m_callbacks.empty())
        {
            context->m_bIsStopped = false;
            doDispatch(eventType, context);
            if (context->m_bIsStopped)
                return;
        }
        if (p != nullptr) {
            p->doBubble(eventType, context);
        }
    }

    //! Transform changed event
    void SceneObject::onTransformChanged(SceneObject& sceneObject)
    {
        if (m_aabbDirty != 0) return;
        //updateAabb();
        m_aabbDirty = 2;
    }

    //! Update AABB
    void SceneObject::updateAabb()
    {
        // Ignore Canvas and root object
        if (getComponent<Canvas>() != nullptr || getParent() == nullptr)
        {
            m_frameAABB = m_aabbWorld = m_aabb = AABBox({ 0.f, 0.f, 0.f }, { -1.f, -1.f, -1.f });
            return;
        }

        m_aabb = AABBox({ -0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f });

        if (getComponent<FigureComponent>() != nullptr)
        {
            auto figureComp = getComponent<FigureComponent>();
            if (figureComp->getFigure())
            {
                Vec3 aabbMin, aabbMax;
                figureComp->getFigure()->CalcAABBox(0, aabbMin.P(), aabbMax.P(), LocalSpace);
                m_aabb = { aabbMin, aabbMax };
                if (m_aabb.getVolume() == 0)
                {
                    if ((aabbMax[0] - aabbMin[0]) == 0) aabbMax[0] = 0.01f;
                    if ((aabbMax[1] - aabbMin[1]) == 0) aabbMax[1] = 0.01f;
                    if ((aabbMax[2] - aabbMin[2]) == 0) aabbMax[2] = 0.01f;
                    m_aabb = { aabbMin, aabbMax };
                }
            }
        }
        else if (getComponent<SpriteComponent>() != nullptr && !isGUIObject())
        {
            auto spriteComp = getComponent<SpriteComponent>();
            if (spriteComp->getFigure())
            {
                Vec3 aabbMin, aabbMax;
                spriteComp->getFigure()->CalcAABBox(0, aabbMin.P(), aabbMax.P());
                m_aabb = { aabbMin, aabbMax };
            }
        }
        else if (isGUIObject()) {
            if (getComponent<UIText>() != nullptr)
            {
                auto uiText = getComponent<UIText>();
                if (uiText->getFigure())
                {
                    Vec3 aabbMin, aabbMax;
                    uiText->getFigure()->CalcAABBox(0, aabbMin.P(), aabbMax.P());
                    m_aabb = { aabbMin, aabbMax };
                }
            }
            else if (getComponent<RectTransform>() != nullptr) {
                auto rect = getComponent<RectTransform>();
                if (rect) {
                    auto size = rect->getSize();
                    auto scale = rect->getScale();
                    auto rot = rect->getRotation();
                    Vec3 min(-size[0] * 0.5f, -size[1] * 0.5f, -0.5f);
                    Vec3 max(size[0] * 0.5f, size[1] * 0.5f, 0.5f);
                    m_aabb = { min, max };
                }
            }
        }

        // Update world AABB
        m_aabbWorld = m_aabb.Transform(m_transform->getWorldMatrix());
        m_frameAABB = m_bLockedFrameAABB ? m_frameAABB : m_aabbWorld;
    }

    //! Find first child by name
    SceneObject* SceneObject::findChildByName(const std::string& name)
    {
        auto found = std::find_if(m_children.begin(), m_children.end(), [&](auto elem)
        {
            return elem->getName() == name;
        });
        if (found != m_children.end())
            return (*found);
        return nullptr;
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
            {"raycast", m_bIsRaycastTarget},
            {"interactable", m_bIsInteractable}
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
        m_bIsRaycastTarget = j.value("raycast", false);
        m_bIsInteractable = j.value("interactable", false);

        auto jComps = j.at("comps");
        for (auto it : jComps)
        {
            auto key = it.at(0);
            auto val = it.at(1);
            std::shared_ptr<Component> comp = nullptr;
            if (key == "Transform")
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
            else if (key == "Camera")
                comp = addComponent<CameraComponent>(val.at("name"));
            else if (key == "Environment")
                comp = addComponent<EnvironmentComponent>();
            else if (key == "BoneTransform")
                comp = addComponent<BoneTransform>();
            else if (key == "Figure")
                comp = addComponent<FigureComponent>(val.at("path"));
            else if (key == "Sprite")
                comp = addComponent<SpriteComponent>(val.at("path"), val.at("size"));
            else if (key == "Script")
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
            else if (key == "UIButton")
                comp = addComponent<UIButton>();
            else if (key == "UISlider")
                comp = addComponent<UISlider>();
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
                    addComponent<FigureComponent>(GetEditorResource("figures/camera.pyxf"))->setSkipSerialize(true);
            }

            if (auto directionalLight = getComponent<DirectionalLight>())
            {
                if (!getComponent<FigureComponent>() && !getComponent<SpriteComponent>())
                    addComponent<SpriteComponent>(GetEditorResource("sprites/direct-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
            }

            if (auto pointLight = getComponent<PointLight>())
            {
                if (!getComponent<FigureComponent>() && !getComponent<SpriteComponent>())
                    addComponent<SpriteComponent>(GetEditorResource("sprites/point-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
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
