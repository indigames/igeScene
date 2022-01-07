#include "AnimatorTransition.h"

namespace ige::scene {
    AnimatorTransition::AnimatorTransition() {}

    AnimatorTransition::~AnimatorTransition() {}

    std::shared_ptr<AnimatorCondition> AnimatorTransition::addCondition(AnimatorCondition::Mode mode, const std::string& parameter, float threshold)
    {
        auto condition = std::make_shared<AnimatorCondition>(mode, parameter, threshold);
        conditions.push_back(condition);
        return condition;
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

    //! Serialize component
    void to_json(json &j, const AnimatorTransition &obj)
    {
        j["name"] = obj.getName();
        j["mute"] = obj.isMute;
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
}
