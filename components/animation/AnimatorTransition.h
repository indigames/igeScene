#pragma once

#include <string>
#include <vector>
#include <memory>

#include "AnimatorCondition.h"
#include "AnimatorState.h"

#include "event/Event.h"
#include "utils/Serialize.h"

namespace ige::scene
{
    class AnimatorState;
    class AnimatorStateMachine;
    class AnimatorCondition;

    class AnimatorTransition {
    public:
        AnimatorTransition();
        virtual ~AnimatorTransition();

        virtual const std::string& getName() const { return m_name; }
        virtual void setName(const std::string& name) { m_name = name; }

        virtual std::shared_ptr<AnimatorCondition> addCondition(AnimatorCondition::Mode mode, const std::string& parameter, float threshold);
        virtual bool removeCondition(const std::shared_ptr<AnimatorCondition>& condition);

        //! Update, return true when ready for transit
        virtual bool update(float dt);
 
        //! Serialize
        friend void to_json(json &j, const AnimatorTransition &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorTransition &obj);

        bool isMute = false;

        bool hasExitTime = false;
        float exitTime = 0.f;

        bool hasFixedDuration = false;
        float duration = 0.f;

        std::weak_ptr<AnimatorState> destState;
        std::vector<std::shared_ptr<AnimatorCondition>> conditions;

    protected:
        std::string m_name;
    };
}
