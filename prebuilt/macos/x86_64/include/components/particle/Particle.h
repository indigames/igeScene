#pragma once

#include "components/Component.h"
#include "event/Event.h"

#include <Effekseer.h>

namespace ige::scene
{
    class ParticleManager;

    /**
     * Class Particle
     */
    class Particle : public Component
    {
    public:
        enum class Layer {
            _3D,
            _2D
        };

    public:
        Particle(SceneObject& owner, const std::string &path = {});
        virtual ~Particle();

        //! Get name
        virtual std::string getName() const override { return "Particle"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::Particle; }

        //! Cache ParticleManager
        std::shared_ptr<ParticleManager> getManager() const { return m_manager.expired() ? nullptr : m_manager.lock(); }
        void setManager(std::shared_ptr<ParticleManager> manager) { m_manager = manager; }

        //! Enable/disable
        void setEnabled(bool enable = true) override;

        //! Get path
        const std::string &getPath() const { return m_path; }

        //! Set path
        void setPath(const std::string &path);

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

        //! isPaused
        bool isPaused();

        //! isStopped
        bool isStopped();

        //! Created/Destroyed events
        static Event<Particle*>& getCreatedEvent() { return m_onCreatedEvent; }
        static Event<Particle*>& getDestroyedEvent() { return m_onDestroyedEvent; }

        //! Update function
        void onUpdate(float dt) override;

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

        //! Events
        Event<>& getFrameEndedEvent() { return m_frameEndedEvent; }

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Get transform matrix
        inline Effekseer::Matrix43 getEfkMatrix43();

        //! On created event
        static Event<Particle *> m_onCreatedEvent;

        //! On destroyed event
        static Event<Particle *> m_onDestroyedEvent;

    protected:
        //! Effect
        Effekseer::EffectRef m_effect = nullptr;

        //! Particle handle
        int m_handle = -1;

        //! Path to effect file
        std::string m_path;

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

        //! Cache ParticleManager
        std::weak_ptr<ParticleManager> m_manager;

        //! Events
        Event<> m_frameEndedEvent;

        //! Transform matrix
        Mat4 m_lastMatrix = {};
        Effekseer::Matrix43 m_lastMatrixEff = {};
    };
} // namespace ige::scene
