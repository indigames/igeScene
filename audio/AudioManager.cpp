#include "audio/AudioManager.h"

#include <optional>
#include <algorithm>

#include "scene/SceneObject.h"

namespace ige::scene
{
    AudioManager::AudioManager()
    {
        m_engine = std::make_shared<SoLoud::Soloud>();
        m_engine->init();
    }

    AudioManager::~AudioManager()
    {
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
            auto listener = listenerOpt.value().get();
            position = listener.getOwner()->getTransform()->getWorldPosition();
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
        auto found = std::find_if(m_audioSources.begin(), m_audioSources.end(), [&source](std::reference_wrapper<AudioSource> element) {
            return std::addressof(source) == std::addressof(element.get());
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
        auto found = std::find_if(m_audioListeners.begin(), m_audioListeners.end(), [&listener](std::reference_wrapper<AudioListener> element) {
            return std::addressof(listener) == std::addressof(element.get());
        });

        if (found != m_audioListeners.end())
            m_audioListeners.erase(found);
    }

    //! Get active listener
    std::optional<std::reference_wrapper<AudioListener>> AudioManager::getActiveListener() const
    {
        auto found = std::find_if(m_audioListeners.begin(), m_audioListeners.end(), [](std::reference_wrapper<AudioListener> element) {
            return element.get().isEnabled();
        });
        if (found != m_audioListeners.end())
            return { *found };
        return {};
    }

} // namespace ige::scene