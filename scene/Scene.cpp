#include <algorithm>

#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "components/Component.h"
#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/EnvironmentComponent.h"

#define SERIALIZE_VERSION "0.0.1"

namespace ige::scene
{
    Scene::Scene(const std::string& name)
        : m_name(name)
    {
        m_showcase = ResourceCreator::Instance().NewShowcase("scene_showcase");
        m_showcase->Initialize();
        m_showcase->WaitInitialize();
        initialize();
    }

    Scene::~Scene()
    {
        m_root = nullptr;
        m_editorCamera = nullptr;
        m_editorEnvironment = nullptr;
        m_showcase = nullptr;
    }
    
    bool Scene::initialize()
    {
        m_root = createObject("root");
        m_root->addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f));

        m_editorCamera = createObject("editor_camera", m_root);
        m_editorCamera->addComponent<TransformComponent>(Vec3(0.f, 0.f, 10.f));
        auto cam = m_editorCamera->addComponent<CameraComponent>("camera");
        cam.lockonTarget(false);
        cam.setOrthoProjection(true);
        cam.setPosition(Vec3(0.f, 0.f, 10.f));        

        m_editorEnvironment = createObject("editor_environment");
        m_editorEnvironment->addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f));
        auto env = m_editorEnvironment->addComponent<EnvironmentComponent>("environment");
        env.setAmbientGroundColor(Vec3(0.5f, 0.5f, 0.5f));
        env.setDirectionalLightColor(0, Vec3(0.5f, 0.5f, 0.5f));

        auto grid = createObject("grid", m_root);
        grid->addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f), Quat(), Vec3(10.f, 10.f, 10.f));
        auto fig = grid->addComponent<FigureComponent>("Trees_Object");
        fig.getFigure()->SetPosition({ 0.f, 0.f, 0.f });
        fig.getFigure()->SetScale({ 10.f, 10.f, 10.f });

        return true;
    }

    void Scene::update(float dt)
    {        
        m_root->onUpdate(dt);

        m_showcase->Update(dt);
        m_editorCamera->getComponent<CameraComponent>()->getCamera()->Render();
        m_root->onRender();
        m_showcase->Render();
    }

    void Scene::fixedUpdate(float dt)
    {
        m_root->onFixedUpdate(dt);
    }

    void Scene::lateUpdate(float dt)
    {
         m_root->onLateUpdate(dt);
    }

    std::shared_ptr<SceneObject> Scene::createObject(std::string name, std::shared_ptr<SceneObject> parent)
    {
        auto sceneObject = std::make_shared<SceneObject>(m_nextObjectID++, name, parent);
        sceneObject->getComponentAddedEvent().addListener(std::bind(&Scene::onComponentAdded, this, std::placeholders::_1));
        sceneObject->getComponentRemovedEvent().addListener(std::bind(&Scene::onComponentRemoved, this, std::placeholders::_1));
        if(parent != nullptr) parent->addChild(sceneObject);
        return sceneObject;
    }

    bool Scene::removeObject(std::shared_ptr<SceneObject> obj)
    {
        if(!obj) return false;

        if(!m_root || m_root == obj) 
        {
            m_root = nullptr;
            return true;
        }
        
        return m_root->removeChild(obj);        
    }
    
    std::shared_ptr<SceneObject> Scene::findObjectById(uint64_t id) const
    {
        return m_root->findObjectById(id);
    }

    std::shared_ptr<SceneObject> Scene::findObjectByName(std::string name) const
    {
        return m_root->findObjectByName(name);
    }

    //! Component added event
    void Scene::onComponentAdded(std::shared_ptr<Component> component)
    {
        if (component->getName() == "FigureComponent")
        {
            auto figure = ((FigureComponent*)(component.get()))->getFigure();
            m_showcase->Add(figure);
        }
        else if (component->getName() == "EnvironmentComponent")
        {
            auto environment = ((EnvironmentComponent*)(component.get()))->getEnvironment();
            m_showcase->Add(environment);
        }
    }

    //! Component added event
    void Scene::onComponentRemoved(std::shared_ptr<Component> component)
    {
        if (component->getName() == "FigureComponent")
        {
            auto figure = ((FigureComponent*)(component.get()))->getFigure();
            m_showcase->Remove(figure);
        }
        else if (component->getName() == "EnvironmentComponent")
        {
            auto environment = ((EnvironmentComponent*)(component.get()))->getEnvironment();
            m_showcase->Remove(environment);
        }
    }

    bool Scene::loadScene(const std::string& path)
    {
        // TODO: implement this
        
        return false;
    }

    bool Scene::saveScene(const std::string& path)
    {
        // TODO: implement this
        return false;
    }
}
