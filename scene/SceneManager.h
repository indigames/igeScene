#pragma once

#include <memory>
#include <vector>
#include <string>

#include "utils/Singleton.h"
#include "event/Event.h"

namespace ige::scene
{
    class SceneObject;
    class Scene;

    /**
     * Class SceneManager: Manage scene object hierarchy
     */
    class SceneManager: public Singleton<SceneManager>
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

        //! render
        virtual void render();

        //! Create empty scene
        std::shared_ptr<Scene> createScene(const std::string& name = "Untitled");

        //! Load scene
        std::shared_ptr<Scene> loadScene(const std::string& scenePath);

        //! Unload scene
        void unloadScene(const std::shared_ptr<Scene>& scene);

        //! Unload scene by name
        void unloadScene(const std::string& sceneName);

        //! Reload scene
        void reloadScene();

        //! Save scene
        bool saveScene(const std::string& scenePath = std::string());

        //! Get current scene
        const std::shared_ptr<Scene>& getCurrentScene() const { return m_currScene; }

        //! Get scenes
        const std::vector<std::shared_ptr<Scene>>& getScenes() const { return m_scenes; }

        //! Set current scene
        void setCurrentScene(const std::shared_ptr<Scene>& scene);

        //! Set current scene by name
        void setCurrentScene(const std::string& name);

        //! Check if this is editor mode
        bool isEditor() const { return m_bIsEditor; }

        //! Set editor mode
        void setIsEditor(bool isEditor) { m_bIsEditor = isEditor; }

        //! Project path
        const std::string& getProjectPath() const { return m_projectPath; }
        void setProjectPath(const std::string& path);

        //! Editor path
        const std::string& getEditorPath() const { return m_editorPath; }
        void setEditorPath(const std::string& path);

    protected:

        //! Initialize
        void init();

        //! Deinit
        void deinit();

        //! Project path
        std::string m_projectPath = {};

        //! Editor path
        std::string m_editorPath = {};

        //! Scene root node
        std::shared_ptr<Scene> m_currScene;

        //! List of all Scenes
        std::vector<std::shared_ptr<Scene>> m_scenes;

        //! Editor mode
        bool m_bIsEditor = false;
    };

    std::string GetEditorResource(const std::string& path);
}
