
#include "AnimatorController.h"

namespace ige::scene
{    
    //! Constructor
    AnimatorController::AnimatorController()
    {
        m_figure = nullptr;
    }

    // Destructor
    AnimatorController::~AnimatorController()
    {
        m_figure = nullptr;
    }


    void AnimatorController::setPath(const std::string& path)
    {
        if(m_path.compare(path) != 0) {
            //TODO: load state machines

            m_path = path;
        }
    }

    void AnimatorController::setFigure(BaseFigure* figure)
    {
        if(m_figure != figure) {
            m_figure = figure;
        }
    }

    //! Update
    void AnimatorController::update(float dt, bool ignoreTimeScale )
    {
        if(!m_figure) return;
    }


    //! Serialize component
    void to_json(json &j, const AnimatorController &obj)
    {
        
    }

    //! Deserialize component
    void from_json(const json &j, AnimatorController &obj)
    {
        
    }
   
} // namespace ige::scene