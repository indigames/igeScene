#pragma once

#include "utils/Serialize.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{    
    class AnimatorStateMachine;

    enum class AnimatorParameterType {
        Bool = 0,
        Float,
        Int,
        Trigger
    };

    /**
     * Class AnimatorController
     * 1. Drag anim to create state, one - one
     *  - stateName = clipName
     *  - stateDuration - clipDuration
     * 2. Create transition
     *  - add conditions
     * 3. Update
     *  - entry -> state machine -> exit
     *  - using scripts
     */
    class AnimatorController
    {
    public:
        AnimatorController(const std::string& path = {});
        virtual ~AnimatorController();

        //! Path
        const std::string& getPath() const { return m_path; }
        void setPath(const std::string& path);

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

        //! Update
        virtual void update(float dt);

        //! Serialize
        friend void to_json(json &j, const AnimatorController &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorController &obj);

        //! State machine
        std::shared_ptr<AnimatorStateMachine> stateMachine;

    protected:
        void initialize();
        void clear();

    protected:
        std::string m_path;
        float m_timeScale;
        BaseFigure* m_figure;
        std::unordered_map<std::string, std::pair<AnimatorParameterType, float>> m_parameters;
    };
} // namespace ige::scene
