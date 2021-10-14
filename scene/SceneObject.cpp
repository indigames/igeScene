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
#include "components/TextComponent.h"
#include "components/TextBitmapComponent.h"
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"
#include "components/gui/UIImage.h"
#include "components/gui/UIText.h"
#include "components/gui/UITextBitmap.h"
#include "components/gui/UITextField.h"
#include "components/gui/UIButton.h"
#include "components/gui/UISlider.h"
#include "components/gui/UIScrollView.h"
#include "components/gui/UIScrollBar.h"
#include "components/gui/UIMask.h"
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
#include "components/light/SpotLight.h"
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
    SceneObject::SceneObject(Scene *scene, uint64_t id, std::string name, bool isGui, const Vec2 &size, const std::string& prefabId)
        : m_scene(scene), m_id(id), m_name(name), m_bIsGui(isGui), m_isActive(true), m_isSelected(false), m_transform(nullptr),
        m_dispatching(0), m_aabbDirty(2), m_bIsInMask(false)
    {
        m_parent.reset();

        // Generate new UUID
        m_uuid = generateUUID();

        // Create and add transform component
        if (isGui)
            m_transform = addComponent<RectTransform>(Vec3(0.f, 0.f, 0.f), size);
        else
            m_transform = addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f));

        // Set AABB to default
        m_aabb = AABBox({ 0.f, 0.f, 0.f }, { -1.f, -1.f, -1.f });
        m_frameAABB = AABBox({ -5.f, -5.f, -5.f }, { 5.f, 5.f, 5.f });
        getTransformChangedEvent().addListener(std::bind(&SceneObject::onTransformChanged, this, std::placeholders::_1));

        // Set prefabId
        setPrefabId(prefabId);

        // Invoke created event
        getCreatedEvent().invoke(*this);
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
    void SceneObject::setParent(std::shared_ptr<SceneObject> parent)
    {
        if (getParent())
        {
            getParent()->removeChildById(getId());
            getDetachedEvent().invoke(*this);

            if (getTransform()) getTransform()->setParent(nullptr);
            setCanvas(nullptr);
            m_parent.reset();
        }

        //auto parent_temp = (parent == nullptr) ? getRoot() : parent;

        if (parent)
        {
            m_parent = parent;
            getParent()->addChildById(getId());
            if (getTransform())
                getTransform()->setParent(getParent()->getTransform());
            setCanvas(getParent()->getCanvas());
            m_transform->setParent(getParent()->getTransform());
            if (isGUIObject()) {
                if (parent->isInMask()) {
                    setInMask(true);
                } else {
                    auto mask = parent->getComponent<UIMask>();
                    if (mask) setInMask(mask->isUseMask());
                }
            }
            getAttachedEvent().invoke(*this);
        }

        dispatchEvent((int)EventType::SetParent);
    }

    // Get thj
    std::shared_ptr<SceneObject> SceneObject::getSharedPtr() const
    {
        return getScene() ? getScene()->findObjectById(getId()) : nullptr;
    }

    // Get parent
    std::shared_ptr<SceneObject> SceneObject::getParent() const
    {
        return m_parent.expired() ? nullptr : m_parent.lock();
    }

    // Check relative recursive
    bool SceneObject::isRelative(uint64_t id)
    {
        if (m_id == id) return true;
        if (getParent() == nullptr) return false;
        if (getParent()->getId() == id) return true;
        return getParent()->isRelative(id);
    }

    //! Get all children
    std::vector<std::weak_ptr<SceneObject>>& SceneObject::getChildren()
    {
        return m_children;
    }

    //! Add child
    void SceneObject::addChild(std::shared_ptr<SceneObject> child)
    {
        if (child == nullptr) return;
        auto found = std::find_if(m_children.begin(), m_children.end(), [&](auto elem) { return !elem.expired() && elem.lock()->getId() == child->getId(); });
        if (found == m_children.end())
        {
            m_children.push_back(child);
            dispatchEvent((int)EventType::AddChild, Value(child->getUUID()));
        }            
    }

    //! Add child by id
    void SceneObject::addChildById(uint64_t id)
    {
        if (getScene() == nullptr) return;
        return addChild(getScene()->findObjectById(id));
    }

    //! Removes child
    void SceneObject::removeChild(std::shared_ptr<SceneObject> child)
    {
        auto itr = std::find_if(m_children.begin(), m_children.end(), [&](auto elem) { return !elem.expired() && elem.lock()->getId() == child->getId(); });
        if (itr != m_children.end()) {
            m_children.erase(itr);
            dispatchEvent((int)EventType::RemoveChild, Value(child->getUUID()));
        }
    }

    //! Removes child
    void SceneObject::removeChildById(uint64_t id)
    {
        auto itr = std::find_if(m_children.begin(), m_children.end(), [&](auto elem) { return !elem.expired() && (elem.lock()->getId() == id); });
        if (itr != m_children.end()) {
            if (!itr->expired())
                dispatchEvent((int)EventType::RemoveChild, Value((*itr).lock()->getUUID()));
            m_children.erase(itr);
        }
    }

    //! Remove children
    void SceneObject::removeChildren()
    {
        m_children.clear();
        dispatchEvent((int)EventType::RemoveChildren);
    }

    //! Find Child
    std::shared_ptr<SceneObject> SceneObject::findChild(const std::string& uuid)
    {
        auto itr = std::find_if(m_children.begin(), m_children.end(), [&](auto elem) { return !elem.expired() && (elem.lock()->getUUID().compare(uuid) == 0); });
        if (itr != m_children.end() && !itr->expired())
            return (*itr).lock();
        return nullptr;
    }

    //! Find Child
    std::shared_ptr<SceneObject> SceneObject::findChildById(uint64_t id)
    {
        auto itr = std::find_if(m_children.begin(), m_children.end(), [&](auto elem) { return !elem.expired() && (elem.lock()->getId() == id); });
        if (itr != m_children.end() && !itr->expired())
            return (*itr).lock();
        return nullptr;
    }

    //! Create a component by name
    std::shared_ptr<Component> SceneObject::createComponent(const std::string& name)
    {
        std::shared_ptr<Component> comp = nullptr;
        if (name == "Transform")
        {
            if (getTransform())
                return getTransform();
            return addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f));
        }
        if (name == "RectTransform")
        {
            if (getRectTransform())
                return getRectTransform();
            return addComponent<RectTransform>(Vec3(0.f, 0.f, 0.f));
        }
        
        if (name == "Canvas")
        {
            auto comp = addComponent<Canvas>();
            setCanvas(getComponent<Canvas>());
            return comp;
        }

        if (name == "Camera") return addComponent<CameraComponent>();
        if (name == "Environment") return addComponent<EnvironmentComponent>();
        if (name == "BoneTransform") return addComponent<BoneTransform>();
        if (name == "Figure") return addComponent<FigureComponent>();
        if (name == "Sprite") return addComponent<SpriteComponent>();
        if (name == "Text") return addComponent<TextComponent>();
        if (name == "TextBitmap") return addComponent<TextBitmapComponent>();
        if (name == "Script") return addComponent<ScriptComponent>();
        if (name == "PhysicManager") return addComponent<PhysicManager>();
        if (name == "PhysicBox") return addComponent<PhysicBox>();
        if (name == "PhysicSphere") return addComponent<PhysicSphere>();
        if (name == "PhysicCapsule") return addComponent<PhysicCapsule>();
        if (name == "PhysicMesh") return addComponent<PhysicMesh>();
        if (name == "PhysicSoftBody") return addComponent<PhysicSoftBody>();
        if (name == "UIImage") return addComponent<UIImage>();
        if (name == "UIText") return addComponent<UIText>();
        if (name == "UITextBitmap") return addComponent<UITextBitmap>();
        if (name == "UITextField") return addComponent<UITextField>();
        if (name == "UIButton") return addComponent<UIButton>();
        if (name == "UISlider") return addComponent<UISlider>();
        if (name == "UIScrollView") return addComponent<UIScrollView>();
        if (name == "UIScrollBar") return addComponent<UIScrollBar>();
        if (name == "UIMask") return addComponent<UIMask>();
        if (name == "AudioManager") return addComponent<AudioManager>();
        if (name == "AudioSource") return addComponent<AudioSource>();
        if (name == "AudioListener") return addComponent<AudioListener>();
        if (name == "AmbientLight") return addComponent<AmbientLight>();
        if (name == "DirectionalLight") return addComponent<DirectionalLight>();
        if (name == "PointLight") return addComponent<PointLight>();
        if (name == "SpotLight") return addComponent<SpotLight>();
        if (name == "ParticleManager") return addComponent<ParticleManager>();
        if (name == "Particle") return addComponent<Particle>();
        if (name == "DynamicNavMesh") return addComponent<DynamicNavMesh>();
        if (name == "NavAgent") return addComponent<NavAgent>();
        if (name == "NavAgentManager") return addComponent<NavAgentManager>();
        if (name == "Navigable") return addComponent<Navigable>();
        if (name == "NavMesh") return addComponent<NavMesh>();
        if (name == "NavObstacle") return addComponent<NavObstacle>();
        if (name == "OffMeshLink") return addComponent<OffMeshLink>();
        return nullptr;
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


    //! Remove a component
    bool SceneObject::removeComponent(uint64_t id)
    {
        auto it = std::find_if(m_components.begin(), m_components.end(), [&](auto& elem) {
            return elem->getInstanceId() == id;
        });
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
            if (!child.expired())
                child.lock()->getComponentsRecursive(components, type);
        }
    }

    //! Get components by type recursively
    std::shared_ptr<Component> SceneObject::getFirstComponentRecursive(const std::string& type) const
    {
        auto comp = getComponent(type);
        if(comp != nullptr)
            return comp;

        for (const auto &child : m_children)
        {
            if (!child.expired()) {
                auto comp = child.lock()->getFirstComponentRecursive(type);
                if (comp != nullptr)
                    return comp;
            }
        }
        return nullptr;
    }

    std::shared_ptr<ScriptComponent> SceneObject::getScript(const std::string& path) const
    {
        for (int i = 0; i < m_components.size(); ++i)
        {
            if (m_components[i]->getType() == Component::Type::Script) {
                auto script = std::dynamic_pointer_cast<ScriptComponent>(m_components[i]);
                if(script->getPath().compare(path) == 0)
                    return script;
            }
        }
        return nullptr;
    }

    //! Update function
    void SceneObject::onUpdate(float dt)
    {
        if (m_aabbDirty > 0) {
            updateAabb();
            m_aabbDirty--;
        }

        auto itr = m_components.begin();
        while (itr != m_components.end()) {
            if (*itr == nullptr) { itr = m_components.erase(itr); continue; }
            auto comp = *itr;
            if (isActive() && comp->getName() != "Camera" && comp->isEnabled())
                comp->onUpdate(dt);
            ++itr;
        }
    }

    //! Update function
    void SceneObject::onFixedUpdate(float dt)
    {
        if (isActive())
        {
            for (auto &comp : m_components)
            {
                if(comp->isEnabled())
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
                if(comp->isEnabled())
                    comp->onLateUpdate(dt);
            }
        }
    }

    //! Physic Update function
    void SceneObject::onPhysicUpdate(float dt)
    {
        if (isActive())
        {
            for (auto& comp : m_components)
            {
                if(comp->isEnabled())
                    comp->onPhysicUpdate(dt);
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
                if (comp->getName() != "Camera" && comp->isEnabled())
                    comp->onRender();
            }
        }
    }

    //! Enable or disable the actor
    void SceneObject::setActive(bool isActive, bool recursive)
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

            if (recursive) {
                for (auto child : m_children) {
                    if (!child.expired())
                        child.lock()->setActive(isActive, recursive);
                }
            }
            else {
                activeChildren(isActive);
            }
        }
    }

    void SceneObject::activeChildren(bool active)
    {
        for (auto& child : m_children) {
            if (!child.expired()) {
                auto childPtr = child.lock();
                if (childPtr) {
                    auto comps = childPtr->getComponents();
                    for (auto& comp : comps) {
                        if (isActive() && active)
                            comp->onEnable();
                        else
                            comp->onDisable();
                    }
                    childPtr->activeChildren(active);
                }
            }
        }
    }

    //! Check active
    bool SceneObject::isActive() const
    {
        return m_isActive;
    }

    //! Enable or disable the actor
    void SceneObject::setSelected(bool select, bool recursive)
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

        if (recursive)
        {
            for (auto& child : m_children) {
                if(!child.expired())
                    child.lock()->setSelected(select, recursive);
            }
        }
    }

    //! Check active
    bool SceneObject::isSelected() const
    {
        return m_isSelected;
    }

    //! Get scene root
    std::shared_ptr<SceneObject> SceneObject::getRoot()
    {
        return m_scene ? m_scene->getRoot() : nullptr;
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
        context.m_sender = this;
        context.m_type = eventType;
        context.m_dataValue = dataValue;

        doDispatch(eventType, &context);

        return context.isDefaultPrevented();
    }

    bool SceneObject::dispatchEventIncludeChild(int eventType, const Value& dataValue)
    {
        /*if (m_callbacks.size() == 0)
            return false;*/

        EventContext context;
        context.m_sender = this;
        context.m_type = eventType;
        context.m_dataValue = dataValue;

        doDispatch(eventType, &context, true);

        return context.isDefaultPrevented();
    }

    bool SceneObject::bubbleEvent(int eventType, const Value& dataValue)
    {
        EventContext context;
        context.m_type = eventType;
        context.m_dataValue = dataValue;

        doBubble(eventType, &context);

        return context.m_defaultPrevented;
    }

    bool SceneObject::dispatchInputEvent(int eventType, const Value& dataValue)
    {
        if (m_callbacks.size() == 0)
            return false;

        InputEventContext context;
        if (InputProcessor::getInstance())
            context.m_inputEvent = InputProcessor::getInstance()->getRecentInput();
        context.m_sender = this;
        context.m_type = eventType;
        context.m_dataValue = dataValue;

        doDispatch(eventType, &context);

        return context.isDefaultPrevented();
    }

    bool SceneObject::bubbleInputEvent(int eventType, const Value& dataValue)
    {
        InputEventContext context;
        if (InputProcessor::getInstance())
            context.m_inputEvent = InputProcessor::getInstance()->getRecentInput();
        context.m_type = eventType;
        context.m_dataValue = dataValue;

        doBubble(eventType, &context);

        return context.m_defaultPrevented;
    }


    void SceneObject::doDispatch(int eventType, EventContext* context, bool includeChild)
    {
        if (includeChild) {
            for (auto& child : m_children) {
                if(!child.expired())
                    child.lock()->doDispatch(eventType, context, includeChild);
            }
        }

        if (m_callbacks.empty()) return;

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
                auto inputContext = dynamic_cast<InputEventContext*>(context);
                if (inputContext != nullptr) {
                    ci->dispatching++;
                    inputContext->m_touchCapture = 0;
                    ci->callback(inputContext);
                    ci->dispatching--;
                    if (inputContext->m_touchCapture != 0)
                    {
                        auto thisPtr = getSharedPtr();
                        if (inputContext->isCaptureTouch() && eventType == (int)EventType::TouchBegin)
                            inputContext->getInput()->getProcessor()->addTouchMonitor(inputContext->getInput()->getTouchId(), thisPtr);
                        else if (inputContext->isUnCaptureTouch())
                            inputContext->getInput()->getProcessor()->removeTouchMonitor(thisPtr);
                    }
                }
                else
                {
                    ci->dispatching++;
                    ci->callback(context);
                    ci->dispatching--;
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
                figureComp->getFigure()->CalcAABBox(-1, aabbMin.P(), aabbMax.P(), LocalSpace);
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
                spriteComp->getFigure()->CalcAABBox(-1, aabbMin.P(), aabbMax.P());
                m_aabb = { aabbMin, aabbMax };
            }
        }
        else if (getComponent<TextComponent>() != nullptr && !isGUIObject())
        {
            auto comp = getComponent<TextComponent>();
            if (comp->getFigure())
            {
                Vec3 aabbMin, aabbMax;
                comp->getFigure()->CalcAABBox(-1, aabbMin.P(), aabbMax.P());
                m_aabb = { aabbMin, aabbMax };
            }
        }
        else if (getComponent<TextBitmapComponent>() != nullptr && !isGUIObject())
        {
            auto comp = getComponent<TextBitmapComponent>();
            if (comp->getFigure())
            {
                Vec3 aabbMin, aabbMax;
                comp->getFigure()->CalcAABBox(-1, aabbMin.P(), aabbMax.P());
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
                    uiText->getFigure()->CalcAABBox(-1, aabbMin.P(), aabbMax.P());
                    m_aabb = { aabbMin, aabbMax };
                }
            }
            else if (getComponent<UITextBitmap>() != nullptr)
            {
                auto uiText = getComponent<UITextBitmap>();
                if (uiText->getFigure())
                {
                    Vec3 aabbMin, aabbMax;
                    uiText->getFigure()->CalcAABBox(-1, aabbMin.P(), aabbMax.P());
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
    std::shared_ptr<SceneObject> SceneObject::findChildByName(const std::string & name)
    {
        auto found = std::find_if(m_children.begin(), m_children.end(), [&](auto elem)
        {
            return !elem.expired() && elem.lock()->getName().compare(name) == 0;
        });
        auto va = m_children.end();
        //pyxie_printf("")
        if (found != m_children.end() && !found->expired())
            return (*found).lock();
        return nullptr;
    }

    //! Get prefabId recursive
    std::string SceneObject::getPrefabId() {
        return m_prefabId;
    }

    //! Unpack prefab
    bool SceneObject::unpackPrefab() {
        if (isInPrefab()) {
            for (auto& child : m_children) {
                if (!child.expired()) {
                    child.lock()->unpackPrefab();
                }
            }
            if(isPrefab()) setPrefabId(std::string());
            return true;
        }
        return false;
    }

    std::string SceneObject::getPrefabRootId()
    {
        if (getParent() && !getParent()->getPrefabRootId().empty()
            && getParent()->getPrefabRootId().compare(getScene()->getPrefabId()) != 0) {
            return getParent()->getPrefabRootId();
        }
        if (!getPrefabId().empty()) return getPrefabId();
        return std::string();
    }

    std::shared_ptr<SceneObject> SceneObject::getPrefabRoot()
    {
        if (getParent() && getParent()->getPrefabRoot()
            && getParent()->getPrefabRoot()->getPrefabId().compare(getScene()->getPrefabId()) != 0) {
            return getParent()->getPrefabRoot();
        }
        if (!getPrefabId().empty())
            return getSharedPtr();
        return nullptr;
    }

    void SceneObject::setPrefabId(const std::string& id)
    {
        m_prefabId = id;
    }

    void SceneObject::setInMask(bool value) 
    {
        m_bIsInMask = value;
    }

    void SceneObject::reloadScripts(bool includeChild)
    {
        if (includeChild) {
            for (auto& child : m_children) {
                if(!child.expired())
                    child.lock()->reloadScripts(includeChild);
            }
        }

        int c_size = getComponentsCount();
        for (int i = 0; i < c_size; i++) {
            if (m_components[i]->getType() == Component::Type::Script) {
                std::shared_ptr<ScriptComponent> Script = std::dynamic_pointer_cast<ScriptComponent>(m_components[i]);
                if (Script != nullptr) {
                    Script->Reload();
                }
            }
        }
    }

    // Check if this object is a prefab
    bool SceneObject::isPrefab() const
    {
        if (getScene() == nullptr) return false;
        auto prefabIdScene = getScene()->getPrefabId();
        return (!m_prefabId.empty() && m_prefabId.compare(prefabIdScene) != 0);
    }

    // Check if this object belong to a prefab
    bool SceneObject::isInPrefab() const
    {
        if (isPrefab()) return true;
        if (getParent() == nullptr) return false;
        if (getParent()->isPrefab()) return true;
        return getParent()->isInPrefab();
    }

    //! Serialize
    void SceneObject::to_json(json &j)
    {
        j = json{
            {"uuid", m_uuid},
            {"name", m_name},
            {"active", m_isActive},
            {"selected", m_isSelected},
            {"gui", m_bIsGui},
            {"raycast", m_bIsRaycastTarget},
            {"interactable", m_bIsInteractable},
        };

        if (!m_prefabId.empty()) j["prefabId"] = m_prefabId;

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
            if (!child.expired())
            {
                json jChild;
                child.lock()->to_json(jChild);
                jChildren.push_back(jChild);
            }
        }
        j["childs"] = jChildren;
    }

    //! Deserialize
    void SceneObject::from_json(const json &j)
    {
        setName(j.value("name", ""));
        setActive(j.value("active", false));
        setPrefabId(j.value("prefabId", std::string()));
        if (!isInPrefab()) {
            setUUID(j.value("uuid", getUUID()));
            setSelected(j.value("selected", false));
        }
        m_bIsGui = j.value("gui", false);
        m_bIsRaycastTarget = j.value("raycast", false);
        m_bIsInteractable = j.value("interactable", false);

        auto jComps = j.at("comps");
        for (auto it : jComps)
        {
            auto key = it.at(0);
            auto val = it.at(1);
            auto comp = createComponent(key);
            if (comp)
                val.get_to(*comp);
        }

        // Add editor camera figure debug
        if (SceneManager::getInstance()->isEditor())
        {
            if (auto camera = getComponent<CameraComponent>())
            {
                if (!getComponent<FigureComponent>())
                    addComponent<FigureComponent>(GetEditorResource("figures/camera"))->setSkipSerialize(true);
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
        auto thisObj = getSharedPtr();
        for (auto it : jChildren)
        {
            auto child = getScene()->createObject(it.at("name"), thisObj, it.value("gui", false), {});
            child->from_json(it);
        }
    }

} // namespace ige::scene
