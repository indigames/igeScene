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

        virtual std::shared_ptr<AnimatorCondition> addCondition(const std::string& param, AnimatorCondition::Mode mode, float threshold);
        virtual std::shared_ptr<AnimatorCondition> getCondition(uint64_t id);
        virtual bool removeCondition(const std::shared_ptr<AnimatorCondition>& condition);
        virtual bool removeCondition(uint64_t id);
 
        //! Serialize
        friend void to_json(json &j, const AnimatorTransition &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorTransition &obj);

        //! Serialize finished
        void onSerializeFinished(AnimatorState& state);

        //! Mute
        bool isMute = false;

        //! Exit Time: consider the normalized time of the animation before the parameters
        bool hasExitTime = false;
        float exitTime = 1.f;

        //! Fixed Duration: consider exit time in second
        bool hasFixedDuration = false;
        float duration = 0.f;

        //! Transition offset
        float offset = 0.f;

        //! Destination state
        std::weak_ptr<AnimatorState> destState;

        //! Dest state uuid: only used for serialization
        std::string destStateUUID = std::string();

        //! Conditions
        std::vector<std::shared_ptr<AnimatorCondition>> conditions;

        //! Cache the link id for editor
        uint64_t linkId = (uint64_t)-1;

    protected:
        std::string m_name;
        static std::atomic<uint64_t> s_condId;
    };
}
