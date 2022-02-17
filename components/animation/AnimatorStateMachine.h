#pragma once

#include "AnimatorState.h"

#include "utils/Serialize.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <string>

namespace ige::scene
{
    class AnimatorState;
    class AnimatorController;

    enum class AnimationPart {
        PartA = 0,
        PartB,
        PartC
    };

    class AnimatorStateMachine : public std::enable_shared_from_this<AnimatorStateMachine> 
    {
    public:
        AnimatorStateMachine();
        virtual ~AnimatorStateMachine();

        virtual void clear();

        virtual void update(float dt);
        virtual bool save(const std::string& path);

        int getLayer() const { return m_layer; }
        void setLayer(int layer) { m_layer = layer; }

        // Helper to get shared pointer from 'this'
        std::shared_ptr<AnimatorStateMachine> getSharedPtr() { return shared_from_this(); }

        virtual void setCurrentState(const std::shared_ptr<AnimatorState>& state);

        virtual bool hasState(const std::shared_ptr<AnimatorState>& state);
        virtual bool hasState(const std::string& name);
        virtual std::shared_ptr<AnimatorState> findState(const std::string& uuid);
        virtual std::shared_ptr<AnimatorState> findState(const std::shared_ptr<AnimatorState>& state);

        virtual bool hasTransition(const std::shared_ptr<AnimatorState>& stateA, const std::shared_ptr<AnimatorState>& stateB);

        std::vector<std::shared_ptr<AnimatorState>>& getStates();

        std::shared_ptr<AnimatorState>& getCurrentState() { return currentState; }
        std::shared_ptr<AnimatorState> getEnterState() { return enterState; }
        std::shared_ptr<AnimatorState> getExitState() { return exitState; }
        std::shared_ptr<AnimatorState> getAnyState() { return anyState; }

        std::shared_ptr<AnimatorState> addState(const std::string& name);
        void addState(const std::shared_ptr<AnimatorState>& state);
        bool removeState(const std::shared_ptr<AnimatorState>& state);

        std::shared_ptr<AnimatorController> getController();
        void setController(std::shared_ptr<AnimatorController> controller);

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
        std::shared_ptr<AnimatorState> enterState = nullptr;
        std::shared_ptr<AnimatorState> exitState = nullptr;
        std::shared_ptr<AnimatorState> anyState = nullptr;
        std::weak_ptr<AnimatorState> nextState;
        int m_layer;

        std::weak_ptr<AnimatorController> m_controller;

        //! Cache transition time and duration for state switching
        float transitionTime = 0.f;
        float transitionDuration = 0.f;
    };
}
