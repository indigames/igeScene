#pragma once

#include "AnimatorState.h"

#include "utils/Serialize.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <string>

namespace ige::scene
{
    class AnimatorState;
    class AnimatorStateMachine;
    class AnimatorTransition;

    class AnimatorStateMachine : public std::enable_shared_from_this<AnimatorStateMachine> 
    {
    public:
        AnimatorStateMachine();
        virtual ~AnimatorStateMachine();

        virtual const std::string& getName() const { return m_name; }
        virtual void setName(const std::string& name) { m_name = name; }

        virtual void update(float dt);

        // Helper to get shared pointer from 'this'
        std::shared_ptr<AnimatorStateMachine> getSharedPtr() { return shared_from_this(); }

        virtual bool hasState(const std::shared_ptr<AnimatorState>& state, bool recursive = false);
        virtual bool hasStateMachine(const std::shared_ptr<AnimatorStateMachine>& stateMachine, bool recursive = false);
        virtual bool hasTransition(const std::shared_ptr<AnimatorState>& stateA, const std::shared_ptr<AnimatorState>& stateB);

        virtual std::shared_ptr<AnimatorState> findState(const std::string& name, bool recursive = false);
        virtual std::shared_ptr<AnimatorState> findState(const std::shared_ptr<AnimatorState>& state, bool recursive = false);

        virtual std::shared_ptr<AnimatorStateMachine> findStateMachine(const std::shared_ptr<AnimatorStateMachine>& stateMachine, bool recursive = false);
        virtual std::shared_ptr<AnimatorStateMachine> findStateMachine(const std::shared_ptr<AnimatorState>& state);

        virtual std::shared_ptr<AnimatorTransition> findTransition(const std::shared_ptr<AnimatorState>& dstState);

        virtual std::vector<std::weak_ptr<AnimatorState>> getStatesRecursive();
        virtual std::vector<std::weak_ptr<AnimatorStateMachine>> getStatesMachinesRecursive();
        virtual std::vector<std::weak_ptr<AnimatorTransition>> getTransitionsRecursive();

        virtual std::shared_ptr<AnimatorState> addState(const std::string& name);
        virtual void addState(const std::shared_ptr<AnimatorState>& state);
        virtual bool removeState(const std::shared_ptr<AnimatorState>& state);

        virtual std::shared_ptr<AnimatorStateMachine> addStateMachine(const std::string& name);
        virtual void addStateMachine(const std::shared_ptr<AnimatorStateMachine>& machine);
        virtual bool removeStateMachine(const std::shared_ptr<AnimatorStateMachine>& machine);

        virtual std::shared_ptr<AnimatorTransition> addAnyStateTransition();
        virtual std::shared_ptr<AnimatorTransition> addAnyStateTransition(const std::shared_ptr<AnimatorState>& state);
        virtual std::shared_ptr<AnimatorTransition> addAnyStateTransition(const std::shared_ptr<AnimatorStateMachine>& machine);
        virtual bool removeAnyStateTransition(const std::shared_ptr<AnimatorTransition>& transition, bool recursive = false);

        virtual std::shared_ptr<AnimatorTransition> addStateMachineTransition();
        virtual std::shared_ptr<AnimatorTransition> addStateMachineTransition(const std::shared_ptr<AnimatorState>& dstState);
        virtual std::shared_ptr<AnimatorTransition> addStateMachineTransition(const std::shared_ptr<AnimatorStateMachine>& dstMachine);
        virtual bool removeStateMachineTransition(const std::shared_ptr<AnimatorTransition>& transition);

        virtual std::shared_ptr<AnimatorTransition> addEntryTransition();
        virtual std::shared_ptr<AnimatorTransition> addEntryTransition(const std::shared_ptr<AnimatorState>& state);
        virtual std::shared_ptr<AnimatorTransition> addEntryTransition(const std::shared_ptr<AnimatorStateMachine>& machine);
        virtual bool removeEntryTransition(const std::shared_ptr<AnimatorTransition>& transition);
        
        //! Serialize
        friend void to_json(json &j, const AnimatorStateMachine &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorStateMachine &obj);

        std::vector<std::weak_ptr<AnimatorState>> states;
        std::vector<std::weak_ptr<AnimatorStateMachine>> stateMachines;

        std::vector<std::weak_ptr<AnimatorTransition>> anyStateTransitions;
        std::vector<std::weak_ptr<AnimatorTransition>> entryTransitions;
        std::vector<std::weak_ptr<AnimatorTransition>> stateMachineTransitions;

        std::shared_ptr<AnimatorState> currentState = nullptr;

    protected:
        virtual std::shared_ptr<AnimatorState> findEntryState();
        
    protected:
        std::string m_name;
    };
}
