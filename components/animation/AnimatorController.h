#pragma once

#include "utils/Serialize.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{    
    class AnimatorStateMachine;

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

        //! Update
        virtual void update(float dt, bool ignoreTimeScale = false);

        //! Serialize
        friend void to_json(json &j, const AnimatorController &obj);

        //! Deserialize
        friend void from_json(const json &j, AnimatorController &obj);

        std::vector<std::shared_ptr<Animator>> animatorClips;

        std::shared_ptr<AnimatorStateMachine> stateMachine;

    protected:
        void initialize();
        void clear();

    protected:
        std::string m_path;
        float m_timeScale;
        BaseFigure* m_figure;
    };
} // namespace ige::scene
