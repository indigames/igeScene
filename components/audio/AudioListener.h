#pragma once

#include "components/Component.h"
#include "event/Event.h"

namespace ige::scene
{
    /**
     * Class AudioListener
     */
    class AudioListener : public Component
    {
    public:
        AudioListener(SceneObject& owner);
        virtual ~AudioListener();

        //! Get name
        virtual std::string getName() const override { return "AudioListener"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::AudioListener; }

        //! Created/Destroyed events
        static Event<AudioListener&>& getCreatedEvent() { return m_onCreatedEvent; }
        static Event<AudioListener&>& getDestroyedEvent() { return m_onDestroyedEvent; }

    protected:
        //! On created event
        static Event<AudioListener &> m_onCreatedEvent;

        //! On destroyed event
        static Event<AudioListener &> m_onDestroyedEvent;
    };
} // namespace ige::scene
