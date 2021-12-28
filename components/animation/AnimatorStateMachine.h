#pragma once

#include "AnimatorState.h"

#include "utils/Serialize.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <string>

namespace ige::scene
{
    class AnimatorState;

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

        virtual void setCurrentState(const std::shared_ptr<AnimatorState>& state);

        virtual bool hasState(const std::shared_ptr<AnimatorState>& state);
        virtual std::shared_ptr<AnimatorState> findState(const std::string& name);
        virtual std::shared_ptr<AnimatorState> findState(const std::shared_ptr<AnimatorState>& state);

        virtual bool hasTransition(const std::shared_ptr<AnimatorState>& stateA, const std::shared_ptr<AnimatorState>& stateB);

        std::vector<std::shared_ptr<AnimatorState>>& getStates();

        std::shared_ptr<AnimatorState>& getCurrentState() { return currentState; }
        std::shared_ptr<AnimatorState>& getEnterState() { return enterState; }
        std::shared_ptr<AnimatorState>& getExitState() { return exitState; }
        std::shared_ptr<AnimatorState>& getAnyState() { return anyState; }

        std::shared_ptr<AnimatorState> addState(const std::string& name);
        void addState(const std::shared_ptr<AnimatorState>& state);
        bool removeState(const std::shared_ptr<AnimatorState>& state);
                
        //! Serialize
        friend void to_json(json &j, const AnimatorStateMachine &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorStateMachine &obj);

    protected:
        virtual std::shared_ptr<AnimatorState> addEnterState();
        virtual std::shared_ptr<AnimatorState> addExitState();
        virtual std::shared_ptr<AnimatorState> addAnyState();
        
    protected:
        std::vector<std::shared_ptr<AnimatorState>> states;
        std::shared_ptr<AnimatorState> currentState = nullptr;

        // Cache predefined states
        std::shared_ptr<AnimatorState> enterState = nullptr;
        std::shared_ptr<AnimatorState> exitState = nullptr;
        std::shared_ptr<AnimatorState> anyState = nullptr;

        std::string m_name;
    };
}
