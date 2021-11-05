#include <algorithm>
#include <fstream>

#include "scene/SceneManager.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "components/Component.h"
#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "components/EnvironmentComponent.h"
#include "components/FigureComponent.h"
#include "components/light/AmbientLight.h"
#include "components/light/DirectionalLight.h"
#include "utils/ShapeDrawer.h"

#include <Python.h>

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

#include "utils/PyxieHeaders.h"
#include <iomanip>
using namespace pyxie;

#ifdef _WIN32
#  define DELIMITER ";"
#else
#  define DELIMITER ":"
#endif

namespace ige::scene
{
    SceneManager::SceneManager()
    {
        m_currScene = nullptr;

        // Initialize shape drawer
        ShapeDrawer::initialize();
    }

    SceneManager::~SceneManager()
    {
        deinit();
    }

    void SceneManager::init()
    {
        // Initialize python runtime
        auto root = m_projectPath;
        std::replace(root.begin(), root.end(), '\\', '/');
        std::string path = root;
        wchar_t pathw[1024];
        mbstowcs(pathw, path.c_str(), 1024);
        Py_SetPythonHome(pathw);

        FileIO::Instance().SetRoot(path.c_str());
        path.append(DELIMITER);
        path.append(root);

#if EDITOR_MODE
        root = m_editorPath;
        std::replace(root.begin(), root.end(), '\\', '/');
#endif

        path.append(DELIMITER);
        path.append(root);
        path.append("/PyLib");

        path.append(DELIMITER);
        path.append(root);
        path.append("/PyLib/site-packages");

        mbstowcs(pathw, path.c_str(), 1024);
        Py_SetPath(pathw);

        Py_Initialize();

        m_prefabPaths.clear();
        for (const auto& entry : fs::recursive_directory_iterator(m_projectPath))
        {
            if (entry.is_regular_file() && entry.path().extension().string().compare(".prefab") == 0)
            {
                json jFile;
                std::ifstream file(entry.path().string());
                if (file.is_open())
                {
                    file >> jFile;
                    file.close();
                    auto prefabId = jFile.value("prefabId", std::string());
                    if (!prefabId.empty())
                    {
                        m_prefabPaths[prefabId] = entry.path().string();
                    }
                }
            }
        }

    #ifndef EDITOR_MODE
        setIsPlaying(true);
    #endif
    }

    void SceneManager::deinit()
    {
        m_currScene = nullptr;

        for (auto& scene : m_scenes)
            scene = nullptr;
        m_scenes.clear();

        // Destroy python runtime
        Py_Finalize();
    }

    void SceneManager::update(float dt)
    {
        if (m_currScene) {
            m_currScene->update(dt);
        }
    }

    void SceneManager::fixedUpdate(float dt)
    {
        if(m_currScene) m_currScene->fixedUpdate(dt);
    }

    void SceneManager::physicUpdate(float dt)
    {
        if (m_currScene) m_currScene->physicUpdate(dt);
    }

    void SceneManager::lateUpdate(float dt)
    {
        if (m_currScene) m_currScene->lateUpdate(dt);
    }

    void SceneManager::preRender(Camera* camera)
    {
        if (m_currScene)
            m_currScene->preRender(camera);
    }

    void SceneManager::render()
    {
        if (m_currScene)
        {
            ShapeDrawer::setViewProjectionMatrix(RenderContext::InstancePtr()->GetRenderViewProjectionMatrix());
            m_currScene->render();
            m_currScene->renderUI();
            ShapeDrawer::flush();
        }
    }

    void SceneManager::setProjectPath(const std::string& path)
    {
        auto _path = path;
        if (path[_path.size() - 1] == '/' || path[_path.size() - 1] == '\\') {
            _path.erase(_path.end() - 1);
        }
        if (m_projectPath.compare(_path) != 0) {
            m_projectPath = _path;
            deinit();
            init();
        }
    }

    void SceneManager::setEditorPath(const std::string& path)
    {
        auto _path = path;
        if (_path[_path.size() - 1] == '/' || path[_path.size() - 1] == '\\') {
            _path.erase(_path.end() - 1);
        }
        if (m_editorPath.compare(_path) != 0) {
            m_editorPath = _path;
        }
    }

    //! Get/Set prefab path by id
    std::string SceneManager::getPrefabPath(const std::string& prefabId)
    {
        if (m_prefabPaths.count(prefabId) > 0)
            return m_prefabPaths[prefabId];
        return std::string();
    }

    void SceneManager::setPrefabPath(const std::string& id, const std::string& path)
    {
        if (!id.empty())
            m_prefabPaths[id] = path;
    }

    std::shared_ptr<Scene> SceneManager::createScene(const std::string& name, bool empty)
    {
        auto scene = std::make_shared<Scene>(name);
        scene->initialize(empty);
        m_scenes.push_back(scene);
        return scene;
    }
    //! Find prefab scene from prefabId
    std::shared_ptr<Scene> SceneManager::findPrefabSceneById(const std::string& prefabId)
    {
        auto found = std::find_if(m_scenes.begin(), m_scenes.end(), [&](const auto& scene) {
            return scene && scene->getPrefabId().compare(prefabId) == 0;
        });
        return (found != m_scenes.end()) ? *found : nullptr;
    }
    
    std::shared_ptr<Scene> SceneManager::createSceneFromPrefab(const std::string& path)
    {
        auto fsPath = fs::path(path);
        if (fsPath.extension().string() != ".prefab")
            return nullptr;

        std::ifstream file(fsPath);
        if (!file.is_open()) return nullptr;
        json jObj;
        file >> jObj;
        file.close();

        auto prefabId = jObj.value("prefabId", std::string());
        auto scene = findPrefabSceneById(prefabId);
        if (scene != nullptr) return scene;

        scene = createScene(jObj.value("name", "Prefab"), true);        
        auto obj = scene->createObject(jObj.at("name"), nullptr, jObj.value("gui", false), {64.f, 64.f}, prefabId);
        obj->from_json(jObj);

        // Just to display
        obj->addComponent<EnvironmentComponent>()->setSkipSerialize(true);
        auto ambientLight = obj->addComponent<AmbientLight>();
        ambientLight->setSkyColor({0.8f, 0.8f, 0.8f});
        ambientLight->setSkipSerialize(true);
        return scene;
    }

    void SceneManager::setCurrentScene(const std::shared_ptr<Scene>& scene)
    {
        if (m_currScene == scene) return;

        auto it = std::find(m_scenes.begin(), m_scenes.end(), scene);
        if (it != m_scenes.end())
        {
            m_currScene = scene;
        }
    }

    void SceneManager::setCurrentScene(const std::string& name)
    {
        auto found = std::find_if(m_scenes.begin(), m_scenes.end(), [&](const auto& itr)
        {
            return itr->getName() == name;
        });

        if (found != m_scenes.end())
        {
            m_currScene = *found;
        }
    }

    bool SceneManager::loadScene(std::shared_ptr<Scene> scene, const std::string& path)
    {
        json jScene;

        auto fsPath = fs::path(path);
        auto ext = fsPath.extension();

        std::ifstream file(fsPath);
        if (!file.is_open())
            return false;

        file >> jScene;
        file.close();

        if (scene == nullptr)
        {
            scene = std::make_shared<Scene>(jScene.at("name"));
            scene->initialize(true);
        }
        else
        {
            scene->removeAllObjects();
        }

        scene->from_json(jScene);

        if (ext.string() != ".tmp")
        {
            auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
            std::replace(relPath.begin(), relPath.end(), '\\', '/');
            scene->setPath(relPath);
        }

        m_scenes.push_back(scene);

        return true;
    }

    bool SceneManager::saveScene(const std::string& path)
    {
        if (m_currScene)
        {
            auto fsPath = path.empty() ? fs::path(m_currScene->getPath()) : fs::path(path);
            auto ext = fsPath.extension();

            if (ext.string() != ".scene" && ext.string() != ".tmp")
            {
                fsPath = fsPath.replace_extension(".scene");
            }

            if (path.find(".tmp") == std::string::npos)
            {
                if (m_currScene->getName() == "Untitled")
                {
                    m_currScene->setName(fsPath.filename().stem());
                }

                auto relPath = fsPath.is_absolute() ? fs::relative(fsPath, fs::current_path()).string() : fsPath.string();
                std::replace(relPath.begin(), relPath.end(), '\\', '/');
                m_currScene->setPath(relPath);
            }

            json jScene;
            m_currScene->to_json(jScene);
            fs::create_directories(fsPath.parent_path());
            std::ofstream file(fsPath.string());
            file << std::setw(2) << jScene << std::endl;
            file.close();
            return true;
        }
        return false;
    }

    std::shared_ptr<Scene> SceneManager::getScene(const std::string& uuid) 
    {
        auto found = std::find_if(m_scenes.begin(), m_scenes.end(), [&](auto elem) {
            return elem && elem->getUUID() == uuid;
        });
        if (found != m_scenes.end())
            return std::dynamic_pointer_cast<Scene>(*found);
        return nullptr;
    }

    void SceneManager::unloadScene(std::shared_ptr<Scene>& scene)
    {
        if (m_currScene == scene)
            m_currScene = nullptr;

        auto it = std::find(m_scenes.begin(), m_scenes.end(), scene);
        if (it != m_scenes.end())
        {
            m_scenes.erase(it);
            *it = nullptr;
        }
        scene = nullptr;
    }

    void SceneManager::unloadScene(const std::string& name)
    {
        if (m_currScene && m_currScene->getName() == name)
            m_currScene = nullptr;

        auto found = std::find_if(m_scenes.begin(), m_scenes.end(), [&](const auto& itr)
        {
            return itr->getName() == name;
        });

        if (found != m_scenes.end())
        {
            m_scenes.erase(found);
            *found = nullptr;
        }
    }

    //! Reload scene
    void SceneManager::reloadScene()
    {
        loadScene(m_currScene, m_currScene->getPath());
    }

    std::string GetEditorResource(const std::string& path)
    {
        if (SceneManager::getInstance()->getEditorPath().compare(SceneManager::getInstance()->getProjectPath()) == 0)
            return path;
        auto retPath = (fs::path(SceneManager::getInstance()->getEditorPath()).append(path)).string();
        std::replace(retPath.begin(), retPath.end(), '\\', '/');
        return retPath;
    }

    void SceneManager::dispathEvent(int eventType)
    {
        auto scene = getCurrentScene();
        if (scene) {
            auto node = scene->getRoot();
            if (node) node->dispatchEventIncludeChild(eventType);
            
            node = scene->getRootUI();
            if (node) node->dispatchEventIncludeChild(eventType);
        }
    }
}
