#include "AnimatorController.h"
#include "AnimatorStateMachine.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

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
        clear();
        m_stateMachine = std::make_shared<AnimatorStateMachine>();
        if (!m_path.empty()) {
            auto fsPath = fs::path(m_path);
            if (fsPath.extension().string().compare(".anim") == 0) {
                std::ifstream file(fsPath);
                if (file.is_open()) {
                    json jObj;
                    file >> jObj;
                    file.close();
                    jObj.get_to(*(m_stateMachine.get()));
                }
            }
        }
    }

    void AnimatorController::clear()
    {
        m_path.clear();
        m_stateMachine = nullptr;
        m_figure = nullptr;
    }

    void AnimatorController::setPath(const std::string& path)
    {
        if(m_path.compare(path) != 0) {
            m_path = path;
        }
        initialize();
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
        getStateMachine()->update(dt * m_timeScale);
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
        j["path"] = obj.getPath();
        j["timeScale"] = obj.getTimeScale();
        j["params"] = obj.m_parameters;
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorController &obj)
    {
        obj.setPath(j.value("path", std::string()));
        obj.setTimeScale(j.value("timeScale", 1.f));
        obj.m_parameters = j["params"].get<std::unordered_map<std::string, std::pair<AnimatorParameterType, float>>>();
    }
} // namespace ige::scene
