#include "AnimatorCondition.h"

namespace ige::scene {
    AnimatorCondition::AnimatorCondition(Mode mode = Mode::If, const std::string& param, float threshold)
    : mode(mode), parameter(param), threshold(threshold) {
    }
}
