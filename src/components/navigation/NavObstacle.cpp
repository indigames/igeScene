#include "components/navigation/NavObstacle.h"

namespace ige::scene
{
    //! Initialize static members
    Event<NavObstacle*> NavObstacle::m_onCreatedEvent;
    Event<NavObstacle*> NavObstacle::m_onDestroyedEvent;
    Event<NavObstacle*> NavObstacle::m_onActivatedEvent;
    Event<NavObstacle*> NavObstacle::m_onDeactivatedEvent;

    //! Constructor
    NavObstacle::NavObstacle(SceneObject& owner)
        : Component(owner)
    {
        getCreatedEvent().invoke(this);
    }

    //! Destructor
    NavObstacle::~NavObstacle()
    {
        getDestroyedEvent().invoke(this);
    }

    //! Set enabled
    void NavObstacle::setEnabled(bool enabled)
    {
        Component::setEnabled(enabled);

        if(enabled && !m_bIsActivated)
        {
            getActivatedEvent().invoke(this);
            m_bIsActivated = true;
        }

        if(!enabled && m_bIsActivated)
        {
            getDeactivatedEvent().invoke(this);
            m_bIsActivated = false;
        }
    }

    //! Radius of this obstacle.
    void NavObstacle::setRadius(float radius)
    {
        if(m_bIsActivated)
            getDeactivatedEvent().invoke(this);

        m_radius = radius;

        if(m_bIsActivated)
            getActivatedEvent().invoke(this);
    }

    //! Height of this obstacle
    void NavObstacle::setHeight(float height)
    {
        if(m_bIsActivated)
            getDeactivatedEvent().invoke(this);

        m_height = height;

        if(m_bIsActivated)
            getActivatedEvent().invoke(this);
    }

    //! Serialize
    void NavObstacle::to_json(json &j) const
    {
        Component::to_json(j);
        j["radius"] = getRadius();
        j["height"] = getHeight();
    }

    //! Deserialize
    void NavObstacle::from_json(const json &j)
    {
        setRadius(j.value("radius", 1.f));
        setHeight(j.value("height", 1.f));
        Component::from_json(j);
    }

    //! Serialize finished event
    void NavObstacle::onSerializeFinished()
    {
        if (isEnabled()) {
            getActivatedEvent().invoke(this);
            m_bIsActivated = true;
        }
    }

    //! Update property by key value
    void NavObstacle::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("radius") == 0)
        {
            setRadius(val);
        }
        else if(key.compare("height") == 0)
        {
            setHeight(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }
} // namespace ige::scene
