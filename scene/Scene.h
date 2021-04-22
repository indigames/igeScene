#pragma once

#include <memory>
#include <vector>
#include <string>

#include <utils/PyxieHeaders.h>
#include "components/Component.h"
#include "components/CameraComponent.h"
#include "event/Event.h"

#define MAX_DIRECTIONAL_LIGHT_NUMBER    3
#define MAX_POINT_LIGHT_NUMBER          7
#define MAX_SPOT_LIGHT_NUMBER           7


namespace ige::scene
{
    class SceneObject;
    class TweenManager;

    /**
     * Class Scene: Manage scene objects hierarchy
     */
    class Scene
    {
    public:
        //! Constructor
        Scene(const std::string& name);

        //! Destructor
        virtual ~Scene();

        //! Initialize
        bool initialize();

        //! Clear
        void clear();

        //! Get name
        const std::string& getName() const { return m_name; }

        //! Get name
        void setName(const std::string& name);

        //! Update
        virtual void update(float dt);

        //! Update with fixed dt
        virtual void fixedUpdate(float dt);

        //! Late update
        virtual void lateUpdate(float dt);

        //! Reset flag
        virtual void resetFlag();

        //! Render
        virtual void render();

        //! Canvas Render
        virtual void renderUI();

        //! Create scene object
        virtual std::shared_ptr<SceneObject> createObject(const std::string& name = "", const std::shared_ptr<SceneObject>& parent = nullptr, bool isGUI = false, const Vec2& size = { 64, 64 }, bool isCanvas = false);

        //! Remove scene object
        virtual bool removeObject(const std::shared_ptr<SceneObject>& obj);

        //! Remove scene object by its name
        virtual bool removeObjectById(uint64_t id);

        //! Find object in hierarchy by it's id
        virtual std::shared_ptr<SceneObject> findObjectById(uint64_t id);

        //! Find object in hierarchy by it's uuid
        virtual std::shared_ptr<SceneObject> findObjectByUUID(const std::string& uuid);

        //! Find object in hierarchy by name
        virtual std::shared_ptr<SceneObject> findObjectByName(const std::string& name);

        //! Serialize
        virtual void to_json(json& j) const;

        //! Deserialize
        virtual void from_json(const json& j);

        //! Get root of scene
        std::shared_ptr<SceneObject>& getRoot() { return m_root; };

        //! Get root UI scene
        std::shared_ptr<SceneObject>& getRootUI() { return m_rootUI; };

        //! Get Canvas
        std::shared_ptr<SceneObject>& getCanvas() { return m_canvas; };

        //! Get objects list
        std::vector<std::shared_ptr<SceneObject>>& getObjects() { return m_objects; };

        //! Set active camera
        void setActiveCamera(CameraComponent* camera);

        //! Get active camera
        CameraComponent* getActiveCamera() { return m_activeCamera; }

        //! Return the cached path
        const std::string& getPath() const { return m_path; }

        //! Cache the path
        void setPath(const std::string& path) { m_path = path; }

        //! Get showcase
        Showcase* getShowcase() { return m_showcase; }

        //! Get ui showcase
        Showcase* getUIShowcase() { return m_uiShowcase; }

        //! Get environment
        Environment* getEnvironment() { return m_environment; }

        //! Internal event
        Event<Resource*>& getResourceAddedEvent() { return m_resourceAddedEvent; }
        Event<Resource*>& getResourceRemovedEvent() { return m_resourceRemovedEvent; }
        Event<Resource*>& getUIResourceAddedEvent() { return m_uiResourceAddedEvent; }
        Event<Resource*>& getUIResourceRemovedEvent() { return m_uiResourceRemovedEvent; }
        Event<Scene*>& getSerializeFinishedEvent() { return m_serializeFinishedEvent; }

        //! Resource added/removed event
        void onResourceAdded(Resource* resource);
        void onResourceRemoved(Resource* resource);
        void onUIResourceAdded(Resource* resource);
        void onUIResourceRemoved(Resource* resource);

        //! Prefab save/load
        bool savePrefab(uint64_t objectId, const std::string& file);
        bool loadPrefab(uint64_t parentId, const std::string& file);

        //! Acquire directional light
        bool isDirectionalLightAvailable();
        int acquireDirectionalLight();
        void releaseDirectionalLight(int index);

        //! Acquire point light
        bool isPointLightAvailable();
        int acquirePointLight();
        void releasePointLight(int index);

        //! Acquire spot light
        bool isSpotLightAvailable();
        int acquireSpotLight();
        void releaseSpotLight(int index);

        //! Shadow texture size
        const Vec2& getShadowTextureSize() const;
        void setShadowTextureSize(const Vec2& size);

        //! Raycast: return first hit point from single ray
        //! forceRaycast : can ignore raycastCapture
        std::pair<SceneObject*, Vec3> raycast(const Vec2& screenPos, Camera* camera, float maxDistance = 10000.f, bool forceRaycast = false);

        //! Raycast UI 
        std::pair<SceneObject*, Vec3> raycastUI(const Vec2& screenPos);

        //! Window position
        const Vec2& getWindowPosition() const { return m_windowPosition; }
        void setWindowPosition(const Vec2& pos) { m_windowPosition = pos; }

        //! Window size
        const Vec2& getWindowSize() const { return m_windowSize; }
        void setWindowSize(const Vec2& size) { m_windowSize = size; }

        std::shared_ptr<TweenManager> getTweenManager() const;

    protected:
        //! Create root Objects
        virtual std::shared_ptr<SceneObject> createRootObject(const std::string& name = "");

        void populateTestData(const std::shared_ptr<SceneObject>& parent = nullptr, int num = 1000);

        std::pair<SceneObject*, Vec3>  findIntersectInHierachy(const SceneObject* target, std::pair<Vec3, Vec3> ray);
                
        //! Raycast UI 
        Vec3 raycastCanvas(const Vec2& screenPos);
    protected:
        //! Scene root node
        std::shared_ptr<SceneObject> m_root;
        //! UI root node
        std::shared_ptr<SceneObject> m_rootUI;
        //! Canvas
        std::shared_ptr<SceneObject> m_canvas;

        //! Cache all objects
        std::vector<std::shared_ptr<SceneObject>> m_objects;

        //! Showcase which contains all rendering resources
        Showcase* m_showcase = nullptr;

        Showcase* m_uiShowcase = nullptr;

        //! Environment settings of the scene
        Environment* m_environment = nullptr;

        //! Internal events
        //! Main ShowCase
        Event<Resource*> m_resourceAddedEvent;
        Event<Resource*> m_resourceRemovedEvent;
        //! UI ShowCase
        Event<Resource*> m_uiResourceAddedEvent;
        Event<Resource*> m_uiResourceRemovedEvent;

        Event<Scene*> m_serializeFinishedEvent;

        //! Cache active camera
        CameraComponent* m_activeCamera = nullptr;

        //! Canvas Camera 
        Camera* m_canvasCamera = nullptr;

        //! Shadow render target
        Texture* m_shadowTexture = nullptr;
        RenderTarget* m_shadowFBO = nullptr;
        EditableFigure* m_shadowEdgeMask = nullptr;
        Vec2 m_shadowTextureSize = {2048, 2048};

        //! Object ID counter
        uint64_t m_nextObjectID = 0;

        //! Scene name
        std::string m_name;

        //! Cached path
        std::string m_path = {};

        //! Directional light index
        bool m_directionalLights[MAX_DIRECTIONAL_LIGHT_NUMBER] = { false };

        //! Point light index
        bool m_pointLights[MAX_POINT_LIGHT_NUMBER] = { false };

        //! Spot light index
        bool m_spotLights[MAX_SPOT_LIGHT_NUMBER] = { false };

        //! Cache window position
        Vec2 m_windowPosition = {0.f, 0.f};

        //! Cache window size
        Vec2 m_windowSize = {-1.f, -1.f};

        //!TweenManager
        std::shared_ptr<TweenManager> m_tweenManager;

        //!Capture flag
        bool m_raycastCapture;
    };
}
