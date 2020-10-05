
#include "components/audio/AudioSource.h"
#include "scene/SceneObject.h"
#include "audio/AudioManager.h"

#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_wavstream.h>

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Static members initialization
    Event<AudioSource &> AudioSource::m_onCreatedEvent;
    Event<AudioSource &> AudioSource::m_onDestroyedEvent;

    //! Constructor
    AudioSource::AudioSource(const std::shared_ptr<SceneObject> &owner, const std::string &path, bool stream)
        : Component(owner), m_bIsStream(stream)
    {
        if (!path.empty())
        {
            setPath(path);
        }
        m_onCreatedEvent.invoke(*this);
    }

    //! Destructor
    AudioSource::~AudioSource()
    {
        m_onDestroyedEvent.invoke(*this);
        m_audioSource = nullptr;
    }

    //! Set enabled
    void AudioSource::setEnabled(bool enable)
    {
        if (m_bIsEnabled != enable)
        {
            m_bIsEnabled = enable;

            if (m_bPlayOnEnabled && m_bIsEnabled)
            {
                play();
            }

            if (!m_bIsEnabled)
            {
                stop();
            }
        }
    }

    //! Set path
    void AudioSource::setPath(const std::string &path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        if (strcmp(m_path.c_str(), relPath.c_str()) != 0)
        {
            m_path = relPath;
            m_audioSource = nullptr;

            auto engine = AudioManager::getInstance()->getEngine();
            if (m_bIsStream)
            {
                auto source = std::make_unique<SoLoud::WavStream>();
                source->load(m_path.c_str());
                m_audioSource = std::move(source);
            }
            else
            {
                auto source = std::make_unique<SoLoud::Wav>();
                source->load(m_path.c_str());
                m_audioSource = std::move(source);
            }

            if (m_audioSource)
            {
                m_audioSource->setLooping(m_bIsLooped);
                m_audioSource->setSingleInstance(m_bIsSingleInstance);
                m_audioSource->setVolume(m_volume);
                m_audioSource->set3dMinMaxDistance(m_minDistance, m_maxDistance);
                m_audioSource->set3dAttenuation(m_attenuationModel, m_attenuationRollOffFactor);
                m_audioSource->set3dDopplerFactor(m_dopplerFactor);
            }
        }
    }

    //! Stream
    void AudioSource::setStream(bool stream)
    {
        m_bIsStream = stream;
    }

    //! Check loop
    bool AudioSource::isLooped() const
    {
        return m_bIsLooped;
    }

    //! Set loop
    void AudioSource::setLoop(bool loop)
    {
        if (m_bIsLooped != loop)
        {
            m_bIsLooped = loop;
            if (m_audioSource)
                m_audioSource->setLooping(m_bIsLooped);
            if (m_handle != 0)
            {
                auto engine = AudioManager::getInstance()->getEngine();
                engine->setLooping(m_handle, m_bIsLooped);
            }
        }
    }

    //! Get volume
    float AudioSource::getVolume() const
    {
        return m_volume;
    }

    //! Set volume
    void AudioSource::setVolume(float volume, double fadingTime)
    {
        if (m_volume != volume)
        {
            m_volume = volume;
            if (m_handle != 0)
            {
                auto engine = AudioManager::getInstance()->getEngine();
                engine->fadeVolume(m_handle, m_volume, fadingTime);
            }
        }
    }

    //! Single instance
    bool AudioSource::isSingleInstance() const
    {
        return m_bIsSingleInstance;
    }

    void AudioSource::setSingleInstance(bool single)
    {
        if (m_bIsSingleInstance != single)
        {
            m_bIsSingleInstance = single;
            if (m_audioSource)
                m_audioSource->setSingleInstance(m_bIsSingleInstance);
        }
    }

    //! Pan
    float AudioSource::getPan() const
    {
        return m_pan;
    }

    void AudioSource::setPan(float pan)
    {
        if (m_pan != pan)
        {
            m_pan = pan;
            if (m_handle != 0)
            {
                auto engine = AudioManager::getInstance()->getEngine();
                engine->setPan(m_handle, m_pan);
            }
        }
    }

    //! Velocity
    const Vec3 &AudioSource::getVelocity() const
    {
        return m_velocity;
    }

    void AudioSource::setVelocity(const Vec3 &velocity)
    {
        if (m_velocity != velocity)
        {
            m_velocity = velocity;
            if (m_handle != 0)
            {
                auto engine = AudioManager::getInstance()->getEngine();
                engine->set3dSourceVelocity(m_handle, m_velocity[0], m_velocity[1], m_velocity[2]);
            }
        }
    }

    //! Min distance
    float AudioSource::getMinDistance() const
    {
        return m_minDistance;
    }

    void AudioSource::setMinDistance(float distance)
    {
        if (m_minDistance != distance)
        {
            m_minDistance = distance;

            if (m_audioSource != nullptr)
            {
                m_audioSource->set3dMinMaxDistance(m_minDistance, m_maxDistance);
            }

            if (m_handle != 0)
            {
                auto engine = AudioManager::getInstance()->getEngine();
                engine->set3dSourceMinMaxDistance(m_handle, m_minDistance, m_maxDistance);
            }
        }
    }

    //! Max distance
    float AudioSource::getMaxDistance() const
    {
        return m_maxDistance;
    }

    void AudioSource::setMaxDistance(float distance)
    {
        if (m_maxDistance != distance)
        {
            m_maxDistance = distance;

            if (m_audioSource != nullptr)
            {
                m_audioSource->set3dMinMaxDistance(m_minDistance, m_maxDistance);
            }

            if (m_handle != 0)
            {
                auto engine = AudioManager::getInstance()->getEngine();
                engine->set3dSourceMinMaxDistance(m_handle, m_minDistance, m_maxDistance);
            }
        }
    }

    //! Attenuation model
    int AudioSource::getAttenuationModel() const
    {
        return m_attenuationModel;
    }

    void AudioSource::setAttenuationModel(int model)
    {
        if (m_attenuationModel != model)
        {
            m_attenuationModel = model;

            if (m_audioSource != nullptr)
            {
                m_audioSource->set3dAttenuation(m_attenuationModel, m_attenuationRollOffFactor);
            }

            if (m_handle != 0)
            {
                auto engine = AudioManager::getInstance()->getEngine();
                engine->set3dSourceAttenuation(m_handle, m_attenuationModel, m_attenuationRollOffFactor);
            }
        }
    }

    //! Attenuation roll off factor
    float AudioSource::getAttenuationRollOffFactor() const
    {
        return m_attenuationRollOffFactor;
    }

    void AudioSource::setAttenuationRollOffFactor(float factor)
    {
        if (m_attenuationRollOffFactor != factor)
        {
            m_attenuationRollOffFactor = factor;

            if (m_audioSource != nullptr)
            {
                m_audioSource->set3dAttenuation(m_attenuationModel, m_attenuationRollOffFactor);
            }

            if (m_handle != 0)
            {
                auto engine = AudioManager::getInstance()->getEngine();
                engine->set3dSourceAttenuation(m_handle, m_attenuationModel, m_attenuationRollOffFactor);
            }
        }
    }

    //! Doppler factor
    float AudioSource::getDopplerFactor() const
    {
        return m_dopplerFactor;
    }

    void AudioSource::setDopplerFactor(float factor)
    {
        if (m_dopplerFactor != factor)
        {
            m_dopplerFactor = factor;

            if (m_audioSource != nullptr)
            {
                m_audioSource->set3dDopplerFactor(m_dopplerFactor);
            }

            if (m_handle != 0)
            {
                auto engine = AudioManager::getInstance()->getEngine();
                engine->set3dSourceDopplerFactor(m_handle, m_dopplerFactor);
            }
        }
    }

    //! Play
    void AudioSource::play()
    {
        auto engine = AudioManager::getInstance()->getEngine();
        auto position = getOwner()->getTransform()->getWorldPosition();
        m_handle = engine->play3d(*m_audioSource.get(), position[0], position[0], position[2], m_velocity[0], m_velocity[1], m_velocity[2], m_volume);
        if (m_handle != 0)
        {
            engine->setPan(m_handle, m_pan);
        }
    }

    //! Seek
    void AudioSource::seek(double seconds)
    {
        if (m_handle != 0)
        {
            auto engine = AudioManager::getInstance()->getEngine();
            engine->seek(m_handle, seconds);
        }
    }

    //! Pause
    void AudioSource::pause()
    {
        if (m_handle != 0)
        {
            auto engine = AudioManager::getInstance()->getEngine();
            engine->setPause(m_handle, true);
        }
    }

    //! Resume
    void AudioSource::resume()
    {
        if (m_handle != 0)
        {
            auto engine = AudioManager::getInstance()->getEngine();
            engine->setPause(m_handle, false);
        }
    }

    //! Stop
    void AudioSource::stop()
    {
        if (m_handle != 0)
        {
            auto engine = AudioManager::getInstance()->getEngine();
            engine->stop(m_handle);
            m_handle = (unsigned int)(-1);
        }
    }

    //! Check if sound paused
    bool AudioSource::isPaused()
    {
        auto engine = AudioManager::getInstance()->getEngine();
        return (m_handle != 0) && engine->getPause(m_handle);
    }

    //! Check if sound stopped
    bool AudioSource::isStopped()
    {
        return m_handle == (unsigned int)(-1);
    }

    //! Serialize
    void AudioSource::to_json(json &j) const
    {
        j = json{
            {"isEnabled", m_bIsEnabled},
            {"stream", m_bIsStream},
            {"path", m_path},
            {"play", m_bPlayOnEnabled},
            {"loop", m_bIsLooped},
            {"single", m_bIsSingleInstance},
            {"volume", m_volume},
            {"pan", m_pan},
            {"velocity", m_velocity},
            {"minDist", m_minDistance},
            {"maxDist", m_maxDistance},
            {"attModel", m_attenuationModel},
            {"attFactor", m_attenuationRollOffFactor},
            {"dopFactor", m_dopplerFactor},
        };
    }

    //! Deserialize
    void AudioSource::from_json(const json &j)
    {
        setEnabled(j.value("isEnabled", false));
        setStream(j.value("stream", false));
        setPath(j.value("path", std::string()));
        setPlayOnEnabled(j.value("play", false));
        setLoop(j.value("loop", false));
        setSingleInstance(j.value("single", false));
        setVolume(j.value("volume", 1.f));
        setPan(j.value("pan", 0.f));
        setVelocity(j.value("velocity", Vec3()));
        setMinDistance(j.value("minDist", 0.f));
        setMaxDistance(j.value("maxDist", 10000.f));
        setAttenuationModel(j.value("attModel", SoLoud::AudioSource::LINEAR_DISTANCE));
        setAttenuationRollOffFactor(j.value("attFactor", 0.5f));
        setDopplerFactor(j.value("dopFactor", 1.f));
    }

} // namespace ige::scene