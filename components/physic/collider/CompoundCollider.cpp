#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "components/physic/collider/CompoundCollider.h"
#include "components/physic/Rigidbody.h"
#include "scene/SceneObject.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    CompoundCollider::CompoundCollider(SceneObject& owner)
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

            auto transform = getOwner()->getTransform();
            

            for (auto& collider : colliders) {
                if (collider->getType() != Component::Type::CompoundCollider && collider->getType() != Component::Type::MeshCollider && !collider->getOwner()->getComponent<Rigidbody>()) {
                    collider->setCompoundCollider(compoundCollider);

                    auto localMatrix = transform->getWorldMatrix().Inverse() * collider->getOwner()->getTransform()->getWorldMatrix();

                    // Update local position
                    Vec3 localPosition;
                    localPosition.X(localMatrix[3][0]);
                    localPosition.Y(localMatrix[3][1]);
                    localPosition.Z(localMatrix[3][2]);

                    Vec3 columns[3] = {
                        {localMatrix[0][0], localMatrix[0][1], localMatrix[0][2]},
                        {localMatrix[1][0], localMatrix[1][1], localMatrix[1][2]},
                        {localMatrix[2][0], localMatrix[2][1], localMatrix[2][2]}
                    };

                    // Update local scale
                    Vec3 localScale;
                    localScale.X(columns[0].Length());
                    localScale.Y(columns[1].Length());
                    localScale.Z(columns[2].Length());

                    if (localScale.X()) columns[0] /= localScale.X();
                    if (localScale.Y()) columns[1] /= localScale.Y();
                    if (localScale.Z()) columns[2] /= localScale.Z();

                    // Update local rotation
                    Mat3 rotationMatrix(columns[0], columns[1], columns[2]);
                    auto localRotation = Quat(rotationMatrix);

                    shape->addChildShape(PhysicHelper::to_btTransform(localRotation, localPosition), collider->getShape().get());
                }
            }

            if (m_shape) {
                setScale(m_scale);
                setMargin(m_margin);
            }
        }
    }


    //! Serialize
    void CompoundCollider::to_json(json &j) const
    {
       Collider::to_json(j);
    }

    //! Serialize finished event
    void CompoundCollider::onSerializeFinished(Scene* scene)
    {
        Collider::onSerializeFinished(scene);
        m_json.clear();
    }

    //! Update property by key value
    void CompoundCollider::setProperty(const std::string& key, const json& val)
    {
        Collider::setProperty(key, val);
    }

} // namespace ige::scene