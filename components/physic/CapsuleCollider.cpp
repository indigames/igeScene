#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "components/physic/CapsuleCollider.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    CapsuleCollider::CapsuleCollider(SceneObject &owner, float radius, float height)
        : Collider(owner), m_radius(radius), m_height(height)
    {
        createShape();
    }

    //! Destructor
    CapsuleCollider::~CapsuleCollider()
    {
        if (m_shape != nullptr)
            m_shape.reset();
    }

    //! Get radius
    float CapsuleCollider::getRadius() const
    {
        return m_radius;
    }

    //! Set radius
    void CapsuleCollider::setRadius(float radius)
    {
        if (m_radius != radius) {
            m_radius = radius;
            recreateShape();
        }
    }

    //! Get height
    float CapsuleCollider::getHeight() const
    {
        return m_height;
    }

    //! Set height
    void CapsuleCollider::setHeight(float height)
    {
        if (m_height != height) {
            m_height = height;
            recreateShape();
        }
    }

    //! Create collision shape
    void CapsuleCollider::createShape()
    {
        // Create collision shape
        if (m_shape != nullptr)
            m_shape.reset();
        m_shape = std::make_unique<btCapsuleShape>(m_radius, m_height);
        setScale(m_scale);
    }


    //! Serialize
    void CapsuleCollider::to_json(json &j) const
    {
       Collider::to_json(j);
       j["height"] = getHeight();
       j["radius"] = getRadius();
    }

    //! Deserialize
    void CapsuleCollider::from_json(const json &j)
    {
        Collider::from_json(j);
        setHeight(j.at("height"));
        setRadius(j.at("radius"));
    }

    //! Update property by key value
    void CapsuleCollider::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("height") == 0)
            setHeight(val);
        else if (key.compare("radius") == 0)
            setRadius(val);
        else
            Collider::setProperty(key, val);
    }

} // namespace ige::scene