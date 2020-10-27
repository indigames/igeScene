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
#include "components/AmbientLight.h"
#include "components/DirectionalLight.h"
#include "components/ScriptComponent.h"
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"
#include "components/gui/UIImage.h"

#include "utils/GraphicsHelper.h"

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
        m_environment = ResourceCreator::Instance().NewEnvironmentSet(name.c_str(), nullptr);
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

        m_shadowEdgeMask = ResourceCreator::Instance().NewEditableFigure("shadowEdgeMask");
        
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

        // Create root object
        m_root = createObject(m_name);

        // Set ambient color
        m_root->addComponent<AmbientLight>()->setSkyColor({ 0.75f, 0.75f, 0.75f });

        // Set general environment
        m_root->addComponent<EnvironmentComponent>();

        // Create default camera
        auto camObj = createObject("Default Camera");
        camObj->getTransform()->setPosition(Vec3(0.f, 0.f, 10.f));
        auto camComp = camObj->addComponent<CameraComponent>("default_camera");
        camComp->lockOnTarget(false);
        camComp->setAspectRatio(SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH());

        // Create default directional light
        auto directionalLight = createObject("Directional Light");
        directionalLight->addComponent<DirectionalLight>();
        directionalLight->getTransform()->setPosition({0.f, 5.f, 0.f});
        directionalLight->getTransform()->setRotation({ DEGREES_TO_RADIANS(90.f), 0.f, .0f });

        // Add editor debug
        if (SceneManager::getInstance()->isEditor())
        {
            camObj->addComponent<FigureComponent>("figure/camera.pyxf")->setSkipSerialize(true);
            directionalLight->addComponent<SpriteComponent>("sprite/sun", Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
        }

        // Fill up test data
        // populateTestData(sceneObject, 1000);
    }

    Scene::~Scene()
    {
        clear();

        getResourceAddedEvent().removeAllListeners();
        getResourceRemovedEvent().removeAllListeners();

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

        ResourceManager::Instance().DeleteDaemon();
    }

    bool Scene::initialize()
    {
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

            if (!SceneManager::getInstance()->isEditor() && m_activeCamera)
                m_activeCamera->onRender();

            m_showcase->Render(RenderPassFilter::ShadowPass);

            renderContext->BeginPass(TransparentPass);
            m_shadowEdgeMask->Pose();
            m_shadowEdgeMask->Render();

            renderContext->EndScene();
        }
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

    void Scene::render()
    {
        for (auto& obj : m_objects)
            if (obj) obj->onRender();

        if (!SceneManager::getInstance()->isEditor() && m_activeCamera)
            m_activeCamera->onRender();

        m_showcase->Render();
    }

    std::shared_ptr<SceneObject> Scene::createObject(std::string name, const std::shared_ptr<SceneObject>& parent, bool isGUI, const Vec2& size, bool isCanvas)
    {
        auto parentObject = parent ? parent : m_root;

        // Not canvas, create defaut canvas
        if (isGUI && !parentObject->getCanvas() && !isCanvas)
        {
            auto canvasObject = std::make_shared<SceneObject>(this, m_nextObjectID++, "Canvas", parentObject.get(), true, size, true);
            auto canvas = canvasObject->addComponent<Canvas>();
            canvas->setDesignCanvasSize(Vec2(540.f, 960.f));
            canvas->setTargetCanvasSize(Vec2(SystemInfo::Instance().GetGameW(), SystemInfo::Instance().GetGameW()));
            canvasObject->setCanvas(canvas);

            if (SceneManager::getInstance()->isEditor())
                canvasObject->addComponent<UIImage>("sprite/rect")->setSkipSerialize(true);

            parentObject = canvasObject;
            m_objects.push_back(canvasObject);
        }

        auto sceneObject = std::make_shared<SceneObject>(this, m_nextObjectID++, name, parentObject.get(), isGUI, size, isCanvas);
        m_objects.push_back(sceneObject);
        return sceneObject;
    }

    void Scene::populateTestData(const std::shared_ptr<SceneObject>& parent, int numObjects)
    {
        //! [IGE]: To test performance with huge amount of objects
        std::srand(std::time(nullptr));

        for (int i = 0; i < numObjects; ++i)
        {
            auto newObj = createObject("Object_" + std::to_string(i), parent);
            if((std::rand() / (float)RAND_MAX) < 0.5f)
                if ((std::rand() / (float)RAND_MAX) < 0.5f)
                    newObj->getTransform()->setPosition(Vec3(-std::rand() % 50, 0.f, -std::rand() % 50));
                else
                    newObj->getTransform()->setPosition(Vec3(-std::rand() % 50, 0.f, std::rand() % 50));
            else
                if ((std::rand() / (float)RAND_MAX) < 0.5f)
                    newObj->getTransform()->setPosition(Vec3(std::rand() % 50, 0.f, -std::rand() % 50));
                else
                    newObj->getTransform()->setPosition(Vec3(std::rand() % 50, 0.f, std::rand() % 50));
            newObj->addComponent<FigureComponent>("figure/Tree/Tree.pyxf");
        }
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
    }

    //! Remove scene object by its id
    bool Scene::removeObjectById(uint64_t id)
    {
        return removeObject(findObjectById(id));
    }

    std::shared_ptr<SceneObject> Scene::findObjectById(uint64_t id)
    {
        auto found = std::find_if(m_objects.begin(), m_objects.end(), [&](auto elem)
        {
            return (elem->getId() == id);
        });
        if (found != m_objects.end())
            return std::dynamic_pointer_cast<SceneObject>(*found);
        return nullptr;
    }

    std::shared_ptr<SceneObject> Scene::findObjectByName(std::string name)
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
        if (resource)
            m_showcase->Add(resource);
    }

    //! Resource removed event
    void Scene::onResourceRemoved(Resource* resource)
    {
        if (resource)
            m_showcase->Remove(resource);
    }

    //! Serialize
    void Scene::to_json(json& j) const
    {
        j = json {
            {"name", m_name},
            {"objId", m_nextObjectID},
        };
        json jRoot;
        m_root->to_json(jRoot);
        j["root"] = jRoot;
    }

    //! Deserialize
    void Scene::from_json(const json& j)
    {
        clear();
        j.at("name").get_to(m_name);

        auto jRoot = j.at("root");
        m_root = createObject(m_name);
        m_root->from_json(jRoot);
       
        j.at("objId").get_to(m_nextObjectID);
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
            return nullptr;

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
}
