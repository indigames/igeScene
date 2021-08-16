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

    void SceneManager::lateUpdate(float dt)
    {
         if(m_currScene) m_currScene->lateUpdate(dt);
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
        if (m_projectPath.compare(path) != 0)
        {
            m_projectPath = path;
            deinit();
            init();
        }
    }

    void SceneManager::setEditorPath(const std::string& path)
    {
        if (m_editorPath.compare(path) != 0)
        {
            m_editorPath = path;
        }
    }

    std::shared_ptr<Scene> SceneManager::createScene(const std::string& name, bool empty)
    {
        auto scene = std::make_shared<Scene>(name);
        scene->initialize(empty);
        m_scenes.push_back(scene);
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
        }
    }
}
