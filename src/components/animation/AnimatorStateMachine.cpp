#include "AnimatorStateMachine.h"
#include "AnimatorTransition.h"
#include "AnimatorController.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

#include <iomanip>

namespace ige::scene {
    AnimatorStateMachine::AnimatorStateMachine() : m_layer(-1) {
        currentState = enterState = addEnterState();
        exitState = addExitState();
        anyState = addAnyState();
    }

    AnimatorStateMachine::~AnimatorStateMachine() 
    {
        clear();
    }

    void AnimatorStateMachine::clear()
    {
        m_layer = -1;
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

    bool AnimatorStateMachine::hasState(const std::string& name)
    {
        auto itr = std::find_if(states.begin(), states.end(), [&](const auto& elem) {
            return elem->getName().compare(name) == 0;
        });
        return (itr != states.end());
    }

    bool AnimatorStateMachine::hasTransition(const std::shared_ptr<AnimatorState>& stateA, const std::shared_ptr<AnimatorState>& stateB)
    {
        auto itr = std::find_if(stateA->transitions.begin(), stateA->transitions.end(), [stateB](const auto& elem){
            return !elem->destState.expired() && elem->destState.lock() == stateB;
        });
        if(itr != stateA->transitions.end())
            return true;

        itr = std::find_if(stateB->transitions.begin(), stateB->transitions.end(), [stateA](const auto& elem){
            return !elem->destState.expired() && elem->destState.lock() == stateA;
        });
        return (itr != stateB->transitions.end());
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::findState(const std::shared_ptr<AnimatorState>& state)
    {
        auto itr = std::find(states.begin(), states.end(), state);
        return (itr != states.end()) ? (*itr) : nullptr;
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::findState(const std::string& uuid)
    {
        auto itr = std::find_if(states.begin(), states.end(), [&](const auto& elem) {
            return elem->getUUID().compare(uuid) == 0;
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
        if (!hasState(state)) {
            states.push_back(state);
        }
    }

    bool AnimatorStateMachine::removeState(const std::shared_ptr<AnimatorState>& state)
    {
        auto itr = std::find(states.begin(), states.end(), state);
        if (itr != states.end()) {
            states.erase(itr);
            return true;
        }
        return false;
    }

    void AnimatorStateMachine::setCurrentState(const std::shared_ptr<AnimatorState>& state)
    {
        if (currentState != state) {
            if (currentState) currentState->exit();
            currentState = state;
            if(currentState) currentState->enter();
            if (getController()) {
                getController()->getFigure()->BindAnimator((BaseFigure::AnimatorSlot)(m_layer * 2 + 1), currentState ? currentState->getAnimator() : (Animator*)nullptr);
                getController()->getFigure()->BindAnimator((BaseFigure::AnimatorSlot)(m_layer * 2 + 2), (Animator*)nullptr);
            }
        }
        transitionTime = transitionDuration = 0.f;
        nextState.reset();
    }

    std::shared_ptr<AnimatorController> AnimatorStateMachine::getController()
    {
        return m_controller.expired() ? nullptr : m_controller.lock();
    }

    void AnimatorStateMachine::setController(std::shared_ptr<AnimatorController> controller)
    {
        m_controller = controller;
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::addEnterState() {
        auto state = addState("Entry");
        state->setType((int)AnimatorState::Type::Enter);
        state->setPosition({100.f, 100.f});
        return state;
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::addExitState() {
        auto state = addState("Exit");
        state->setType((int)AnimatorState::Type::Exit);
        state->setPosition({ 500.f, 100.f });
        return state;
    }

    std::shared_ptr<AnimatorState> AnimatorStateMachine::addAnyState() {
        auto state = addState("Any");
        state->setType((int)AnimatorState::Type::Any);
        state->setPosition({ 100.f, 200.f });
        return state;
    }

    void AnimatorStateMachine::update(float dt)
    {
        // Layer A, B, C only, otherwise return
        if (m_layer < 0 || m_layer > 3)
            return;

        // Update states
        if (currentState != nullptr) {
            // Exit state
            if (currentState->isExit()) {
                currentState = nullptr;
                return;
            }

            auto animator = currentState->getAnimator();
            // TODO: this is controled by Figure for now, should refactor
            // if (animator) animator->Step(dt);

            // Update transition blending
            if (!nextState.expired() && currentState != nextState.lock()) {
                if (transitionDuration <= 0.f || transitionTime >= transitionDuration) {
                    setCurrentState(nextState.lock());
                    return;
                }
                transitionTime += dt;
                if(transitionTime > transitionDuration) transitionTime = transitionDuration;
                getController()->getFigure()->SetBlendingWeight(m_layer, transitionTime / transitionDuration);
                return;
            }
            
            // Update transitions, which are from current state and Any state
            std::shared_ptr<AnimatorTransition> activeTransition = nullptr;
            auto transitions = currentState->transitions;
            transitions.insert(transitions.end(), anyState->transitions.begin(), anyState->transitions.end());
            for (auto& transition : transitions) {
                if (!transition->destState.expired() && !transition->isMute) {
                    if (transition->conditions.empty()) {
                        activeTransition = transition;
                        break;
                    }

                    // Transition with exit time go first
                    if (animator && transition->hasExitTime) {
                        if (transition->hasFixedDuration) {
                            if (animator->GetEvalTime() > transition->exitTime) {
                                activeTransition = transition;
                                break;
                            }
                        }
                        else if (animator->GetEvalTime() / animator->GetEndTime() > transition->exitTime) {
                            activeTransition = transition;
                            break;
                        }
                    }

                    // Check conditions
                    bool shouldActiveTransition = true;
                    for (auto& condition : transition->conditions) {
                        if (getController()->hasParameter(condition->parameter)) {
                            auto [type, value] = getController()->getParameter(condition->parameter);
                            if (!((condition->mode == AnimatorCondition::Mode::If && value != 0.f)
                                || (condition->mode == AnimatorCondition::Mode::IfNot && value == 0.f)
                                || (condition->mode == AnimatorCondition::Mode::Equal && value == condition->threshold)
                                || (condition->mode == AnimatorCondition::Mode::NotEqual && value != condition->threshold)
                                || (condition->mode == AnimatorCondition::Mode::Greater && value > condition->threshold)
                                || (condition->mode == AnimatorCondition::Mode::GreaterOrEqual && value >= condition->threshold)
                                || (condition->mode == AnimatorCondition::Mode::Less && value < condition->threshold)
                                || (condition->mode == AnimatorCondition::Mode::LessOrEqual && value <= condition->threshold)
                                )) {
                                shouldActiveTransition = false;
                                break;                  
                            }
                            // Reset trigger state
                            if (type == AnimatorParameterType::Trigger) {
                                getController()->setParameter(condition->parameter, (int)AnimatorParameterType::Trigger, 0.f);
                            }
                        }
                    }
                    if (shouldActiveTransition) {
                        activeTransition = transition;
                        break;
                    }
                }
            }

            if (activeTransition) {
                nextState = activeTransition->destState.lock();
                if (nextState.lock() != currentState) {
                    transitionTime = 0.f;
                    transitionDuration = activeTransition->offset + (activeTransition->hasExitTime ? activeTransition->hasFixedDuration ? activeTransition->duration : activeTransition->exitTime * nextState.lock()->getAnimator()->GetEndTime() : 0.f);
                    if (transitionDuration > 0.f) {
                        getController()->getFigure()->BindAnimator((BaseFigure::AnimatorSlot)(m_layer * 2 + 2), nextState.lock()->getAnimator());
                    }
                    else {
                        setCurrentState(nextState.lock());
                    }
                }
                activeTransition = nullptr;
            }
        }
    }

    bool AnimatorStateMachine::save(const std::string& path) {
        if (path.empty())
            return false;
        json jObj;
        to_json(jObj, *this);
        auto fsPath = fs::path(path).replace_extension(".anim");
#ifdef __cpp_exceptions
        try {
#endif
            std::ofstream file(fsPath.string());
            file << std::setw(2) << jObj << std::endl;
            file.close();
            return true;
#ifdef __cpp_exceptions
        }
        catch (std::exception e) {}
#endif
        return false;
    }

    void AnimatorStateMachine::restore_from_json(const json& j)
    {
        clear();
        setLayer(j.value("layer", -1));
        if (j.count("states") > 0) {
            auto jStates = j.at("states");
            for (auto jState : jStates) {
                auto state = std::make_shared<AnimatorState>();
                jState.get_to(*state);
                states.push_back(state);
                state->stateMachine = shared_from_this();
            }
            for (const auto& state : states) {
                state->onSerializeFinished();
            }
        }

        // Should always contain Enter, Exit, and Any states
        if (states.size() >= 3) {
            currentState = enterState = states[0];
            exitState = states[1];
            anyState = states[2];
        }
    }

    //! Serialize component
    void to_json(json &j, const AnimatorStateMachine &obj)
    {
        j["layer"] = obj.getLayer();
        auto jStates = json::array();
        for (const auto& state : obj.states) {
            jStates.push_back(json(*state.get()));
        }
        j["states"] = jStates;
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorStateMachine &obj)
    {
        obj.clear();
        obj.setLayer(j.value("layer", -1));
        if (j.count("states") > 0) {
            auto jStates = j.at("states");
            for (auto jState : jStates) {
                auto state = std::make_shared<AnimatorState>();
                jState.get_to(*state);
                obj.states.push_back(state);
                state->stateMachine = obj.shared_from_this();
            }
            for (const auto& state : obj.states) {
                state->onSerializeFinished();
            }
        }

        // Should always contain Enter, Exit, and Any states
        if (obj.states.size() >= 3) {
            obj.currentState = obj.enterState = obj.states[0];
            obj.exitState = obj.states[1];
            obj.anyState = obj.states[2];
        }
    }
}
