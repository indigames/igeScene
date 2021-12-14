#include "AnimatorState.h"

namespace ige::scene {
    AnimatorState::AnimatorState() {}

    AnimatorState::~AnimatorState() {}

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
        auto itr = std::find_if(transitions.begin(), transitions.end(), [](const auto& elem){
            return (!elem.expired() && elem.lock()->destState == state);            
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
    }    
}
