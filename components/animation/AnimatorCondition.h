#pragma once

#include <string>

namespace ige::scene
{
    struct AnimatorCondition 
    {
        enum class Mode: int {
            If = 0,
            IfNot,
            Greater,
            Less,
            Equal,
            NotEqual,
        };

        // Constructor
        AnimatorCondition() {}
        AnimatorCondition(Mode mode, const std::string& param, float threshold);

        // Destructor
        virtual ~AnimatorCondition();

        // Members
        Mode mode;
        std::string parameter;
        float threshold;
    };
}
