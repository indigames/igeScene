#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>

#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "scene/SceneManager.h"

#include "components/Component.h"
#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "components/EnvironmentComponent.h"
#include "components/light/AmbientLight.h"
#include "components/light/DirectionalLight.h"
#include "components/light/PointLight.h"
#include "components/light/SpotLight.h"
#include "components/ScriptComponent.h"
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"
#include "components/gui/UIImage.h"
#include "components/tween/TweenManager.h"

#include "utils/GraphicsHelper.h"
#include "utils/RayOBBChecker.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    static const float ip = 0.995f;
    static const float op = 1.0f;

    static const float verts[] = {
        -op, -op, 0,  1.0f,0,0,1.0f,
         op, -op, 0,  1.0f,0,0,1.0f,
         op,  op, 0,  1.0f,0,0,1.0f,
        -op,  op, 0,  1.0f,0,0,1.0f,
        -ip, -ip, 0,  1.0f,0,0,1.0f,
         ip, -ip, 0,  1.0f,0,0,1.0f,
         ip,  ip, 0,  1.0f,0,0,1.0f,
        -ip,  ip, 0,  1.0f,0,0,1.0f,
    };

    static const uint32_t tris[] = {
        0,1,5, 0,5,4,
        0,4,7, 0,7,3,
        1,2,5, 5,2,6,
        7,6,2, 7,2,3
    };

    Scene::Scene(const std::string& name)
        : m_name(name)
    {
    }

    Scene::~Scene()
    {
        clear();
    }

    bool Scene::initialize(bool empty)
    {
        m_environment = ResourceCreator::Instance().NewEnvironmentSet(m_name.c_str(), nullptr);
        m_environment->WaitBuild();

        // Set directional lights deactivated
        for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_NUMBER; ++i)
            m_environment->SetDirectionalLampIntensity(i, 0.f);

        // Set point lights deactivated
        for (int i = 0; i < MAX_POINT_LIGHT_NUMBER; ++i)
            m_environment->SetPointLampIntensity(i, 0.f);

        m_showcase = ResourceCreator::Instance().NewShowcase((m_name + "_showcase").c_str());
        m_shadowTexture = ResourceCreator::Instance().NewTexture("Shadow_texture", nullptr, 2048, 2048, GL_RED);
        m_shadowTexture->WaitBuild();
        m_showcase->SetShadowBuffer(m_shadowTexture);

        m_shadowEdgeMask = ResourceCreator::Instance().NewEditableFigure("shadowEdgeMask", true);

        m_uiShowcase = ResourceCreator::Instance().NewShowcase((m_name + "_ui_showcase").c_str());

        ShaderDescriptor desc;
        desc.DicardViewProj(true);
        desc.SetVertexColor(true);
        desc.SetVertexAlpha(true);
        m_shadowEdgeMask->AddMaterial("mate", desc);

        float diffuse[4] = { 1,1,1,1 };
        m_shadowEdgeMask->SetMaterialParam(0, GenerateNameHash("DiffuseColor"), diffuse, ParamTypeFloat4);

        float dt = 0;
        m_shadowEdgeMask->SetMaterialState(0, Key_depth_test_enable, &dt);
        m_shadowEdgeMask->AddMesh("mesh", "mate");
        m_shadowEdgeMask->SetMeshVertices(0, verts, 8);
        m_shadowEdgeMask->SetMeshIndices(0, 0, tris, 24, 4);

        Joint pose;
        m_shadowEdgeMask->AddJoint(-1, pose, false, "j01");
        m_shadowEdgeMask->SetMeshAlpha(0, 0.97f);

        m_shadowFBO = ResourceCreator::Instance().NewRenderTarget(m_shadowTexture, true, false);
        m_showcase->Add(m_environment);

        getResourceAddedEvent().addListener(std::bind(&Scene::onResourceAdded, this, std::placeholders::_1));
        getResourceRemovedEvent().addListener(std::bind(&Scene::onResourceRemoved, this, std::placeholders::_1));

        getUIResourceAddedEvent().addListener(std::bind(&Scene::onUIResourceAdded, this, std::placeholders::_1));
        getUIResourceRemovedEvent().addListener(std::bind(&Scene::onUIResourceRemoved, this, std::placeholders::_1));

        if (!empty)
        {
            m_nextObjectID = 3;

            // Create root object
            m_root = createRootObject(m_name);

            // Set ambient color
            m_root->addComponent<AmbientLight>()->setSkyColor({ 0.75f, 0.75f, 0.75f });

            // Set general environment
            m_root->addComponent<EnvironmentComponent>();

            // Create Canvas Object
            m_rootUI = createRootObject("UI");

            // Create default camera
            auto camObj = createObject("Default Camera");
            camObj->getTransform()->setPosition(Vec3(0.f, 0.f, 10.f));
            auto camComp = camObj->addComponent<CameraComponent>("default_camera");
            camComp->lockOnTarget(false);
            camComp->setAspectRatio(SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH());

            // Create default directional light
            auto directionalLight = createObject("Directional Light");
            directionalLight->addComponent<DirectionalLight>();
            directionalLight->getTransform()->setPosition({ 0.f, 5.f, 0.f });
            directionalLight->getTransform()->setRotation({ DEGREES_TO_RADIANS(90.f), 0.f, .0f });

            // Add editor debug
            if (SceneManager::getInstance()->isEditor())
            {
                camObj->addComponent<FigureComponent>(GetEditorResource("figures/camera"))->setSkipSerialize(true);
                directionalLight->addComponent<SpriteComponent>(GetEditorResource("sprites/direct-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
            }
        }        

        // Tween manager
        m_tweenManager = std::make_shared<TweenManager>();
        m_tweenManager->init();

        setWindowSize({ -1.f, -1.f });
        return true;
    }

    void Scene::clear()
    {
        removeAllObjects();

        getResourceAddedEvent().removeAllListeners();
        getResourceRemovedEvent().removeAllListeners();
        getUIResourceAddedEvent().removeAllListeners();
        getUIResourceRemovedEvent().removeAllListeners();
        getSerializeFinishedEvent().removeAllListeners();

        m_nextObjectID = 1;
        m_activeCamera = nullptr;
        m_root = nullptr;
        m_rootUI = nullptr;
        m_canvas = nullptr;

        if (m_shadowTexture)
        {
            m_shadowTexture->DecReference();
            m_shadowTexture = nullptr;
        }

        if (m_shadowFBO)
        {
            m_shadowFBO->DecReference();
            m_shadowFBO = nullptr;
        }

        if (m_shadowEdgeMask)
        {
            m_shadowEdgeMask->DecReference();
            m_shadowEdgeMask = nullptr;
        }

        if (m_showcase)
        {
            if (m_environment)
                m_showcase->Remove(m_environment);
            m_showcase->Clear();
            m_showcase->DecReference();
            m_showcase = nullptr;
        }

        if (m_environment)
        {
            m_environment->DecReference();
            m_environment = nullptr;
        }

        if (m_tweenManager) {
            m_tweenManager->clean();
            m_tweenManager = nullptr;
        }

        if (m_uiShowcase)
        {
            m_uiShowcase->Clear();
            m_uiShowcase->DecReference();
            m_uiShowcase = nullptr;
        }

        ResourceManager::Instance().DeleteDaemon();
    }

    void Scene::setName(const std::string& name)
    {
        if (m_name.compare(name) != 0)
        {
            m_name = name;
            if (m_root)
            {
                m_root->setName(name);
            }
        }
    }

    void Scene::update(float dt)
    {
        for (auto& obj : m_objects)
            if (obj) obj->onUpdate(dt);

        if (!SceneManager::getInstance()->isEditor() && m_activeCamera)
            m_activeCamera->onUpdate(dt);

        m_showcase->Update(dt);

        // Render shadow before actually render objects
        auto renderContext = RenderContext::InstancePtr();
        if (renderContext)
        {
            renderContext->BeginScene(m_shadowFBO, Vec4(1.f, 1.f, 1.f, 1.f), true, true);

            if (!SceneManager::getInstance()->isEditor() && m_activeCamera) {
                m_activeCamera->onRender();
            }

            m_showcase->Render(RenderPassFilter::ShadowPass);

            renderContext->BeginPass(TransparentPass);
            m_shadowEdgeMask->Pose();
            m_shadowEdgeMask->Render();
            renderContext->EndScene();
        }

        m_uiShowcase->Update(dt);

        if(m_tweenManager) m_tweenManager->update(dt);

        resetFlag();
    }

    void Scene::fixedUpdate(float dt)
    {
        for (auto& obj : m_objects)
            if (obj) obj->onFixedUpdate(dt);
    }

    void Scene::lateUpdate(float dt)
    {
        for (auto& obj : m_objects)
            if (obj) obj->onLateUpdate(dt);
    }

    void Scene::physicUpdate(float dt)
    {
        for (auto& obj : m_objects)
            if (obj) obj->onPhysicUpdate(dt);
    }

    void Scene::resetFlag()
    {
        m_raycastCapture = false;
    }

    void Scene::render()
    {
        if (!SceneManager::getInstance()->isEditor() && m_activeCamera) {
            m_showcase->ZSort(m_activeCamera->getCamera());
            m_activeCamera->onRender();
        }
        m_showcase->Render();

        for (auto& obj : m_objects)
            if (obj) obj->onRender();
    }

    void Scene::renderUI() {
        if (!SceneManager::getInstance()->isEditor()) {
            if (getCanvas() && getCanvas()->getCamera()) {
                getCanvas()->getCamera()->Render();
            }
        }
        m_uiShowcase->Render();
    }

    std::shared_ptr<SceneObject> Scene::createObject(const std::string& name, const std::shared_ptr<SceneObject>& parent, bool isGUI, const Vec2& size, const std::string& prefabId)
    {
        auto parentObject = parent ? parent : m_root;
        if (name.compare("Canvas") != 0) {
            if (isGUI) {
                parentObject = parentObject->isGUIObject() ? parentObject : m_canvas ? m_canvas->getOwner()->getSharedPtr() : m_rootUI;
                if (parentObject == m_rootUI) {
                    auto canvasObject = std::make_shared<SceneObject>(this, m_nextObjectID++, "Canvas", true, size, prefabId);
                    m_objects.push_back(canvasObject);
                    m_canvas = canvasObject->addComponent<Canvas>();
                    m_canvas->setDesignCanvasSize(Vec2(540.f, 960.f));
                    m_canvas->setTargetCanvasSize(Vec2(540.f, 960.f));
                    canvasObject->setCanvas(m_canvas);
                    canvasObject->setParent(parentObject);
                    parentObject = canvasObject;
                }
            }
            else if (parentObject) {
                if (parentObject->isGUIObject())
                    isGUI = true;
            }
        }
        auto sceneObject = std::make_shared<SceneObject>(this, m_nextObjectID++, name, isGUI, size, prefabId);
        m_objects.push_back(sceneObject);
        sceneObject->setParent(parentObject);
        return sceneObject;
    }

    std::shared_ptr<SceneObject> Scene::createObjectFromPrefab(const std::string& path, const std::string& name, const std::shared_ptr<SceneObject>& parent)
    {
        auto object = loadPrefab(parent ? parent->getId() : -1, path);
        if (object) object->setName(name);
        return object;
    }

    std::shared_ptr<SceneObject> Scene::createRootObject(const std::string& name) {
        auto sceneObject = std::make_shared<SceneObject>(this, m_nextObjectID++, name);
        m_objects.push_back(sceneObject);
        return sceneObject;
    }

    bool Scene::removeAllObjects()
    {
        for (auto& obj : m_objects)
            obj = nullptr;
        m_objects.clear();
        return true;
    }

    bool Scene::removeObject(std::shared_ptr<SceneObject>& obj)
    {
        if (!obj) return false;

        // Remove active camera
        if (auto camera = obj->getComponent<CameraComponent>())
        {
            if (getActiveCamera() == camera.get())
                setActiveCamera(nullptr);
        }

        // Remove root object
        if (obj == m_root)
        {
            m_root = nullptr;
        }

        if (obj == m_rootUI)
        {
            m_rootUI = nullptr;
        }

        if (obj->getName().compare("Canvas") == 0) {
            m_canvas = nullptr;
        }

        // Remove from objects list
        auto itr = std::find(m_objects.begin(), m_objects.end(), obj);
        if (itr != m_objects.end()) {
            for (auto& child : obj->getChildren()) {
                if (!child.expired()) {
                    removeObjectById(child.lock()->getId());
                }
            }
            m_objects.erase(itr);
            obj = nullptr;
            return true;
        }
        return false;
    }

    //! Remove scene object by its id
    bool Scene::removeObjectById(uint64_t id)
    {
        auto object = findObjectById(id);
        auto removed = removeObject(object);
        object = nullptr;
        return removed;
    }

    std::shared_ptr<SceneObject> Scene::findObjectById(uint64_t id)
    {
        // Perform binary search for best performance, 'cause m_objects sorted by id.
        auto found = std::lower_bound(m_objects.begin(), m_objects.end(), id, [](auto elem, uint64_t id)
        {
            return elem && elem->getId() < id;
        });
        return (found != m_objects.end() && (*found)->getId() == id) ? *found : nullptr;
    }

    std::shared_ptr<SceneObject> Scene::findObjectByUUID(const std::string& uuid)
    {
        auto found = std::find_if(m_objects.begin(), m_objects.end(), [&](auto elem)
        {
            return elem->getUUID() == uuid;
        });
        if (found != m_objects.end())
            return std::dynamic_pointer_cast<SceneObject>(*found);
        return nullptr;
    }

    std::shared_ptr<SceneObject> Scene::findObjectByName(const std::string& name)
    {
        auto found = std::find_if(m_objects.begin(), m_objects.end(), [&](auto elem)
        {
            return elem->getName() == name;
        });
        if (found != m_objects.end())
            return std::dynamic_pointer_cast<SceneObject>(*found);
        return nullptr;
    }


    //! Set active camera
    void Scene::setActiveCamera(CameraComponent* camera)
    {
        if (m_activeCamera != camera)
        {
            if (camera && camera->getOwner()->isActive())
            {
                m_activeCamera = camera;
            }
            else
            {
                m_activeCamera = nullptr;
            }
        }
    }

    //! Set path
    void Scene::setPath(const std::string& path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');
        m_path = relPath;
    }

    //! Resource added event
    void Scene::onResourceAdded(Resource* resource)
    {
        if (resource) {
            m_showcase->Add(resource);
        }
    }

    //! Resource removed event
    void Scene::onResourceRemoved(Resource* resource)
    {
        if (resource) {
            m_showcase->Remove(resource);
        }
    }

    //! UI Resource added event
    void Scene::onUIResourceAdded(Resource* resource)
    {
        if (resource) {
            m_uiShowcase->Add(resource);
        }
    }

    //! UI Resource removed event
    void Scene::onUIResourceRemoved(Resource* resource)
    {
        if (resource) {
            m_uiShowcase->Remove(resource);
        }
    }

    //! Prefab save/load
    bool Scene::savePrefab(uint64_t objectId, const std::string& path)
    {
        auto object = findObjectById(objectId);
        if (object != nullptr)
        {
            m_bIsSavingPrefab = true;
            json jObj;
            object->to_json(jObj);

            auto fsPath = path.empty() ? fs::path(object->getName()) : fs::path(path);
            auto ext = fsPath.extension();
            if (ext.string() != ".prefab")
                fsPath = fsPath.replace_extension(".prefab");

            auto prefabId = std::string();
            if (fs::exists(fsPath)) {
                std::ifstream file(fsPath);
                if (!file.is_open())
                    return false;
                json jObj;
                file >> jObj;
                file.close();
                prefabId = jObj.value("prefabId", std::string());
            }

            if(prefabId.empty())
                prefabId = SceneObject::generateUUID();

            jObj["prefabId"] = prefabId;
            std::ofstream file(fsPath.string());
            file << std::setw(2) << jObj << std::endl;
            SceneManager::getInstance()->setPrefabPath(prefabId, fsPath.string());
            m_bIsSavingPrefab = false;
            return true;
        }
        return false;
    }

    std::shared_ptr<SceneObject> Scene::loadPrefab(uint64_t parentId, const std::string& path)
    {
        if (path.empty())
            return nullptr;

        auto fsPath = fs::path(path);
        if (fsPath.extension().string() != ".prefab")
            return nullptr;

        std::ifstream file(fsPath);
        if (!file.is_open())
            return nullptr;

        json jObj;
        file >> jObj;

        auto parent = findObjectById(parentId);
        auto prefabId = jObj.value("prefabId", std::string());
        auto obj = createObject(jObj.at("name"), parent, jObj.value("gui", false), jObj.value("size", Vec2{ 64.f, 64.f }), prefabId);
        obj->from_json(jObj);
        return obj;
    }

    bool  Scene::reloadPrefabs(const std::string& prefabId) {
        std::vector<uint64_t> objectsToRemove = {};
        std::vector<uint64_t> objectsToLoad = {};
        for (const auto& object: m_objects) {
            if (object->getPrefabId().compare(prefabId) == 0) {
                objectsToRemove.push_back(object->getId());
                if(object->getParent()) objectsToLoad.push_back(object->getParent()->getId());
            }
        }
        for (auto id : objectsToRemove) {
            removeObjectById(id);
            auto itr = std::find(objectsToLoad.begin(), objectsToLoad.end(), id);
            if(itr != objectsToLoad.end()) objectsToLoad.erase(itr);
        }
        auto prefabPath = SceneManager::getInstance()->getPrefabPath(prefabId);
        for (auto id : objectsToLoad) {
            auto object = findObjectById(id);
            if(object) loadPrefab(object->getId(), prefabPath);
        }
        objectsToRemove.clear();
        objectsToLoad.clear();
        return true;
    }

    bool Scene::reloadAllPrefabs()
    {
        for (const auto& [id, path] : SceneManager::getInstance()->getPrefabPaths()) {
            reloadPrefabs(id);
        }
        return true;
    }

    //! Acquire directional light
    bool Scene::isDirectionalLightAvailable()
    {
        for (auto enable : m_directionalLights)
        {
            if (!enable)
                return true;
        }
        return false;
    }

    int Scene::acquireDirectionalLight()
    {
        for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_NUMBER; ++i)
        {
            if (m_directionalLights[i] == false)
            {
                m_directionalLights[i] = true;
                return i;
            }
        }

        // safe value
        return 0;
    }

    void Scene::releaseDirectionalLight(int idx)
    {
        m_directionalLights[idx] = false;

        // Reset to default value
        if (m_environment)
        {
            m_environment->SetDirectionalLampColor(idx, { 0.5f, 0.5f, 0.5f });
            m_environment->SetDirectionalLampDirection(idx, { 5.f, 10.f, 5.f });
            m_environment->SetDirectionalLampIntensity(idx, 0.f);
        }
    }

    //! Acquire point light
    bool Scene::isPointLightAvailable()
    {
        for (auto enable : m_pointLights)
        {
            if (!enable)
                return true;
        }
        return false;
    }

    int Scene::acquirePointLight()
    {
        for (int i = 0; i < MAX_POINT_LIGHT_NUMBER; ++i)
        {
            if (m_pointLights[i] == false)
            {
                m_pointLights[i] = true;
                return i;
            }
        }

        // safe value
        return 0;
    }

    void Scene::releasePointLight(int idx)
    {
        m_pointLights[idx] = false;

        // Reset to default value
        m_environment->SetPointLampColor(idx, { 1.0f, 1.0f, 1.0f });
        m_environment->SetPointLampPosition(idx, { 0.0f, 100.0f, 0.0f });
        m_environment->SetPointLampIntensity(idx, 0.0f);
        m_environment->SetPointLampRange(idx, 100.0f);
    }

    //! Acquire spot light
    bool Scene::isSpotLightAvailable()
    {
        for (auto enable : m_spotLights)
        {
            if (!enable)
                return true;
        }
        return false;
    }

    int Scene::acquireSpotLight()
    {
        for (int i = 0; i < MAX_SPOT_LIGHT_NUMBER; ++i)
        {
            if (m_spotLights[i] == false)
            {
                m_spotLights[i] = true;
                return i;
            }
        }
        // safe value
        return 0;
    }

    void Scene::releaseSpotLight(int idx)
    {
        m_spotLights[idx] = false;

        // Reset to default value
        m_environment->SetSpotLampColor(idx, { 0.5f, 0.5f, 0.5f });
        m_environment->SetSpotLampPosition(idx, { 0.0f, 100.0f, 0.0f });
        m_environment->SetSpotLampIntensity(idx, 0.0f);
        m_environment->SetSpotLampRange(idx, 100.0f);
        m_environment->SetSpotLampDirection(idx, {0.f, -1.f, 0.f});
        m_environment->SetSpotLampAngle(idx, 30.f);
    }

    //! Shadow texture size
    const Vec2& Scene::getShadowTextureSize() const
    {
        return m_shadowTextureSize;
    }

    void Scene::setShadowTextureSize(const Vec2& size)
    {
        if (m_shadowTextureSize != size)
        {
            m_shadowTextureSize = size;
            m_shadowFBO->Resize(size[0], size[1]);
        }
    }

    void Scene::setWindowSize(const Vec2& size)
    {
        m_windowSize[0] = size.X() > 0 ? size.X() : SystemInfo::Instance().GetGameW();
        m_windowSize[1] = size.Y() > 0 ? size.Y() : SystemInfo::Instance().GetGameH();
    }

    Vec2 Scene::screenToClient(const Vec2& pos)
    {
        auto wSize = getWindowSize();
        auto wPos = getWindowPosition();
        auto x = pos.X() - wPos.X() * 0.5f;
        auto y = pos.Y() + wPos.Y() * 0.5f;
        x = x + (SystemInfo::Instance().GetGameW() - (wSize.X() + wPos.X())) * 0.5f;
        y = y - (SystemInfo::Instance().GetGameH() - (wSize.Y() + wPos.Y())) * 0.5f;
        return Vec2(x, y);
    }

    //! Raycast
    std::pair<std::shared_ptr<SceneObject>, Vec3> Scene::raycast(const Vec2& screenPos, Camera* camera, float maxDistance, bool forceRaycast)
    {
        auto hit = std::pair<std::shared_ptr<SceneObject>, Vec3>(nullptr, Vec3());
        if(!camera)
            return hit;

        if (m_raycastCapture && !forceRaycast)
            return hit;

        auto wSize = getWindowSize();
        auto pos = screenToClient(screenPos);

        Mat4 proj;
        camera->GetProjectionMatrix(proj);

        Mat4 viewInv;
        camera->GetViewInverseMatrix(viewInv);
        
        float distance, minDistance = maxDistance;
        auto ray = RayOBBChecker::screenPosToWorldRay(pos.X(), pos.Y(), wSize.X(), wSize.Y(), viewInv, proj);
        for (const auto& obj : m_objects)
        {
            if (obj && RayOBBChecker::checkIntersect(obj->getAABB(), obj->getTransform()->getWorldMatrix(), distance, maxDistance))
            {
                if (minDistance > distance)
                {
                    minDistance = distance;
                    hit.first = obj;
                    hit.second = ray.first + ray.second * distance;
                }
            }
        }
        return hit;
    }

    std::pair<std::shared_ptr<SceneObject>, Vec3> Scene::raycast(const Vec3& position, Vec3& direction, float maxDistance, bool forceRaycast)
    {
        auto hit = std::pair<std::shared_ptr<SceneObject>, Vec3>(nullptr, Vec3());

        /*if (m_raycastCapture && !forceRaycast)
            return hit;*/

        float distance = 0, minDistance = maxDistance;
        std::pair<Vec3, Vec3> ray = RayOBBChecker::RayOBB(position, direction);
        for (const auto& obj : m_objects)
        {
            if (obj && RayOBBChecker::checkIntersect(obj->getAABB(), obj->getTransform()->getWorldMatrix(), distance, maxDistance))
            {
                if (minDistance > distance)
                {
                    minDistance = distance;
                    hit.first = obj;
                    hit.second = ray.first + ray.second * distance;
                }
            }
        }
        return hit;
    }

    std::pair<std::shared_ptr<SceneObject>, Vec3> Scene::raycastUI(const Vec2& screenPos)
    {
        auto hit = std::pair<std::shared_ptr<SceneObject>, Vec3>(nullptr, Vec3());
        if (!getCanvas() || !getCanvas()->getCamera())
            return hit;

        auto wSize = getWindowSize();
        auto pos = screenToClient(screenPos);

        Mat4 proj;
        getCanvas()->getCamera()->GetProjectionMatrix(proj);

        Mat4 viewInv;
        getCanvas()->getCamera()->GetViewInverseMatrix(viewInv);

        auto ray = RayOBBChecker::screenPosToWorldRay(pos.X(), pos.Y(), wSize.X(), wSize.Y(), viewInv, proj);
        float distance, minDistance = 100.0f;

        bool m_isEnd = false;
        std::shared_ptr<SceneObject> m_node = m_canvas->getOwner()->getSharedPtr();
        hit = findIntersectInHierachy(m_node, ray);
        if(hit.first != nullptr && hit.first->isInteractable()) m_raycastCapture = true;
        else if (hit.first == nullptr) {
            if (m_canvas) {
                hit.first = m_canvas->getOwner()->getSharedPtr();
                hit.second = raycastCanvas(screenPos);
            }
        }
        return hit;
    }

    Vec3 Scene::raycastCanvas(const Vec2& screenPos)
    {
        Vec3 hit;
        if (!getCanvas() || !getCanvas()->getCamera())
            return hit;
        auto wSize = getWindowSize();
        auto pos = screenToClient(screenPos);

        Mat4 proj;
        getCanvas()->getCamera()->GetProjectionMatrix(proj);

        Mat4 viewInv;
        getCanvas()->getCamera()->GetViewInverseMatrix(viewInv);

        auto ray = RayOBBChecker::screenPosToWorldRay(pos.X(), pos.Y(), wSize.X(), wSize.Y(), viewInv, proj);
        hit = ray.first;
        return hit;
    }

    std::pair<std::shared_ptr<SceneObject>, Vec3> Scene::findIntersectInHierachy(std::shared_ptr<SceneObject> target, std::pair<Vec3, Vec3> ray)
    {
        auto hit = std::pair<std::shared_ptr<SceneObject>, Vec3>(nullptr, Vec3());
        if (target == nullptr) return hit;
        const auto& transform = target->getTransform();
        float distance = 100.f;
        if (target->isRaycastTarget()) 
        {
            if (RayOBBChecker::checkIntersect(target->getAABB(), transform->getWorldMatrix(), distance, 100.f))
            {
                hit.first = target;
                hit.second = ray.first + ray.second * distance;
            }
        }
        if (target->getChildren().size() > 0) {
            const auto& children = target->getChildren();
            int cnt = children.size();
            for (int i = cnt-1; i >= 0; i--) {
                if (!children[i].expired()) {
                    auto temp_hit = findIntersectInHierachy(children[i].lock(), ray);
                    if (temp_hit.first != nullptr) {
                        hit = temp_hit;
                        break;
                    }
                }
            }
        }
        return hit;
    }

    std::shared_ptr<TweenManager> Scene::getTweenManager() const
    {
        return m_tweenManager;
    }

    bool Scene::isPrefab()
    {
        return m_objects.size() > 0 && !m_objects[0]->getPrefabId().empty();
    }

    std::string Scene::getPrefabId()
    {
        return isPrefab() ? m_objects[0]->getPrefabId() : std::string();
    }
   
    //! Serialize
    void Scene::to_json(json& j) const
    {
        j = json{
            {"name", m_name},
            {"path", m_path},
            {"objId", m_nextObjectID},
        };

        if (m_root)
        {
            json jRoot;
            m_root->to_json(jRoot);
            j["root"] = jRoot;
        }

        if (m_rootUI)
        {
            json jUI;
            m_rootUI->to_json(jUI);
            j["ui"] = jUI;
        }
    }

    //! Deserialize
    void Scene::from_json(const json& j)
    {
        clear();
        initialize(true);
        
        j.at("name").get_to(m_name);
        setPath(j.value("path", std::string()));

        if (j.contains("root"))
        {
            auto jRoot = j.at("root");
            m_root = createRootObject(m_name);
            m_root->from_json(jRoot);
        }

        if(j.contains("ui")) 
        {
            auto jUI = j.at("ui");
            m_rootUI = createRootObject("UI");
            m_rootUI->from_json(jUI);
        }

        if (j.contains("objId")) 
        {
            uint64_t nextId = m_nextObjectID;
            j.at("objId").get_to(nextId);
            if (nextId > m_nextObjectID)
                m_nextObjectID = nextId;
        }

        // Notify serialize finished
        getSerializeFinishedEvent().invoke(this);

        // Set active camera
        if (getActiveCamera() == nullptr)
        {
            auto camObj = findObjectByName("Default Camera");
            if (camObj)
            {
                auto cam = camObj->getComponent<CameraComponent>();
                if (cam)
                {
                    setActiveCamera(cam.get());
                }
            }
        }
        if (m_canvas == nullptr) {
            m_canvas = findObjectByName("Canvas")->getComponent<Canvas>();
        }
    }
}
