#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "components/physic/collider/CompoundCollider.h"
#include "components/physic/Rigidbody.h"
#include "scene/SceneObject.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    CompoundCollider::CompoundCollider(SceneObject &owner)
        : Collider(owner)
    {
    }

    //! Destructor
    CompoundCollider::~CompoundCollider()
    {
        destroyShape();
    }

    //! Create collision shape
    void CompoundCollider::createShape()
    {
        // Create collision shape
        destroyShape();
        
        // TODO: create shape by looking for self collider and child collider as well
        auto rigidBody = getOwner()->getComponent<Rigidbody>();
        if (rigidBody) {
            auto compoundShape = std::make_unique<btCompoundShape>();
            m_shape = std::move(compoundShape);

            auto* shape = (btCompoundShape*)m_shape.get();
            auto compoundCollider = getOwner()->getComponent<CompoundCollider>();

            std::vector<std::shared_ptr<Collider>> colliders;
            getOwner()->getComponentsRecursive<Collider>(colliders);
            for (auto& collider : colliders) {
                if (collider->getType() != Component::Type::CompoundCollider && collider->getType() != Component::Type::MeshCollider && !collider->getOwner()->getComponent<Rigidbody>()) {
                    collider->setCompoundCollider(compoundCollider);
                    shape->addChildShape(PhysicHelper::to_btTransform(collider->getOwner()->getTransform()->getLocalRotation(), collider->getOwner()->getTransform()->getLocalPosition()), collider->getShape().get());
                }
            }

            if (m_shape) {
                setScale(m_scale);
            }
        }
    }


    //! Serialize
    void CompoundCollider::to_json(json &j) const
    {
       Collider::to_json(j);
    }

    //! Deserialize
    void CompoundCollider::from_json(const json &j)
    {
        Collider::from_json(j);
    }

    //! Update property by key value
    void CompoundCollider::setProperty(const std::string& key, const json& val)
    {
        Collider::setProperty(key, val);
    }

} // namespace ige::scene