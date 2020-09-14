#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include "components/physic/PhysicCapsule.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    PhysicCapsule::PhysicCapsule(const std::shared_ptr<SceneObject> &owner, float radius, float height)
        : PhysicBase(owner)
    {
        createCollisionShape(radius, height);
        init();
    }

    //! Destructor
    PhysicCapsule::~PhysicCapsule()
    {
        if (m_shape != nullptr)
            m_shape.reset();
    }

    //! Get radius
    float PhysicCapsule::getRadius()
    {
        return m_radius;
    }

    //! Set radius
    void PhysicCapsule::setRadius(float radius)
    {
        if (m_radius != radius)
            recreateCollisionShape(radius, m_height);
    }

    //! Get height
    float PhysicCapsule::getHeight()
    {
        return m_height;
    }

    //! Set height
    void PhysicCapsule::setHeight(float height)
    {
        if (m_height != height)
            recreateCollisionShape(m_radius, height);
    }

    //! Create collision shape
    void PhysicCapsule::createCollisionShape(float radius, float height)
    {
        // Create collision shape
        if (m_shape != nullptr)
            m_shape.reset();
        m_shape = std::make_unique<btCapsuleShape>(radius, height);
        m_radius = radius;
        m_height = height;
    }

    //! Recreate collision shape
    void PhysicCapsule::recreateCollisionShape(float radius, float height)
    {
        // Create collision shape
        if (m_shape != nullptr)
            m_shape.reset();
        createCollisionShape(radius, height);

        // Create body
        recreateBody();
    }

    //! Set local scale of the box
    void PhysicCapsule::setLocalScale(const Vec3 &scale)
    {
        if (m_shape)
            m_shape->setLocalScaling(PhysicHelper::to_btVector3(scale));
    }
} // namespace ige::scene