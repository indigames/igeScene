#include <algorithm>

#include "scene/Scene.h"
#include "scene/SceneObject.h"

#include "components/Component.h"
#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "components/EnvironmentComponent.h"
#include "components/ScriptComponent.h"
#include "components/RectTransform.h"

#include "utils/GraphicsHelper.h"

#define SERIALIZE_VERSION "0.0.1"

namespace ige::scene
{
    Scene::Scene(const std::string& name)
        : m_name(name)
    {
        SceneObject::getComponentAddedEvent().addListener(std::bind(&Scene::onComponentAdded, this, std::placeholders::_1, std::placeholders::_2));
        SceneObject::getComponentRemovedEvent().addListener(std::bind(&Scene::onComponentRemoved, this, std::placeholders::_1, std::placeholders::_2));
    }

    Scene::~Scene()
    {
        clear();
    }
    
    bool Scene::initialize()
    {
        auto root = createObject(getName());
        auto envComp = root->addComponent<EnvironmentComponent>("environment");
        envComp->setAmbientGroundColor(Vec3(0.5f, 0.5f, 0.5f));
        envComp->setDirectionalLightColor(0, Vec3(0.5f, 0.5f, 0.5f));
        m_roots.push_back(root);

        auto camObj = createObject("Default Camera", root);
        camObj->getTransform()->setPosition(Vec3(0.f, 5.f, 20.f));
        auto camComp = camObj->addComponent<CameraComponent>("default_camera");
        camComp->lockOnTarget(false);
        camComp->setAspectRatio(SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH());
        camComp->setShootTarget(root);
        setActiveCamera(camComp);
        return true;
    }

    void Scene::clear()
    {
        for (auto& root : m_roots)
            root = nullptr;
        m_roots.clear();

        SceneObject::getComponentAddedEvent().removeAllListeners();
        SceneObject::getComponentRemovedEvent().removeAllListeners();
    }

    void Scene::update(float dt)
    {
        for (auto& root : m_roots)
            if (root) root->onUpdate(dt);

        if (m_activeCamera)
        {
            m_activeCamera->onUpdate(dt);
            auto target = m_activeCamera->getShootTarget();
            if (target && target->getShowcase())
            {
                target->getShowcase()->Update(dt);
            }
        }
    }

    void Scene::fixedUpdate(float dt)
    {
        for (auto& root : m_roots)
            if (root) root->onFixedUpdate(dt);
    }

    void Scene::lateUpdate(float dt)
    {
        for (auto& root : m_roots)
            if (root) root->onLateUpdate(dt);
    }

    void Scene::render()
    {
        if (m_activeCamera)
        {
            m_activeCamera->onRender();
            auto target = m_activeCamera->getShootTarget();
            if (target && target->getShowcase())
            {
                target->getShowcase()->Render();
            }
        }
        for (auto& root : m_roots)
            if (root) root->onRender();
    }

    std::shared_ptr<SceneObject> Scene::createObject(std::string name, std::shared_ptr<SceneObject> parent)
    {
        auto sceneObject = std::make_shared<SceneObject>(m_nextObjectID++, name, parent.get());
        if(parent != nullptr) parent->addChild(sceneObject);
        auto transform = sceneObject->addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f));
        sceneObject->setTransform(transform);
        return sceneObject;
    }

    std::shared_ptr<SceneObject> Scene::createGUIObject(std::string name, std::shared_ptr<SceneObject> parent)
    {
        auto sceneObject = std::make_shared<SceneObject>(m_nextObjectID++, name, parent.get(), true);
        if (parent != nullptr) parent->addChild(sceneObject);
        auto transform = sceneObject->addComponent<RectTransform>(Vec3(0.f, 0.f, 0.f));
        sceneObject->setTransform(transform);
        return sceneObject;
    }

    bool Scene::removeObject(const std::shared_ptr<SceneObject>& obj)
    {
        if(!obj) return false;

        auto found = std::find(m_roots.begin(), m_roots.end(), obj);
        if (found != m_roots.end())
        {
            m_roots.erase(found);
            return true;
        }
        for (auto& root : m_roots)
            if (root && root->removeChild(obj))
                return true;
        return false;
    }

    //! Remove scene object by its id
    bool Scene::removeObjectById(uint64_t id)
    {
        return removeObject(findObjectById(id));
    }
    
    std::shared_ptr<SceneObject> Scene::findObjectById(uint64_t id) const
    {
        for (auto& root : m_roots)
        {
            if (root)
            {
                auto obj = root->findObjectById(id);
                if (obj) return obj;
            }
        }
        return nullptr;
    }

    std::shared_ptr<SceneObject> Scene::findObjectByName(std::string name) const
    {
        for (auto& root : m_roots)
        {
            if (root)
            {
                auto obj = root->findObjectByName(name);
                if (obj) return obj;
            }
        }
        return nullptr;
    }

    //! Component added event
    void Scene::onComponentAdded(SceneObject& obj, const std::shared_ptr<Component>& component)
    { 
        if (component->getName() == "CameraComponent")
        {
            m_cameras.push_back(std::static_pointer_cast<CameraComponent>(component));
        }
    }

    //! Component removed event
    void Scene::onComponentRemoved(SceneObject& obj, const std::shared_ptr<Component>& component)
    {
        if (m_activeCamera == component) m_activeCamera = nullptr;

        if (component->getName() == "CameraComponent")
        {
            auto found = std::find_if(m_cameras.begin(), m_cameras.end(), [&](const auto& cam) {
                auto cameraComponent = std::static_pointer_cast<CameraComponent>(component);
                return strcmp(cam->getCamera()->ResourceName(), cameraComponent->getCamera()->ResourceName()) == 0;
            });

            if (found != m_cameras.end())
            {
                m_cameras.erase(found);
            }
        }
        if (m_cameras.size() > 0) m_activeCamera = m_cameras[0];
    }

    //! Set active camera
    void Scene::setActiveCamera(const std::string& cameraName)
    {
        for (const auto& cam : m_cameras)
        {
            if (strcmp(cam->getCamera()->ResourceName(), cameraName.c_str()) == 0)
            {
                m_activeCamera = cam;
                return;
            }
        }
        // m_activeCamera = nullptr;
    }

    //! Serialize
    void Scene::to_json(json& j) const
    {
        j = json {
            {"name", m_name},
            {"objId", m_nextObjectID}
        };

        auto jRoots = json::array();
        for (const auto& root : m_roots)
        {
            if (root)
            {
                json jRoot;
                root->to_json(jRoot);
                jRoots.push_back(jRoot);
            }
        }       
        j["roots"] = jRoots;
    }

    //! Deserialize 
    void Scene::from_json(const json& j)
    {
        j.at("name").get_to(m_name);
        auto jRoots = j.at("roots");
        for (auto it : jRoots)
        {
            auto root = std::make_shared<SceneObject>(it.at("id"), it.at("name"));
            root->from_json(it);
        }
        j.at("objId").get_to(m_nextObjectID);
    }
}
