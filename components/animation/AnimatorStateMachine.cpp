#include "AnimatorStateMachine.h"

namespace ige::scene {

    AnimatorStateMachine::AnimatorStateMachine() {}

    AnimatorStateMachine::~AnimatorStateMachine() {}

    bool AnimatorStateMachine::hasState(const std::shared_ptr<AnimatorState>& state, bool recursive)
    {
        return findState(state, recursive) != nullptr;
    }

    bool AnimatorStateMachine::hasStateMachine(const std::shared_ptr<AnimatorStateMachine>& stateMachine, bool recursive)
    {
        return findStateMachine(stateMachine, recursive) != nullptr;
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

    std::shared_ptr<AnimatorState> AnimatorStateMachine::findState(const std::shared_ptr<AnimatorState>& state, bool recursive)
    {
        auto itr = std::find_if(states.begin(), states.end(), [state](const auto& elem){
            return !elem.expired() && elem.lock() == state;
        });

        if(!recursive && itr != states.end())
            return (*itr).lock();

        const auto& statesRecursive = getStatesRecursive();
        auto itr2 = std::find_if(statesRecursive.begin(), statesRecursive.end(), [state](const auto& elem){
            return !elem.expired() && elem.lock() == state;
        });

        if(itr2 != statesRecursive.end())
            return (*itr2).lock();

        return nullptr;
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::findState(const std::string& name, bool recursive)
    {
        auto itr = std::find_if(states.begin(), states.end(), [name](const auto& elem){
            return !elem.expired() && !elem.lock()->getName().compare(name) == 0;
        });

        if(!recursive && itr != states.end())
            return (*itr).lock();

        const auto& statesRecursive = getStatesRecursive();
        auto itr2 = std::find_if(statesRecursive.begin(), statesRecursive.end(), [name](const auto& elem){
            return !elem.expired() && !elem.lock()->getName().compare(name) == 0;
        });

        if(itr2 != statesRecursive.end())
            return (*itr2).lock();

        return nullptr;
    }

    std::shared_ptr<AnimatorStateMachine> AnimatorStateMachine::findStateMachine(const std::shared_ptr<AnimatorStateMachine>& stateMachine, bool recursive)
    {
        auto itr = std::find_if(stateMachines.begin(), stateMachines.end(), [stateMachine](const auto& elem){
            return !elem.expired() && elem.lock() == stateMachine;
        });

        if(!recursive && itr != stateMachines.end())
            return (*itr).lock();

        const auto& stateMachinesRecursive = getStatesMachinesRecursive();
        auto itr2 = std::find_if(stateMachinesRecursive.begin(), stateMachinesRecursive.end(), [stateMachine](const auto& elem){
            return !elem.expired() && elem.lock() == stateMachine;
        });

        if(itr2 != stateMachinesRecursive.end())
            return (*itr2).lock();

        return nullptr;
    }

    std::shared_ptr<AnimatorStateMachine> AnimatorStateMachine::findStateMachine(const std::shared_ptr<AnimatorState>& state)
    {
        if (hasState(state, false)) return getSharedPtr();
        const auto& stateMachinesRecursive = getStatesMachinesRecursive();
        auto itr = std::find_if(stateMachinesRecursive.begin(), stateMachinesRecursive.end(), [state](const auto& elem){
            return !elem.expired() && !elem.lock()->hasState(state, false);
        });
        return (itr != stateMachinesRecursive.end()) ? (*itr).lock() : nullptr;
    }

    std::shared_ptr<AnimatorTransition> AnimatorStateMachine::findTransition(const std::shared_ptr<AnimatorState>& dstState)
    {
        auto itr = std::find_if(anyStateTransitions.begin(), anyStateTransitions.end(), [dstState](const auto& elem){
            return !elem.expired() && elem.lock()->destState.expired() && elem.lock()->destState.lock() == dstState;
        });
        return (itr != anyStateTransitions.end()) ? (*itr).lock() : nullptr;
    }

    std::vector<std::weak_ptr<AnimatorState>> AnimatorStateMachine::getStatesRecursive()
    {
        auto ret = std::vector<std::weak_ptr<AnimatorState>>(states);
        for(const auto& sm: stateMachines) {
            if(!sm.expired()) {
                auto childStates = sm.lock()->getStatesRecursive();
                ret.reserve(ret.size() + childStates.size());
                std::copy(childStates.begin(), childStates.end(), std::back_inserter(ret));
            }
        }
        return ret;
    }

    std::vector<std::weak_ptr<AnimatorStateMachine>> AnimatorStateMachine::getStatesMachinesRecursive()
    {
        auto childSM = std::vector<std::weak_ptr<AnimatorStateMachine>>(stateMachines);
        auto ret = std::vector<std::weak_ptr<AnimatorStateMachine>>(childSM);
        for(const auto& sm: childSM) {
            if(!sm.expired()) {
                auto childMachines = sm.lock()->getStatesMachinesRecursive();
                ret.reserve(ret.size() + childMachines.size());
                std::copy(childMachines.begin(), childMachines.end(), std::back_inserter(ret));
            }
        }
        return ret;
    }

    std::vector<std::weak_ptr<AnimatorTransition>> AnimatorStateMachine::getTransitionsRecursive()
    {
        auto ret = std::vector<std::weak_ptr<AnimatorTransition>>(anyStateTransitions);
        for(const auto& sm: stateMachines) {
            if(!sm.expired()) {
                auto childTransitions = sm.lock()->getTransitionsRecursive();
                ret.reserve(ret.size() + childTransitions.size());
                std::copy(childTransitions.begin(), childTransitions.end(), std::back_inserter(ret));
            }
        }
        return ret;
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
        auto itr = std::find_if(states.begin(), states.end(), [state](const auto& elem){
            return !elem.expired() && elem.lock() == state;
        });

        if(itr != states.end()) {
            states.erase(itr);
            return true;
        }
        return false;
    }

    std::shared_ptr<AnimatorStateMachine> AnimatorStateMachine::addStateMachine(const std::string& name)
    {
        auto machine = std::make_shared<AnimatorStateMachine>();
        machine->setName(name);
        addStateMachine(machine);
        return machine;
    }

    void AnimatorStateMachine::addStateMachine(const std::shared_ptr<AnimatorStateMachine>& machine){
        if(!hasStateMachine(machine)) {
            stateMachines.push_back(machine);
        }
    }

    bool AnimatorStateMachine::removeStateMachine(const std::shared_ptr<AnimatorStateMachine>& machine)
    {
        auto itr = std::find_if(stateMachines.begin(), stateMachines.end(), [machine](const auto& elem){
            return !elem.expired() && elem.lock() == machine;
        });

        if(itr != stateMachines.end()) {
            stateMachines.erase(itr);
            return true;
        }
        return false;
    }

    std::shared_ptr<AnimatorTransition> AnimatorStateMachine::addAnyStateTransition()
    {
        auto transition = std::make_shared<AnimatorTransition>();
        transition->hasExitTime = false;
        transition->hasFixedDuration = true;
        transition->duration = 0.25f;
        transition->exitTime = 0.75f;
        anyStateTransitions.push_back(transition);
        return transition;
    }

    std::shared_ptr<AnimatorTransition> AnimatorStateMachine::addAnyStateTransition(const std::shared_ptr<AnimatorState>& state)
    {
        auto transition = addAnyStateTransition();
        transition->destState = state;
        return transition;
    }

    std::shared_ptr<AnimatorTransition> AnimatorStateMachine::addAnyStateTransition(const std::shared_ptr<AnimatorStateMachine>& machine)
    {
        auto transition = addAnyStateTransition();
        transition->destStateMachine = machine;
        return transition;
    }

    bool AnimatorStateMachine::removeAnyStateTransition(const std::shared_ptr<AnimatorTransition>& transition, bool recursive)
    {
        auto itr = std::find_if(anyStateTransitions.begin(), anyStateTransitions.end(), [transition](const auto& elem){
            return !elem.expired() && elem.lock() == transition;
        });
        if (itr != anyStateTransitions.end()) {
            anyStateTransitions.erase(itr);
            return true;
        }
        if(recursive) {
            auto stateMachinesRecursive = getStatesMachinesRecursive();
            for(auto& sm: stateMachinesRecursive) {
                if(!sm.expired() && !sm.lock()->removeAnyStateTransition(transition))
                    return true;
            }
        }
        return false;
    }

    std::shared_ptr<AnimatorTransition> AnimatorStateMachine::addStateMachineTransition()
    {
        auto dstMachine = std::shared_ptr<AnimatorStateMachine>(nullptr);
        return addStateMachineTransition(dstMachine);
    }

    std::shared_ptr<AnimatorTransition> AnimatorStateMachine::addStateMachineTransition(const std::shared_ptr<AnimatorState>& dstState)
    {  
        auto newTransition = addStateMachineTransition();
        newTransition->destState = dstState;
        return newTransition;
    }

    std::shared_ptr<AnimatorTransition> AnimatorStateMachine::addStateMachineTransition(const std::shared_ptr<AnimatorStateMachine>& dstMachine)
    {
        auto newTransition = std::make_shared<AnimatorTransition>();
        newTransition->destStateMachine = dstMachine;
        stateMachineTransitions.push_back(newTransition);
        return newTransition;
    }

    bool AnimatorStateMachine::removeStateMachineTransition(const std::shared_ptr<AnimatorTransition>& transition)
    {
        auto itr = std::find_if(stateMachineTransitions.begin(), stateMachineTransitions.end(), [transition](const auto& elem){
            return !elem.expired() && elem.lock() == transition;
        });
        if (itr != stateMachineTransitions.end()) {
            stateMachineTransitions.erase(itr);
            return true;
        }
        return false;
    }

    std::shared_ptr<AnimatorTransition> AnimatorStateMachine::addEntryTransition()
    {
        auto transition = std::make_shared<AnimatorTransition>();
        entryTransitions.push_back(transition);
        return transition;
    }
    std::shared_ptr<AnimatorTransition> AnimatorStateMachine::addEntryTransition(const std::shared_ptr<AnimatorState>& state)
    {
        auto transition = addEntryTransition();
        transition->destState = state;
        return transition;
    }

    std::shared_ptr<AnimatorTransition> AnimatorStateMachine::addEntryTransition(const std::shared_ptr<AnimatorStateMachine>& machine)
    {
        auto transition = addEntryTransition();
        transition->destStateMachine = machine;
        return transition;
    }

    bool AnimatorStateMachine::removeEntryTransition(const std::shared_ptr<AnimatorTransition>& transition)
    {
        auto itr = std::find_if(entryTransitions.begin(), entryTransitions.end(), [transition](const auto& elem){
            return !elem.expired() && elem.lock() == transition;
        });
        if (itr != entryTransitions.end()) {
            entryTransitions.erase(itr);
            return true;
        }
        return false;
    }

    void AnimatorStateMachine::update(float dt)
    {
        if(currentState == nullptr) {
            currentState = findEntryState();
        }

        if(currentState != nullptr) 
        {
            // currentState->update(dt);

            // if(!currentState->destState.expired() && currentState->findTransition(currentState->destState.lock())) {
            //     currentState->exit();
                
            //     currentState = currentState->destState.lock();
            //     currentState->enter();
            // }               
        }

        for(auto& machine: stateMachines) {
            if(!machine.expired())
                machine.lock()->update(dt);
        }
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::findEntryState()
    {
        // Find entry transitions
        auto itr = std::find_if(entryTransitions.begin(), entryTransitions.end(), [](const auto& elem){
            return !elem.expired() && !elem.lock()->isMute && !elem.lock()->destState.expired();
        });
        if(itr != entryTransitions.end())
            return (*itr).lock()->destState.lock();
        
        // Find any transitions
        itr = std::find_if(anyStateTransitions.begin(), anyStateTransitions.end(), [](const auto& elem){
            return !elem.expired() && !elem.lock()->isMute && !elem.lock()->destState.expired();
        });
        if(itr != anyStateTransitions.end())
            return (*itr).lock()->destState.lock();

        // Not found
        return nullptr;
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
