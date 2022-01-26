#include "AnimatorCondition.h"

namespace ige::scene {
    AnimatorCondition::AnimatorCondition()
        :mode(Mode::Equal), parameter(std::string()), threshold(0.f) {
    }

    AnimatorCondition::AnimatorCondition(const std::string& param, Mode mode,float threshold)
        : mode(mode), parameter(param), threshold(threshold) {
    }

    //! Serialize component
    void to_json(json &j, const AnimatorCondition &obj)
    {
        j["mode"] = (int)obj.mode;
        j["param"] = obj.parameter;
        j["threshold"] = obj.threshold;
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorCondition &obj)
    {
        obj.mode = j.value("mode", AnimatorCondition::Mode::Equal);
        obj.parameter = j.value("param", std::string());
        obj.threshold = j.value("threshold", 0.f);
    }
}
