#pragma once

#include "components\Component.h"
#include "event/Event.h"

namespace ige::scene
{
    /**
     * Class AudioListener
     */
    class AudioListener : public Component
    {
    public:
        AudioListener(const std::shared_ptr<SceneObject> &owner);
        virtual ~AudioListener();

        //! Get name
        virtual std::string getName() const override { return "AudioListener"; }

        //! Serialize
        virtual void to_json(json &j) const override;

        //! Deserialize
        virtual void from_json(const json &j) override;

        //! Enable/disable
        bool isEnabled() const { return m_bIsEnabled; }
        void setEnabled(bool enable = true);

        //! Created/Destroyed events
        static Event<AudioListener&>& getCreatedEvent() { return m_onCreatedEvent; }
        static Event<AudioListener&>& getDestroyedEvent() { return m_onDestroyedEvent; }

    protected:
        //! On created event
        static Event<AudioListener &> m_onCreatedEvent;

        //! On destroyed event
        static Event<AudioListener &> m_onDestroyedEvent;

    protected:
        //! Enable/disable
        bool m_bIsEnabled = false;
    };
} // namespace ige::scene
