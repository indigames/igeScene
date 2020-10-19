#pragma once

#include "components\Component.h"
#include "event/Event.h"

#include <soloud.h>

namespace ige::scene
{
    /**
     * Class AudioSource
     */
    class AudioSource : public Component
    {
    public:
        AudioSource(SceneObject &owner, const std::string &path = {}, bool stream = false);
        virtual ~AudioSource();

        //! Get name
        virtual std::string getName() const override { return "AudioSource"; }

        //! Serialize
        virtual void to_json(json &j) const override;

        //! Deserialize
        virtual void from_json(const json &j) override;

        //! Enable/disable
        bool isEnabled() const { return m_bIsEnabled; }
        void setEnabled(bool enable = true);

        //! Get play on enabled
        bool getPlayOnEnabled() const { return m_bPlayOnEnabled; }

        //! Set play on enabled
        void setPlayOnEnabled(bool enable = true) { m_bPlayOnEnabled = enable; }

        //! Get path
        const std::string &getPath() const { return m_path; }

        //! Set path
        void setPath(const std::string &path);

        //! Stream
        bool isStream() const { return m_bIsStream; }
        void setStream(bool stream = true);

        //! Loop
        bool isLooped() const;
        void setLoop(bool loop = true);

        //! Volume
        float getVolume() const;
        void setVolume(float volume, double fadingTime = 0.0f);

        //! Single instance
        bool isSingleInstance() const;
        void setSingleInstance(bool single = true);

        //! Pan
        float getPan() const;
        void setPan(float pan);

        //! Velocity
        const Vec3& getVelocity() const;
        void setVelocity(const Vec3& velocity);

        //! Min distance
        float getMinDistance() const;
        void setMinDistance(float distance);

        //! Max distance
        float getMaxDistance() const;
        void setMaxDistance(float distance);

        //! Attenuation model
        int getAttenuationModel() const;
        void setAttenuationModel(int model);

        //! Attenuation roll off factor
        float getAttenuationRollOffFactor() const;
        void setAttenuationRollOffFactor(float factor);

        //! Doppler factor
        float getDopplerFactor() const;
        void setDopplerFactor(float factor);

        //! Play
        void play();

        //! Seek
        void seek(double seconds = 0.1f);

        //! Pause
        void pause();

        //! Resume
        void resume();

        //! Stop
        void stop();

        //! Check if sound paused
        bool isPaused();

        //! Check if sound stopped
        bool isStopped();

        //! Created/Destroyed events
        static Event<AudioSource&>& getCreatedEvent() { return m_onCreatedEvent; }
        static Event<AudioSource&>& getDestroyedEvent() { return m_onDestroyedEvent; }

    protected:
        //! On created event
        static Event<AudioSource &> m_onCreatedEvent;

        //! On destroyed event
        static Event<AudioSource &> m_onDestroyedEvent;

    protected:
        //! Enable/disable
        bool m_bIsEnabled = false;

        //! Play on enabled
        bool m_bPlayOnEnabled = false;

        //! Path to audio file
        std::string m_path;

        //! Stream
        bool m_bIsStream = false;

        //! SoLoud Audio Source
        std::unique_ptr<SoLoud::AudioSource> m_audioSource = nullptr;

        //! SoLoud audio handle
        unsigned int m_handle = 0;

        //! Loop
        bool m_bIsLooped = false;

        //! Single instance
        bool m_bIsSingleInstance = false;

        //! Volume
        float m_volume = 1.f;

        //! Pan
        float m_pan = 0.f;

        //! Velocity
        Vec3 m_velocity = {};

        //! Min distance
        float m_minDistance = 0.f;

        //! Max distance
        float m_maxDistance = 10000.f;

        //! Attenuation model
        int m_attenuationModel = SoLoud::AudioSource::LINEAR_DISTANCE;

        //! Attenuation roll off factor
        float m_attenuationRollOffFactor = 0.5f;

        //! Doppler factor
        float m_dopplerFactor = 1.f;
    };
} // namespace ige::scene
