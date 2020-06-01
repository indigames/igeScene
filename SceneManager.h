#pragma once

#include <memory>
#include <vector>
#include <string>

namespace ige::scene
{
    class Scene;

    /**
	* SceneManager manage scenes in hierarchy
	*/
    class SceneManager
    {
    public:
        SceneManager();
        virtual ~SceneManager();

        virtual void update(float dt);

        virtual void loadScene(std::string path = "");
        
        virtual std::shared_ptr<Scene> getCurrentScene();
        
    protected:
        std::shared_ptr<Scene> m_scene;
    };
}
