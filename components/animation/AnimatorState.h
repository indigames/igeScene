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
        enum class Type {
            Enter = 0,
            Exit,
            Any,
            Normal
        };

        AnimatorState(uint64_t id);
        virtual ~AnimatorState();

        //! ID
        uint64_t getId() { return m_id; }
        uint64_t getInputId() { return m_id + 1; }
        uint64_t getOutputId() { return m_id + 2; }

        //! UUID
        virtual const std::string& getUUID() const { return m_uuid; }
        void setUUID(const std::string& uuid) { m_uuid = uuid; }

        //! Name
        virtual const std::string& getName() const { return m_name; }
        virtual void setName(const std::string& name) { m_name = name; }

        //! Path
        virtual const std::string& getPath() const { return m_path; }
        virtual void setPath(const std::string& path);

        // Helper to get shared pointer from 'this'
        std::shared_ptr<AnimatorState> getSharedPtr() { return shared_from_this(); }

        //! Motion clip
        Animator* getAnimator() { return m_animator; }

        //! Motion speed
        float getSpeed() const { return m_speed; }
        void setSpeed(float speed);

        //! Motion start time
        float getStartTime() const { return m_startTime; }
        void setStartTime(float st);

        //! Motion evaluation time
        float getEvalTime() const { return m_evalTime; }
        void setEvalTime(float et);

        //! Motion loop
        bool isLoop() const { return m_isLoop; }
        void setLoop(bool loop);

        //! Position
        const Vec2& getPosition() const { return m_position; }
        void setPosition(const Vec2& pos) { m_position = pos; }

        //! Enter
        virtual void enter();

        //! Exit
        virtual void exit();

        virtual void addTransition(const std::shared_ptr<AnimatorTransition>& transition);
        virtual bool removeTransition(const std::shared_ptr<AnimatorTransition>& transition);

        virtual std::shared_ptr<AnimatorTransition> addTransition(const std::shared_ptr<AnimatorState>& state, bool withExitTime = false);
        virtual std::shared_ptr<AnimatorTransition> addExitTransition(bool withExitTime = false);

        //! Find transition to dest state
        virtual std::shared_ptr<AnimatorTransition> findTransition(const std::shared_ptr<AnimatorState>& dstState);

        Event<AnimatorState&>& getOnEnterEvent() { return m_onEnterEvent; }
        Event<AnimatorState&>& getOnExitEvent() { return m_onExitEvent; }

        //! Type
        Type getType() const { return m_type; }
        void setType(int type) { m_type = (Type)type; }

        //! Type check shortcut
        bool isEnter() const { return m_type == Type::Enter; }
        bool isExit() const { return m_type == Type::Exit; }
        bool isAny() const { return m_type == Type::Any; }

        //! Serialize
        friend void to_json(json &j, const AnimatorState &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorState &obj);
        
        //! Serialize finished handline
        void onSerializeFinished();

    protected:
        virtual std::shared_ptr<AnimatorTransition> createTransition(bool withExitTime = false);
        virtual void setDefaultExitTime(std::shared_ptr<AnimatorTransition>& transition);

    public:
        std::weak_ptr<AnimatorStateMachine>stateMachine;
        std::vector<std::shared_ptr<AnimatorTransition>> transitions;

    protected:
        Event<AnimatorState&> m_onEnterEvent;
        Event<AnimatorState&> m_onExitEvent;

        std::string m_name;
        std::string m_path;
        std::string m_uuid;

        Type m_type = Type::Normal;

        Animator* m_animator;
        float m_speed = 1.f;
        float m_startTime = 0.f;
        float m_evalTime = 0.f;
        bool m_isLoop = false;
        uint64_t m_id;
        Vec2 m_position;
    };
}
