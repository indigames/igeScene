#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "components/physic/PhysicSphere.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    PhysicSphere::PhysicSphere(SceneObject &owner, float radius)
        : PhysicObject(owner)
    {
        createCollisionShape(radius);
        init();
    }

    //! Destructor
    PhysicSphere::~PhysicSphere()
    {
        if (m_shape != nullptr)
            m_shape.reset();
    }

    //! Get radius
    float PhysicSphere::getRadius() const
    {
        return m_radius;
    }

    //! Set radius
    void PhysicSphere::setRadius(float radius)
    {
        if (m_radius != radius)
            recreateCollisionShape(radius);
    }

    //! Create collision shape
    void PhysicSphere::createCollisionShape(float radius)
    {
        // Create collision shape
        if (m_shape != nullptr)
            m_shape.reset();
        m_shape = std::make_unique<btSphereShape>(radius);
        m_radius = radius;

        m_bIsDirty = true;
        setLocalScale(m_previousScale);
        m_bIsDirty = false;
    }

    //! Recreate collision shape
    void PhysicSphere::recreateCollisionShape(float radius)
    {
        // Create collision shape
        if (m_shape != nullptr)
            m_shape.reset();
        createCollisionShape(radius);

        // Create body
        recreateBody();
    }

    //! Set local scale of the box
    void PhysicSphere::setLocalScale(const Vec3 &scale)
    {
        if (m_shape && (m_bIsDirty || m_previousScale != scale))
        {
            float radiusScale = std::max(std::max(scale[0], scale[1]), scale[2]);
            m_shape->setLocalScaling({radiusScale, radiusScale, radiusScale});
            m_previousScale = scale;
        }
    }

    //! Serialize
    void PhysicSphere::to_json(json &j) const
    {
        PhysicObject::to_json(j);
        j["radius"] = getRadius();
    }

    //! Deserialize
    void PhysicSphere::from_json(const json &j)
    {
        setRadius(j.at("radius"));
        PhysicObject::from_json(j);
    }

} // namespace ige::scene