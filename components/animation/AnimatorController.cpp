#include "AnimatorController.h"
#include "AnimatorStateMachine.h"

namespace ige::scene
{    
    //! Constructor
    AnimatorController::AnimatorController(const std::string& path)
    {
        m_figure = nullptr;
        setPath(path);
    }

    // Destructor
    AnimatorController::~AnimatorController()
    {
        m_figure = nullptr;
    }

    void AnimatorController::initialize()
    {
    }

    void AnimatorController::clear()
    {

    }

    void AnimatorController::setPath(const std::string& path)
    {
        if(m_path.compare(path) != 0) {
            //TODO: load state machines
            m_path = path;

            clear();
            initialize();
        }
    }

    void AnimatorController::setFigure(BaseFigure* figure)
    {
        if(m_figure != figure) {
            m_figure = figure;
        }
    }

    void AnimatorController::setTimeScale(float ts)
    {
        m_timeScale = ts;
    }

    //! Update
    void AnimatorController::update(float dt)
    {
        if(!m_figure) return;
        stateMachine->update(dt);
    }

    //! Parameters
    void AnimatorController::setParameter(const std::string& param, int type, float value)
    {
        type = std::clamp(type, (int)AnimatorParameterType::Bool, (int)AnimatorParameterType::Trigger);
        m_parameters[param] = {(AnimatorParameterType)type, value};
    }

    std::pair<AnimatorParameterType, float> AnimatorController::getParameter(const std::string& param) const
    {
        return hasParameter(param) ? m_parameters.at(param) : std::make_pair<AnimatorParameterType, float>(AnimatorParameterType::Float, 0.f);
    }

    bool AnimatorController::hasParameter(const std::string& param) const
    {
        return m_parameters.count(param) > 0;
    }

    bool AnimatorController::removeParameter(const std::string& param)
    {
        return m_parameters.erase(param);
    }

    //! Serialize component
    void to_json(json &j, const AnimatorController &obj)
    {
        
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorController &obj)
    {
        
    }
   
} // namespace ige::scene