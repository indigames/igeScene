#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "components/physic/collider/CapsuleCollider.h"
#include "components/physic/Rigidbody.h"
#include "scene/SceneObject.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    CapsuleCollider::CapsuleCollider(SceneObject &owner)
        : Collider(owner)
    {
        auto size = getOwner()->getAABB().getExtent() * 0.5f;
        if (m_radius <= 0.f) m_radius = size.X();
        if (m_height <= 0.f) m_height = size.Y();
    }

    //! Destructor
    CapsuleCollider::~CapsuleCollider()
    {
        destroyShape();
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
        destroyShape();
        m_shape = std::make_unique<btCapsuleShape>(m_radius, m_height);
        setScale(m_scale);
        setMargin(m_margin);
    }


    //! Serialize
    void CapsuleCollider::to_json(json &j) const
    {
       Collider::to_json(j);
       j["height"] = getHeight();
       j["radius"] = getRadius();
    }

    //! Serialize finished event
    void CapsuleCollider::onSerializeFinished(Scene* scene)
    {
        Collider::onSerializeFinished(scene);
        setHeight(m_json.value("height", 1.f));
        setRadius(m_json.value("radius", 1.f));
        m_json.clear();
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