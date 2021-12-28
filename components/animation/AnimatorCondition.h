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
        };

        //! Constructor
        AnimatorCondition();
        AnimatorCondition(Mode mode, const std::string& param, float threshold);

        //! Destructor
        virtual ~AnimatorCondition() {}

        //! Serialize
        friend void to_json(json &j, const AnimatorCondition &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorCondition &obj);

        // Members
        Mode mode;
        std::string parameter;
        float threshold;
    };
}
