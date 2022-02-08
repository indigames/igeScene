#include "AnimatorTransition.h"
#include "AnimatorState.h"
#include "AnimatorStateMachine.h"

namespace ige::scene {
    AnimatorTransition::AnimatorTransition() {}

    AnimatorTransition::~AnimatorTransition() {
        linkId = (uint64_t)-1;
    }

    std::shared_ptr<AnimatorCondition> AnimatorTransition::addCondition(const std::string& parameter, AnimatorCondition::Mode mode, float threshold)
    {
        auto condition = getCondition(parameter);
        if (condition == nullptr) {
            condition = std::make_shared<AnimatorCondition>(parameter, mode, threshold);
            conditions.push_back(condition);
        }
        return condition;
    }

    std::shared_ptr<AnimatorCondition> AnimatorTransition::getCondition(const std::string& param)
    {
        auto itr = std::find_if(conditions.begin(), conditions.end(), [param](const auto& elem) {
            return elem->parameter.compare(param) == 0;
        });
        return (itr != conditions.end()) ? (*itr) : nullptr;
    }

    bool AnimatorTransition::removeCondition(const std::shared_ptr<AnimatorCondition>& condition)
    {
        auto itr = std::find(conditions.begin(), conditions.end(), condition);
        if(itr != conditions.end()) {
            conditions.erase(itr);
            return true;
        }
        return false;
    }

    bool AnimatorTransition::removeCondition(const std::string& param)
    {
        auto itr = std::find_if(conditions.begin(), conditions.end(), [param](const auto& elem) {
            return elem->parameter.compare(param) == 0;
        });
        if(itr != conditions.end()) {
            conditions.erase(itr);
            return true;
        }
        return false;
    }

    //! Serialize component
    void to_json(json &j, const AnimatorTransition &obj)
    {
        j["name"] = obj.getName();
        j["mute"] = obj.isMute;
        j["solo"] = obj.isSolo;
        j["hasExitTime"] = obj.hasExitTime;
        j["exitTime"] = obj.exitTime;
        j["hasFixedDuration"] = obj.hasFixedDuration;
        j["duration"] = obj.duration;
        j["offset"] = obj.offset;
        if(!obj.destState.expired())
            j["destState"] = obj.destState.lock()->getUUID();

        auto jConditions = json::array();
        for (const auto& cond : obj.conditions){
            jConditions.push_back(json(*cond.get()));
        }
        j["conds"] = jConditions;
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorTransition &obj)
    {
        obj.setName(j.value("name", std::string()));
        obj.isMute = j.value("mute", false);
        obj.isSolo = j.value("solo", false);
        obj.hasExitTime = j.value("hasExitTime", true);
        obj.exitTime = j.value("exitTime", 1.f);
        obj.hasFixedDuration = j.value("hasFixedDuration", false);
        obj.duration = j.value("duration", 1.f);
        obj.offset = j.value("offset", 0.f);
        obj.destStateUUID = j.value("destState", std::string());
        
        if (j.count("conds") > 0) {
            auto jConds = j.at("conds");
            for (auto jCond : jConds) {
                auto cond = std::make_shared<AnimatorCondition>();
                jCond.get_to(*cond);
                obj.conditions.push_back(cond);
            }
        }        
    }

    //! Serialize finished handline
    void AnimatorTransition::onSerializeFinished(AnimatorState& state) {
        if (!destStateUUID.empty()) {
            auto dstState = state.stateMachine.lock()->findState(destStateUUID);
            destState = dstState;
            destStateUUID.clear(); // loaded, just clear it
        }
    }
}
