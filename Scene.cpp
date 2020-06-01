#include <algorithm>

#include "Component.h"
#include "Scene.h"
#include "SceneObject.h"

namespace ige::scene
{
    Scene::Scene(std::string name)
    {
        
    }

    Scene::~Scene()
    {
        
    }

    void Scene::update(float dt)
    {
        std::for_each(m_objects.begin(), m_objects.end(), [&](auto element) { element->onUpdate(dt); });
    }

    void Scene::fixedUpdate(float dt)
    {
        std::for_each(m_objects.begin(), m_objects.end(), [&](auto element) { element->onFixedUpdate(dt); });
    }

    void Scene::lateUpdate(float dt)
    {
        std::for_each(m_objects.begin(), m_objects.end(), [&](auto element) { element->onLateUpdate(dt); });
    }

    std::vector<std::shared_ptr<SceneObject>> Scene::getObjects() const
    {
        return m_objects;
    }
    
    std::shared_ptr<SceneObject> Scene::findObjectById(int id) const
    {
        auto it = std::find_if(m_objects.begin(), m_objects.end(), [&](auto elem) { return elem->getId() == id; });
        if(it != m_objects.end())
            return *it;
        return nullptr;            
    }

    std::shared_ptr<SceneObject> Scene::findObjectByName(std::string name) const
    {
        auto it = std::find_if(m_objects.begin(), m_objects.end(), [&](auto elem) { return elem->getName() == name; });
        if(it != m_objects.end())
            return *it;
        return nullptr;
    }
}
