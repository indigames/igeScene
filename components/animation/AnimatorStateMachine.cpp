#include "AnimatorStateMachine.h"
#include "AnimatorTransition.h"
#include "AnimatorController.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

#include <iomanip>

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
            if (getController()) {
                if(currentState) getController()->getFigure()->BindAnimator(BaseFigure::AnimatorSlot::SlotA0, currentState->getAnimator());
                getController()->getFigure()->BindAnimator(BaseFigure::AnimatorSlot::SlotA1, (Animator*)nullptr);
            }
        }
        transitionDuration = 0.f;
        nextState.reset();
    }

    std::shared_ptr<AnimatorController> AnimatorStateMachine::getController()
    {
        return m_controller.expired() ? nullptr : m_controller.lock();
    }

    void AnimatorStateMachine::setController(const std::shared_ptr<AnimatorController>& controller)
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
        // Update states
        if (currentState != nullptr) {
            // Exit state
            if (currentState == exitState) {
                currentState = nullptr;
                return;
            }

            auto animator = currentState->getAnimator();
            if (animator == nullptr)
                return;

            // TODO: this is controled by Figure for now, should refactor
            // animator->Step(dt);

            // Update transition blending
            if (!nextState.expired() && currentState != nextState.lock()) {
                if (transitionTime >= transitionDuration) {
                    setCurrentState(nextState.lock());
                    return;
                }
                transitionTime += dt;
                if(transitionTime > transitionDuration) transitionTime = transitionDuration;
                getController()->getFigure()->SetBlendingWeight((uint32_t)AnimationPart::PartA, transitionTime / transitionDuration);
                return;
            }
            
            // Update transitions
            std::shared_ptr<AnimatorTransition> activeTransition = nullptr;
            for (auto& transition : currentState->transitions) {
                if (!transition->destState.expired() && !transition->isMute) {
                    for (auto& condition : transition->conditions) {
                        if (getController()->hasParameter(condition->parameter)) {
                            auto [type, defaultValue] = getController()->getParameter(condition->parameter);

                            // Transition with exit time go first
                            if (transition->hasExitTime) {
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
                            if ((condition->mode == AnimatorCondition::Mode::If && condition->threshold)
                                || (condition->mode == AnimatorCondition::Mode::IfNot && !condition->threshold)
                                || (condition->mode == AnimatorCondition::Mode::Equal && condition->threshold == defaultValue)
                                || (condition->mode == AnimatorCondition::Mode::NotEqual && condition->threshold != defaultValue)
                                || (condition->mode == AnimatorCondition::Mode::Greater && condition->threshold > defaultValue)
                                || (condition->mode == AnimatorCondition::Mode::Less && condition->threshold < defaultValue)
                                ) {
                                activeTransition = transition;

                                // Reset trigger state
                                if (type == AnimatorParameterType::Trigger) {
                                    condition->threshold = defaultValue;
                                }
                                break;
                            }
                        }
                    }
                }
            }

            if (activeTransition) {
                nextState = activeTransition->destState.lock();
                transitionDuration = activeTransition->hasFixedDuration ? activeTransition->duration : activeTransition->duration * nextState.lock()->getAnimator()->GetEndTime();
                getController()->getFigure()->BindAnimator(BaseFigure::AnimatorSlot::SlotA1, nextState.lock()->getAnimator());
                nextState.lock()->enter();
                activeTransition = nullptr;
            }
        }
    }

    bool AnimatorStateMachine::save(const std::string& path) {
        json jObj;
        to_json(jObj, *this);
        auto fsPath = path.empty() ? fs::path(std::string("anims/") + getName()) : fs::path(path);
        if (fsPath.extension().string() != ".anim") fsPath = fsPath.replace_extension(".anim");
        try {
            std::ofstream file(fsPath.string());
            file << std::setw(2) << jObj << std::endl;
            file.close();
        }
        catch (std::exception e) {
            return false;
        }
        return true;
    }

    //! Serialize component
    void to_json(json &j, const AnimatorStateMachine &obj)
    {
        j["name"] = obj.getName();
        auto jStates = json::array();
        for (const auto& state : obj.states) {
            jStates.push_back(json(*state.get()));
        }
        j["states"] = jStates;
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorStateMachine &obj)
    {
        obj.setName(j.value("name", std::string()));
        obj.states.clear();
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
    }
}
