#include "TransformComponent.h"
#include "SceneObject.h"

namespace ige::scene {
    TransformComponent::TransformComponent(std::shared_ptr<SceneObject> owner, const Vec3& pos, const Quat& rot, const Vec3& scale)
        : Component(owner), m_localPosition(pos), m_localRotation(rot), m_localScale(scale)
    {
        updateLocalMatrix();

        if(owner)
        {
            auto pTransform = owner->getComponent<TransformComponent>();
            if(pTransform)
            {
                setParent(pTransform->getParent());
            }
        }
    }

    TransformComponent::~TransformComponent() 
    {
        m_parent.reset();
        notifyObservers(ETransformMessage::TRANSFORM_DESTROYED);
    }

    void TransformComponent::setParent(std::shared_ptr<TransformComponent> parent)
    {
        if(m_parent.lock() != parent)
        {
            m_parent = parent;
            getParent()->addObserver(shared_from_this());

            updateWorldMatrix();
        }
    }

    void TransformComponent::removeParent()
    {
        if(hasParent())
        {
            getParent()->removeObserver(shared_from_this());

            m_parent.reset();
            updateWorldMatrix();
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
            updateLocalMatrix();
        }        
    }

    Vec3 TransformComponent::getPosition() const
    {
        return m_localPosition;
    }

    Vec3 TransformComponent::getWorldPosition() const
    {
        return m_worldPosition;
    }

    void TransformComponent::setRotation(const Quat& rot)
    {
        if(m_localRotation != rot)
        {
            m_localRotation = Quat(rot);
            updateLocalMatrix();
        }        
    }

    Quat TransformComponent::getRotation() const
    {
        return m_localRotation;
    }

    Quat TransformComponent::getWorldRotation() const
    {
        return m_worldRotation;
    }

    void TransformComponent::setScale(const Vec3& scale)
    {
        if(m_localScale != scale)
        {
            m_localScale = Vec3(scale);
            updateLocalMatrix();
        }     
    }

    Vec3 TransformComponent::getScale() const
    {
        return m_localScale;
    }

    Vec3 TransformComponent::getWorldScale() const
    {
        return m_worldScale;
    }

    Mat4 TransformComponent::getLocalMatrix() const
    {
        return m_localMatrix;
    }

    Mat4 TransformComponent::getWorldMatrix() const
    {
        return m_worldMatrix;
    }

    Vec3 TransformComponent::getLocalRight() const
    {
        return m_localPosition.xAxis();
    }

    Vec3 TransformComponent::getLocalUp() const
    {
        return m_localPosition.yAxis();
    }

    Vec3 TransformComponent::getLocalForward() const
    {
        return m_localPosition.zAxis();
    }

    Vec3 TransformComponent::getWorldRight() const
    {
        return m_worldPosition.xAxis();
    }

    Vec3 TransformComponent::getWorldUp() const
    {
        return m_worldPosition.yAxis();
    }

    Vec3 TransformComponent::getWorldForward() const
    {
        return m_worldPosition.zAxis();
    }

    void TransformComponent::updateLocalMatrix()
    {
        m_localMatrix.Identity();
        vmath_mat4_from_rottrans(m_localRotation.P(), m_localPosition.P(), m_localMatrix.P());
        vmath_mat_appendScale(m_localMatrix.P(), m_localScale.P(), 4, 4, m_localMatrix.P());

        updateWorldMatrix();
    }

    void TransformComponent::updateWorldMatrix()
    {
        m_worldMatrix = hasParent() ? getParent()->getWorldMatrix() * m_localMatrix : m_localMatrix;

        m_worldPosition.X(m_worldMatrix[0][3]);
        m_worldPosition.Y(m_worldMatrix[1][3]);
        m_worldPosition.Z(m_worldMatrix[2][3]);

        Vec3 columns[3] = 
        {
            { m_worldMatrix[0][0], m_worldMatrix[1][0], m_worldMatrix[2][0]},
            { m_worldMatrix[0][1], m_worldMatrix[1][1], m_worldMatrix[2][1]},
            { m_worldMatrix[0][2], m_worldMatrix[1][2], m_worldMatrix[2][2]},
        };

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

        Mat3 rotationMatrix(columns[0], columns[1], columns[2]);
        m_worldRotation = Quat(rotationMatrix);

        notifyObservers(ETransformMessage::TRANSFORM_CHANGED);
    }

    void TransformComponent::addObserver(std::shared_ptr<ITransformObserver> observer)
    {
        m_observers.emplace(observer);
    }

    void TransformComponent::removeObserver(std::shared_ptr<ITransformObserver> observer)
    {
        m_observers.erase(observer);
    }

    void TransformComponent::notifyObservers(const ETransformMessage &message)
    {
        std::for_each(m_observers.begin(), m_observers.end(), [&](auto observer) {
            observer->onNotified(message);
        });
    }

    void TransformComponent::onNotified(const ETransformMessage &message)
    {
        switch (message)
        {
        case ETransformMessage::TRANSFORM_CHANGED:
            updateWorldMatrix();
            break;

        case ETransformMessage::TRANSFORM_DESTROYED:
            m_localPosition = m_worldPosition;
            m_localRotation = m_worldRotation;
            m_localScale = m_worldScale;
            updateLocalMatrix();

            m_parent.reset();
            updateWorldMatrix();
            break;
        }
    }
}
