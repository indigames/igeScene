#include <algorithm>
#include <cstdlib>
#include <ctime>

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
        initialize();
    }

    Scene::~Scene()
    {
        clear();

        getResourceAddedEvent().removeAllListeners();
        getResourceRemovedEvent().removeAllListeners();

        getUIResourceAddedEvent().removeAllListeners();
        getUIResourceRemovedEvent().removeAllListeners();

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
            if(m_environment)
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

        if (m_canvasCamera) {
            m_canvasCamera->DecReference();
            m_canvasCamera = nullptr;
        }

        if (m_uiShowcase)
        {
            m_uiShowcase->Clear();
            m_uiShowcase->DecReference();
            m_uiShowcase = nullptr;
        }

        ResourceManager::Instance().DeleteDaemon();
    }

    bool Scene::initialize()
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
        m_shadowEdgeMask->SetMaterialParam(0, "DiffuseColor", diffuse, ParamTypeFloat4);

        float dt = 0;
        m_shadowEdgeMask->SetMaterialState(0, Key_depth_test_enable, &dt);
        m_shadowEdgeMask->AddMesh("mesh", "mate");
        m_shadowEdgeMask->SetMeshVertices(0, verts, 8);
        m_shadowEdgeMask->SetMeshIndices(0, 0, tris, 8, 4);

        Joint pose;
        m_shadowEdgeMask->AddJoint(-1, pose, false, "j01");
        m_shadowEdgeMask->SetMeshAlpha(0, 0.97f);

        m_shadowFBO = ResourceCreator::Instance().NewRenderTarget(m_shadowTexture, true, false);
        m_showcase->Add(m_environment);

        getResourceAddedEvent().addListener(std::bind(&Scene::onResourceAdded, this, std::placeholders::_1));
        getResourceRemovedEvent().addListener(std::bind(&Scene::onResourceRemoved, this, std::placeholders::_1));

        getUIResourceAddedEvent().addListener(std::bind(&Scene::onUIResourceAdded, this, std::placeholders::_1));
        getUIResourceRemovedEvent().addListener(std::bind(&Scene::onUIResourceRemoved, this, std::placeholders::_1));

        m_nextObjectID = 2;
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
            camObj->addComponent<FigureComponent>(SceneManager::getInstance()->getEditorPath() + "/figures/camera.pyxf")->setSkipSerialize(true);
            directionalLight->addComponent<SpriteComponent>(SceneManager::getInstance()->getEditorPath() + "/sprites/direct-light", Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
        }

        // Tween manager
        m_tweenManager = std::make_shared<TweenManager>();
        m_tweenManager->init();

        // Canvas camera
        m_canvasCamera = ResourceCreator::Instance().NewCamera("canvas_camera", nullptr);
        m_canvasCamera->SetPosition({ 0.f, 0.f, 10.f });
        m_canvasCamera->LockonTarget(false);
        m_canvasCamera->SetAspectRate(SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH());
        m_canvasCamera->SetOrthographicProjection(true);
        m_canvasCamera->SetWidthBase(false);

        return true;
    }

    void Scene::clear()
    {
        m_nextObjectID = 0;
        m_activeCamera = nullptr;

        for (auto& obj : m_objects)
        {
            obj->removeChildren();
            obj->setParent(nullptr);
        }

        m_root = nullptr;

        m_rootUI = nullptr;

        m_canvas = nullptr;

        // Destroy all objects
        for (auto& obj : m_objects)
        {
            obj = nullptr;
        }
        m_objects.clear();
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

    void Scene::resetFlag()
    {
        m_raycastCapture = false;
    }

    void Scene::render()
    {

        if (!SceneManager::getInstance()->isEditor() && m_activeCamera) {
            m_activeCamera->onRender();
        }

        m_showcase->Render();

        for (auto& obj : m_objects)
            if (obj) obj->onRender();
    }

    void Scene::renderUI() {
        if (!SceneManager::getInstance()->isEditor() && m_canvasCamera) {
            auto canvasObject = findObjectByName("Canvas");
            if (canvasObject) {
                auto canvas = canvasObject->getComponent<ige::scene::Canvas>();
                if (canvas) {
                    auto position = canvasObject->getTransform()->getWorldPosition();
                    m_canvasCamera->SetPosition({ position.X(), position.Y(), position.Z() + 10.0f});
                    m_canvasCamera->Render();
                }
            }
        }
        
        m_uiShowcase->Render();
    }

    std::shared_ptr<SceneObject> Scene::createObject(const std::string& name, const std::shared_ptr<SceneObject>& parent, bool isGUI, const Vec2& size, bool isCanvas)
    {
        auto parentObject = parent ? parent : m_root;
        if (isGUI) {
            parentObject = parentObject->isGUIObject() ? parentObject : m_canvas ? m_canvas : m_rootUI;
            if (parentObject == m_rootUI && !isCanvas) {
                auto canvasObject = std::make_shared<SceneObject>(this, m_nextObjectID++, "Canvas", parentObject.get(), true, size, true);
                auto canvas = canvasObject->addComponent<Canvas>();
                canvas->setDesignCanvasSize(Vec2(540.f, 960.f));
                Vec2 canvasSize(SystemInfo::Instance().GetGameW(), SystemInfo::Instance().GetGameW());
                canvas->setTargetCanvasSize(canvasSize);
                canvasObject->setCanvas(canvas);

        
                parentObject = canvasObject;
                m_objects.push_back(canvasObject);

                m_canvas = canvasObject;
            }
        }
        else if (parentObject) {
            if (parentObject->isGUIObject())
                isGUI = true;//parentObject = m_root;
        }

        auto sceneObject = std::make_shared<SceneObject>(this, m_nextObjectID++, name, parentObject.get(), isGUI, size, isCanvas);
        m_objects.push_back(sceneObject);
        return sceneObject;
    }

    std::shared_ptr<SceneObject> Scene::createRootObject(const std::string& name) {
        auto sceneObject = std::make_shared<SceneObject>(this, m_nextObjectID++, name);
        m_objects.push_back(sceneObject);
        return sceneObject;
    }

    bool Scene::removeObject(const std::shared_ptr<SceneObject>& obj)
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

        if (obj == m_canvas) {
            m_canvas = nullptr;
        }

        // Remove all children
        auto children = obj->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            auto elem = children[i];
            auto itr = std::find_if(m_objects.begin(), m_objects.end(), [&](auto el)
            {
                return el && elem && (el->getId() == elem->getId());
            });
            if (itr != m_objects.end())
                removeObject(*itr);
        }

        // Remove from objects list
        auto itr = std::find(m_objects.begin(), m_objects.end(), obj);
        if (itr != m_objects.end())
        {
            m_objects.erase(itr);
        }

        return true;
    }

    //! Remove scene object by its id
    bool Scene::removeObjectById(uint64_t id)
    {
        return removeObject(findObjectById(id));
    }

    std::shared_ptr<SceneObject> Scene::findObjectById(uint64_t id)
    {
        //! Interpolation Search   
        uint64_t cnt = m_objects.size();
        if (cnt > 0) {
            uint64_t l = 0;
            uint64_t r = cnt - 1;
            uint64_t o = -1;
            while (m_objects[r]->getId() != m_objects[l]->getId() 
                && id >= m_objects[l]->getId() && id <= m_objects[r]->getId())
            {
                uint64_t mid = l + (r - 1) * (id - m_objects[l]->getId()) / (m_objects[r]->getId() - m_objects[l]->getId());
                if (mid >= cnt || mid == o) {
                    break;
                }
                if (m_objects[mid]->getId() < id) {
                    l = mid + 1;
                }
                else if (m_objects[mid]->getId() > id) {
                    r = mid - 1;
                }
                else {
                    if (mid > 0 && m_objects[mid - 1]->getId() == id) {
                        r = mid - 1;
                        return m_objects[r];
                    }
                    else {
                        return m_objects[mid];
                    }
                }
                o = mid;
            }
            if (m_objects[l]->getId() == id)
                return m_objects[l];
        }
        return nullptr;
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
            json jObj;
            object->to_json(jObj);

            auto fsPath = path.empty() ? fs::path(object->getName()) : fs::path(path + "/" + object->getName());
            auto ext = fsPath.extension();
            if (ext.string() != ".prefab")
            {
                fsPath = fsPath.replace_extension(".prefab");
            }
            std::ofstream file(fsPath.string());
            file << jObj;
            return true;
        }
        return true;
    }

    bool Scene::loadPrefab(uint64_t parentId, const std::string& path)
    {
        if (path.empty())
            return false;

        auto fsPath = fs::path(path);
        if (fsPath.extension().string() != ".prefab")
            return false;

        std::ifstream file(fsPath);
        if (!file.is_open())
            return false;

        json jObj;
        file >> jObj;

        auto parent = findObjectById(parentId);
        auto obj = createObject(jObj.at("name"), parent, jObj.value("gui", false));
        obj->from_json(jObj);

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
        m_environment->SetDirectionalLampColor(idx, { 0.5f, 0.5f, 0.5f });
        m_environment->SetDirectionalLampDirection(idx, { 5.f, 10.f, 5.f });
        m_environment->SetDirectionalLampIntensity(idx, 0.f);
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

    //! Raycast
    std::pair<SceneObject*, Vec3> Scene::raycast(const Vec2& screenPos, Camera* camera, float maxDistance, bool forceRaycast)
    {
        std::pair<SceneObject*, Vec3> hit(nullptr, Vec3());
        if(!camera)
            return hit;

        if (m_raycastCapture && !forceRaycast)
            return hit;

        auto size = getWindowSize();
        float w = size.X() > 0 ? size.X() : SystemInfo::Instance().GetGameW();
        float h = size.Y() > 0 ? size.Y() : SystemInfo::Instance().GetGameH();

        auto pos = getWindowPosition();
        float dx = pos.X();
        float dy = pos.Y();

        float x = screenPos.X() - dx / 2.f;
        float y = screenPos.Y() + dy / 2.f;

        if(size.X() > 0)
            x = x + (SystemInfo::Instance().GetGameW() - (w + dx)) / 2.f;

        if(size.Y() > 0)
            y = y - (SystemInfo::Instance().GetGameH() - (h + dy)) / 2.f;

        Mat4 proj;
        camera->GetProjectionMatrix(proj);

        Mat4 viewInv;
        camera->GetViewInverseMatrix(viewInv);

        float distance, minDistance = maxDistance;
        auto ray = RayOBBChecker::screenPosToWorldRay(x, y, w, h, viewInv, proj);
        for (const auto& obj : m_objects)
        {
            const auto& transform = obj->getTransform();
            auto aabbTransform = Mat4::IdentityMat();
            vmath_mat4_from_rottrans(transform->getWorldRotation().P(), Vec3().P(), aabbTransform.P());
            vmath_mat_appendScale(aabbTransform.P(), transform->getWorldScale().P(), 4, 4, aabbTransform.P());
            auto& m_aabb = obj->getAABB();
            auto aabb = m_aabb.Transform(aabbTransform);

            if (obj && RayOBBChecker::checkIntersect(aabb, obj->getTransform()->getWorldMatrix(), distance, maxDistance))
            {
                if (minDistance > distance)
                {
                    minDistance = distance;
                    hit.first = obj.get();
                    hit.second = ray.first + ray.second * distance;
                }
            }
        }

        return hit;
    }

    std::pair<SceneObject*, Vec3> Scene::raycastUI(const Vec2& screenPos)
    {
        std::pair<SceneObject*, Vec3> hit(nullptr, Vec3());
        if (!m_canvasCamera)
            return hit;
        auto size = getWindowSize();
        float w = size.X() > 0 ? size.X() : SystemInfo::Instance().GetGameW();
        float h = size.Y() > 0 ? size.Y() : SystemInfo::Instance().GetGameH();

        auto pos = getWindowPosition();
        float dx = pos.X();
        float dy = pos.Y();

        float x = screenPos.X() - dx / 2.f;
        float y = screenPos.Y() + dy / 2.f;

        if (size.X() > 0)
            x = x + (SystemInfo::Instance().GetGameW() - (w + dx)) / 2.f;

        if (size.Y() > 0)
            y = y - (SystemInfo::Instance().GetGameH() - (h + dy)) / 2.f;

        Mat4 proj;
        m_canvasCamera->GetProjectionMatrix(proj);

        Mat4 viewInv;
        m_canvasCamera->GetViewInverseMatrix(viewInv);

        auto ray = RayOBBChecker::screenPosToWorldRay(x, y, w, h, viewInv, proj);
        float distance, minDistance = 100.0f;

        bool m_isEnd = false;
        std::shared_ptr<SceneObject> m_node = m_canvas;

        hit = findIntersectInHierachy(m_node.get(), ray);
        if(hit.first != nullptr && hit.first->isInteractable()) m_raycastCapture = true;
        else if (hit.first == nullptr) {
            if (m_canvas) {
                hit.first = m_canvas.get();
                hit.second = raycastCanvas(screenPos);
            }
        }
        return hit;
    }

    Vec3 Scene::raycastCanvas(const Vec2& screenPos)
    {
        Vec3 hit;
        if (!m_canvasCamera)
            return hit;
        auto size = getWindowSize();
        float w = size.X() > 0 ? size.X() : SystemInfo::Instance().GetGameW();
        float h = size.Y() > 0 ? size.Y() : SystemInfo::Instance().GetGameH();

        auto pos = getWindowPosition();
        float dx = pos.X();
        float dy = pos.Y();

        float x = screenPos.X() - dx / 2.f;
        float y = screenPos.Y() + dy / 2.f;

        if (size.X() > 0)
            x = x + (SystemInfo::Instance().GetGameW() - (w + dx)) / 2.f;

        if (size.Y() > 0)
            y = y - (SystemInfo::Instance().GetGameH() - (h + dy)) / 2.f;

        Mat4 proj;
        m_canvasCamera->GetProjectionMatrix(proj);

        Mat4 viewInv;
        m_canvasCamera->GetViewInverseMatrix(viewInv);

        auto ray = RayOBBChecker::screenPosToWorldRay(x, y, w, h, viewInv, proj);
        float distance, minDistance = 100.0f;

        bool m_isEnd = false;
        std::shared_ptr<SceneObject> m_node = m_canvas;
        auto worldPos = m_canvas->getTransform()->getWorldPosition();
        hit = ray.first;
        return hit;
    }

    std::pair<SceneObject*, Vec3> Scene::findIntersectInHierachy(const SceneObject* target, std::pair<Vec3, Vec3> ray)
    {
        std::pair<SceneObject*, Vec3> hit(nullptr, Vec3());
        if (target == nullptr) return hit;
        SceneObject* obj = const_cast<SceneObject*>(target);
        const auto& transform = obj->getTransform();
        auto aabbTransform = Mat4::IdentityMat();
        vmath_mat4_from_rottrans(transform->getWorldRotation().P(), Vec3().P(), aabbTransform.P());
        vmath_mat_appendScale(aabbTransform.P(), transform->getWorldScale().P(), 4, 4, aabbTransform.P());
        auto &m_aabb = obj->getAABB();
        auto aabb = m_aabb.Transform(aabbTransform);
        float distance = 100.f;
        if (target->isRaycastTarget()) 
        {
            if (RayOBBChecker::checkIntersect(aabb, transform->getWorldMatrix(), distance, 100.f))
            {
                hit.first = obj;
                hit.second = ray.first + ray.second * distance;
            }
        }
        if (target->getChildren().size() > 0) {
            const auto& children = target->getChildren();
            int cnt = children.size();
            for (int i = cnt-1; i >= 0; i--) {
                auto temp_hit = findIntersectInHierachy(children[i], ray);
                if (temp_hit.first != nullptr) {
                    hit = temp_hit;
                    break;
                }
            }
        }
        return hit;
    }

    std::shared_ptr<TweenManager> Scene::getTweenManager() const
    {
        return m_tweenManager;
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
        j.at("name").get_to(m_name);
        setPath(j.value("path", std::string()));

        auto jRoot = j.at("root");
        m_root = createRootObject(m_name);

        m_rootUI = createRootObject("UI");

        //m_nextObjectID = 2;
        m_root->from_json(jRoot);
        if(j.contains("ui")) {
            auto jUI = j.at("ui");
            m_rootUI->from_json(jUI);
        }

        if (j.contains("objId")) {
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
            m_canvas = findObjectByName("Canvas");
        }
    }
}
