#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "components/physic/PhysicCapsule.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    PhysicCapsule::PhysicCapsule(SceneObject &owner, float radius, float height)
        : PhysicObject(owner)
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
    float PhysicCapsule::getRadius() const
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
    float PhysicCapsule::getHeight() const
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

        m_bIsDirty = true;
        setLocalScale(m_previousScale);
        m_bIsDirty = false;
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

    //! Serialize
    void PhysicCapsule::to_json(json &j) const
    {
        PhysicObject::to_json(j);
       j["height"] = getHeight();
       j["radius"] = getRadius();
    }

    //! Deserialize
    void PhysicCapsule::from_json(const json &j)
    {
        setHeight(j.at("height"));
        setRadius(j.at("radius"));
        PhysicObject::from_json(j);
    }

    //! Update property by key value
    void PhysicCapsule::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("height") == 0)
            setHeight(val);
        else if (key.compare("radius") == 0)
            setRadius(val);
        else
            PhysicObject::setProperty(key, val);
    }

} // namespace ige::scene