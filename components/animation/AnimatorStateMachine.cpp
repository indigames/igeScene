#include "AnimatorStateMachine.h"
#include "AnimatorTransition.h"


namespace ige::scene {

    AnimatorStateMachine::AnimatorStateMachine() {
        currentState = enterState = addEnterState();
        exitState = addExitState();
        anyState = addAnyState();
    }

    AnimatorStateMachine::~AnimatorStateMachine() 
    {
        currentState = nullptr;
        for (auto& state : states) {
            state = nullptr;
        }
        states.clear();
    }

    bool AnimatorStateMachine::hasState(const std::shared_ptr<AnimatorState>& state)
    {
        return findState(state) != nullptr;
    }

    bool AnimatorStateMachine::hasTransition(const std::shared_ptr<AnimatorState>& stateA, const std::shared_ptr<AnimatorState>& stateB)
    {
        auto itr = std::find_if(stateA->transitions.begin(), stateA->transitions.end(), [stateB](const auto& elem){
            return (!elem.expired() && !elem.lock()->destState.expired() && elem.lock()->destState.lock() == stateB);
        });
        if(itr != stateA->transitions.end())
            return true;

        itr = std::find_if(stateB->transitions.begin(), stateB->transitions.end(), [stateA](const auto& elem){
            return (!elem.expired() && !elem.lock()->destState.expired() && elem.lock()->destState.lock() == stateA);
        });
        return (itr != stateB->transitions.end());
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::findState(const std::shared_ptr<AnimatorState>& state)
    {
        auto itr = std::find(states.begin(), states.end(), state);
        return (itr != states.end()) ? (*itr) : nullptr;
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::findState(const std::string& name)
    {
        auto itr = std::find_if(states.begin(), states.end(), [name](const auto& elem){
            return elem->getName().compare(name) == 0;
        });
        return (itr != states.end()) ? (*itr) : nullptr;
    }

    std::vector<std::shared_ptr<AnimatorState>>& AnimatorStateMachine::getStates()
    {
        return states;
    }
   
    std::shared_ptr<AnimatorState> AnimatorStateMachine::addState(const std::string& name)
    {
        auto state = std::make_shared<AnimatorState>();
        state->setName(name);
        addState(state);
        return state;
    }

    void AnimatorStateMachine::addState(const std::shared_ptr<AnimatorState>& state)
    {
        if(!hasState(state)) {
            states.push_back(state);
        }
    }

    bool AnimatorStateMachine::removeState(const std::shared_ptr<AnimatorState>& state)
    {
        auto itr = std::find(states.begin(), states.end(), state);
        if(itr != states.end()) {
            states.erase(itr);
            return true;
        }
        return false;
    }

    void AnimatorStateMachine::setCurrentState(const std::shared_ptr<AnimatorState>& state)
    {
        if (currentState != state) {
            if (currentState)
                currentState->getOnExitEvent().invoke(*currentState);
            currentState = state;
            if (currentState)
                currentState->getOnEnterEvent().invoke(*currentState);
        }        
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::addEnterState() {
        auto state = addState("Enter");
        state->isEnter = true;
        return state;
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::addExitState() {
        auto state = addState("Exit");
        state->isExit = true;
        return state;
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::addAnyState() {
        auto state = addState("Any");
        state->isAny = true;
        return state;
    }

    void AnimatorStateMachine::update(float dt)
    {      
        // Update states
        if(currentState != nullptr) {
            currentState->update(dt);            
        }

        // Update transition

    }


    //! Serialize component
    void to_json(json &j, const AnimatorStateMachine &obj)
    {
        
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorStateMachine &obj)
    {
        
    }
}
