
#include "AnimationClip.h"

namespace ige::scene
{    
    //! Constructor
    AnimationClip::AnimationClip(const std::string& path)
        : m_path(path)
    {
        auto figure = ResourceManager::GetResource(path, FIGURETYPE);
        if(figure != nullptr) {
            for (auto i = 0; i < figure->NumAnimations(); ++i) {
                auto animName = figure->GetEmbeddedAnimationName(i);
                auto hash = GenerateNameHash(animName);
                m_animators[animName] = figure->GetEmbeddedAnimator(hash);
            }
        }
    }

    // Destructor
    AnimationClip::~AnimationClip()
    {
        for(auto& animator: m_animators) {
            if(animator) animator->DecReference();
        }
        m_animators.clear();
    }

    //! Serialize component
    void to_json(json &j, const AnimationClip &obj)
    {
        
    }

    //! Deserialize component
    void from_json(const json &j, AnimationClip &obj)
    {
        
    }
   
} // namespace ige::scene