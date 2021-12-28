#include "AnimatorState.h"
#include "AnimatorStateMachine.h"
#include "AnimatorTransition.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene {
    AnimatorState::AnimatorState(): m_animator(nullptr) {}

    AnimatorState::~AnimatorState() {
        if(m_animator != nullptr) {
            m_animator->DecReference();
            m_animator = nullptr;
        }
    }

    void AnimatorState::setPath(const std::string& path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        if (relPath.size() == 0) relPath = fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        if(m_path.compare(relPath) != 0) {
            m_path = relPath;
            m_name = fsPath.stem().string();
            if(m_animator != nullptr) m_animator->DecReference();
            m_animator = (Animator*)ResourceManager::Instance().GetResource(m_path.c_str(), ANIMATORTYPE);
            if (m_animator) {
                m_animator->SetSpeed(m_speed);
            }
        }
    }        

    void AnimatorState::addTransition(const std::shared_ptr<AnimatorTransition>& transition)
    {
        transitions.push_back(transition);
    }

    bool AnimatorState::removeTransition(const std::shared_ptr<AnimatorTransition>& transition)
    {
        auto itr = std::find_if(transitions.begin(), transitions.end(), [transition](const auto& elem) {
            return (!elem.expired() && elem.lock() == transition);
        });
        if(itr != transitions.end()) {
            transitions.erase(itr);
            return true;
        }
        return false;
    }

    std::shared_ptr<AnimatorTransition> AnimatorState::addTransition(const std::shared_ptr<AnimatorState>& state, bool withExitTime)
    {
        auto transition = createTransition(withExitTime);
        transition->destState = state;
        addTransition(transition);
        return transition;
    }

    std::shared_ptr<AnimatorTransition> AnimatorState::addExitTransition(bool withExitTime)
    {
        auto transition = createTransition(withExitTime);
        transition->destState = stateMachine.lock()->getExitState();
        addTransition(transition);
        return transition;
    }

    std::shared_ptr<AnimatorTransition> AnimatorState::findTransition(const std::shared_ptr<AnimatorState>& dstState)
    {
        auto itr = std::find_if(transitions.begin(), transitions.end(), [&](const auto& elem){
            return (!elem.expired() && !elem.lock()->destState.expired() && elem.lock()->destState.lock() == dstState);
        });
        return (itr != transitions.end()) ? (*itr).lock() : nullptr;
    }

    std::shared_ptr<AnimatorTransition> AnimatorState::createTransition(bool withExitTime)
    {
        auto newTransition = std::make_shared<AnimatorTransition>();
        if (withExitTime) {
            setDefaultExitTime(newTransition);
        }
        return newTransition;
    }

    void AnimatorState::setDefaultExitTime(std::shared_ptr<AnimatorTransition>& transition)
    {

        transition->hasExitTime = true;
        transition->exitTime = m_animator ? m_animator->GetEndTime() : 1.f;
        transition->duration = m_animator ? m_animator->GetTotalEvalTime() : 1.f;
    }

    void AnimatorState::setSpeed(float speed)
    {
        if (m_speed != speed) {
            m_speed = speed;

            if (m_animator) {
                m_animator->SetSpeed(m_speed);
            }
        }
    }

    void AnimatorState::update(float dt) {
        if (m_animator) {
            // check animation time and ending
            if (m_animator->GetEvalTime() + 0.01f >= m_animator->GetEndTime()) {
                //auto nextState = find
            }
            
        }
    }

    //! Serialize component
    void to_json(json &j, const AnimatorState &obj)
    {
        j["path"] = obj.getPath();
        j["name"] = obj.getName();
        j["speed"] = obj.getSpeed();
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorState &obj)
    {
        obj.setPath(j.value("path", obj.getPath()));
        obj.setName(j.value("name", obj.getName()));
        obj.setSpeed(j.value("name", obj.getSpeed()));
    }
}
