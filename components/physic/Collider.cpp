#include "components/physic/Collider.h"
#include "components/physic/Rigidbody.h"
#include "components/physic/collider/CompoundCollider.h"
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
        destroyShape();

        if (!m_compoundCollider.expired()) {
            getCompoundCollider()->recreateShape();
        }
        m_compoundCollider.reset();
    }
    
    void Collider::init() {
        createShape();
        auto rigidBody = getOwner()->getComponent<Rigidbody>();
        if (rigidBody) {
            rigidBody->recreateBody();
        }
        else {
            if (getType() != Component::Type::MeshCollider && getType() != Component::Type::CompoundCollider) {
                auto compoundCollider = getOwner()->getFirstParentComponents<CompoundCollider>();
                if (compoundCollider) {
                    compoundCollider->recreateShape();
                }
            }
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

    void Collider::setMargin(float margin)
    {
        m_margin = margin;
        if (m_shape) {
            m_shape->setMargin(m_margin);
        }
    }

    void Collider::recreateShape() {
        createShape();
        if (m_shape) {
            if (getCompoundCollider()) {
                getCompoundCollider()->recreateShape();
            }
            else {
                auto rigidBody = getOwner()->getComponent<Rigidbody>();
                if (rigidBody) {
                    rigidBody->recreateBody();
                }
            }
        }
    }
    void Collider::destroyShape() {
        if (!getCompoundCollider()) {
            auto rigidBody = getOwner()->getComponent<Rigidbody>();
            if (rigidBody) {
                rigidBody->destroyBody();
            }
        }
        for (auto& shape : m_shapes) {
            shape = nullptr;
        }
        m_shapes.clear();
        m_shape.reset();
    }

    void Collider::setCompoundCollider(std::shared_ptr<CompoundCollider> collider) {
        m_compoundCollider = collider;
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
        j["margin"] = getMargin();
    }

    //! Deserialize
    void Collider::from_json(const json &j)
    {
        Component::from_json(j);
        m_json = json(j);
    }

    void Collider::onSerializeFinished(Scene* scene) {
        Component::onSerializeFinished(scene);
        setScale(m_json.value("scale", Vec3(1.f, 1.f, 1.f)));
        setMargin(m_json.value("margin", 0.025f));
        m_json.clear();
    }

    //! Update property by key value
    void Collider::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("scale") == 0)
            setScale(val);
        else if (key.compare("margin") == 0)
            setMargin(val);
        else
            Component::setProperty(key, val);
    }
} // namespace ige::scene