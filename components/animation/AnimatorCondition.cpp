#include "AnimatorCondition.h"

namespace ige::scene {
    AnimatorCondition::AnimatorCondition()
        : id(-1), parameter(std::string()), mode(Mode::Equal), threshold(0.f) {
    }

    AnimatorCondition::AnimatorCondition(uint64_t id, const std::string& param, Mode mode,float threshold)
        : id(id), parameter(param), mode(mode), threshold(threshold) {
    }

    //! Serialize component
    void to_json(json &j, const AnimatorCondition &obj)
    {
        j["id"] = (int)obj.id;
        j["param"] = obj.parameter;
        j["mode"] = (int)obj.mode;
        j["threshold"] = obj.threshold;
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorCondition &obj)
    {
        obj.id = j.value("id", -1);
        obj.parameter = j.value("param", std::string());
        obj.mode = j.value("mode", AnimatorCondition::Mode::Equal);
        obj.threshold = j.value("threshold", 0.f);
    }
}
