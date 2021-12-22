#include "AnimatorCondition.h"

namespace ige::scene {
    AnimatorCondition::AnimatorCondition(Mode mode, const std::string& param, float threshold)
    : mode(mode), parameter(param), threshold(threshold) {
    }

    //! Serialize component
    void to_json(json &j, const AnimatorCondition &obj)
    {
        
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorCondition &obj)
    {
        
    }
}
