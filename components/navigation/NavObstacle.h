#pragma once

#include "components/Component.h"
#include "event/Event.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    class DynamicNavMesh;

    //! NavObstacle
    class NavObstacle : public Component
    {
    public:
        //! Constructor
        NavObstacle(SceneObject &owner);

        //! Destructor
        virtual ~NavObstacle();

        //! Get name
        std::string getName() const override { return "NavObstacle"; }

        //! Set enabled
        void setEnabled(bool enabled = true) override;

        //! Radius of this obstacle.
        float getRadius() const { return m_radius; }
        void setRadius(float radius);

        //! Height of this obstacle
        float getHeight() const { return m_height; }
        void setHeight(float height);

        //! Obstacle id received from tile cache.
        uint32_t getObstacleId() const { return m_obstacleId; }
        void setObstacleId(uint32_t id) { m_obstacleId = id; }

        //! Created/Destroyed events
        static Event<NavObstacle *> &getCreatedEvent() { return m_onCreatedEvent; }
        static Event<NavObstacle *> &getDestroyedEvent() { return m_onDestroyedEvent; }

        //! Activated/ Deactivated events
        static Event<NavObstacle *> &getActivatedEvent() { return m_onActivatedEvent; }
        static Event<NavObstacle *> &getDeactivatedEvent() { return m_onDeactivatedEvent; }

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

    protected:
        //! Radius of this obstacle.
        float m_radius = 1.f;

        //! Height of this obstacle
        float m_height = 1.f;

        //! Obstacle id received from tile cache.
        uint32_t m_obstacleId = 0;

        //! Cache the owner NavMesh
        DynamicNavMesh *m_ownerMesh = nullptr;

        //! Activated state
        bool m_bIsActivated = false;

        //! On created event
        static Event<NavObstacle *> m_onCreatedEvent;

        //! On destroyed event
        static Event<NavObstacle *> m_onDestroyedEvent;

        //! On activated event
        static Event<NavObstacle *> m_onActivatedEvent;

        //! On deactivated event
        static Event<NavObstacle *> m_onDeactivatedEvent;
    };
} // namespace ige::scene
