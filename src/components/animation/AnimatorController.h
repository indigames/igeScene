#pragma once

#include "AnimatorCondition.h"

#include "utils/Serialize.h"

#include "utils/PyxieHeaders.h"

#include "core/Object.h"
using namespace pyxie;

namespace ige::scene
{    
    class AnimatorStateMachine;

    /**
     * Class AnimatorController
     */
    class AnimatorController : public std::enable_shared_from_this<AnimatorController>, Object
    {
    public:
        AnimatorController();
        virtual ~AnimatorController();

        //! Object Type
        std::string getType() const override { return "AnimatorController"; }

        //! Path
        const std::string& getPath() const { return m_path; }
        void setPath(const std::string& path);

        //! Base Model Path
        const std::string& getBaseModelPath() const { return m_baseModelPath; }
        void setBaseModelPath(const std::string& path);

        //! Save
        void save(const std::string& path);

        //! Figure
        BaseFigure* getFigure() { return m_figure; }
        void setFigure(BaseFigure* figure);

        //! TimeScale
        float getTimeScale() const { return m_timeScale; }
        void setTimeScale(float ts);

        //! Layer
        bool addLayer();
        bool removeLayer(int layer);

        //! Parameters
        void setParameter(const std::string& param, int type, float value);
        std::pair<AnimatorParameterType, float> getParameter(const std::string& param) const;
        bool hasParameter(const std::string& param) const;
        bool removeParameter(const std::string& param);
        std::unordered_map<std::string, std::pair<AnimatorParameterType, float>>& getParameters() { return m_parameters; }

        //! Update
        virtual void update(float dt);
        
        //! Serialize
        virtual void to_json(json& j);
        friend void to_json(json &j, const AnimatorController &obj);

        //! Deserialize
        virtual void from_json(const json& j);
        friend void from_json(const json &j, AnimatorController &obj);

        virtual void restore_from_json(const json& j);
        

        //! State machine
        std::vector<std::shared_ptr<AnimatorStateMachine>>& getStateMachines() { return m_stateMachines; }
        std::shared_ptr<AnimatorStateMachine> getStateMachine(int layer = 0);

        bool isDirty() const { return m_dirty; }
        void resetDirty() { m_dirty = false; }
    protected:
        void initialize();
        void clear();

    protected:
        //! State machine
        std::vector<std::shared_ptr<AnimatorStateMachine>> m_stateMachines = {};

        std::string m_path;
        std::string m_baseModelPath;
        float m_timeScale = 1.f;
        BaseFigure* m_figure = nullptr;
        std::unordered_map<std::string, std::pair<AnimatorParameterType, float>> m_parameters;
        bool m_dirty = false;
    };
} // namespace ige::scene
