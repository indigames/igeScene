#pragma once

#include <memory>
#include <vector>
#include <string>
#include <optional>

#include <soloud.h>

#include "event/Event.h"
#include "components/Component.h"
#include "components/audio/AudioSource.h"
#include "components/audio/AudioListener.h"

namespace ige::scene
{
    /**
     * Class AudioManager: Manage audio sources and listeners. Should be added to the root node.
     */
    class AudioManager : public Component
    {
    public:
        AudioManager(SceneObject &owner);
        virtual ~AudioManager();

        //! Get name
        std::string getName() const override { return "AudioManager"; }

        //! Update
        void onUpdate(float dt) override;

        //! Get global volume
        float getGlobalVolume() const;

        //! Set global volume
        void setGlobalVolume(float volume);

        //! Get sound engine
        SoLoud::Soloud* getEngine() const { return m_engine.get(); }

        //! Get active listener
        std::optional<std::reference_wrapper<AudioListener>> getActiveListener() const;;

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

        //! Check if multple edit allowed
        virtual bool canMultiEdit() override { return false; }

    protected:
        //! AudioSource created/destroyed events
        void onCreated(AudioSource &source);
        void onDestroyed(AudioSource &source);

        //! AudioListener created/destroyed events
        void onCreated(AudioListener &listener);
        void onDestroyed(AudioListener &listener);

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

    protected:
        //! Audio sources
        std::vector<std::reference_wrapper<AudioSource>> m_audioSources;

        //! Audio listeners
        std::vector<std::reference_wrapper<AudioListener>> m_audioListeners;

        //! SoLoud engine instance
        std::unique_ptr<SoLoud::Soloud> m_engine = nullptr;
    };
} // namespace ige::scene
