#pragma once

#include <string>
#include <vector>

#include "AnimatorTransition.h"
#include "AnimatorState.h"
#include "AnimatorStateMachine.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
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

        virtual void addTransition(const std::shared_ptr<AnimatorTransition>& transition);
        virtual bool removeTransition(const std::shared_ptr<AnimatorTransition>& transition);

        virtual std::shared_ptr<AnimatorTransition> addTransition(const std::shared_ptr<AnimatorState>& state, bool withExitTime = false);
        virtual std::shared_ptr<AnimatorTransition> addTransition(const std::shared_ptr<AnimatorStateMachine>& machine, bool withExitTime = false);
        virtual std::shared_ptr<AnimatorTransition> addExitTransition(bool withExitTime = false);

        virtual std::shared_ptr<AnimatorTransition> findTransition(const std::shared_ptr<AnimatorState>& state);
        virtual std::shared_ptr<AnimatorStateMachine> findStateMachine(const std::shared_ptr<AnimatorStateMachine>& rootStateMachine);

        //! Serialize
        friend void to_json(json &j, const AnimatorState &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorState &obj);

    protected:
        virtual std::shared_ptr<AnimatorTransition> createTransition(bool withExitTime = false);
        virtual void setDefaultExitTime(std::shared_ptr<AnimatorTransition>& transition);

    public:
        std::weak_ptr<AnimatorState> destState;
        std::weak_ptr<AnimatorStateMachine> destStateMachine;        
        std::vector<std::weak_ptr<AnimatorTransition>> transitions;

    protected:
        std::string m_name;
        std::string m_path;
        Animator* m_animator;
        float m_motionSpeed = 1.f;
    };
}
