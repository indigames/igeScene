#pragma once

#include <string>

#include "utils/Serialize.h"

namespace ige::scene
{
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
