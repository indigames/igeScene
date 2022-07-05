#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "components/physic/collider/SphereCollider.h"
#include "components/physic/Rigidbody.h"
#include "scene/SceneObject.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    SphereCollider::SphereCollider(SceneObject& owner)
        : Collider(owner)
    {
    }

    //! Destructor
    SphereCollider::~SphereCollider()
    {
        destroyShape();
    }

    //! Get radius
    float SphereCollider::getRadius() const
    {
        return m_radius;
    }

    //! Set radius
    void SphereCollider::setRadius(float radius)
    {
        if (m_radius != radius) {
            m_radius = radius;
            recreateShape();
        }
    }

    //! Create collision shape
    void SphereCollider::createShape()
    {
        // Create collision shape
        destroyShape();
        if (m_radius == 0.f) {
            auto size = getOwner()->getAABB().getExtent() * 0.5f;
            m_radius = std::max({ size.X(), size.Y(), size.Z() });
        }
        m_shape = std::make_unique<btSphereShape>(m_radius);
        setScale(m_scale);
        setMargin(m_margin);
    }

    //! Set local scale of the box
    void SphereCollider::setScale(const Vec3 &scale)
    {
        m_scale = scale;
        float radiusScale = std::max(std::max(scale[0], scale[1]), scale[2]);
        if(m_shape) m_shape->setLocalScaling({radiusScale, radiusScale, radiusScale});
    }

    //! Serialize
    void SphereCollider::to_json(json &j) const
    {
        Collider::to_json(j);
        j["radius"] = getRadius();
    }

    void SphereCollider::from_json(const json& j)
    {
        Collider::from_json(j);
        setRadius(j.value("radius", 1.f));
    }

    //! Update property by key value
    void SphereCollider::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("radius") == 0)
            setRadius(val);
        else
            Collider::setProperty(key, val);
    }
} // namespace ige::scene