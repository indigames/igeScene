
#include "AnimatorComponent.h"
#include "AnimatorController.h"

#include "components/FigureComponent.h"
#include "components/EditableFigureComponent.h"

#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    AnimatorComponent::AnimatorComponent(SceneObject &owner)
        : Component(owner), m_figure(nullptr)
    {
    }

    // Destructor
    AnimatorComponent::~AnimatorComponent()
    {
        clear();
    }

    //! Active when start runtime
    void AnimatorComponent::initialize()
    {
        clear();

        BaseFigure* figure = nullptr;        
        if(auto figComp = getOwner()->getComponent<FigureComponent>()) {
            figure = figComp->getFigure();
        }
        else if(auto figComp = getOwner()->getComponent<EditableFigureComponent>()) {
            figure = figComp->getFigure();
        }

        if(figure) {
            controller = std::make_shared<AnimatorController>(m_controllerPath);
            controller->setFigure(figure);
        }
    }

    //! Active when stop runtime
    void AnimatorComponent::clear()
    {
        if(controller) {
            controller->setFigure(nullptr);
            controller = nullptr;
        }
    }

    void AnimatorComponent::setControllerPath(const std::string& path)
    {
        if(m_controllerPath.compare(path) != 0) {
            m_controllerPath = path;
            initialize();
        }
    }

    // Update
    AnimatorComponent::onUpdate( float dt)
    {
       if(m_updateMode == UpdateMode::Normal) {
           if(controller) controller->update(dt);
       }
       else if(m_updateMode == UpdateMode::UnscaledTime) {
           if(controller) controller->update(dt, true);
       }
    }

    // Fixed Update
    AnimatorComponent::onFixedUpdate( float dt)
    {
        if(m_updateMode == UpdateMode::AnimatePhysic) {
            if(controller) controller->update(dt, true);
        }
    }
} // namespace ige::scene