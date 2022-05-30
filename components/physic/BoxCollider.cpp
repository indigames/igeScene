#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "components/physic/BoxCollider.h"
#include "components/physic/Rigidbody.h"
#include "scene/SceneObject.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    BoxCollider::BoxCollider(SceneObject &owner, const Vec3 &size)
        : Collider(owner), m_size(size)
    {
    }

    //! Destructor
    BoxCollider::~BoxCollider()
    {
        if (m_shape != nullptr)
            m_shape.reset();
    }

    //! Get size
    const Vec3 &BoxCollider::getSize() const
    {
        return m_size;
    }

    //! Set size
    void BoxCollider::setSize(const Vec3 &size)
    {
        if (m_size != size) {
            m_size = size;
            recreateShape();
        }
    }

    //! Create collision shape
    void BoxCollider::createShape()
    {
        // Create collision shape
        if (m_shape != nullptr)
            m_shape.reset();
        m_shape = std::make_unique<btBoxShape>(PhysicHelper::to_btVector3(m_size));
        setScale(m_scale);
        auto body = getOwner()->getComponent<Rigidbody>();
        if (body) {
            body->recreateBody();
        }
    }

    //! Serialize
    void BoxCollider::to_json(json &j) const
    {
        Collider::to_json(j);
        j["size"] = getSize();
    }

    //! Deserialize
    void BoxCollider::from_json(const json &j)
    {
        Collider::from_json(j);
        setSize(j.value("size", Vec3(1.f, 1.f, 1.f)));
    }

    //! Update property by key value
    void BoxCollider::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("size") == 0)
            setSize(val);       
        else
            Collider::setProperty(key, val);
    }

} // namespace ige::scene