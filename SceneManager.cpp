#include <algorithm>

#include "SceneObject.h"
#include "SceneManager.h"
#include "components/Component.h"

#define SERIALIZE_VERSION "0.0.1"

namespace ige::scene
{
    SceneManager::SceneManager() {}

    SceneManager::~SceneManager()
    {
        m_root = nullptr;
    }
    
    void SceneManager::update(float dt)
    {
        m_root->onUpdate(dt);
    }

    void SceneManager::fixedUpdate(float dt)
    {
        m_root->onFixedUpdate(dt);
    }

    void SceneManager::lateUpdate(float dt)
    {
         m_root->onLateUpdate(dt);
    }

    std::shared_ptr<SceneObject> SceneManager::createObject(std::string name, std::shared_ptr<SceneObject> parent)
    {
        return std::make_shared<SceneObject>(m_nextObjectID++, name, parent);
    }

    bool SceneManager::removeObject(std::shared_ptr<SceneObject> obj)
    {
        if(!obj) return false;

        if(!m_root || m_root == obj) 
        {
            m_root = nullptr;
            return true;
        }
        
        return m_root->removeChild(obj);        
    }
    
    std::shared_ptr<SceneObject> SceneManager::findObjectById(uint64_t id) const
    {
        return m_root->findObjectById(id);
    }

    std::shared_ptr<SceneObject> SceneManager::findObjectByName(std::string name) const
    {
        return m_root->findObjectByName(name);
    }

    bool SceneManager::createEmptyScene()
    {
        // TODO: implement this
        return false;
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
