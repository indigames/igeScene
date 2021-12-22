#include "AnimatorState.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene {
    AnimatorState::AnimatorState() {}

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

    std::shared_ptr<AnimatorTransition> AnimatorState::addTransition(const std::shared_ptr<AnimatorStateMachine>& machine, bool withExitTime)
    {
        auto transition = createTransition(withExitTime);
        transition->destStateMachine = machine;
        addTransition(transition);
        return transition;
    }

    std::shared_ptr<AnimatorTransition> AnimatorState::addExitTransition(bool withExitTime)
    {
        auto transition = createTransition(withExitTime);
        transition->isExit = true;
        addTransition(transition);
        return transition;
    }

    std::shared_ptr<AnimatorTransition> AnimatorState::findTransition(const std::shared_ptr<AnimatorState>& state)
    {
        auto itr = std::find_if(transitions.begin(), transitions.end(), [&](const auto& elem){
            return (!elem.expired() && !elem.lock()->destState.expired() && elem.lock()->destState.lock() == state);
        });
        return (itr != transitions.end()) ? (*itr).lock() : nullptr;
    }

    std::shared_ptr<AnimatorStateMachine> AnimatorState::findStateMachine(const std::shared_ptr<AnimatorStateMachine>& root)
    {
        if (root->hasState(getSharedPtr(), false))
            return root;
        return root->findStateMachine(getSharedPtr());
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
        // TODO: parse from animation clip
        transition->hasExitTime = true;
        transition->exitTime = 1.f;
        transition->duration = 1.f;

        if(m_animator) {
            transition->exitTime = m_animator->GetEndTime();
            transition->duration = m_animator->GetEndTime() - m_animator->GetStartTime();
        }
    }

    //! Serialize component
    void to_json(json &j, const AnimatorState &obj)
    {
        
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorState &obj)
    {
        
    }
}
