
#include "components/audio/AudioListener.h"

namespace ige::scene
{
    //! Static members initialization
    Event<AudioListener &> AudioListener::m_onCreatedEvent;
    Event<AudioListener &> AudioListener::m_onDestroyedEvent;

    //! Constructor
    AudioListener::AudioListener(const std::shared_ptr<SceneObject> &owner)
        : Component(owner)
    {
        m_onCreatedEvent.invoke(*this);
    }

    // Destructor
    AudioListener::~AudioListener()
    {
        m_onDestroyedEvent.invoke(*this);
    }

    //! Set enabled
    void AudioListener::setEnabled(bool enable)
    {
        if (m_bIsEnabled != enable)
            m_bIsEnabled = enable;
    }

    //! Serialize
    void AudioListener::to_json(json &j) const
    {
        j = json{
            {"isEnabled", m_bIsEnabled},
        };
    }

    //! Deserialize
    void AudioListener::from_json(const json &j)
    {
        setEnabled(j.value("isEnabled", false));
    }

} // namespace ige::scene