#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "scene/SceneObject.h"
#include "utils/RayOBBChecker.h"

namespace ige::scene {
    TransformComponent::TransformComponent(const std::shared_ptr<SceneObject>& owner, const Vec3& pos, const Quat& rot, const Vec3& scale)
        : Component(owner), m_localPosition(pos), m_localRotation(rot), m_localScale(scale), m_parent(nullptr)
    {
        m_bLocalDirty = true;

        if(owner && owner->getParent())
        {
            setParent(owner->getParent()->getTransform().get());
        }
    }

    TransformComponent::~TransformComponent() 
    {
        removeParent();
        notifyObservers(ETransformMessage::TRANSFORM_DESTROYED);
        m_observers.clear();
    }

    void TransformComponent::setParent(TransformComponent* parent)
    {
        removeParent();
        m_parent = parent;
        if(hasParent()) getParent()->addObserver(this);
        m_bWorldDirty = true;
    }

    void TransformComponent::removeParent()
    {
        if(hasParent())
        {
            if (hasParent()) getParent()->removeObserver(this);
            m_parent = nullptr;
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

        if(RayOBBChecker::isChecking())
        {
            bool intersected = false;
            float distance;

            auto owner = getOwner();
            auto figureComp = owner->getComponent<FigureComponent>();
            if (figureComp)
            {
                auto figure = figureComp->getFigure();
                if (figure)
                {
                    Vec3 aabbMin(-1.f, -1.f, -1.f), aabbMax(1.f, 1.f, 1.f);
                    figure->CalcAABBox(0, aabbMin.P(), aabbMax.P(), LocalSpace);
                    intersected = RayOBBChecker::checkIntersect(aabbMin, aabbMax, m_worldMatrix, distance);
                }
            }
            else
            {
                auto spriteComp = owner->getComponent<SpriteComponent>();
                if (spriteComp)
                {
                    auto figure = spriteComp->getFigure();
                    if (figure)
                    {
                        Vec3 aabbMin(-1.f, -1.f, -1.f), aabbMax(1.f, 1.f, 1.f);
                        figure->CalcAABBox(0, aabbMin.P(), aabbMax.P());
                        intersected = RayOBBChecker::checkIntersect(aabbMin, aabbMax, m_worldMatrix, distance);
                    }
                }
                else
                {
                    // bool intersected = RayOBBChecker::checkIntersect({ -1.0f, -1.0f, -1.0f }, { 1.0f,  1.0f,  1.0f }, m_worldMatrix, distance);
                    // getOwner()->setSelected(intersected);
                }
            }

            // Update selected info
            owner->setSelected(intersected);

            // Scene object found, no more checking
            if (intersected)
            {
                RayOBBChecker::setChecking(false);
            }
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
            m_localPosition = pos;
            m_bLocalDirty = true;
        }        
    }

    const Vec3& TransformComponent::getPosition() const
    {
        return m_localPosition;
    }

    void TransformComponent::setWorldPosition(const Vec3& pos)
    {
        if (m_worldPosition != pos)
        {
            m_worldPosition = pos;
            updateWorldToLocal();
        }
    }

    const Vec3& TransformComponent::getWorldPosition() const
    {
        return m_worldPosition;
    }

    void TransformComponent::setRotation(const Quat& rot)
    {
        if(m_localRotation != rot)
        {
            m_localRotation = rot;
            m_bLocalDirty = true;
        }        
    }

    const Quat& TransformComponent::getRotation() const
    {
        return m_localRotation;
    }

    void TransformComponent::setWorldRotation(const Quat& rot)
    {
        if (m_worldRotation != rot)
        {
            m_worldRotation = rot;
            updateWorldToLocal();
        }
    }

    const Quat& TransformComponent::getWorldRotation() const
    {
        return m_worldRotation;
    }

    void TransformComponent::setScale(const Vec3& scale)
    {
        if(m_localScale != scale)
        {
            m_localScale = scale;
            m_bLocalDirty = true;
        }     
    }

    const Vec3& TransformComponent::getScale() const
    {
        return m_localScale;
    }

    void TransformComponent::setWorldScale(const Vec3& scale)
    {
        if (m_worldScale != scale)
        {
            m_worldScale = scale;
            updateWorldToLocal();
        }
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
        m_bWorldDirty = true;
    }

    void TransformComponent::updateWorldMatrix()
    {
        // Update world matrix
        m_worldMatrix = (hasParent() && getParent()->getName() == "TransformComponent") ? getParent()->getWorldMatrix() * m_localMatrix : m_localMatrix;

        // Update world position
        m_worldPosition.X(m_worldMatrix[3][0]);
        m_worldPosition.Y(m_worldMatrix[3][1]);
        m_worldPosition.Z(m_worldMatrix[3][2]);

        Vec3 columns[3] = 
        {
            { m_worldMatrix[0][0], m_worldMatrix[0][1], m_worldMatrix[0][2]},
            { m_worldMatrix[1][0], m_worldMatrix[1][1], m_worldMatrix[1][2]},
            { m_worldMatrix[2][0], m_worldMatrix[2][1], m_worldMatrix[2][2]},
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

    void TransformComponent::updateWorldToLocal()
    {
        // Update world matrix
        m_worldMatrix.Identity();
        vmath_mat4_from_rottrans(m_worldRotation.P(), m_worldPosition.P(), m_worldMatrix.P());
        vmath_mat_appendScale(m_worldMatrix.P(), m_worldScale.P(), 4, 4, m_worldMatrix.P());

        // Update local matrix
        m_localMatrix = hasParent() ? getParent()->getWorldMatrix().Inverse() * m_worldMatrix : m_worldMatrix;

        // Update local position
        m_localPosition.X(m_localMatrix[3][0]);
        m_localPosition.Y(m_localMatrix[3][1]);
        m_localPosition.Z(m_localMatrix[3][2]);

        Vec3 columns[3] =
        {
            { m_localMatrix[0][0], m_localMatrix[0][1], m_localMatrix[0][2]},
            { m_localMatrix[1][0], m_localMatrix[1][1], m_localMatrix[1][2]},
            { m_localMatrix[2][0], m_localMatrix[2][1], m_localMatrix[2][2]},
        };

        // Update local scale
        m_localScale.X(columns[0].Length());
        m_localScale.Y(columns[1].Length());
        m_localScale.Z(columns[2].Length());

        if (m_localScale.X())
        {
            columns[0] /= m_localScale.X();
        }

        if (m_localScale.Y())
        {
            columns[1] /= m_localScale.Y();
        }

        if (m_localScale.Z())
        {
            columns[2] /= m_localScale.Z();
        }

        // Update local rotation
        Mat3 rotationMatrix(columns[0], columns[1], columns[2]);
        m_localRotation = Quat(rotationMatrix);

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

            m_parent = nullptr;
            m_bWorldDirty = true;
            break;
        }

        // Update new transform
        onUpdate(0.f);
    }

    //! Serialize
    void TransformComponent::to_json(json& j) const
    {
        j = json {
            {"pos", m_localPosition},
            {"rot", m_localRotation},
            {"scale", m_localScale},
        };
    }

    //! Deserialize
    void TransformComponent::from_json(const json& j)
    {
        if (hasOwner())
        {
            if(getOwner()->hasParent())
                setParent(getOwner()->getParent()->getTransform().get());
        }

        setPosition(j.at("pos"));
        setRotation(j.at("rot"));
        setScale(j.at("scale"));
    }
}
