#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

namespace ige::scene {
    TransformComponent::TransformComponent(std::shared_ptr<SceneObject> owner, const Vec3& pos, const Quat& rot, const Vec3& scale)
        : Component(owner), m_localPosition(pos), m_localRotation(rot), m_localScale(scale)
    {
        m_bLocalDirty = true;

        if(owner && owner->getParent())
        {
            setParent(owner->getParent()->getComponent<TransformComponent>());
        }
    }

    TransformComponent::~TransformComponent() 
    {
        m_parent.reset();
        notifyObservers(ETransformMessage::TRANSFORM_DESTROYED);
    }

    void TransformComponent::setParent(std::shared_ptr<TransformComponent> parent)
    {
        m_parent = parent;
        getParent()->addObserver(this);
        m_bWorldDirty = true;
    }

    void TransformComponent::removeParent()
    {
        if(hasParent())
        {
            getParent()->removeObserver(this);
            m_parent.reset();
            m_bWorldDirty = true;
        }
    }

    void TransformComponent::onUpdate(float dt)
    {
        if (m_bLocalDirty)
        {
            updateLocalMatrix();
            m_bLocalDirty = false;
        }

        if (m_bWorldDirty)
        {
            updateWorldMatrix();
            m_bWorldDirty = false;
        }
    }

    void TransformComponent::translate(const Vec3& trans)
    {
        setPosition(m_localPosition + trans);
    }

    void TransformComponent::rotate(const Quat& rot)
    {
        setRotation(m_localRotation * rot);
    }

    void TransformComponent::scale(const Vec3& scale)
    {
        setScale(Vec3(m_localScale.X() * scale.X(), m_localScale.Y() * scale.Y(), m_localScale.Z() * scale.Z()));
    }

    void TransformComponent::setPosition(const Vec3& pos)
    {
        if(m_localPosition != pos)
        {
            m_localPosition = Vec3(pos);
            m_bLocalDirty = true;
        }        
    }

    const Vec3& TransformComponent::getPosition() const
    {
        return m_localPosition;
    }

    const Vec3& TransformComponent::getWorldPosition() const
    {
        return m_worldPosition;
    }

    void TransformComponent::setRotation(const Quat& rot)
    {
        if(m_localRotation != rot)
        {
            m_localRotation = Quat(rot);
            m_bLocalDirty = true;
        }        
    }

    const Quat& TransformComponent::getRotation() const
    {
        return m_localRotation;
    }

    const Quat& TransformComponent::getWorldRotation() const
    {
        return m_worldRotation;
    }

    void TransformComponent::setScale(const Vec3& scale)
    {
        if(m_localScale != scale)
        {
            m_localScale = Vec3(scale);
            m_bLocalDirty = true;
        }     
    }

    const Vec3& TransformComponent::getScale() const
    {
        return m_localScale;
    }

    const Vec3& TransformComponent::getWorldScale() const
    {
        return m_worldScale;
    }

    const Mat4& TransformComponent::getLocalMatrix() const
    {
        return m_localMatrix;
    }

    const Mat4& TransformComponent::getWorldMatrix() const
    {
        return m_worldMatrix;
    }

    const Vec3& TransformComponent::getLocalRight() const
    {
        return m_localPosition.xAxis();
    }

    const Vec3& TransformComponent::getLocalUp() const
    {
        return m_localPosition.yAxis();
    }

    const Vec3& TransformComponent::getLocalForward() const
    {
        return m_localPosition.zAxis();
    }

    const Vec3& TransformComponent::getWorldRight() const
    {
        return m_worldPosition.xAxis();
    }

    const Vec3& TransformComponent::getWorldUp() const
    {
        return m_worldPosition.yAxis();
    }

    const Vec3& TransformComponent::getWorldForward() const
    {
        return m_worldPosition.zAxis();
    }

    void TransformComponent::updateLocalMatrix()
    {
        m_localMatrix.Identity();
        vmath_mat4_from_rottrans(m_localRotation.P(), m_localPosition.P(), m_localMatrix.P());
        vmath_mat_appendScale(m_localMatrix.P(), m_localScale.P(), 4, 4, m_localMatrix.P());
        m_bWorldDirty = true;
    }

    void TransformComponent::updateWorldMatrix()
    {
        // Update world matrix
        m_worldMatrix = hasParent() ? getParent()->getWorldMatrix() * m_localMatrix : m_localMatrix;

        // Update world position
        m_worldPosition.X(m_worldMatrix[0][3]);
        m_worldPosition.Y(m_worldMatrix[1][3]);
        m_worldPosition.Z(m_worldMatrix[2][3]);

        Vec3 columns[3] = 
        {
            { m_worldMatrix[0][0], m_worldMatrix[1][0], m_worldMatrix[2][0]},
            { m_worldMatrix[0][1], m_worldMatrix[1][1], m_worldMatrix[2][1]},
            { m_worldMatrix[0][2], m_worldMatrix[1][2], m_worldMatrix[2][2]},
        };

        // Update world scale
        m_worldScale.X(columns[0].Length());
        m_worldScale.Y(columns[1].Length());
        m_worldScale.Z(columns[2].Length());

        if (m_worldScale.X())
        {
            columns[0] /= m_worldScale.X();
        }

        if (m_worldScale.Y())
        {
            columns[1] /= m_worldScale.Y();
        }

        if (m_worldScale.Z())
        {
            columns[2] /= m_worldScale.Z();
        }

        // Update world rotation
        Mat3 rotationMatrix(columns[0], columns[1], columns[2]);
        m_worldRotation = Quat(rotationMatrix);

        // Notify all children
        notifyObservers(ETransformMessage::TRANSFORM_CHANGED);
    }

    void TransformComponent::addObserver(TransformComponent* observer)
    {
        // Avoid add `this`, should never happened
        if(observer && observer != this) m_observers.emplace(observer);
    }

    void TransformComponent::removeObserver(TransformComponent* observer)
    {
        if(observer) m_observers.erase(observer);
    }

    void TransformComponent::notifyObservers(const ETransformMessage &message)
    {
        for( auto observer: m_observers) 
        {
            observer->onNotified(message);
        }
    }

    void TransformComponent::onNotified(const ETransformMessage &message)
    {
        switch (message)
        {
        case ETransformMessage::TRANSFORM_CHANGED:
            m_bWorldDirty = true;
            break;

        case ETransformMessage::TRANSFORM_DESTROYED:
            m_localPosition = m_worldPosition;
            m_localRotation = m_worldRotation;
            m_localScale = m_worldScale;
            m_bLocalDirty = true;

            m_parent.reset();
            m_bWorldDirty = true;
            break;
        }

        // Update new transform
        onUpdate(0.f);
    }
}
