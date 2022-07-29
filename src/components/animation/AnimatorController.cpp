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
        addLayer(); // Add default layer
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
                    jObj.get_to(*this);
                }
            }
        }
    }

    void AnimatorController::clear()
    {
        m_stateMachines.clear();
        m_figure = nullptr;
    }

    bool AnimatorController::addLayer()
    {
        if (m_stateMachines.size() > (int)AnimationPart::PartC) {
            return false;
        }
        auto sm = std::make_shared<AnimatorStateMachine>();
        sm->setLayer(m_stateMachines.size());
        // sm->setController(shared_from_this());
        m_stateMachines.push_back(sm);
        return true;
    }

    bool AnimatorController::removeLayer(int layer)
    {
        if (layer < 0 || layer >= m_stateMachines.size())
            return false;
        m_stateMachines.erase(m_stateMachines.begin() + layer);
        return true;
    }

    std::shared_ptr<AnimatorStateMachine> AnimatorController::getStateMachine(int layer)
    {
        return (layer >= 0 && layer < m_stateMachines.size()) ? m_stateMachines[layer] : nullptr;
    }

    void AnimatorController::setBaseModelPath(const std::string& path)
    {
        auto fsPath = fs::path(path).replace_extension(".pyxf");
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path)).string() : fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');
        if (m_baseModelPath.compare(relPath) != 0) {
            m_baseModelPath = relPath;
        }
    }

    void AnimatorController::setPath(const std::string& path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path)).string() : fsPath.string();
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
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path)).string() : fsPath.string();
        if (relPath.size() == 0) relPath = fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');
        if (m_path.compare(relPath) != 0) {
            m_path = relPath;
        }

        json jScene;
        to_json(jScene);
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
        for(auto& sm: m_stateMachines)
            if(sm) sm->update(dt * m_timeScale);
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

    void AnimatorController::to_json(json& j)
    {
        j["model"] = getBaseModelPath();
        j["timeScale"] = getTimeScale();
        j["params"] = m_parameters;
        json jSms = json::array();
        for (const auto& sm : m_stateMachines) {
            jSms.push_back((*sm.get()));
        }
        j["layers"] = jSms;
    }

    void AnimatorController::from_json(const json& j)
    {
        setBaseModelPath(j.value("model", std::string()));
        setTimeScale(j.value("timeScale", 1.f));
        m_parameters = j["params"].get<std::unordered_map<std::string, std::pair<AnimatorParameterType, float>>>();

        if (j.contains("layers")) {
            m_stateMachines.clear();
            auto jSms = j.at("layers");
            for (auto jSm : jSms) {
                auto sm = std::make_shared<AnimatorStateMachine>();;
                sm->setController(shared_from_this());
                jSm.get_to(*sm.get());
                m_stateMachines.push_back(sm);
            }
        }
        else {
            addLayer();
        }
    }

    void AnimatorController::restore_from_json(const json& j) {
        setBaseModelPath(j.value("model", std::string()));
        setTimeScale(j.value("timeScale", 1.f));
        m_parameters = j["params"].get<std::unordered_map<std::string, std::pair<AnimatorParameterType, float>>>();
        if (j.contains("layers")) {
            
            m_stateMachines.clear();
            auto jSms = j.at("layers");
            for (auto jSm : jSms) {
                std::shared_ptr< AnimatorStateMachine> sm = nullptr;
                if(sm == nullptr)
                    sm = std::make_shared<AnimatorStateMachine>();
                sm->setController(shared_from_this());
                sm->restore_from_json(jSm);
                m_stateMachines.push_back(sm);
            }
        }
        /*else {
            addLayer();
        }*/
        m_dirty = true;
    }

    //! Serialize component
    void to_json(json &j, const AnimatorController &obj)
    {
        j["model"] = obj.getBaseModelPath();
        j["timeScale"] = obj.getTimeScale();
        j["params"] = obj.m_parameters;
        json jSms = json::array();
        for (const auto& sm : obj.m_stateMachines) {
            jSms.push_back((*sm.get()));
        }
        j["layers"] = jSms;
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorController &obj)
    {
        obj.setBaseModelPath(j.value("model", std::string()));
        obj.setTimeScale(j.value("timeScale", 1.f));
        obj.m_parameters = j["params"].get<std::unordered_map<std::string, std::pair<AnimatorParameterType, float>>>();
                
        if (j.contains("layers")) {
            obj.m_stateMachines.clear();
            auto jSms = j.at("layers");
            for (auto jSm : jSms) {
                auto sm = std::make_shared<AnimatorStateMachine>();;
                sm->setController(obj.shared_from_this());
                jSm.get_to(*sm.get());
                obj.m_stateMachines.push_back(sm);
            }
        }
        else {
            obj.addLayer();
        }
    }

    
} // namespace ige::scene
