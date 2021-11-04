#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

#include "utils/Singleton.h"
#include "event/Event.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

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
        virtual void physicUpdate(float dt);

        //! Late update
        virtual void lateUpdate(float dt);

        //! Pre-Render
        virtual void preRender(Camera* camera = nullptr);

        //! render
        virtual void render();

        //! Create empty scene
        std::shared_ptr<Scene> createScene(const std::string& name = "Untitled", bool empty = false);

        //! Creator function: create a scene from prefab file
        std::shared_ptr<Scene> createSceneFromPrefab(const std::string& file);

        //! Find prefab scene from prefabId
        std::shared_ptr<Scene> findPrefabSceneById(const std::string& prefabId);

        //! Load scene
        bool loadScene(std::shared_ptr<Scene> scene, const std::string& scenePath);

        //! Unload scene
        void unloadScene(std::shared_ptr<Scene>& scene);

        //! Unload scene by name
        void unloadScene(const std::string& sceneName);

        //! Reload scene
        void reloadScene();

        //! Save scene
        bool saveScene(const std::string& scenePath = std::string());

        //! Get current scene
        const std::shared_ptr<Scene>& getCurrentScene() const { return m_currScene; }

        //! Get scene
        std::shared_ptr<Scene> getScene(const std::string& uuid);

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

        bool isIgeEditor() const { return m_bIsIgeEditor; }

        void setIsIgeEditor(bool isEditor) { m_bIsIgeEditor = isEditor; }

        //! Check if this is editor playing mode
        bool isPlaying() const { return m_bIsPlaying; }

        //! Set editor playing mode
        void setIsPlaying(bool isPlaying) { m_bIsPlaying = isPlaying; }

        //! Project path
        const std::string& getProjectPath() const { return m_projectPath; }
        void setProjectPath(const std::string& path);

        //! Editor path
        const std::string& getEditorPath() const { return m_editorPath; }
        void setEditorPath(const std::string& path);

        //! Get/Set prefab path by id
        std::string getPrefabPath(const std::string& prefabId);
        void setPrefabPath(const std::string& id, const std::string& path);

        //! Get prefab paths map
        const std::map<std::string, std::string>& getPrefabPaths() const { return m_prefabPaths; }

        //! Dispatch event
        void dispathEvent(int eventType);

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

        //! Map of prefabId and it paths
        std::map<std::string, std::string> m_prefabPaths;

        //! Editor mode
        bool m_bIsEditor = false;
        bool m_bIsIgeEditor = false;
        bool m_bIsPlaying = false;
    };

    std::string GetEditorResource(const std::string& path);
}
