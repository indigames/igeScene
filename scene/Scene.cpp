#include <algorithm>
#include <cstdlib> 
#include <ctime>

#include "scene/Scene.h"
#include "scene/SceneObject.h"

#include "components/Component.h"
#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "components/EnvironmentComponent.h"
#include "components/ScriptComponent.h"
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"

#include "utils/GraphicsHelper.h"

#define SERIALIZE_VERSION "0.0.1"

namespace ige::scene
{
    Scene::Scene(const std::string& name)
        : m_name(name)
    {
        m_componentAddedEventId = SceneObject::getComponentAddedEvent().addListener(std::bind(&Scene::onComponentAdded, this, std::placeholders::_1, std::placeholders::_2));
        m_componentRemovedEventId = SceneObject::getComponentRemovedEvent().addListener(std::bind(&Scene::onComponentRemoved, this, std::placeholders::_1, std::placeholders::_2));
        m_objectSelectedEventId = SceneObject::getSelectedEvent().addListener(std::bind(&Scene::onSceneObjectSelected, this, std::placeholders::_1));
    }

    Scene::~Scene()
    {
        SceneObject::getComponentAddedEvent().removeListener(m_componentAddedEventId);
        SceneObject::getComponentRemovedEvent().removeListener(m_componentRemovedEventId);
        SceneObject::getSelectedEvent().removeListener(m_objectSelectedEventId);
        getOnActiveCameraChangedEvent().removeAllListeners();

        clear();
    }

    bool Scene::initialize()
    {
        return true;
    }

    void Scene::clear()
    {
        m_activeCamera = nullptr;
        for (auto& cam : m_cameras)
            cam = nullptr;
        m_cameras.clear();

        for (auto& root : m_roots)
            root = nullptr;
        m_roots.clear();
    }

    void Scene::update(float dt)
    {
        for (auto& root : m_roots)
            if (root) root->onUpdate(dt);

        //if (m_activeCamera)
        //{
        //    m_activeCamera->onUpdate(dt);
        //    auto target = m_activeCamera->getShootTarget();
        //    if (target && target->getShowcase())
        //    {
        //        target->getShowcase()->Update(dt);
        //    }
        //}

        for (auto& camera : m_cameras) 
        {
            if (camera && camera->getOwner()->isActive())
            {
                auto target = camera->getShootTarget();
                if (target && target->isActive() && target->getShowcase())
                {
                    camera->onUpdate(dt);
                    target->getShowcase()->Update(dt);
                }
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
        //if (m_activeCamera)
        //{
        //    m_activeCamera->onRender();
        //    auto target = m_activeCamera->getShootTarget();
        //    if (target && target->getShowcase())
        //    {
        //        target->getShowcase()->Render();
        //    }
        //}

        for (auto& camera : m_cameras)
        {
            if (camera && camera->getOwner()->isActive())
            {
                auto target = camera->getShootTarget();
                if (target && target->isActive() && target->getShowcase())
                {
                    camera->onRender();
                    target->getShowcase()->Render();
                }
            }
        }
    }

    std::shared_ptr<SceneObject> Scene::createObject(std::string name, std::shared_ptr<SceneObject> parent)
    {
        auto sceneObject = std::make_shared<SceneObject>(m_nextObjectID++, name, parent ? parent.get() : nullptr);
        auto transform = sceneObject->addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f));
        sceneObject->setTransform(transform);
        if (parent != nullptr) parent->addChild(sceneObject);

        if (parent == nullptr)
        {
            // This is a root object, setup default environment and camera
            auto envComp = sceneObject->addComponent<EnvironmentComponent>("environment");
            envComp->setAmbientSkyColor(Vec3(0.5f, 0.5f, 0.5f));
            envComp->setDirectionalLightColor(0, Vec3(0.5f, 0.5f, 0.5f));

            auto camObj = createObject("Default Camera", sceneObject);
            camObj->getTransform()->setPosition(Vec3(0.f, 0.f, 10.f));
            auto camComp = camObj->addComponent<CameraComponent>("default_camera");
            camComp->lockOnTarget(false);
            camComp->setAspectRatio(SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH());
            camComp->setShootTarget(sceneObject.get());
            setActiveCamera(camComp.get());
            m_roots.push_back(sceneObject);

        /*
            //! [IGE]: To test perfornamce with huge amount of objects
            std::srand(std::time(nullptr));

            for (int i = 0; i < 10000; ++i)
            {
                auto newObj = createObject("Object_" + std::to_string(i), sceneObject);
                if((std::rand() / (float)RAND_MAX) < 0.5f)
                    if ((std::rand() / (float)RAND_MAX) < 0.5f)
                        newObj->getTransform()->setPosition(Vec3(-std::rand() % 50, 0.f, -std::rand() % 50));
                    else
                        newObj->getTransform()->setPosition(Vec3(-std::rand() % 50, 0.f, std::rand() % 50));
                else
                    if ((std::rand() / (float)RAND_MAX) < 0.5f)
                        newObj->getTransform()->setPosition(Vec3(std::rand() % 50, 0.f, -std::rand() % 50));
                    else
                        newObj->getTransform()->setPosition(Vec3(std::rand() % 50, 0.f, std::rand() % 50));
                newObj->addComponent<FigureComponent>("figure/Tree/Tree.pyxf");
            }
        */
        }
        return sceneObject;
    }

    std::shared_ptr<SceneObject> Scene::createGUIObject(std::string name, std::shared_ptr<SceneObject> parent, const Vec3& pos, const Vec2& size)
    {
        auto sceneObject = std::make_shared<SceneObject>(m_nextObjectID++, name, parent ? parent.get() : nullptr, true);
        auto transform = sceneObject->addComponent<RectTransform>(pos, size);
        sceneObject->setTransform(transform);
        if (parent != nullptr) parent->addChild(sceneObject);

        if (parent == nullptr)
        {
            // Canvas, pivot at top-left
            transform->setPivot(Vec2(0.f, 0.f));

            // This is a GUI root object, setup default environment, canvas and camera
            auto envComp = sceneObject->addComponent<EnvironmentComponent>("environment");
            envComp->setAmbientSkyColor(Vec3(0.5f, 0.5f, 0.5f));
            envComp->setDirectionalLightColor(0, Vec3(0.5f, 0.5f, 0.5f));

            auto canvas = sceneObject->addComponent<Canvas>();
            canvas->setTargetCanvasSize(Vec2(SystemInfo::Instance().GetGameW(), SystemInfo::Instance().GetGameW()));

            auto camObj = createGUIObject("GUI Camera", sceneObject);
            camObj->getTransform()->setPosition(Vec3(0.f, 0.f, 10.f));
            auto camComp = camObj->addComponent<CameraComponent>("default_2d_camera");
            camComp->lockOnTarget(false);
            camComp->setAspectRatio(SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH());
            camComp->setOrthoProjection(true);
            camComp->setWidthBase(false);
            camComp->setShootTarget(sceneObject.get());
            m_roots.push_back(sceneObject);
        }
        return sceneObject;
    }

    bool Scene::removeObject(const std::shared_ptr<SceneObject>& obj)
    {
        if(!obj) return false;

        auto found = std::find(m_roots.begin(), m_roots.end(), obj);
        if (found != m_roots.end())
        {
            if (m_activeCamera && m_activeCamera->getOwner()->getRoot() == (*found).get())
                setActiveCamera(nullptr);
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

    std::shared_ptr<SceneObject> Scene::findObjectById(uint64_t id)
    {
        for (auto root : m_roots)
        {
            if (root)
            {
                if (root->getId() == id)
                    return root;
                auto obj = root->findObjectById(id);
                if (obj) return obj;
            }
        }
        return nullptr;
    }

    std::shared_ptr<SceneObject> Scene::findObjectByName(std::string name)
    {
        for (auto root : m_roots)
        {
            if (root)
            {
                if (root->getName() == name)
                    return root;
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
            m_cameras.push_back(std::dynamic_pointer_cast<CameraComponent>(component));
        }
    }

    //! Component removed event
    void Scene::onComponentRemoved(SceneObject& obj, const std::shared_ptr<Component>& component)
    {
        if (component->getName() == "CameraComponent")
        {
            auto found = std::find_if(m_cameras.begin(), m_cameras.end(), [&](const auto& cam) {
                auto cameraComponent = std::dynamic_pointer_cast<CameraComponent>(component);
                return strcmp(cam->getCamera()->ResourceName(), cameraComponent->getCamera()->ResourceName()) == 0;
            });

            if (found != m_cameras.end())
            {
                auto cameraComponent = *found;
                if(cameraComponent.get() == getActiveCamera()) 
                    setActiveCamera(nullptr);
                m_cameras.erase(found);
            }
        }
    }

    void Scene::onSceneObjectSelected(SceneObject& sceneObject)
    {
        for (const auto& cam : m_cameras)
        {
            if (cam && cam->getShootTarget() == sceneObject.getRoot())
            {
                setActiveCamera(cam.get());
                break;
            }
        }
    }

    //! Set active camera
    void Scene::setActiveCamera(CameraComponent* camera)
    {
        if (m_activeCamera != camera)
        {
            if (camera && camera->getOwner()->isActive())
            {
                // Invoke callback
                getOnActiveCameraChangedEvent().invoke(camera);
                m_activeCamera = camera;
            }
            else
            {
                getOnActiveCameraChangedEvent().invoke(nullptr);
                m_activeCamera = nullptr;
            }
        }
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
            auto root = std::make_shared<SceneObject>(it.at("id"), it.at("name"), nullptr, it.value("gui", false));
            root->from_json(it);
            m_roots.push_back(root);
        }
        j.at("objId").get_to(m_nextObjectID);

        for (auto& cam : m_cameras)
        {
            auto target = findObjectById(cam->getTargetId());
            if (target)
                cam->setShootTarget(target.get());
        }
    }
}
