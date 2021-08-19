
#include "components/audio/AudioSource.h"
#include "components/audio/AudioManager.h"
#include "scene/SceneObject.h"

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
    AudioSource::AudioSource(SceneObject &owner, const std::string &path, bool stream)
        : Component(owner), m_bIsStream(stream)
    {
        // Register audio manager
        if (getOwner()->getRoot())
        {
            auto manager = getOwner()->getRoot()->getComponent<AudioManager>();
            if (manager == nullptr) manager = getOwner()->getRoot()->addComponent<AudioManager>();
            setManager(manager);
        }

        // Set path
        if (!path.empty())
        {
            setPath(path);
        }

        // Invoke created event
        m_onCreatedEvent.invoke(*this);
    }

    //! Destructor
    AudioSource::~AudioSource()
    {
        m_onDestroyedEvent.invoke(*this);
        m_audioSource = nullptr;
        m_manager.reset();
    }

    //! Set enabled
    void AudioSource::setEnabled(bool enable)
    {
        Component::setEnabled(enable);

        if (isEnabled() && m_bPlayOnEnabled)
        {
            play();
        }

        if (!isEnabled())
        {
            stop();
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

            if (getManager() == nullptr) return;
            auto engine = getManager()->getEngine();
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
            if (!isStopped() && getManager())
            {
                auto engine = getManager()->getEngine();
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
            if (!isStopped() && getManager())
            {
                auto engine = getManager()->getEngine();
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
            if (!isStopped() && getManager())
            {
                auto engine = getManager()->getEngine();
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
            if (!isStopped() && getManager())
            {
                auto engine = getManager()->getEngine();
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

            if (!isStopped() && getManager())
            {
                auto engine = getManager()->getEngine();
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
            if (!isStopped() && getManager())
            {
                auto engine = getManager()->getEngine();
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
            if (!isStopped() && getManager())
            {
                auto engine = getManager()->getEngine();
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
            if (!isStopped() && getManager())
            {
                auto engine = getManager()->getEngine();
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
            if (!isStopped() && getManager())
            {
                auto engine = getManager()->getEngine();
                engine->set3dSourceDopplerFactor(m_handle, m_dopplerFactor);
            }
        }
    }

    //! Play
    void AudioSource::play()
    {
        if (getManager() == nullptr) return;
        auto engine = getManager()->getEngine();
        auto position = getOwner()->getTransform()->getWorldPosition();
        m_handle = engine->play3d(*m_audioSource.get(), position[0], position[0], position[2], m_velocity[0], m_velocity[1], m_velocity[2], m_volume);
        engine->setPan(m_handle, m_pan);
    }

    //! Seek
    void AudioSource::seek(double seconds)
    {
        if (getManager() == nullptr) return;
        if (!isStopped())
        {
            auto engine = getManager()->getEngine();
            engine->seek(m_handle, seconds);
        }
    }

    //! Pause
    void AudioSource::pause()
    {
        if (getManager() == nullptr) return;
        if (!isStopped())
        {
            auto engine = getManager()->getEngine();
            engine->setPause(m_handle, true);
        }
    }

    //! Resume
    void AudioSource::resume()
    {
        if (getManager() == nullptr) return;
        if (!isStopped())
        {
            auto engine = getManager()->getEngine();
            engine->setPause(m_handle, false);
        }
    }

    //! Stop
    void AudioSource::stop()
    {
        if (getManager() == nullptr) return;
        if (!isStopped())
        {
            auto engine = getManager()->getEngine();
            engine->stop(m_handle);
            m_handle = 0;
        }
    }

    //! Check if sound paused
    bool AudioSource::isPaused()
    {
        if (getManager() == nullptr) return false;
        auto engine = getManager()->getEngine();
        return (!isStopped()) && engine->getPause(m_handle);
    }

    //! Check if sound stopped
    bool AudioSource::isStopped()
    {
        return (m_handle == 0);
    }

    //! Serialize
    void AudioSource::to_json(json &j) const
    {
        Component::to_json(j);
        j["stream"] = m_bIsStream;
        j["path"] = m_path;
        j["play"] = m_bPlayOnEnabled;
        j["loop"] = m_bIsLooped;
        j["single"] = m_bIsSingleInstance;
        j["volume"] = m_volume;
        j["pan"] = m_pan;
        j["velocity"] = m_velocity;
        j["minDist"] = m_minDistance;
        j["maxDist"] = m_maxDistance;
        j["attModel"] = m_attenuationModel;
        j["attFactor"] = m_attenuationRollOffFactor;
        j["dopFactor"] = m_dopplerFactor;
    }

    //! Deserialize
    void AudioSource::from_json(const json &j)
    {
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
        Component::from_json(j);
    }

    //! Update property by key value
    void AudioSource::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("stream") == 0)
        {
            setStream(val);
        }
        else if (key.compare("path") == 0)
        {
            setPath(val);
        }
        else if (key.compare("play") == 0)
        {
            setPlayOnEnabled(val);
        }
        else if (key.compare("loop") == 0)
        {
            setLoop(val);
        }
        else if (key.compare("single") == 0)
        {
            setSingleInstance(val);
        }
        else if (key.compare("volume") == 0)
        {
            setVolume(val);
        }
        else if (key.compare("pan") == 0)
        {
            setPan(val);
        }
        else if (key.compare("velocity") == 0)
        {
            setVelocity(val);
        }
        else if (key.compare("minDist") == 0)
        {
            setMinDistance(val);
        }
        else if (key.compare("maxDist") == 0)
        {
            setMaxDistance(val);
        }
        else if (key.compare("attModel") == 0)
        {
            setAttenuationModel(val);
        }
        else if (key.compare("attFactor") == 0)
        {
            setAttenuationRollOffFactor(val);
        }
        else if (key.compare("dopFactor") == 0)
        {
            setDopplerFactor(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }
} // namespace ige::scene