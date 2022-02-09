#include "AnimatorController.h"
#include "AnimatorStateMachine.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

#include <fstream>
#include <iomanip>

namespace ige::scene
{    
    //! Constructor
    AnimatorController::AnimatorController()
    {
        m_stateMachine = std::make_shared<AnimatorStateMachine>();
        m_figure = nullptr;
    }

    // Destructor
    AnimatorController::~AnimatorController()
    {
        clear();
    }

    void AnimatorController::initialize()
    {
        clear();
        
        if (!m_path.empty()) {
            auto fsPath = fs::path(m_path);
            if (fsPath.extension().string().compare(".anim") == 0) {
                std::ifstream file(fsPath);
                if (file.is_open()) {
                    json jObj;
                    file >> jObj;
                    file.close();

                    m_stateMachine = std::make_shared<AnimatorStateMachine>();
                    m_stateMachine->setController(shared_from_this());
                    jObj.get_to(*this);
                }
            }
        }
    }

    void AnimatorController::clear()
    {
        m_stateMachine = nullptr;
        m_figure = nullptr;
    }

    void AnimatorController::setPath(const std::string& path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        if (relPath.size() == 0) relPath = fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');
        if(m_path.compare(relPath) != 0) {
            m_path = relPath;
            initialize();
        }
    }

    void AnimatorController::save(const std::string& path)
    {
        auto fsPath = fs::path(path).replace_extension(".anim");
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        if (relPath.size() == 0) relPath = fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');
        if (m_path.compare(relPath) != 0) {
            m_path = relPath;
        }

        json jScene;
        to_json(jScene, *this);
        fs::create_directories(fsPath.parent_path());
        std::ofstream file(fsPath.string());
        file << std::setw(2) << jScene << std::endl;
        file.close();
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
        j["sm"] = (*obj.m_stateMachine.get());
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorController &obj)
    {
        obj.setPath(j.value("path", std::string()));
        obj.setTimeScale(j.value("timeScale", 1.f));
        obj.m_parameters = j["params"].get<std::unordered_map<std::string, std::pair<AnimatorParameterType, float>>>();
        j["sm"].get_to(*obj.m_stateMachine.get());
    }
} // namespace ige::scene
