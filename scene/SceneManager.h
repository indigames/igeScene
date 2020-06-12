#pragma once

#include <memory>
#include <vector>
#include <string>

namespace ige::scene
{
    class SceneObject;
    class Scene;

    /**
     * Class SceneManager: Manage scene object hierarchy 
     */
    class SceneManager
    {
    public:
        //! Constructor
        SceneManager();

        //! Destructor
        virtual ~SceneManager();

        //! Update
        virtual void update(float dt);

        //! Update with fixed dt
        virtual void fixedUpdate(float dt);

        //! Late update
        virtual void lateUpdate(float dt);

        //! Create empty scene
        std::shared_ptr<Scene> createEmptyScene();

        //! Load scene from IO
        bool loadScene(const std::string& scenePath);

        //! Save scene
        bool saveScene(const std::string& scenePath);

        //! Get current scene
        std::shared_ptr<Scene> getCurrentScene() { return m_currScene; }

        //! Set current scene
        void setCurrentScene(const std::string& name);
        void setCurrentScene(std::shared_ptr<Scene> scene);

    protected:
        //! Scene root node
        std::shared_ptr<Scene> m_currScene;
        std::vector<std::shared_ptr<Scene>> m_scenes;
    };
}
