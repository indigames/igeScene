#pragma once

#include <string>

#include "AnimatorState.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    class AnimatorStateMachine;

    struct ChildStateMachine {            
        std::weak_ptr<AnimatorStateMachine> stateMachine;
        Vec3 position;
    };

    class StateMachineCache {
        public:
            static void init();
            static void clear();
            static const std::vector<std::weak_ptr<ChildStateMachine>>& getChildStateMachines(const std::shared_ptr<AnimatorStateMachine>& parent);

        protected:
            static bool initialized;
            static std::map<std::weak_ptr<AnimatorStateMachine>, std::vector<std::weak_ptr<ChildStateMachine>>> childStateMachines;
    }; 
    
    class AnimatorStateMachine : public std::enable_shared_from_this<AnimatorStateMachine> 
    {
    public:
        AnimatorStateMachine();
        virtual ~AnimatorStateMachine();

        virtual const std::string& getName() const { return m_name; }
        virtual void setName(const std::string& name) { m_name = name; }

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

        virtual std::vector<std::weak_ptr<ChildState>> getStatesRecursive();
        virtual std::vector<std::weak_ptr<ChildStateMachine>> getStatesMachinesRecursive();
        virtual std::vector<std::weak_ptr<AnimatorTransition>> getTransitionsRecursive();

        virtual Vec3 getStatePosition(const std::shared_ptr<AnimatorState>& state) const;
        virtual void setStatePosition(const std::shared_ptr<AnimatorState>&, const Vec3& pos);

        virtual Vec3 getStateMachinePosition(const std::shared_ptr<AnimatorStateMachine>& state) const;
        virtual void setStateMachinePosition(const std::shared_ptr<AnimatorStateMachine>&, const Vec3& pos);

        virtual std::shared_ptr<AnimatorState> addState(const std::string& name);
        virtual std::shared_ptr<AnimatorState> addState(const std::string& name, const Vec3& pos);
        virtual void addState(const std::shared_ptr<AnimatorState>& state, const Vec3& pos = {});
        virtual bool removeState(const std::shared_ptr<AnimatorState>& state);

        virtual std::shared_ptr<AnimatorStateMachine> addStateMachine(const std::string& name, const Vec3& pos = {});
        virtual void addStateMachine(const std::shared_ptr<AnimatorStateMachine>& machine, const Vec3& pos = {});
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

        std::vector<std::weak_ptr<ChildState>> states;
        std::vector<std::weak_ptr<ChildStateMachine>> stateMachines;

        std::vector<std::weak_ptr<AnimatorTransition>> anyStateTransitions;
        std::vector<std::weak_ptr<AnimatorTransition>> entryTransitions;
        std::vector<std::weak_ptr<AnimatorTransition>> stateMachineTransitions;

    protected:
        std::string m_name;
    };
}
