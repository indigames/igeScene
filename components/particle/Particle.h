#pragma once

#include "components/Component.h"
#include "event/Event.h"

#include <Effekseer.h>

namespace ige::scene
{
    /**
     * Class Particle
     */
    class Particle : public Component
    {
    public:
        Particle(SceneObject &owner, const std::string &path = {});
        virtual ~Particle();

        //! Get name
        virtual std::string getName() const override { return "Particle"; }

        //! Enable/disable
        void setEnabled(bool enable = true) override;

        //! Get path
        const std::string &getPath() const { return m_path; }

        //! Set path
        void setPath(const std::string &path);

        //! Layer
        int getLayer() const { return m_layer; }
        void setLayer(int layer);

        //! Group mask
        int getGroupMask() const { return m_groupMask; }
        void setGroupMask(int mask);

        //! Speed
        float getSpeed() const { return m_speed; }
        void setSpeed(float speed);

        //! Time scale
        float getTimeScale() const { return m_timeScale; }
        void setTimeScale(float timeScale);

        //! Auto Drawing
        bool isAutoDrawing() const { return m_bIsAutoDrawing; }
        void setAutoDrawing(bool autoDraw);

        //! Loop
        bool isLooped() const;
        void setLoop(bool loop = true);

        //!Target Location
        const Vec3& getTargetLocation() const { return m_targetLocation; }
        void setTargetLocation(const Vec3& location);

        //! Dynamic input parameter
        const Vec4& getDynamicInputParameter() const { return m_dynamicInputParameter; }
        void setDynamicInputParameter(const Vec4& param);

        //! Dynamic input parameter
        const Vec4& getColor() const { return m_color; }
        void setColor(const Vec4& color);

        //! Handle
        int getHandle() const { return m_handle; }

        //! Play
        void play();

        //! Pause
        void pause();

        //! Resume
        void resume();

        //! Stop
        void stop();

        //! Created/Destroyed events
        static Event<Particle*>& getCreatedEvent() { return m_onCreatedEvent; }
        static Event<Particle*>& getDestroyedEvent() { return m_onDestroyedEvent; }

        //! Update function
        void onUpdate(float dt) override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! On created event
        static Event<Particle *> m_onCreatedEvent;

        //! On destroyed event
        static Event<Particle *> m_onDestroyedEvent;

    protected:
        //! Effect
        Effekseer::Effect* m_effect = nullptr;

        //! Particle handle
        int m_handle = -1;

        //! Path to effect file
        std::string m_path;

        //! Layer
        int m_layer = 0;

        //! Group mask
        int m_groupMask = 0;

        //! Speed
        float m_speed = 1.f;

        //! Time Scale
        float m_timeScale = 1.f;

        //! Auto drawing
        bool m_bIsAutoDrawing = true;

        //! Loop
        bool m_bIsLooped = false;

        //! Target location
        Vec3 m_targetLocation = {};

        //! Dynamic Input Parameter
        Vec4 m_dynamicInputParameter = {};

        //! Color
        Vec4 m_color = {1.f, 1.f, 1.f, 1.f};

        //! Cache last position
        Vec3 m_lastPosition = {};

        //! Cache last rotation
        Quat m_lastRotation = {};

        //! Cache last scale
        Vec3 m_lastScale = {};
    };
} // namespace ige::scene
