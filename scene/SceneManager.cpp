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

#include <Python.h>

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

#include "utils/PyxieHeaders.h"
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
    }

    SceneManager::~SceneManager()
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
        if (!m_bInitialized)
        {
            // Initialize python runtime
            std::string root = pyxieFios::Instance().GetRoot();
            std::string path = root;

            wchar_t pathw[1024];
            mbstowcs(pathw, path.c_str(), 1024);
            Py_SetPythonHome(pathw);

            path.append(DELIMITER);
            path.append("scripts");

            path.append(DELIMITER);
            path.append(root);
            path.append("PyLib");

            path.append(DELIMITER);
            path.append(root);
            path.append("PyLib/site-packages");
            mbstowcs(pathw, path.c_str(), 1024);
            Py_SetPath(pathw);

            Py_Initialize();

            m_bInitialized = true;
        }
        if(m_currScene) m_currScene->update(dt);
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
        if (m_currScene) m_currScene->render();
    }

    std::shared_ptr<Scene> SceneManager::createScene(const std::string& name)
    {
        auto scene = std::make_shared<Scene>(name);
        scene->initialize();
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

    std::shared_ptr<Scene> SceneManager::loadScene(const std::string& path)
    {
        json jScene;

        auto fsPath = fs::path(path);
        auto ext = fsPath.extension();
        if (ext.string() != ".scene")
        {
            fsPath = fsPath.replace_extension(".scene");
        }

        std::ifstream file(fsPath);
        if (!file.is_open())
            return nullptr;

        file >> jScene;

        auto scene = std::make_shared<Scene>(jScene.at("name"));
        scene->from_json(jScene);
        scene->setPath(path);
        m_scenes.push_back(scene);
        return scene;
    }

    bool SceneManager::saveScene(const std::string& path)
    {
        if (m_currScene)
        {
            json jScene;
            m_currScene->to_json(jScene);

            auto fsPath = path.empty() ? fs::path(m_currScene->getPath()) : fs::path(path);
            auto ext = fsPath.extension();
            if (ext.string() != ".scene")
            {
                fsPath = fsPath.replace_extension(".scene");
            }

            std::ofstream file(fsPath.string());
            file << jScene;
            return true;
        }
        return false;
    }

    void SceneManager::unloadScene(const std::shared_ptr<Scene>& scene)
    {
        if (m_currScene == scene)
            m_currScene = nullptr;

        auto it = std::find(m_scenes.begin(), m_scenes.end(), scene);
        if (it != m_scenes.end())
        {
            m_scenes.erase(it);
        }
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
        }
    }

    //! Reload scene
    void SceneManager::reloadScene()
    {
        auto path = m_currScene->getPath();
        auto name = m_currScene->getName();
        m_currScene = nullptr;
        unloadScene(name);
        loadScene(path);
    }
}
