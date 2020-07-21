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
        std::shared_ptr<Scene> createEmptyScene(const std::string& name);

        //! Load scene
        std::shared_ptr<Scene> loadScene(const std::string& scenePath);

        //! Save scene
        bool saveScene(const std::string& scenePath);

        //! unload scene
        void unloadScene(const std::shared_ptr<Scene>& scene);

        //! Get all scenes
        std::vector<std::shared_ptr<Scene>>& getAllScenes() { return m_scenes; }

        //! Get current scene
        std::shared_ptr<Scene>& getCurrentScene() { return m_currScene; }

        //! Set current scene
        void setCurrentScene(const std::shared_ptr<Scene>& scene);

        //! Set current scene by name
        void setCurrentScene(const std::string& name);

        //! Check if this is editor mode
        bool isEditor() const { return m_bIsEditor; }

        //! Set editor mode
        void setIsEditor(bool isEditor) { m_bIsEditor = isEditor; }

        //! Internal events
        Event<Scene&>& getOnSceneAddedEvent() { return m_sceneAddedEvent; }
        Event<Scene&>& getOnSceneRemovedEvent() { return m_sceneRemovedEvent; }

    protected:
        //! Scene root node
        std::shared_ptr<Scene> m_currScene;

        //! List of all Scenes
        std::vector<std::shared_ptr<Scene>> m_scenes;

        //! Init in main thread
        bool m_bInitialized = false;

        //! Editor mode
        bool m_bIsEditor = false;

        //! Internal events
        Event<Scene&> m_sceneAddedEvent;
        Event<Scene&> m_sceneRemovedEvent;
    };
}
