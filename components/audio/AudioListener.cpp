
#include "components/audio/AudioListener.h"
#include "components/audio/AudioManager.h"
#include "components/Component.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Static members initialization
    Event<AudioListener &> AudioListener::m_onCreatedEvent;
    Event<AudioListener &> AudioListener::m_onDestroyedEvent;

    //! Constructor
    AudioListener::AudioListener(SceneObject &owner)
        : Component(owner)
    {
        // Ensure manager was added in the root node of the scene
        if(!getOwner()->getRoot()->getComponent<AudioManager>())
            getOwner()->getRoot()->addComponent<AudioManager>();

        // Invoke created event
        m_onCreatedEvent.invoke(*this);
    }

    // Destructor
    AudioListener::~AudioListener()
    {
        m_onDestroyedEvent.invoke(*this);
    }
} // namespace ige::scene