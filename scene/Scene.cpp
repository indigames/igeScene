#include <algorithm>

#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "components/Component.h"
#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/EditableFigureComponent.h"
#include "components/EnvironmentComponent.h"

#include "utils/GraphicsHelper.h"

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
        m_showcase = nullptr;
    }
    
    bool Scene::initialize()
    {
        m_root = createObject("root");
        m_root->addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f));

        auto envComp = m_root->addComponent<EnvironmentComponent>("environment");
        envComp.setAmbientGroundColor(Vec3(0.5f, 0.5f, 0.5f));
        envComp.setDirectionalLightColor(0, Vec3(0.5f, 0.5f, 0.5f));

        auto camera = createObject("camera", m_root);
        camera->addComponent<TransformComponent>(Vec3(0.f, 5.f, 10.f));
        auto cameraComp = camera->addComponent<CameraComponent>("camera");
        cameraComp.lockonTarget(false);

        auto grid = createObject("grid", m_root);
        grid->addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f), Quat::RotationX(PI / 2.f));
        grid->addComponent<EditableFigureComponent>("grid", GraphicsHelper::getInstance()->createGridMesh({ 10000, 10000 }, "grid"));

        auto tree = createObject("tree", m_root);
        tree->addComponent<TransformComponent>(Vec3(-5.f, 0.f, 0.f));
        tree->addComponent<FigureComponent>("Trees_Object");

        auto tree2 = createObject("tree2", tree);
        tree2->addComponent<TransformComponent>(Vec3(2.f, 0.f, 0.f));
        tree2->addComponent<FigureComponent>("Trees_Object");

        auto tree3 = createObject("tree3", tree2);
        tree3->addComponent<TransformComponent>(Vec3(2.f, 0.f, 0.f));
        tree3->addComponent<FigureComponent>("Trees_Object");

        return true;
    }

    void Scene::update(float dt)
    {        
        m_root->onUpdate(dt);
        m_showcase->Update(dt);
    }

    void Scene::fixedUpdate(float dt)
    {
        m_root->onFixedUpdate(dt);
    }

    void Scene::lateUpdate(float dt)
    {
         m_root->onLateUpdate(dt);
    }

    void Scene::render()
    {
        auto camera = m_root->findObjectByName("camera");
        if(camera != nullptr) camera->getComponent<CameraComponent>()->getCamera()->Render();

        m_root->onRender();
        m_showcase->Render();
    }

    std::shared_ptr<SceneObject> Scene::createObject(std::string name, std::shared_ptr<SceneObject> parent)
    {
        auto sceneObject = std::make_shared<SceneObject>(m_nextObjectID++, name, parent.get());
        sceneObject->getComponentAddedEvent().addListener(std::bind(&Scene::onComponentAdded, this, std::placeholders::_1));
        sceneObject->getComponentRemovedEvent().addListener(std::bind(&Scene::onComponentRemoved, this, std::placeholders::_1));
        if(parent != nullptr) parent->addChild(sceneObject);
        return sceneObject;
    }

    bool Scene::removeObject(const std::shared_ptr<SceneObject>& obj)
    {
        if(!obj) return false;

        obj->getComponentAddedEvent().removeAllListeners();
        obj->getComponentRemovedEvent().removeAllListeners();

        if(!m_root || m_root == obj) 
        {
            m_root = nullptr;
            return true;
        }
        
        return m_root->removeChild(obj);
    }

    //! Remove scene object by its id
    bool Scene::removeObjectById(uint64_t id)
    {
        return removeObject(findObjectById(id));
    }
    
    std::shared_ptr<SceneObject> Scene::findObjectById(uint64_t id) const
    {
        if (m_root == nullptr) return nullptr;
        return m_root->findObjectById(id);
    }

    std::shared_ptr<SceneObject> Scene::findObjectByName(std::string name) const
    {
        if (m_root == nullptr) return nullptr;
        return m_root->findObjectByName(name);
    }

    //! Component added event
    void Scene::onComponentAdded(std::shared_ptr<Component> component)
    {
        if (component->getName() == "FigureComponent")
        {
            auto figureComponent = std::dynamic_pointer_cast<FigureComponent>(component);
            auto figure = figureComponent->getFigure();
            if(figure) m_showcase->Add(figure);

            figureComponent->getOnFigureCreatedEvent().addListener([this](auto figure) {
                if (figure) m_showcase->Add(figure);
            });

            figureComponent->getOnFigureDestroyedEvent().addListener([this](auto figure) {
                if (figure) m_showcase->Remove(figure);
            });
        }
        else if (component->getName() == "EnvironmentComponent")
        {
            auto environment = ((EnvironmentComponent*)(component.get()))->getEnvironment();
            if(environment) m_showcase->Add(environment);
        }
    }

    //! Component added event
    void Scene::onComponentRemoved(std::shared_ptr<Component> component)
    {
        if (component->getName() == "FigureComponent")
        {
            auto figure = ((FigureComponent*)(component.get()))->getFigure();
            if(figure) m_showcase->Remove(figure);

            ((FigureComponent*)(component.get()))->getOnFigureCreatedEvent().removeAllListeners();
            ((FigureComponent*)(component.get()))->getOnFigureDestroyedEvent().removeAllListeners();
        }
        else if (component->getName() == "EnvironmentComponent")
        {
            auto environment = ((EnvironmentComponent*)(component.get()))->getEnvironment();
            if(environment) m_showcase->Remove(environment);
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
