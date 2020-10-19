#include <algorithm>
#include <cstdlib> 
#include <ctime>

#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "scene/SceneManager.h"

#include "components/Component.h"
#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "components/EnvironmentComponent.h"
#include "components/ScriptComponent.h"
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"
#include "components/gui/UIImage.h"

#include "utils/GraphicsHelper.h"

#define SERIALIZE_VERSION "0.0.1"

namespace ige::scene
{
    Scene::Scene(const std::string& name)
        : m_name(name)
    {
        m_showcase = ResourceCreator::Instance().NewShowcase((m_name + "_showcase").c_str());
        getResourceAddedEvent().addListener(std::bind(&Scene::onResourceAdded, this, std::placeholders::_1));
        getResourceRemovedEvent().addListener(std::bind(&Scene::onResourceRemoved, this, std::placeholders::_1));
    }

    Scene::~Scene()
    {
        clear();
    }

    bool Scene::initialize()
    {
        return true;
    }

    void Scene::clear()
    {
        m_activeCamera = nullptr;

        for (auto& obj : m_objects)
        {
            obj->removeChildren();
            obj->setParent(nullptr);
        }

        // Destroy all objects
        for (auto& obj : m_objects)
        {
            obj = nullptr;
        }
        m_objects.clear();

        getResourceAddedEvent().removeAllListeners();
        getResourceRemovedEvent().removeAllListeners();

        if (m_showcase)
        {
            m_showcase->Clear();
            m_showcase->DecReference();
            m_showcase = nullptr;
        }

        ResourceManager::Instance().DeleteDaemon();
    }

    void Scene::update(float dt)
    {
        for (auto& obj : m_objects)
            if (obj) obj->onUpdate(dt);

        if (!SceneManager::getInstance()->isEditor() && m_activeCamera)
            m_activeCamera->onUpdate(dt);

        m_showcase->Update(dt);
    }

    void Scene::fixedUpdate(float dt)
    {
        for (auto& obj : m_objects)
            if (obj) obj->onFixedUpdate(dt);
    }

    void Scene::lateUpdate(float dt)
    {
        for (auto& obj : m_objects)
            if (obj) obj->onLateUpdate(dt);
    }

    void Scene::render()
    {
        for (auto& obj : m_objects)
            if (obj) obj->onRender();

        if (!SceneManager::getInstance()->isEditor() && m_activeCamera)
            m_activeCamera->onRender();

        m_showcase->Render();
    }

    std::shared_ptr<SceneObject> Scene::createObject(std::string name, std::shared_ptr<SceneObject> parent)
    {
        auto sceneObject = std::make_shared<SceneObject>(this, m_nextObjectID++, name, parent ? parent.get() : nullptr);
        m_objects.push_back(sceneObject);

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
            setActiveCamera(camComp.get());
            camObj->addComponent<FigureComponent>("figure/camera.pyxf");

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
        std::shared_ptr<SceneObject> sceneObject = nullptr;
        if (!parent || parent->getCanvas() == nullptr)
        {
            auto canvasObject = std::make_shared<SceneObject>(this, m_nextObjectID++, "Canvas", parent ? parent.get() : nullptr, true, size);
            m_objects.push_back(canvasObject);

            auto uiImage = canvasObject->addComponent<UIImage>();
            uiImage->setPath("sprite/rect");

            auto canvas = canvasObject->addComponent<Canvas>();
            canvas->setDesignCanvasSize(Vec2(540.f, 960.f));
            canvas->setTargetCanvasSize(Vec2(SystemInfo::Instance().GetGameW(), SystemInfo::Instance().GetGameW()));
            canvasObject->setCanvas(canvas);

            if (!parent)
            {
                // This is a GUI root object, setup default environment, canvas and camera
                auto envComp = canvasObject->addComponent<EnvironmentComponent>("environment");
                envComp->setAmbientSkyColor(Vec3(0.5f, 0.5f, 0.5f));
                envComp->setDirectionalLightColor(0, Vec3(0.5f, 0.5f, 0.5f));

                auto camObj = createGUIObject("GUI Camera", canvasObject);
                camObj->getTransform()->setPosition(Vec3(0.f, 0.f, 10.f));
                auto camComp = camObj->addComponent<CameraComponent>("default_2d_camera");
                camComp->lockOnTarget(false);
                camComp->setAspectRatio(SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH());
                camComp->setOrthoProjection(true);
                camComp->setWidthBase(false);
                camObj->onUpdate(0.f);
            }
        }
        else
        {
            sceneObject = std::make_shared<SceneObject>(this, m_nextObjectID++, name, parent ? parent.get() : nullptr, true, size);
            m_objects.push_back(sceneObject);
        }

        return sceneObject;
    }

    bool Scene::removeObject(const std::shared_ptr<SceneObject>& obj)
    {
        if (!obj) return false;

        auto children = obj->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            auto elem = children[i];
            auto itr = std::find_if(m_objects.begin(), m_objects.end(), [&](auto el)
            {
                return el && elem && (el->getId() == elem->getId());
            });
            if (itr != m_objects.end())
                m_objects.erase(itr);
        }

        auto itr = std::find(m_objects.begin(), m_objects.end(), obj);
        if (itr != m_objects.end())
        {
            m_objects.erase(itr);
        }
    }

    //! Remove scene object by its id
    bool Scene::removeObjectById(uint64_t id)
    {
        return removeObject(findObjectById(id));
    }

    std::shared_ptr<SceneObject> Scene::findObjectById(uint64_t id)
    {
        auto found = std::find_if(m_objects.begin(), m_objects.end(), [&](auto elem)
        {
            return (elem->getId() == id);
        });
        if (found != m_objects.end())
            return std::dynamic_pointer_cast<SceneObject>(*found);
        return nullptr;
    }

    std::shared_ptr<SceneObject> Scene::findObjectByName(std::string name)
    {
        auto found = std::find_if(m_objects.begin(), m_objects.end(), [&](auto elem)
        {
            return elem->getName() == name;
        });
        if (found != m_objects.end())
            return std::dynamic_pointer_cast<SceneObject>(*found);
        return nullptr;
    }


    //! Set active camera
    void Scene::setActiveCamera(CameraComponent* camera)
    {
        if (m_activeCamera != camera)
        {
            if (camera && camera->getOwner()->isActive())
            {
                m_activeCamera = camera;
            }
            else
            {
                m_activeCamera = nullptr;
            }
        }
    }

    //! Resource added event
    void Scene::onResourceAdded(Resource* resource)
    {
        if (resource)
            m_showcase->Add(resource);
    }

    //! Resource removed event
    void Scene::onResourceRemoved(Resource* resource)
    {
        if (resource)
            m_showcase->Remove(resource);
    }

    //! Serialize
    void Scene::to_json(json& j) const
    {
        j = json {
            {"name", m_name},
            {"objId", m_nextObjectID}
        };

        auto jRoots = json::array();
        for (const auto& obj : m_objects)
        {
            if (obj)
            {
                json jRoot;
                obj->to_json(jRoot);
                jRoots.push_back(jRoot);
            }
        }
        j["objects"] = jRoots;
    }

    //! Deserialize
    void Scene::from_json(const json& j)
    {
        j.at("name").get_to(m_name);
        auto jRoots = j.at("objects");
        for (auto it : jRoots)
        {
            auto root = std::make_shared<SceneObject>(this, it.at("id"), it.at("name"), nullptr, it.value("gui", false));
            root->from_json(it);
            m_objects.push_back(root);
        }

        for (auto& obj : m_objects)
        {
            if (obj)
            {
                auto pid = obj->getParentId();
                if (pid != (uint64_t)-1)
                {
                    auto parentObj = findObjectById(pid);
                    obj->setParent(parentObj.get());
                }
            }
        }

        j.at("objId").get_to(m_nextObjectID);
    }
}
