#pragma once

#include <string>

#include "utils/Serialize.h"

namespace ige::scene
{
    enum class AnimatorParameterType {
        Bool = 0,
        Float,
        Int,
        Trigger
    };

    struct AnimatorCondition 
    {
        enum class Mode {
            If = 0,
            IfNot,
            Equal,
            NotEqual,
            Greater,
            GreaterOrEqual,
            Less,
            LessOrEqual
        };

        static std::string getMode(Mode mode) {
            const std::vector<std::string> Modes = {
                    "If",
                    "IfNot", 
                    "Equal",
                    "NotEqual",
                    "Greater",
                    "GreaterOrEqual",
                    "Less",
                    "LessOrEqual"
            };
            return Modes[(int)mode];
        }

        static std::vector<Mode> getValidModes(AnimatorParameterType type) {
            if (type == AnimatorParameterType::Trigger) {
                return { Mode::If };
            }
            if (type == AnimatorParameterType::Bool) {
                return { Mode::If, Mode::IfNot, Mode::Equal, Mode::NotEqual};
            }
            return { Mode::If, Mode::IfNot, Mode::Equal, Mode::NotEqual, Mode::Greater, Mode::GreaterOrEqual, Mode::Less, Mode::LessOrEqual };
        }

        //! Constructor
        AnimatorCondition();
        AnimatorCondition(uint64_t id, const std::string& param, Mode mode, float threshold);

        //! Destructor
        virtual ~AnimatorCondition() {}

        //! Serialize
        friend void to_json(json &j, const AnimatorCondition &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorCondition &obj);

        // Members
        uint64_t id;
        std::string parameter;
        Mode mode;
        float threshold;
    };
}
