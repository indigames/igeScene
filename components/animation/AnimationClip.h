#pragma once

#include "event/Event.h"
#include "utils/Serialize.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    class AnimationClip
    {
    public:
        AnimationClip(const std::string& path);
        virtual ~AnimationClip();

        //! Serialize
        friend void to_json(json &j, const AnimationClip &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimationClip &obj);

    protected:
        std::string m_path;
        std::vector<Animator*> m_animators;
    };
} // namespace ige::scene
