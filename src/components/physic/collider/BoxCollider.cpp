#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "components/physic/collider/BoxCollider.h"
#include "components/physic/Rigidbody.h"
#include "scene/SceneObject.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    BoxCollider::BoxCollider(SceneObject& owner)
        : Collider(owner)
    {
    }

    //! Destructor
    BoxCollider::~BoxCollider()
    {
        destroyShape();
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
        destroyShape();
        if (m_size.LengthSqr() <= 0) {
            m_size = getOwner()->getAABB().getExtent() * 0.5f;
        }
        m_shape = std::make_unique<btBoxShape>(PhysicHelper::to_btVector3(m_size));
        setScale(m_scale);
        setMargin(m_margin);
    }

    //! Serialize
    void BoxCollider::to_json(json &j) const
    {
        Collider::to_json(j);
        j["size"] = getSize();
    }

    //! Serialize
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