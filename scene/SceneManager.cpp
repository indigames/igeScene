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

#include <filesystem>
namespace fs = std::filesystem;

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#ifdef _WIN32
#  define DELIMITER ";"
#else
#  define DELIMITER ":"
#endif

#define SERIALIZE_VERSION "0.0.1"

namespace ige::scene
{
    SceneManager::SceneManager() 
    {
        m_currScene = nullptr;
    }

    SceneManager::~SceneManager()
    {
        m_currScene = nullptr;
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

    std::shared_ptr<Scene> SceneManager::createEmptyScene()
    {
        auto scene = std::make_shared<Scene>("EmptyScene");
        scene->initialize();
        m_scenes.push_back(scene);

        //auto tree = scene->createObject("tree", scene->getRoot());
        //tree->getComponent<TransformComponent>()->setPosition(Vec3(-5.f, 0.f, -10.f));
        //tree->addComponent<FigureComponent>("Trees_Object");

        //auto tree2 = scene->createObject("tree2", tree);
        //tree2->getComponent<TransformComponent>()->setPosition(Vec3(-2.f, 0.f, -10.f));
        //tree2->addComponent<FigureComponent>("Trees_Object");

        //auto tree3 = scene->createObject("tree3", tree2);
        //tree3->getComponent<TransformComponent>()->setPosition(Vec3(8.f, 0.f, -10.f));
        //tree3->addComponent<FigureComponent>("Trees_Object");

    /// Just to test performance
         //for (int i = 0; i < 10000; i++)
         //{
         //    auto newTree = scene->createObject(std::string("tree") + std::to_string(i), tree2);
         //    newTree->getComponent<TransformComponent>()->setPosition(Vec3(-2.f, 0.f, -10.f));
         //    newTree->addComponent<FigureComponent>("Trees_Object");
         //}
    ///
        setCurrentScene(scene);
        return scene;
    }

    void SceneManager::setCurrentScene(const std::string& name)
    {
        auto found = std::find_if(m_scenes.begin(), m_scenes.end(), [&](auto scene) {
            return scene->getName() == name;
        });

        if (found != m_scenes.end())
        {
            m_currScene = (*found);
        }
    }

    void SceneManager::setCurrentScene(std::shared_ptr<Scene> scene)
    {
        if (std::find(m_scenes.begin(), m_scenes.end(), scene) == m_scenes.end())
        {
            m_scenes.push_back(scene);
        }
        m_currScene = scene;
    }

    std::shared_ptr<Scene> SceneManager::loadScene(const std::string& path)
    {
        json jScene;
        std::ifstream file(path);
        file >> jScene;
        auto s = jScene.dump();

        auto scene = std::make_shared<Scene>(jScene.at("name"));
        scene->from_json(jScene);
        m_scenes.push_back(scene);
        return scene;
    }

    bool SceneManager::saveScene(const std::string& path)
    {
        if (m_currScene)
        {
            json jScene;
            m_currScene->to_json(jScene);
            auto savedPath = fs::path(path).append(m_currScene->getName() + ".json").c_str();
            std::ofstream file(savedPath);
            file << jScene;
            return true;
        }
        return false;
    }

}
