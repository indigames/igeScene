#include "components/physic/Collider.h"
#include "components/physic/Rigidbody.h"
#include "components/TransformComponent.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    Collider::Collider(SceneObject &owner)
        : Component(owner)
    {
    }

    //! Destructor
    Collider::~Collider()
    {
        auto rigidBody = getOwner()->getComponent<Rigidbody>();
        if (rigidBody) {
            rigidBody->destroyBody();
        }
    }

    //! Set local scale
    void Collider::setScale(const Vec3 &scale)
    {
        m_scale = scale;
        if (m_shape) {
            m_shape->setLocalScaling(PhysicHelper::to_btVector3(m_scale));
        }
    }

    void Collider::recreateShape() {
        createShape();

        auto body = getOwner()->getComponent<Rigidbody>();
        if (body) {
            body->recreateBody();
        }
    }

    std::unique_ptr<btCollisionShape>& Collider::getShape() {
        if (!m_shape) createShape();
        return m_shape;
    }

    //! Serialize
    void Collider::to_json(json &j) const
    {
        Component::to_json(j);
        j["scale"] = getScale();
    }

    //! Deserialize
    void Collider::from_json(const json &j)
    {
        Component::from_json(j);
        getShape(); // create shape just in case
        setScale(j.value("scale", Vec3(1.f, 1.f, 1.f)));
    }

    //! Update property by key value
    void Collider::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("scale") == 0)
            setScale(val);
        else
            Component::setProperty(key, val);
    }
} // namespace ige::scene