#include "AnimatorStateMachine.h"

namespace ige::scene {
    auto StateMachineCache::childStateMachines = std::map<std::weak_ptr<AnimatorStateMachine>, std::vector<ChildStateMachine>>();

    void StateMachineCache::init()
    {
        clear();
    }

    void StateMachineCache::clear()
    {
        childStateMachines.swap({});
    }

    const std::vector<std::weak_ptr<ChildStateMachine>>& StateMachineCache::getChildStateMachines(const std::shared_ptr<AnimatorStateMachine>& parent)
    {
        init();

        auto itr = childStateMachines.find(parent);
        if(itr != childStateMachines.end()) {
            return (*itr).second;
        }

        const auto& children = parent->stateMachines;
        childStateMachines.insert({parent, children});
        return children;
    }


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
            return (!elem.expired() && elem.lock()->destState == stateB);
        });
        if(itr != stateA->transitions.end())
            return true;

        auto itr = std::find_if(stateB->transitions.begin(), stateB->transitions.end(), [stateA](const auto& elem){
            return (!elem.expired() && elem.lock()->destState == stateA);
        });
        return (itr != stateB->transitions.end());
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::findState(const std::shared_ptr<AnimatorState>& state, bool recursive)
    {
        auto itr = std::find_if(states.begin(), states.end(), [state](const auto& elem){
            return !elem.expired() && !elem.lock()->state.expired() && elem.lock()->state.lock() == state;
        });

        if(!recursive && itr != states.end())
            return (*itr).lock()->state.lock();

        const auto& statesRecursive = getStatesRecursive();
        auto itr2 = std::find_if(statesRecursive.begin(), statesRecursive.end(), [state](const auto& elem){
            return !elem.expired() && !elem.lock()->state.expired() && elem.lock()->state.lock() == state;
        });

        if(itr2 != statesRecursive.end())
            return (*itr2).lock()->state.lock();

        return nullptr;
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::findState(const std::string& name, bool recursive)
    {
        auto itr = std::find_if(states.begin(), states.end(), [name](const auto& elem){
            return !elem.expired() && !elem.lock()->state.expired() && elem.lock()->state.lock()->getName().compare(name) == 0;
        });

        if(!recursive && itr != states.end())
            return (*itr).lock()->state.lock();

        const auto& statesRecursive = getStatesRecursive();
        auto itr2 = std::find_if(statesRecursive.begin(), statesRecursive.end(), [name](const auto& elem){
            return !elem.expired() && !elem.lock()->state.expired() && elem.lock()->state.lock()->getName().compare(name) == 0;
        });

        if(itr2 != statesRecursive.end())
            return (*itr2).lock()->state.lock();

        return nullptr;
    }

    std::shared_ptr<AnimatorStateMachine> AnimatorStateMachine::findStateMachine(const std::shared_ptr<AnimatorStateMachine>& stateMachine, bool recursive)
    {
        auto itr = std::find_if(stateMachines.begin(), stateMachines.end(), [stateMachine](const auto& elem){
            return !elem.expired() && !elem.lock()->stateMachine.expired() && elem.lock()->stateMachine.lock() == stateMachine;
        });

        if(!recursive && itr != stateMachines.end())
            return (*itr).lock()->stateMachine.lock();

        const auto& stateMachinesRecursive = getStatesMachinesRecursive();
        auto itr2 = std::find_if(stateMachinesRecursive.begin(), stateMachinesRecursive.end(), [stateMachine](const auto& elem){
            return !elem.expired() && !elem.lock()->stateMachine.expired() && elem.lock()->stateMachine.lock() == stateMachine;
        });

        if(itr2 != stateMachinesRecursive.end())
            return (*itr2).lock()->stateMachine.lock();

        return nullptr;
    }

    std::shared_ptr<AnimatorStateMachine> AnimatorStateMachine::findStateMachine(const std::shared_ptr<AnimatorState>& state)
    {
        if (hasState(state, false)) return getSharedPtr();
        const auto& stateMachinesRecursive = getStatesMachinesRecursive();
        auto itr = std::find_if(stateMachinesRecursive.begin(), stateMachinesRecursive.end(), [state](const auto& elem){
            return !elem.expired() && !elem.lock()->stateMachine.expired() && elem.lock()->stateMachine.lock()->hasState(state, false);
        });
        return (itr != stateMachinesRecursive.end()) ? (*itr).lock()->stateMachine.lock() : nullptr;
    }

    std::shared_ptr<AnimatorTransition> AnimatorStateMachine::findTransition(const std::shared_ptr<AnimatorState>& dstState)
    {
        auto itr = std::find_if(anyStateTransitions.begin(), anyStateTransitions.end(), [dstState](const auto& elem){
            return !elem.expired() && elem.lock()->destState == dstState;
        });
        return (itr != anyStateTransitions.end()) ? (*itr).lock() : nullptr;
    }

    std::vector<std::weak_ptr<ChildState>> AnimatorStateMachine::getStatesRecursive()
    {
        auto ret = std::vector<std::weak_ptr<ChildState>>(states);
        for(const auto& sm: stateMachines) {
            if(!sm.expired() && !sm.lock()->stateMachine.expired()) {
                auto childStates = sm.lock()->stateMachine.lock()->getStatesRecursive();
                ret.reserve(ret.size() + childStates.size());
                std::copy(childStates.begin(), childStates.end(), std::back_inserter(ret));
            }
        }
        return ret;
    }

    std::vector<std::weak_ptr<ChildStateMachine>> AnimatorStateMachine::getStatesMachinesRecursive()
    {
        auto childSM = StateMachineCache::getChildStateMachines(getSharedPtr());
        auto ret = std::vector<std::weak_ptr<ChildStateMachine>>(childSM);
        for(const auto& sm: childSM) {
            if(!sm.expired() && !sm.lock()->stateMachine.expired()) {
                auto childMachines = sm.lock()->stateMachine.lock()->getStatesMachinesRecursive();
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
            if(!sm.expired() && !sm.lock()->stateMachine.expired()) {
                auto childTransitions = sm.lock()->stateMachine.lock()->getTransitionsRecursive();
                ret.reserve(ret.size() + childTransitions.size());
                std::copy(childTransitions.begin(), childTransitions.end(), std::back_inserter(ret));
            }
        }
        return ret;
    }

    Vec3 AnimatorStateMachine::getStatePosition(const std::shared_ptr<AnimatorState>& state) const
    {
        for(const auto& s: states) {
            if(!s.expired() && !s.lock()->state.expired()) {
                if(state == s.lock()->state.lock()) {
                    return s.lock()->position;
                }
            }
        }
        return Vec3();
    }

    void AnimatorStateMachine::setStatePosition(const std::shared_ptr<AnimatorState>& state, const Vec3& pos)
    {
        for(const auto& s: states) {
            if(!s.expired() && !s.lock()->state.expired()) {
                if(state == s.lock()->state.lock()) {
                    s.lock()->position = pos;
                    return;
                }
            }
        }
    }

    Vec3 AnimatorStateMachine::getStateMachinePosition(const std::shared_ptr<AnimatorStateMachine>& stateMachine) const
    {
        for(const auto& sm: stateMachines) {
            if(!sm.expired() && !sm.lock()->stateMachine.expired()) {
                if(stateMachine == sm.lock()->stateMachine.lock()) {
                    return sm.lock()->position;
                }
            }
        }
        return Vec3();
    }

    void AnimatorStateMachine::setStateMachinePosition(const std::shared_ptr<AnimatorStateMachine>& stateMachine, const Vec3& pos)
    {
        for(const auto& sm: stateMachines) {
            if(!sm.expired() && !sm.lock()->stateMachine.expired()) {
                if(stateMachine == sm.lock()->stateMachine.lock()) {
                    sm.lock()->position = pos;
                    return;
                }
            }
        }
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::addState(const std::string& name)
    {
        auto size = states.size();
        auto pos = (size > 0 &&  !states[size - 1].expired()) ? states[size - 1].lock()->position + Vec3(30.f, 60.f, 0.f) : Vec3();
        return addState(name, pos);
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::addState(const std::string& name, const Vec3& pos)
    {
        auto state = std::make_shared<AnimatorState>();
        state->setName(name);
        addState(state, pos);
        return state;
    }

    void AnimatorStateMachine::addState(const std::shared_ptr<AnimatorState>& state, const Vec3& pos)
    {
        if(!hasState(state)) {
            auto newState = std::make_shared<ChildState>();
            newState->state = state;
            newState->position = pos;
            states.push_back(newState);
        }
    }

    bool AnimatorStateMachine::removeState(const std::shared_ptr<AnimatorState>& state)
    {
        auto itr = std::find_if(states.begin(), states.end(), [state](const auto& elem){
            return !elem.expired() && !elem.lock()->state.expired() && elem.lock()->state.lock() == state;
        });

        if(itr != states.end()) {
            states.erase(itr);
            return true;
        }
        return false;
    }

    std::shared_ptr<AnimatorStateMachine> AnimatorStateMachine::addStateMachine(const std::string& name, const Vec3& pos)
    {
        auto machine = std::make_shared<AnimatorStateMachine>();
        machine->setName(name);
        addStateMachine(machine, pos);
        return machine;
    }

    void AnimatorStateMachine::addStateMachine(const std::shared_ptr<AnimatorStateMachine>& machine, const Vec3& pos){
        if(!hasStateMachine(machine)) {
            auto newMachine = std::make_shared<ChildStateMachine>();
            newMachine->stateMachine = machine;
            newMachine->position = pos;
            stateMachines.push_back(newMachine);
        }
    }

    bool AnimatorStateMachine::removeStateMachine(const std::shared_ptr<AnimatorStateMachine>& machine)
    {
        auto itr = std::find_if(stateMachines.begin(), stateMachines.end(), [machine](const auto& elem){
            return !elem.expired() && !elem.lock()->stateMachine.expired() && elem.lock()->stateMachine.lock() == machine;
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

    bool AnimatorStateMachine::removeAnyStateTransition(const std::shared_ptr<AnimatorTransition>& transition, bool recursive = false)
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
                if(!sm.expired() && !sm.lock()->stateMachine.expired() && sm.lock()->stateMachine.lock()->removeAnyStateTransition(transition))
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

}
