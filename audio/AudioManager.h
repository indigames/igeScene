#pragma once

#include <memory>
#include <vector>
#include <string>
#include <optional>

#include <soloud.h>

#include "utils/Singleton.h"
#include "event/Event.h"

#include "components/audio/AudioSource.h"
#include "components/audio/AudioListener.h"

namespace ige::scene
{
    /**
     * Class AudioManager: Manage audio sources and listeners
     */
    class AudioManager : public Singleton<AudioManager>
    {
    public:
        AudioManager();
        virtual ~AudioManager();

        //! Update
        void onUpdate(float dt);

        //! Get global volume
        float getGlobalVolume() const;

        //! Set global volume
        void setGlobalVolume(float volume);

        //! Get sound engine
        std::shared_ptr<SoLoud::Soloud> &getEngine() { return m_engine; }

        //! Get active listener
        std::optional<std::reference_wrapper<AudioListener>> getActiveListener() const;;

    protected:
        //! AudioSource created/destroyed events
        void onCreated(AudioSource &source);
        void onDestroyed(AudioSource &source);

        //! AudioListener created/destroyed events
        void onCreated(AudioListener &listener);
        void onDestroyed(AudioListener &listener);

    protected:
        //! Audio sources
        std::vector<std::reference_wrapper<AudioSource>> m_audioSources;

        //! Audio listeners
        std::vector<std::reference_wrapper<AudioListener>> m_audioListeners;

        //! SoLoud engine instance
        std::shared_ptr<SoLoud::Soloud> m_engine = nullptr;
    };
} // namespace ige::scene
