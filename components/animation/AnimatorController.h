#pragma once

#include "event/Event.h"
#include "utils/Serialize.h"

namespace ige::scene
{
    class AnimatorStateMachine;

    /**
     * Class Animator
     */
    class AnimatorController
    {
    public:
        AnimatorController();
        virtual ~AnimatorController();

        //! Serialize
        friend void to_json(json &j, const AnimatorController &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorController &obj);

        std::vector<std::shared_ptr<AnimationClip>> animatorClips;

        std::shared_ptr<AnimatorStateMachine> stateMachine;

    protected:
        std::string m_path;
    };
} // namespace ige::scene
