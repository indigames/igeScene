#pragma once

#include "AnimatorCondition.h"

#include "utils/Serialize.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{    
    class AnimatorStateMachine;

    /**
     * Class AnimatorController
     */
    class AnimatorController : public std::enable_shared_from_this<AnimatorController>
    {
    public:
        AnimatorController();
        virtual ~AnimatorController();

        //! Path
        const std::string& getPath() const { return m_path; }
        void setPath(const std::string& path);

        //! Save
        void save(const std::string& path);

        //! Figure
        BaseFigure* getFigure() { return m_figure; }
        void setFigure(BaseFigure* figure);

        //! TimeScale
        float getTimeScale() const { return m_timeScale; }
        void setTimeScale(float ts);

        //! Parameters
        void setParameter(const std::string& param, int type, float value);
        std::pair<AnimatorParameterType, float> getParameter(const std::string& param) const;
        bool hasParameter(const std::string& param) const;
        bool removeParameter(const std::string& param);
        std::unordered_map<std::string, std::pair<AnimatorParameterType, float>>& getParameters() { return m_parameters; }

        //! Update
        virtual void update(float dt);

        //! Serialize
        friend void to_json(json &j, const AnimatorController &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorController &obj);

        //! State machine
        std::shared_ptr<AnimatorStateMachine>& getStateMachine() { return m_stateMachine; }

    protected:
        void initialize();
        void clear();

    protected:
        //! State machine
        std::shared_ptr<AnimatorStateMachine> m_stateMachine = nullptr;

        std::string m_path;
        float m_timeScale = 1.f;
        BaseFigure* m_figure = nullptr;
        std::unordered_map<std::string, std::pair<AnimatorParameterType, float>> m_parameters;
    };
} // namespace ige::scene
