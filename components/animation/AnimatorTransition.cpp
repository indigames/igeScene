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
        auto itr = std::find_if(conditions.begin(), conditions.end(), [condition](const auto& elem) {
            return (!elem.expired() && elem.lock() == condition);
        });
        if(itr != conditions.end()) {
            conditions.erase(itr);
            return true;
        }
        return false;
    }
}
