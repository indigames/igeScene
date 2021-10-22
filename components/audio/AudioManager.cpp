#include "components/audio/AudioManager.h"

#include <optional>
#include <algorithm>

#include "scene/SceneObject.h"

namespace ige::scene
{
    AudioManager::AudioManager(SceneObject &owner)
        : Component(owner)
    {
        m_engine = std::make_unique<SoLoud::Soloud>();
        m_engine->init();

        // Listen to AudioListener events
        AudioListener::getCreatedEvent().addListener(std::bind(static_cast<void(AudioManager::*)(AudioListener&)>(&AudioManager::onCreated), this, std::placeholders::_1));
        AudioListener::getDestroyedEvent().addListener(std::bind(static_cast<void(AudioManager::*)(AudioListener&)>(&AudioManager::onDestroyed), this, std::placeholders::_1));

        // Listen to AudioSource events
        AudioSource::getCreatedEvent().addListener(std::bind(static_cast<void(AudioManager::*)(AudioSource&)>(&AudioManager::onCreated), this, std::placeholders::_1));
        AudioSource::getDestroyedEvent().addListener(std::bind(static_cast<void(AudioManager::*)(AudioSource&)>(&AudioManager::onDestroyed), this, std::placeholders::_1));
    }

    AudioManager::~AudioManager()
    {
        // Clear audio sources
        for (AudioSource& audioSrc : m_audioSources)
            audioSrc.stop();
        m_audioSources.clear();

        // Unregister AudioListener events
        AudioListener::getCreatedEvent().removeAllListeners();
        AudioListener::getDestroyedEvent().removeAllListeners();

        // Clear audio listeners
        m_audioListeners.clear();

        // Unregister AudioSource events
        AudioSource::getCreatedEvent().removeAllListeners();
        AudioSource::getDestroyedEvent().removeAllListeners();

        // Stop audio engine
        m_engine->stopAll();
        m_engine->deinit();
        m_engine = nullptr;
    }

    //! Update
    void AudioManager::onUpdate(float dt)
    {
        Vec3 position = {0.f, 0.f, 0.f}, lookAt = {0.f, 0.f, -1.f};

        // Get position and lookAt vectors from active listener
        auto listenerOpt = getActiveListener();
        if(listenerOpt.has_value())
        {
            auto listener = (*listenerOpt).get();
            position = listener.getOwner()->getTransform()->getPosition();
            lookAt = listener.getOwner()->getTransform()->getWorldForward() * -1.0f;
        }

        // Listener position
        m_engine->set3dListenerPosition(position[0], position[1], position[2]);

        // Listener look-at vector
        m_engine->set3dListenerAt(lookAt[0], lookAt[1], lookAt[2]);

        // Update 3d audio
        m_engine->update3dAudio();
    }

    //! Get global volume
    float AudioManager::getGlobalVolume() const
    {
        return m_engine->getGlobalVolume();
    }

    //! Set global volume
    void AudioManager::setGlobalVolume(float volume)
    {
        m_engine->setGlobalVolume(volume);
    }

    //! AudioSource created/destroyed events
    void AudioManager::onCreated(AudioSource &source)
    {
        m_audioSources.push_back(std::ref(source));
    }

    void AudioManager::onDestroyed(AudioSource &source)
    {
        auto found = std::find_if(m_audioSources.begin(), m_audioSources.end(), [&source](AudioSource& element) {
            return std::addressof(source) == std::addressof(element);
        });

        if (found != m_audioSources.end())
            m_audioSources.erase(found);
    }

    //! AudioListener created/destroyed events
    void AudioManager::onCreated(AudioListener &listener)
    {
        m_audioListeners.push_back(std::ref(listener));
    }

    void AudioManager::onDestroyed(AudioListener &listener)
    {
        auto found = std::find_if(m_audioListeners.begin(), m_audioListeners.end(), [&listener](AudioListener& element) {
            return std::addressof(listener) == std::addressof(element);
        });

        if (found != m_audioListeners.end())
            m_audioListeners.erase(found);
    }

    //! Get active listener
    std::optional<std::reference_wrapper<AudioListener>> AudioManager::getActiveListener() const
    {
        auto found = std::find_if(m_audioListeners.begin(), m_audioListeners.end(), [](AudioListener& element) {
            return element.isEnabled();
        });
        if (found != m_audioListeners.end())
            return { *found };
        return {};
    }

    //! Serialize
    void AudioManager::to_json(json &j) const
    {
        Component::to_json(j);
        j["gVol"] = getGlobalVolume();

    }

    //! Deserialize
    void AudioManager::from_json(const json &j)
    {
        setGlobalVolume(j.value("gVol", 1.f));
        Component::from_json(j);
    }

    //! Update property by key value
    void AudioManager::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("gVol") == 0)
        {
            setGlobalVolume(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }
} // namespace ige::scene