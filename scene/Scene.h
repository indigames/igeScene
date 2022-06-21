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
    class TargetObject;
    class Canvas;

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
        bool initialize(bool empty = false);

        //! Clear
        void clear();

        //! Get uuid
        const std::string& getUUID() const { return m_uuid; }

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

        //! Physics Update
        virtual void physicUpdate(float dt);

        //! Pre-Render
        virtual void preRender(Camera* camera = nullptr);

        //! Render
        virtual void render(RenderTarget* fbo = nullptr, bool skipBeginEnd = false); // skipBeginEnd used for Editor render flow

        //! Canvas Render
        virtual void renderUI(RenderTarget* fbo = nullptr, bool skipBeginEnd = false);

        //! Create scene object
        virtual std::shared_ptr<SceneObject> createObject(const std::string& name = "", const std::shared_ptr<SceneObject>& parent = nullptr, bool isGUI = false, const Vec2& size = { 64, 64 }, const std::string& prefabId = std::string(), const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = { 1,1,1 });

        virtual std::shared_ptr<SceneObject> createObjectFromPrefab(const std::string& file, const std::string& name = "", const std::shared_ptr<SceneObject>& parent = nullptr);
        virtual std::shared_ptr<SceneObject> createObjectFromPrefab(const std::string& file, const std::string& name, const std::shared_ptr<SceneObject>& parent, const Vec3& position);
        virtual std::shared_ptr<SceneObject> createObjectFromPrefab(const std::string& file, const std::string& name, const std::shared_ptr<SceneObject>& parent, const Vec3& position, const Quat& rotation);

        //! Remove all scene objects
        virtual bool removeAllObjects();

        //! Remove scene object
        virtual bool removeObject(std::shared_ptr<SceneObject>& obj);

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
        std::shared_ptr<Canvas> getCanvas() { return m_canvas.expired() ? nullptr : m_canvas.lock(); };

        //! Get objects list
        std::vector<std::shared_ptr<SceneObject>>& getObjects() { return m_objects; };

        //! Set active camera component
        void setActiveCamera(std::shared_ptr<CameraComponent> camera);

        //! Get active camera
        std::shared_ptr<CameraComponent> getActiveCamera() { return m_activeCamera.expired() ? nullptr : m_activeCamera.lock(); }

        //! Return the cached path
        const std::string& getPath() const { return m_path; }
        void setPath(const std::string& path);

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

        //! Resource added/removed event
        void onResourceAdded(Resource* resource);
        void onResourceRemoved(Resource* resource);
        void onUIResourceAdded(Resource* resource);
        void onUIResourceRemoved(Resource* resource);

        //! Prefab save/load
        bool isSavingPrefab() { return m_bIsSavingPrefab; }
        bool savePrefab(uint64_t objectId, const std::string& file);
        std::shared_ptr<SceneObject> loadPrefab(uint64_t parentId, const std::string& file, const Vec3& pos = { 0.f, 0.f, 0.f }, const Quat& rot = {0.f, 0.f, 0.f, 0.f}, const Vec3& scale = { 1.f, 1.f, 1.f });

        bool reloadAllPrefabs();
        bool reloadPrefabs(const std::string& prefabId);

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
        std::pair< std::shared_ptr<SceneObject>, Vec3> raycast(const Vec2& screenPos, Camera* camera, float maxDistance = 10000.f, bool forceRaycast = false);

        std::pair< std::shared_ptr<SceneObject>, Vec3> raycast(const Vec3& position, Vec3& direction, float maxDistance = 10000.f, bool forceRaycast = false);

        std::vector<std::pair<std::shared_ptr<SceneObject>, Vec3>> raycastAll(const Vec3& position, Vec3& direction, float maxDistance = 10000.f, bool forceRaycast = false);
        //! Raycast UI 
        std::pair< std::shared_ptr<SceneObject>, Vec3> raycastUI(const Vec2& screenPos);

        //! Window position
        const Vec2& getWindowPosition() const { return m_windowPosition; }
        void setWindowPosition(const Vec2& pos) { m_windowPosition = pos; }

        //! Window size
        const Vec2& getWindowSize() const { return m_windowSize; }
        void setWindowSize(const Vec2& size);

        //! View size
        const Vec2 getViewSize() const { return m_viewSize; }
        void setViewSize(const Vec2& size);

        //! View position
        const Vec2& getViewPosition() const { return m_viewPosition; }
        void setViewPosition(const Vec2& pos) { m_viewPosition = pos; }

        //! Screen coordinate to client coordinate
        Vec2 screenToClient(const Vec2& pos);

        //! Get tween manager
        std::shared_ptr<TweenManager> getTweenManager() const;

        //! Check if this is prefab scene
        bool isPrefab();

        //! Return the prefabId of the prefab scene
        std::string getPrefabId();

        //! Sceen Point to World Point
        Vec3 screenToWorldPoint(const Vec2& pos, Camera* cam, float nearPlane);
        Vec2 worldToScreenPoint(const Vec3& pos, Camera* cam);

        //Finger
        Vec2 getScreenPos(const Vec2& pos);

        //! Get shared pointer
        std::shared_ptr<Scene> getSharedPtr();

        //! Reload resource
        bool reloadResource(const std::string& path);

    protected:
        //! Create root Objects
        virtual std::shared_ptr<SceneObject> createRootObject(const std::string& name = "");

        //! find intersect in hierarchy
        std::pair< std::shared_ptr<SceneObject>, Vec3> findIntersectInHierachy(std::shared_ptr<SceneObject> target, std::pair<Vec3, Vec3> ray);

        //! Raycast UI 
        Vec3 raycastCanvas(const Vec2& screenPos);

        //! Reset flag
        virtual void resetFlag();

    protected:
        //! Scene root node
        std::shared_ptr<SceneObject> m_root;

        //! UI root node
        std::shared_ptr<SceneObject> m_rootUI;

        //! Canvas
        std::weak_ptr<Canvas> m_canvas;

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

        //! Cache active camera
        std::weak_ptr<CameraComponent> m_activeCamera;

        //! Cache active camera
        Camera* m_camera = nullptr;

        //! Shadow render target
        Texture* m_shadowTexture = nullptr;
        RenderTarget* m_shadowFBO = nullptr;
        EditableFigure* m_shadowEdgeMask = nullptr;
        Vec2 m_shadowTextureSize = { 2048, 2048 };

        //! Object ID counter, reserver 0 for multiple object
        uint64_t m_nextObjectID = 1;

        //! Scene uuid
        std::string m_uuid;

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
        Vec2 m_windowPosition = { 0.f, 0.f };
        Vec2 m_viewPosition = { 0.f, 0.f };

        //! Cache window size
        Vec2 m_windowSize = { -1.f, -1.f };
        Vec2 m_viewSize = { -1.f, -1.f };

        //! TweenManager
        std::shared_ptr<TweenManager> m_tweenManager;

        //! Capture flag
        bool m_raycastCapture;

        //! Saving prefab state
        bool m_bIsSavingPrefab = false;
    };
}
