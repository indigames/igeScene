#include "AnimatorState.h"
#include "AnimatorStateMachine.h"
#include "AnimatorTransition.h"

#include "utils/GraphicsHelper.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene {
    AnimatorState::AnimatorState(): m_animator(nullptr) {
        m_uuid = generateUUID();
    }

    AnimatorState::~AnimatorState() {
        if(m_animator != nullptr) {
            m_animator->DecReference();
            m_animator = nullptr;
        }
    }

    void AnimatorState::enter()
    {
        if (m_animator) {
            m_animator->SetSpeed(m_speed);
            m_animator->SetStartTime(m_startTime);
            m_animator->SetEvalTime(m_evalTime);
            m_animator->SetLoop(m_isLoop);
            m_animator->Rewind();
        }
        getOnEnterEvent().invoke(*this);
    }

    void AnimatorState::exit()
    {
        getOnExitEvent().invoke(*this);
    }

    void AnimatorState::setName(const std::string& name)
    {
        if (m_type != Type::Normal)
            return;
        m_name = name;
    }

    void AnimatorState::setPath(const std::string& path)
    {
        if (m_type != Type::Normal)
            return;

        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        if (relPath.size() == 0) relPath = fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        if(m_path.compare(relPath) != 0) {
            m_path = relPath;
            m_name = fsPath.stem().string();
            if(m_animator != nullptr) m_animator->DecReference();
            m_animator = (Animator*)ResourceCreator::Instance().NewAnimator(m_path.c_str());
            m_animator->WaitInitialize();
        }
    }

    void AnimatorState::addTransition(const std::shared_ptr<AnimatorTransition>& transition)
    {
        transitions.push_back(transition);
    }

    bool AnimatorState::removeTransition(const std::shared_ptr<AnimatorTransition>& transition)
    {
        auto itr = std::find(transitions.begin(), transitions.end(), transition);
        if(itr != transitions.end()) {
            transitions.erase(itr);
            return true;
        }
        return false;
    }

    std::shared_ptr<AnimatorTransition> AnimatorState::addTransition(const std::shared_ptr<AnimatorState>& state, bool withExitTime)
    {
        auto transition = createTransition(withExitTime);
        transition->destState = state;
        transition->setName(getName() + "_" + state->getName());
        addTransition(transition);
        return transition;
    }

    std::shared_ptr<AnimatorTransition> AnimatorState::addExitTransition(bool withExitTime)
    {
        auto transition = createTransition(withExitTime);
        transition->destState = stateMachine.lock()->getExitState();
        addTransition(transition);
        return transition;
    }

    std::shared_ptr<AnimatorTransition> AnimatorState::findTransition(const std::shared_ptr<AnimatorState>& dstState)
    {
        auto itr = std::find_if(transitions.begin(), transitions.end(), [&](const auto& elem){
            return !elem->destState.expired() && elem->destState.lock() == dstState;
        });
        return (itr != transitions.end()) ? (*itr) : nullptr;
    }

    std::shared_ptr<AnimatorTransition> AnimatorState::createTransition(bool withExitTime)
    {
        auto newTransition = std::make_shared<AnimatorTransition>();
        if (withExitTime) {
            setDefaultExitTime(newTransition);
        }
        return newTransition;
    }

    void AnimatorState::setDefaultExitTime(std::shared_ptr<AnimatorTransition>& transition)
    {
        transition->hasExitTime = true;
        transition->exitTime = m_animator ? m_animator->GetEndTime() : 1.f;
        transition->duration = m_animator ? m_animator->GetTotalEvalTime() : 1.f;
    }

    void AnimatorState::setSpeed(float speed)
    {
        m_speed = speed;
        if (m_animator) {
            m_animator->SetSpeed(m_speed);
        }
    }

    void AnimatorState::setStartTime(float st)
    {
        m_startTime = st;
        if (m_animator) {
            m_animator->SetStartTime(m_startTime);
        }
    }

    void AnimatorState::setEvalTime(float et)
    {
        m_evalTime = et;
        if (m_animator) {
            m_animator->SetEvalTime(m_evalTime);
        }
    }
   
    void AnimatorState::setLoop(bool loop)
    {
        m_isLoop = loop;
        if (m_animator) {
            m_animator->SetLoop(m_isLoop);
        }
    }

    //! Serialize component
    void to_json(json &j, const AnimatorState &obj)
    {
        j["uuid"] = obj.getUUID();
        j["path"] = obj.getPath();
        j["name"] = obj.getName();
        j["type"] = (int)obj.getType();
        j["speed"] = obj.getSpeed();
        j["startTime"] = obj.getStartTime();
        j["evalTime"] = obj.getEvalTime();
        j["loop"] = obj.isLoop();
        j["pos"] = obj.getPosition();
        auto jTrans = json::array();
        for (const auto& tran : obj.transitions) {
            jTrans.push_back(json(*tran.get()));
        }
        j["trans"] = jTrans;
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorState &obj)
    {
        obj.setUUID(j.value("uuid", obj.getUUID()));        
        obj.setPath(j.value("path", std::string()));
        obj.setName(j.value("name", std::string()));
        obj.setType(j.value("type", (int)AnimatorState::Type::Normal));
        obj.setSpeed(j.value("speed", 1.f));
        obj.setStartTime(j.value("startTime", 0.f));
        obj.setEvalTime(j.value("evalTime", 0.f));
        obj.setLoop(j.value("loop", true));
        obj.setPosition(j.value("pos", Vec2(0.f, 0.f)));
        if (j.count("trans") > 0) {
            auto jTrans = j.at("trans");
            for (auto jTran : jTrans) {
                auto tran = std::make_shared<AnimatorTransition>();
                jTran.get_to(*tran);
                obj.transitions.push_back(tran);
            }
        }
    }

    //! Serialize finished handline
    void AnimatorState::onSerializeFinished() {
        for (const auto& tran : transitions) {
            tran->onSerializeFinished(*this);
        }
    }
}
