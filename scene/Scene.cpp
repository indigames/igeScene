#include <algorithm>

#include "scene/Scene.h"
#include "scene/SceneObject.h"

#include "components/Component.h"
#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "components/EnvironmentComponent.h"
#include "components/ScriptComponent.h"

#include "utils/GraphicsHelper.h"

#define SERIALIZE_VERSION "0.0.1"

namespace ige::scene
{
    Scene::Scene(const std::string& name)
        : m_name(name)
    {
        m_showcase = ResourceCreator::Instance().NewShowcase((name + "_showcase").c_str());
        m_showcase->Initialize();
        m_showcase->WaitInitialize();

        SceneObject::getComponentAddedEvent().addListener(std::bind(&Scene::onComponentAdded, this, std::placeholders::_1, std::placeholders::_2));
        SceneObject::getComponentRemovedEvent().addListener(std::bind(&Scene::onComponentRemoved, this, std::placeholders::_1, std::placeholders::_2));
    }

    Scene::~Scene()
    {
        SceneObject::getComponentAddedEvent().removeAllListeners();
        SceneObject::getComponentRemovedEvent().removeAllListeners();

        m_root = nullptr;
        if(m_showcase)
            m_showcase->DecReference();
        m_showcase = nullptr;
    }
    
    bool Scene::initialize()
    {
        m_root = createObject("root");
        auto envComp = m_root->addComponent<EnvironmentComponent>("environment");
        envComp->setAmbientGroundColor(Vec3(0.5f, 0.5f, 0.5f));
        envComp->setDirectionalLightColor(0, Vec3(0.5f, 0.5f, 0.5f));
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
        m_root->onRender();
        m_showcase->Render();
    }

    std::shared_ptr<SceneObject> Scene::createObject(std::string name, std::shared_ptr<SceneObject> parent)
    {
        auto sceneObject = std::make_shared<SceneObject>(m_nextObjectID++, name, parent.get());
        if(parent != nullptr) parent->addChild(sceneObject);
        sceneObject->addComponent<TransformComponent>(Vec3(0.f, 0.f, 0.f));
        return sceneObject;
    }

    bool Scene::removeObject(const std::shared_ptr<SceneObject>& obj)
    {
        if(!obj) return false;

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
    void Scene::onComponentAdded(SceneObject& obj, std::shared_ptr<Component> component)
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
        else if (component->getName() == "SpriteComponent")
        {
            auto eFigComp = std::dynamic_pointer_cast<SpriteComponent>(component);
            auto figure = eFigComp->getFigure();
            if (figure) m_showcase->Add(figure);

            eFigComp->getOnFigureCreatedEvent().addListener([this](auto figure) {
                if (figure) m_showcase->Add(figure);
            });

            eFigComp->getOnFigureDestroyedEvent().addListener([this](auto figure) {
                if (figure) m_showcase->Remove(figure);
            });
        }
        else if (component->getName() == "EnvironmentComponent")
        {
            auto envComp = std::dynamic_pointer_cast<EnvironmentComponent>(component);
            auto env = envComp->getEnvironment();
            if (env) m_showcase->Add(env);
        }
    }

    //! Component removed event
    void Scene::onComponentRemoved(SceneObject& obj, std::shared_ptr<Component> component)
    {
        if (component->getName() == "FigureComponent")
        {
            auto figComp = std::dynamic_pointer_cast<FigureComponent>(component);
            auto figure = figComp->getFigure();
            if (figure) m_showcase->Remove(figure);

            figComp->getOnFigureDestroyedEvent().removeAllListeners();
            figComp->getOnFigureDestroyedEvent().removeAllListeners();
        }
        else if (component->getName() == "SpriteComponent")
        {
            auto eFigComp = std::dynamic_pointer_cast<SpriteComponent>(component);
            auto figure = eFigComp->getFigure();
            if (figure) m_showcase->Remove(figure);

            eFigComp->getOnFigureDestroyedEvent().removeAllListeners();
            eFigComp->getOnFigureDestroyedEvent().removeAllListeners();
        }
        else if (component->getName() == "EnvironmentComponent")
        {
            auto envComp = std::dynamic_pointer_cast<EnvironmentComponent>(component);
            auto env = envComp->getEnvironment();
            if (env) m_showcase->Remove(env);
        }
    }

    //! Serialize
    void Scene::to_json(json& j) const
    {
        j = json {
            {"name", m_name}
        };

        json jRoot;
        m_root->to_json(jRoot);
        j["root"] = jRoot;
    }

    //! Deserialize 
    void Scene::from_json(const json& j)
    {
        j.at("name").get_to(m_name);
        auto jRoot = j.at("root");

        m_root = std::make_shared<SceneObject>(jRoot.at("id"), jRoot.at("name"));
        m_root->from_json(jRoot);
    }
}
