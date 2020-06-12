#include <algorithm>

#include "scene/SceneManager.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "components/Component.h"
#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "components/EnvironmentComponent.h"

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
    }
    
    void SceneManager::update(float dt)
    {
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

    std::shared_ptr<Scene> SceneManager::createEmptyScene()
    {
        auto scene = std::make_shared<Scene>("EmptyScene");
        m_scenes.push_back(scene);
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

    bool SceneManager::loadScene(const std::string& path)
    {
        // TODO: implement this

        return false;
    }

    bool SceneManager::saveScene(const std::string& path)
    {
        // TODO: implement this
        return false;
    }

}
