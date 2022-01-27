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
            Greater,
            Less,
            Equal,
            NotEqual,
            Count
        };

        static std::string getMode(Mode mode) {
            const std::vector<std::string> Modes = {
                    "If",
                    "IfNot", 
                    "Greater",
                    "Less",
                    "Equal",
                    "NotEqual"
            };
            return Modes[(int)mode];
        }

        static std::vector<Mode> getValidModes(AnimatorParameterType type) {
            if (type == AnimatorParameterType::Bool || type == AnimatorParameterType::Trigger) {
                return { Mode::If, Mode::IfNot, Mode::Equal, Mode::NotEqual};
            }
            return { Mode::If, Mode::IfNot, Mode::Equal, Mode::NotEqual, Mode::Greater, Mode::Less };
        }

        //! Constructor
        AnimatorCondition();
        AnimatorCondition(const std::string& param, Mode mode, float threshold);

        //! Destructor
        virtual ~AnimatorCondition() {}

        //! Serialize
        friend void to_json(json &j, const AnimatorCondition &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorCondition &obj);

        // Members
        std::string parameter;
        Mode mode;
        float threshold;
    };
}
