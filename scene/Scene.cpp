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

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    Scene::Scene(const std::string& name)
        : m_name(name)
    {
        m_environment = ResourceCreator::Instance().NewEnvironmentSet(name.c_str(), nullptr);
        m_environment->WaitBuild();

        m_showcase = ResourceCreator::Instance().NewShowcase((m_name + "_showcase").c_str());
        m_showcase->Add(m_environment);

        getResourceAddedEvent().addListener(std::bind(&Scene::onResourceAdded, this, std::placeholders::_1));
        getResourceRemovedEvent().addListener(std::bind(&Scene::onResourceRemoved, this, std::placeholders::_1));
    }

    Scene::~Scene()
    {
        clear();

        getResourceAddedEvent().removeAllListeners();
        getResourceRemovedEvent().removeAllListeners();

        if (m_showcase)
        {
            m_showcase->Remove(m_environment);
            m_showcase->Clear();
            m_showcase->DecReference();
            m_showcase = nullptr;

            m_environment->DecReference();
            m_environment = nullptr;
        }

        ResourceManager::Instance().DeleteDaemon();
    }

    bool Scene::initialize()
    {
        return true;
    }

    void Scene::clear()
    {
        m_nextObjectID = 0;
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
        m_roots.clear();
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

    std::shared_ptr<SceneObject> Scene::createObject(std::string name, std::shared_ptr<SceneObject> parent, bool isGUI, const Vec2& size)
    {
        auto sceneObject = std::make_shared<SceneObject>(this, m_nextObjectID++, name, parent ? parent.get() : nullptr, isGUI, size);
        m_objects.push_back(sceneObject);

        // Not canvas, create defaut canvas
        if (isGUI && (!parent || !parent->getCanvas()))
        {
            auto canvas = sceneObject->addComponent<Canvas>();
            canvas->setDesignCanvasSize(Vec2(540.f, 960.f));
            canvas->setTargetCanvasSize(Vec2(SystemInfo::Instance().GetGameW(), SystemInfo::Instance().GetGameW()));
            sceneObject->setCanvas(canvas);
        }

        // Setup default environment and camera for root
        if (!parent)
        {
            // Cache the root object
            m_roots.push_back(sceneObject);

            auto envComp = sceneObject->addComponent<EnvironmentComponent>("environment");
            envComp->setAmbientSkyColor(Vec3(0.5f, 0.5f, 0.5f));
            envComp->setDirectionalLightColor(0, Vec3(0.5f, 0.5f, 0.5f));

            if (isGUI)
            {
                auto camObj = createObject("GUI Camera", sceneObject, true);
                camObj->getTransform()->setPosition(Vec3(0.f, 0.f, 10.f));
                auto camComp = camObj->addComponent<CameraComponent>("default_2d_camera");
                camComp->lockOnTarget(false);
                camComp->setAspectRatio(SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH());
                camComp->setOrthoProjection(true);
                camComp->setWidthBase(false);
            }
            else
            {
                auto camObj = createObject("Default Camera", sceneObject);
                camObj->getTransform()->setPosition(Vec3(0.f, 0.f, 10.f));
                auto camComp = camObj->addComponent<CameraComponent>("default_camera");
                camComp->lockOnTarget(false);
                camComp->setAspectRatio(SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH());

                // Add editor camera figure debug
                if (SceneManager::getInstance()->isEditor())
                {
                    camObj->addComponent<FigureComponent>("figure/camera.pyxf")->setSkipSerialize(true);
                }

                // Fill up test data
                // populateTestData(sceneObject, 1000);
            }
        }
        return sceneObject;
    }

    void Scene::populateTestData(const std::shared_ptr<SceneObject>& parent, int numObjects)
    {
        //! [IGE]: To test perfornamce with huge amount of objects
        std::srand(std::time(nullptr));

        for (int i = 0; i < numObjects; ++i)
        {
            auto newObj = createObject("Object_" + std::to_string(i), parent);
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
    }

    bool Scene::removeObject(const std::shared_ptr<SceneObject>& obj)
    {
        if (!obj) return false;

        // Remove active camera
        if (auto camera = obj->getComponent<CameraComponent>())
        {
            if (getActiveCamera() == camera.get())
                setActiveCamera(nullptr);
        }

        // Remove from roots list
        if (obj->getParent() == nullptr)
        {
            auto itr = std::find(m_roots.begin(), m_roots.end(), obj);
            if (itr != m_roots.end())
            {
                m_roots.erase(itr);
            }
        }

        // Remove all children
        auto children = obj->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            auto elem = children[i];
            auto itr = std::find_if(m_objects.begin(), m_objects.end(), [&](auto el)
            {
                return el && elem && (el->getId() == elem->getId());
            });
            if (itr != m_objects.end())
                removeObject(*itr);
        }

        // Remove from objects list
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
            {"objId", m_nextObjectID},
        };
        auto jRoots = json::array();
        for (const auto& obj : m_roots)
        {
            if (obj)
            {
                json jRoot;
                obj->to_json(jRoot);
                jRoots.push_back(jRoot);
            }
        }
        j["roots"] = jRoots;
    }

    //! Deserialize
    void Scene::from_json(const json& j)
    {
        clear();

        j.at("name").get_to(m_name);

        auto jRoots = j.at("roots");
        for (auto it : jRoots)
        {
            auto root = std::make_shared<SceneObject>(this, m_nextObjectID++, it.at("name"), nullptr, it.value("gui", false));
            m_objects.push_back(root);
            m_roots.push_back(root);

            root->from_json(it);
        }
        j.at("objId").get_to(m_nextObjectID);
    }

    //! Prefab save/load
    bool Scene::savePrefab(uint64_t objectId, const std::string& path)
    {
        auto object = findObjectById(objectId);
        if (object != nullptr)
        {
            json jObj;
            object->to_json(jObj);

            auto fsPath = path.empty() ? fs::path(object->getName()) : fs::path(path + "/" + object->getName());
            auto ext = fsPath.extension();
            if (ext.string() != ".prefab")
            {
                fsPath = fsPath.replace_extension(".prefab");
            }
            std::ofstream file(fsPath.string());
            file << jObj;
            return true;
        }
        return true;
    }

    bool Scene::loadPrefab(uint64_t parentId, const std::string& path)
    {
        if (path.empty())
            return false;

        auto fsPath = fs::path(path);
        if (fsPath.extension().string() != ".prefab")
            return false;

        std::ifstream file(fsPath);
        if (!file.is_open())
            return nullptr;

        json jObj;
        file >> jObj;

        auto parent = findObjectById(parentId);
        auto obj = createObject(jObj.at("name"), parent, jObj.value("gui", false));
        obj->from_json(jObj);

        return true;
    }
}
