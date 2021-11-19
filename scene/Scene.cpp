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
        setWindowSize({-1.f, -1.f});
        setViewSize({ -1.f, -1.f });
        m_uuid = SceneObject::generateUUID();
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
            camObj->getTransform()->setLocalPosition(Vec3(0.f, 0.f, 10.f));
            auto camComp = camObj->addComponent<CameraComponent>("default_camera");
            camComp->lockOnTarget(false);
            camComp->setAspectRatio(SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH());

            // Create default directional light
            auto directionalLight = createObject("Directional Light");
            directionalLight->addComponent<DirectionalLight>();
            directionalLight->getTransform()->setLocalPosition({ 0.f, 5.f, 0.f });
            directionalLight->getTransform()->setLocalRotation({ DEGREES_TO_RADIANS(90.f), 0.f, .0f });

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
        getResourceAddedEvent().removeAllListeners();
        getResourceRemovedEvent().removeAllListeners();
        getUIResourceAddedEvent().removeAllListeners();
        getUIResourceRemovedEvent().removeAllListeners();
        getSerializeFinishedEvent().removeAllListeners();

        m_nextObjectID = 1;
        m_activeCamera.reset();
        m_root = nullptr;
        m_rootUI = nullptr;
        m_canvas = nullptr;

        removeAllObjects();

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
        for(int i = m_objects.size() - 1; i >= 0; i--)
            m_objects[i]->onUpdate(dt);

        if(m_tweenManager) m_tweenManager->update(dt);

        resetFlag();

    // Runtime pre-render
    #ifndef EDITOR_MODE
        preRender();
    #endif
    }

    void Scene::fixedUpdate(float dt)
    {
        for (int i = m_objects.size() - 1; i >= 0; i--)
            m_objects[i]->onFixedUpdate(dt);
    }

    void Scene::lateUpdate(float dt)
    {
        for (int i = m_objects.size() - 1; i >= 0; i--)
            m_objects[i]->onLateUpdate(dt);
    }

    void Scene::physicUpdate(float dt)
    {
        for (int i = m_objects.size() - 1; i >= 0; i--)
            m_objects[i]->onPhysicUpdate(dt);
    }

    void Scene::resetFlag()
    {
        m_raycastCapture = false;
    }

    void Scene::preRender(Camera* camera)
    {
        float dt = Time::Instance().GetElapsedTime();
        if (camera) {
            camera->Step(dt);
        }
        else if (!SceneManager::getInstance()->isEditor() && !m_activeCamera.expired()) {
            m_activeCamera.lock()->onUpdate(dt);
        } 
        m_showcase->Update(dt);

        if (camera) {
            m_showcase->ZSort(camera);
            camera->Render();
        }
        else if (!SceneManager::getInstance()->isEditor() && !m_activeCamera.expired()) {
            m_showcase->ZSort(m_activeCamera.lock()->getCamera());
            m_activeCamera.lock()->onRender();
        }

        // Render shadow before actually render objects
        auto renderContext = RenderContext::InstancePtr();
        if (renderContext) {
            renderContext->BeginScene(m_shadowFBO, Vec4(1.f, 1.f, 1.f, 1.f), true, true);
            m_showcase->Update(0.f);
            if (camera) {
                camera->Render();
            }
            else if (!SceneManager::getInstance()->isEditor() && !m_activeCamera.expired()) {
                m_activeCamera.lock()->onRender();
            }
            m_showcase->Render(RenderPassFilter::ShadowPass);

            renderContext->BeginPass(TransparentPass);
            m_shadowEdgeMask->Pose();
            m_shadowEdgeMask->Render();
            renderContext->EndScene();
        }
    }

    void Scene::render()
    {
        m_showcase->Render();

        for (int i = m_objects.size() - 1; i >= 0; i--) {
            m_objects[i]->onRender();
        }
    }

    void Scene::renderUI() {
        if (SceneManager::getInstance()->isPlaying()) {
            auto renderContext = RenderContext::InstancePtr();
            auto rtt = renderContext->GetCurrentRenderTarget();
            renderContext->EndScene();
            renderContext->ResetRenderStateAll();

            float dt = Time::Instance().GetElapsedTime();
            m_uiShowcase->Update(dt);

            if (getCanvas() != nullptr && getCanvas()->getCamera() != nullptr) {
                getCanvas()->getCamera()->Step(dt);
                m_uiShowcase->ZSort(getCanvas()->getCamera());
                getCanvas()->getCamera()->Render();
            }

            renderContext->BeginScene(rtt, Vec4(1.f, 1.f, 1.f, 1.f), false, false);
        }
        m_uiShowcase->Render();

        for (int i = m_objects.size() - 1; i >= 0; i--) {
            m_objects[i]->onRenderUI();
        }
    }

    std::shared_ptr<SceneObject> Scene::createObject(const std::string& name, const std::shared_ptr<SceneObject>& parent, bool isGUI, const Vec2& size, const std::string& prefabId,
        const Vec3& position, const Quat& rotation, const Vec3& scale)
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
        auto sceneObject = std::make_shared<SceneObject>(this, m_nextObjectID++, name, isGUI, size, prefabId, position, rotation, scale);
        m_objects.push_back(sceneObject);
        sceneObject->setParent(parentObject);
        if (m_root == nullptr) m_root = sceneObject;
        return sceneObject;
    }

    std::shared_ptr<SceneObject> Scene::createObjectFromPrefab(const std::string& path, const std::string& name, const std::shared_ptr<SceneObject>& parent)
    {
        auto object = loadPrefab(parent ? parent->getId() : -1, path);
        if (object) object->setName(name);
        return object;
    }

    std::shared_ptr<SceneObject> Scene::createObjectFromPrefab(const std::string& path, const std::string& name, const std::shared_ptr<SceneObject>& parent, const Vec3& position)
    {
        auto object = loadPrefab(-1, path, position);
        if (object) object->setName(name);
        return object;
    }

    std::shared_ptr<SceneObject> Scene::createObjectFromPrefab(const std::string& path, const std::string& name, const std::shared_ptr<SceneObject>& parent, const Vec3& position, const Quat& rotation)
    {
        auto object = loadPrefab(parent ? parent->getId() : -1, path, position, rotation);
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
        for (auto& obj : m_objects) {
            if (obj) obj->setParent(nullptr);
            obj = nullptr;
        }
        m_objects.clear();
        return true;
    }

    bool Scene::removeObject(std::shared_ptr<SceneObject>& obj)
    {
        if (!obj) return false;
        obj->setParent(nullptr);

        auto children = obj->getChildren();
        for (auto child : children) {
            if (!child.expired()) {
                auto obj = child.lock();
                removeObject(obj);
            }
        }
        obj->removeChildren();

        // Remove active camera
        if (auto camera = obj->getComponent<CameraComponent>()) {
            auto activeCam = getActiveCamera();
            if (activeCam && activeCam->getInstanceId() == camera->getInstanceId())
                setActiveCamera(nullptr);
        }

        // Remove root object
        if (obj == m_root) {
            m_root = nullptr;
        }

        if (obj == m_rootUI) {
            m_rootUI = nullptr;
        }

        // Remove from objects list
        auto itr = std::find(m_objects.begin(), m_objects.end(), obj);
        if (itr != m_objects.end()) {
            m_objects.erase(itr);
        }
        obj = nullptr;
        return true;
    }

    //! Remove scene object by its id
    bool Scene::removeObjectById(uint64_t id)
    {
        auto obj = findObjectById(id);
        return removeObject(obj);
    }

    std::shared_ptr<SceneObject> Scene::findObjectById(uint64_t id)
    {
        // Perform binary search for best performance, 'cause m_objects sorted by id.
        if (m_objects.size() == 0) return nullptr;
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
    void Scene::setActiveCamera(std::shared_ptr<CameraComponent> camera)
    {
        if (camera) {
            if (!camera->getOwner()->isActive())
                camera->getOwner()->setActive(true);
            m_activeCamera = camera;
        }
        else {
            m_activeCamera.reset();
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
        auto obj = createObject(jObj.at("name"), nullptr, jObj.value("gui", false), jObj.value("size", Vec2{ 64.f, 64.f }), prefabId);
        obj->from_json(jObj);
        if (parent) obj->setParent(parent);
        return obj;
    }

    std::shared_ptr<SceneObject> Scene::loadPrefab(uint64_t parentId, const std::string& path, 
        const Vec3& pos)
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
        auto obj = createObject(jObj.at("name"), nullptr, jObj.value("gui", false), jObj.value("size", Vec2{ 64.f, 64.f }), prefabId,
            pos);
        auto transform = obj->getTransform();
        if (transform != nullptr) {
            transform->lockMove(true);
        }
        obj->from_json(jObj);
        if (transform != nullptr && obj->getComponent<Canvas>() == nullptr) {
            transform->lockMove(false);
        }
        if(parent) obj->setParent(parent);
        return obj;
    }

    std::shared_ptr<SceneObject> Scene::loadPrefab(uint64_t parentId, const std::string& path,
        const Vec3& pos, const Quat& rot)
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
        auto obj = createObject(jObj.at("name"), nullptr, jObj.value("gui", false), jObj.value("size", Vec2{ 64.f, 64.f }), prefabId,
            pos, rot);
        auto transform = obj->getTransform();
        if (transform != nullptr) {
            transform->lockMove(true);
            transform->lockRotate(true);
        }
        obj->from_json(jObj);
        if (transform != nullptr && obj->getComponent<Canvas>() == nullptr) {
            transform->lockMove(false);
            transform->lockRotate(false);
        }
        if (parent) obj->setParent(parent);
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

    void Scene::setViewSize(const Vec2& size)
    {
        m_viewSize[0] = size.X() > 0 ? size.X() : SystemInfo::Instance().GetGameW();
        m_viewSize[1] = size.Y() > 0 ? size.Y() : SystemInfo::Instance().GetGameH();
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

    Vec3 Scene::screenToWorldPoint(const Vec2& screenPos, Camera* camera, float nearPlane)
    {
        auto wSize = getWindowSize();
        if (wSize[0] == 0 || wSize[1] == 0 || camera == nullptr) return Vec3(0, 0, 0);
        if (nearPlane < 0) nearPlane = 0;

        auto pos = getScreenPos(screenPos);
        auto camPos = camera->GetPosition();
        Mat4 proj;
        camera->GetProjectionMatrix(proj);

        Mat4 viewInv;
        camera->GetViewInverseMatrix(viewInv);
        
        Vec4 lStart_NDC(
            pos[0] / wSize[0] * 2.0f,
            pos[1] / wSize[1] * 2.0f,
            -1.0f,
            1.0f
        );

        Vec4 lEnd_NDC(
            pos[0] / wSize[0] * 2.0f,
            pos[1] / wSize[1] * 2.0f,
            0.f,
            1.0f
        );

        Vec4 lZEnd_NDC(
            0.f,
            0.f,
            0.f,
            1.0f
        );

        // The Projection matrix goes from Camera Space to NDC.
        Mat4 projectionInversedMatrix = proj.Inverse();

        Vec4 lStart_camera = projectionInversedMatrix * lStart_NDC; lStart_camera /= lStart_camera.W();
        Vec4 lStart_world = viewInv * lStart_camera; lStart_world /= lStart_world.W();

        Vec4 lEnd_camera = projectionInversedMatrix * lEnd_NDC; lEnd_camera /= lEnd_camera.W();
        Vec4 lEnd_world = viewInv * lEnd_camera; lEnd_world /= lEnd_world.W();

        Vec4 lZEnd_camera = projectionInversedMatrix * lZEnd_NDC; lZEnd_camera /= lZEnd_camera.W();
        Vec4 lZEnd_world = viewInv * lZEnd_camera; lZEnd_world /= lZEnd_world.W();
        
        Vec3 p_z(lZEnd_world.X(), lZEnd_world.Y(), lZEnd_world.Z());
        auto lZDir_world = p_z - camPos;
        Vec3 g_z(lZDir_world.X(), lZDir_world.Y(), lZDir_world.Z());
        auto zlen = g_z.Length();

        float per = 0.0;
        if (zlen > 0)
            per = nearPlane * 2 / zlen;

        auto lDir_world = lEnd_world - lStart_world;
        Vec3 g_direction = Vec3(lDir_world.X(), lDir_world.Y(), lDir_world.Z());
        auto len = g_direction.Length();
        g_direction.Normalize();

        auto outPos = camPos + g_direction * len * per;

        return outPos;
    }

    Vec2 Scene::worldToScreenPoint(const Vec3& pos, Camera* camera)
    {
        auto wSize = getWindowSize();

        if (wSize[0] == 0 || wSize[1] == 0 || camera == nullptr) return Vec2(0, 0);

        Mat4 proj;
        camera->GetProjectionMatrix(proj);

        Mat4 viewInv;
        camera->GetViewInverseMatrix(viewInv);

        Mat4 worldMatrix = viewInv.Inverse();

        Mat4 worldToClip = proj * worldMatrix;

        Vec3 clip;
        float w;
        clip[0] = worldToClip[0][0] * pos[0] + worldToClip[0][1] * pos[1] + worldToClip[0][2] * pos[2] + worldToClip[0][3];
        clip[1] = worldToClip[1][0] * pos[0] + worldToClip[1][1] * pos[1] + worldToClip[1][2] * pos[2] + worldToClip[1][3];
        clip[2] = worldToClip[2][0] * pos[0] + worldToClip[2][1] * pos[1] + worldToClip[2][2] * pos[2] + worldToClip[2][3];
        w = worldToClip[3][0] * pos[0] + worldToClip[3][1] * pos[1] + worldToClip[3][2] * pos[2] + worldToClip[3][3];

        w = 1.0f / w;
        clip[0] *= w;
        clip[1] *= w;
        clip[2] *= w;

        Vec2 windowSpacePos =  Vec2(
            (0.0f - clip[0]) * wSize[0] * 1.75f,
            (0.0f - clip[1]) * wSize[1] * 1.75f);

        windowSpacePos[0] = std::roundf(windowSpacePos[0]);
        windowSpacePos[1] = std::roundf(windowSpacePos[1]);

        /*vec<4, T, Q> tmp = vec<4, T, Q>(obj, static_cast<T>(1));
        tmp = model * tmp;
        tmp = proj * tmp;

        tmp /= tmp.w;
        tmp = tmp * static_cast<T>(0.5) + static_cast<T>(0.5);
        tmp[0] = tmp[0] * T(viewport[2]) + T(viewport[0]);
        tmp[1] = tmp[1] * T(viewport[3]) + T(viewport[1]);*/

        return windowSpacePos;
    }

    Vec2 Scene::getScreenPos(const Vec2& pos)
    {
        auto wSize = getWindowSize();
        auto vSize = getViewSize();

        auto wPos = getWindowPosition();
        auto vPos = getViewPosition();

        auto x = pos.X() - wPos.X() * 0.5f;
        auto y = pos.Y() + wPos.Y() * 0.5f;

        x = x + (SystemInfo::Instance().GetGameW() - (vSize.X() + wPos.X())) * 0.5f - vPos[0];
        y = y - (SystemInfo::Instance().GetGameH() - (vSize.Y() + wPos.Y())) * 0.5f - vPos[1];

        return Vec2(x, y);
    }

    std::shared_ptr<Scene> Scene::getSharedPtr()
    {
        return SceneManager::getInstance()->getScene(getUUID());
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
            const auto& transform = obj->getTransform();
            auto tranMat = Mat4::IdentityMat();
            vmath_mat4_translation(transform->getPosition().P(), tranMat.P());

            auto rotMat = Mat4::IdentityMat();
            vmath_mat_from_quat(transform->getRotation().P(), 4, rotMat.P());

            auto modelMat = tranMat * rotMat;
            vmath_mat_appendScale(modelMat.P(), Vec3(1.f / transform->getScale().X(), 1.f / transform->getScale().Y(), 1.f / transform->getScale().Z()).P(), 4, 4, modelMat.P());

            if (RayOBBChecker::checkIntersect(obj->getAABB(), modelMat, distance, maxDistance))
            {
                if (minDistance > distance && distance > 0.f)
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

        float distance = 0, minDistance = maxDistance;
        std::pair<Vec3, Vec3> ray = RayOBBChecker::RayOBB(position, direction);
        for (const auto& obj : m_objects)
        {
            const auto& transform = obj->getTransform();
            auto tranMat = Mat4::IdentityMat();
            vmath_mat4_translation(transform->getPosition().P(), tranMat.P());

            auto rotMat = Mat4::IdentityMat();
            vmath_mat_from_quat(transform->getRotation().P(), 4, rotMat.P());

            auto modelMat = tranMat * rotMat;
            vmath_mat_appendScale(modelMat.P(), Vec3(1.f / transform->getScale().X(), 1.f / transform->getScale().Y(), 1.f / transform->getScale().Z()).P(), 4, 4, modelMat.P());

            if (RayOBBChecker::checkIntersect(obj->getAABB(), modelMat, distance, maxDistance))
            {
                if (minDistance > distance && distance > 0.f)
                {
                    minDistance = distance;
                    hit.first = obj;
                    hit.second = ray.first + ray.second * distance;
                }
            }
        }
        return hit;
    }

    std::vector<std::pair<std::shared_ptr<SceneObject>, Vec3>> Scene::raycastAll(const Vec3& position, Vec3& direction, float maxDistance, bool forceRaycast)
    {
        std::vector<std::pair<std::shared_ptr<SceneObject>, Vec3>> hits;
        std::pair<Vec3, Vec3> ray = RayOBBChecker::RayOBB(position, direction);
        for (const auto& obj : m_objects)
        {
            const auto& transform = obj->getTransform();
            auto tranMat = Mat4::IdentityMat();
            vmath_mat4_translation(transform->getPosition().P(), tranMat.P());

            auto rotMat = Mat4::IdentityMat();
            vmath_mat_from_quat(transform->getRotation().P(), 4, rotMat.P());

            auto modelMat = tranMat * rotMat;
            vmath_mat_appendScale(modelMat.P(), Vec3(1.f / transform->getScale().X(), 1.f / transform->getScale().Y(), 1.f / transform->getScale().Z()).P(), 4, 4, modelMat.P());
            
            float distance = 0;
            if (RayOBBChecker::checkIntersect(obj->getAABB(), modelMat, distance, maxDistance))
            {
                if (maxDistance > distance && distance > 0.f)
                {
                    auto hit = std::pair<std::shared_ptr<SceneObject>, Vec3>(nullptr, Vec3());
                    hit.first = obj;
                    hit.second = ray.first + ray.second * distance;
                    hits.push_back(hit);
                }
            }
        }
        return hits;
    }

    std::pair<std::shared_ptr<SceneObject>, Vec3> Scene::raycastUI(const Vec2& screenPos)
    {
        auto hit = std::pair<std::shared_ptr<SceneObject>, Vec3>(nullptr, Vec3());
        if (!getCanvas() || !getCanvas()->getCamera())
            return hit;

        auto wSize = getWindowSize();
        auto pos = screenToClient(screenPos);

        Mat4 proj;
        auto camera = getCanvas()->getCamera();
        camera->GetProjectionMatrix(proj);

        Mat4 viewInv;
        camera->GetViewInverseMatrix(viewInv);

        auto ray = RayOBBChecker::screenPosToWorldRay(pos.X(), pos.Y(), wSize.X(), wSize.Y(), viewInv, proj);
        float distance, minDistance = 100.0f;

        bool m_isEnd = false;
        std::shared_ptr<SceneObject> m_node = m_canvas->getOwner()->getSharedPtr();
        hit = findIntersectInHierachy(m_node, ray);
        if(hit.first != nullptr && hit.first->isInteractable() && hit.first->isActive()) m_raycastCapture = true;
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
        if (getCanvas() == nullptr || getCanvas()->getCamera() == nullptr)
            return hit;
        auto wSize = getWindowSize();
        auto pos = screenToClient(screenPos);
        auto camera = getCanvas()->getCamera();

        Mat4 proj;
        camera->GetProjectionMatrix(proj);

        Mat4 viewInv;
        camera->GetViewInverseMatrix(viewInv);

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
        if (target->isActive()) {
            if (target->isRaycastTarget())
            {
                const auto& transform = target->getTransform();
                auto tranMat = Mat4::IdentityMat();
                vmath_mat4_translation(transform->getPosition().P(), tranMat.P());

                auto rotMat = Mat4::IdentityMat();
                vmath_mat_from_quat(transform->getRotation().P(), 4, rotMat.P());

                auto modelMat = tranMat * rotMat;
                vmath_mat_appendScale(modelMat.P(), Vec3(1.f / transform->getScale().X(), 1.f / transform->getScale().Y(), 1.f / transform->getScale().Z()).P(), 4, 4, modelMat.P());

                if (RayOBBChecker::checkIntersect(target->getAABB(), modelMat, distance, 100.f))
                {
                    hit.first = target;
                    hit.second = ray.first + ray.second * distance;
                }
            }
            if (target->getChildren().size() > 0) {
                const auto& children = target->getChildren();
                int cnt = children.size();
                for (int i = cnt - 1; i >= 0; i--) {
                    if (!children[i].expired()) {
                        auto temp_hit = findIntersectInHierachy(children[i].lock(), ray);
                        if (temp_hit.first != nullptr) {
                            hit = temp_hit;
                            break;
                        }
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
            {"uuid", m_uuid},
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
        m_uuid = j.value("uuid", SceneObject::generateUUID());
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
                if (cam) {
                    setActiveCamera(cam);
                }
            }
        }

        if (m_canvas == nullptr) {
            auto canvas = findObjectByName("Canvas");
            if (canvas != nullptr) {
                m_canvas = canvas->getComponent<Canvas>();
            }
        }
    }
}
