#include "components/navigation/NavObstacle.h"

namespace ige::scene
{
    //! Constructor
    NavObstacle::NavObstacle(SceneObject &owner)
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
        }

        if(!enabled && m_bIsActivated)
        {
            getDeactivatedEvent().invoke(this);
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
} // namespace ige::scene
