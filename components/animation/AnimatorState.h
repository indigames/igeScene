#pragma once

#include <string>
#include <vector>

#include "event/Event.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "utils/Serialize.h"

namespace ige::scene
{
    class AnimatorTransition;
    class AnimatorStateMachine;

    class AnimatorState : public std::enable_shared_from_this<AnimatorState>
    {
    public:
        AnimatorState();
        virtual ~AnimatorState();

        //! Path
        virtual const std::string& getPath() const { return m_path; }
        virtual void setPath(const std::string& path);

        //! Name
        virtual const std::string& getName() const { return m_name; }
        virtual void setName(const std::string& name) { m_name = name; }

        // Helper to get shared pointer from 'this'
        std::shared_ptr<AnimatorState> getSharedPtr() { return shared_from_this(); }

        //! Motion clip
        Animator* getAnimator() { return m_animator; }

        //! Motion speed
        float getSpeed() const { return m_speed; }
        void setSpeed(float speed);

        virtual void addTransition(const std::shared_ptr<AnimatorTransition>& transition);
        virtual bool removeTransition(const std::shared_ptr<AnimatorTransition>& transition);

        virtual std::shared_ptr<AnimatorTransition> addTransition(const std::shared_ptr<AnimatorState>& state, bool withExitTime = false);
        virtual std::shared_ptr<AnimatorTransition> addExitTransition(bool withExitTime = false);

        //! Find transition to dest state
        virtual std::shared_ptr<AnimatorTransition> findTransition(const std::shared_ptr<AnimatorState>& dstState);

        Event<AnimatorState&>& getOnEnterEvent() { return m_onEnterEvent; }
        Event<AnimatorState&>& getOnExitEvent() { return m_onExitEvent; }

        virtual void update(float dt);

        //! Serialize
        friend void to_json(json &j, const AnimatorState &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorState &obj);

    protected:
        virtual std::shared_ptr<AnimatorTransition> createTransition(bool withExitTime = false);
        virtual void setDefaultExitTime(std::shared_ptr<AnimatorTransition>& transition);

    public:
        std::weak_ptr<AnimatorStateMachine>stateMachine;
        std::weak_ptr<AnimatorState> destState;        
        std::vector<std::weak_ptr<AnimatorTransition>> transitions;

        bool isEnter = false;
        bool isExit = false;
        bool isAny = false;

    protected:
        Event<AnimatorState&> m_onEnterEvent;
        Event<AnimatorState&> m_onExitEvent;

        std::string m_name;
        std::string m_path;
        Animator* m_animator;
        float m_speed = 1.f;
    };
}
