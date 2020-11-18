
#include "components/audio/AudioListener.h"

namespace ige::scene
{
    //! Static members initialization
    Event<AudioListener &> AudioListener::m_onCreatedEvent;
    Event<AudioListener &> AudioListener::m_onDestroyedEvent;

    //! Constructor
    AudioListener::AudioListener(SceneObject &owner)
        : Component(owner)
    {
        m_onCreatedEvent.invoke(*this);
    }

    // Destructor
    AudioListener::~AudioListener()
    {
        m_onDestroyedEvent.invoke(*this);
    }
} // namespace ige::scene