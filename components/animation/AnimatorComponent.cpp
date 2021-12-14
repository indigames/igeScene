
#include "AnimatorComponent.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    Animator::Animator(SceneObject &owner)
        : Component(owner), m_figure(nullptr)
    {
    }

    // Destructor
    Animator::~Animator()
    {
    }

    //! Active when start runtime
    void Animator::Initialize()
    {        
    }

    //! Active when stop runtime
    void Animator::Clear()
    {        
    }

    // Update
    Animator::onUpdate( float dt)
    {
       if(m_updateMode == UpdateMode::Normal) {

       }
       else if(m_updateMode == UpdateMode::UnscaledTime) {

       }
    }

    // Fixed Update
    Animator::onFixedUpdate( float dt)
    {
        if(m_updateMode == UpdateMode::AnimatePhysic) {

        }
    }
} // namespace ige::scene