#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "components/gui/UIImage.h"
#include "components/gui/UIText.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"

namespace ige::scene
{
    TransformComponent::TransformComponent(SceneObject &owner, const Vec3 &pos, const Quat &rot, const Vec3 &scale)
        : Component(owner), m_localPosition(pos), m_localRotation(rot), m_localScale(scale), m_parent(nullptr)
    {
        m_bLocalDirty = true;

        m_worldPosition = pos;
        m_worldRotation = rot;
        m_worldScale = scale;

        if (getOwner()->getParent())
        {
            m_parent = getOwner()->getParent()->getTransform().get();
            if (m_parent)
                m_parent->addObserver(this);
        }
        else
        {
            // Root object should not have bounding box
            m_aabbWorld = m_aabb = AABBox({0.f, 0.f, 0.f}, {0.f, 0.f, 0.f});
        }
    }

    TransformComponent::~TransformComponent()
    {
        if (m_parent)
            m_parent->removeObserver(this);
        m_parent = nullptr;
        notifyObservers(ETransformMessage::TRANSFORM_DESTROYED);
        m_observers.clear();
    }

    TransformComponent *TransformComponent::getParent() const
    {
        return m_parent;
    }

    //! Set parent transform
    void TransformComponent::setParent(TransformComponent *comp)
    {
        if (m_parent)
            m_parent->removeObserver(this);
        m_parent = comp;
        if (m_parent)
            m_parent->addObserver(this);
    }

    void TransformComponent::onUpdate(float dt)
    {
        if (m_bLocalDirty)
        {
            updateLocalToWorld();
            m_bLocalDirty = false;
        }

        if (m_bWorldDirty)
        {
            updateWorldToLocal();
            m_bWorldDirty = false;
        }
    }

    void TransformComponent::translate(const Vec3 &trans)
    {
        setPosition(m_localPosition + trans);
    }

    void TransformComponent::worldTranslate(const Vec3 &trans)
    {
        setWorldPosition(m_worldPosition + trans);
    }

    void TransformComponent::rotate(const Quat &rot)
    {
        setRotation(m_localRotation * rot);
    }

    void TransformComponent::worldRotate(const Quat &rot)
    {
        setWorldRotation(m_worldRotation * rot);
    }

    void TransformComponent::scale(const Vec3 &scale)
    {
        setScale(Vec3(m_localScale.X() * scale.X(), m_localScale.Y() * scale.Y(), m_localScale.Z() * scale.Z()));
    }

    void TransformComponent::worldScale(const Vec3 &scale)
    {
        setWorldScale(Vec3(m_worldScale.X() * scale.X(), m_worldScale.Y() * scale.Y(), m_worldScale.Z() * scale.Z()));
    }

    void TransformComponent::setPosition(const Vec3 &pos)
    {
        if (m_localPosition != pos)
        {
            m_localPosition = pos;
            m_bLocalDirty = true;
        }
    }

    const Vec3 &TransformComponent::getPosition() const
    {
        return m_localPosition;
    }

    void TransformComponent::setWorldPosition(const Vec3 &pos)
    {
        if (m_worldPosition != pos)
        {
            m_worldPosition = pos;
            m_bWorldDirty = true;
        }
    }

    const Vec3 &TransformComponent::getWorldPosition() const
    {
        return m_worldPosition;
    }

    void TransformComponent::setRotation(const Quat &rot)
    {
        if (m_localRotation != rot)
        {
            m_localRotation = rot;
            m_bLocalDirty = true;
        }
    }

    void TransformComponent::setRotation(const Vec3 &rot)
    {
        Quat rotQuat;
        vmath_eulerToQuat(rot.P(), rotQuat.P());

        if (m_localRotation != rotQuat)
        {
            m_localRotation = rotQuat;
            m_bLocalDirty = true;
        }
    }

    const Quat &TransformComponent::getRotation() const
    {
        return m_localRotation;
    }

    void TransformComponent::setWorldRotation(const Quat &rot)
    {
        if (m_worldRotation != rot)
        {
            m_worldRotation = rot;
            m_bWorldDirty = true;
        }
    }

    const Quat &TransformComponent::getWorldRotation() const
    {
        return m_worldRotation;
    }

    void TransformComponent::setScale(const Vec3 &scale)
    {
        if (m_localScale != scale)
        {
            m_localScale = scale;
            m_bLocalDirty = true;
        }
    }

    const Vec3 &TransformComponent::getScale() const
    {
        return m_localScale;
    }

    void TransformComponent::setWorldScale(const Vec3 &scale)
    {
        if (m_worldScale != scale)
        {
            m_worldScale = scale;
            m_bWorldDirty = true;
        }
    }

    const Vec3 &TransformComponent::getWorldScale() const
    {
        return m_worldScale;
    }

    const Mat4 &TransformComponent::getLocalMatrix() const
    {
        return m_localMatrix;
    }

    const Mat4 &TransformComponent::getWorldMatrix() const
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
        return m_worldRotation * Vec3(1.f, 0.f, 0.f);
    }

    Vec3 TransformComponent::getWorldUp() const
    {
        return m_worldRotation * Vec3(0.f, 1.f, 0.f);
    }

    Vec3 TransformComponent::getWorldForward() const
    {
        return m_worldRotation * Vec3(0.f, 0.f, 1.f);
    }

    void TransformComponent::updateLocalToWorld()
    {
        // Update local matrix
        m_localMatrix.Identity();
        vmath_mat4_from_rottrans(m_localRotation.P(), m_localPosition.P(), m_localMatrix.P());
        vmath_mat_appendScale(m_localMatrix.P(), m_localScale.P(), 4, 4, m_localMatrix.P());

        // Update world matrix
        m_worldMatrix = (m_parent) ? m_parent->getWorldMatrix() * m_localMatrix : m_localMatrix;

        // Update world position
        m_worldPosition.X(m_worldMatrix[3][0]);
        m_worldPosition.Y(m_worldMatrix[3][1]);
        m_worldPosition.Z(m_worldMatrix[3][2]);

        Vec3 columns[3] =
            {
                {m_worldMatrix[0][0], m_worldMatrix[0][1], m_worldMatrix[0][2]},
                {m_worldMatrix[1][0], m_worldMatrix[1][1], m_worldMatrix[1][2]},
                {m_worldMatrix[2][0], m_worldMatrix[2][1], m_worldMatrix[2][2]},
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

        // Fire transform changed event
        getOwner()->getTransformChangedEvent().invoke(*getOwner());

        // Update aabb
        updateAabb();
    }

    void TransformComponent::updateWorldToLocal()
    {
        // Update world matrix
        m_worldMatrix.Identity();
        vmath_mat4_from_rottrans(m_worldRotation.P(), m_worldPosition.P(), m_worldMatrix.P());
        vmath_mat_appendScale(m_worldMatrix.P(), m_worldScale.P(), 4, 4, m_worldMatrix.P());

        // Update local matrix
        m_localMatrix = m_parent ? m_parent->getWorldMatrix().Inverse() * m_worldMatrix : m_worldMatrix;

        // Update local position
        m_localPosition.X(m_localMatrix[3][0]);
        m_localPosition.Y(m_localMatrix[3][1]);
        m_localPosition.Z(m_localMatrix[3][2]);

        Vec3 columns[3] =
            {
                {m_localMatrix[0][0], m_localMatrix[0][1], m_localMatrix[0][2]},
                {m_localMatrix[1][0], m_localMatrix[1][1], m_localMatrix[1][2]},
                {m_localMatrix[2][0], m_localMatrix[2][1], m_localMatrix[2][2]},
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

        // Fire transform changed event
        getOwner()->getTransformChangedEvent().invoke(*getOwner());

        // Update aabb
        updateAabb();
    }

    void TransformComponent::updateAabb()
    {
        auto figureComp = getOwner()->getComponent<FigureComponent>();
        if (figureComp && figureComp->getFigure())
        {
            figureComp->onUpdate(0.33f);
            Vec3 aabbMin, aabbMax;
            figureComp->getFigure()->CalcAABBox(0, aabbMin.P(), aabbMax.P(), LocalSpace);
            m_aabb = { aabbMin, aabbMax };
        }
        else
        {
            auto spriteComp = getOwner()->getComponent<SpriteComponent>();
            if (spriteComp && spriteComp->getFigure())
            {
                spriteComp->onUpdate(0.33f);
                Vec3 aabbMin, aabbMax;
                spriteComp->getFigure()->CalcAABBox(0, aabbMin.P(), aabbMax.P());
                m_aabb = { aabbMin, aabbMax };
            }
            else
            {
                auto uiText = getOwner()->getComponent<UIText>();
                if (uiText && uiText->getFigure())
                {
                    uiText->onUpdate(0.33f);
                    Vec3 aabbMin, aabbMax;
                    uiText->getFigure()->CalcAABBox(0, aabbMin.P(), aabbMax.P());
                    m_aabb = { aabbMin, aabbMax };
                }
                else
                {
                    m_aabb = { {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f} };
                }
            }
        }

        // Recalculate world aabb
        m_aabbWorld = m_aabb.Transform(getWorldMatrix());
    }

    void TransformComponent::addObserver(TransformComponent *observer)
    {
        // Avoid add `this`, should never happened
        if (observer && observer != this)
            m_observers.emplace(observer);
    }

    void TransformComponent::removeObserver(TransformComponent *observer)
    {
        if (observer)
            m_observers.erase(observer);
    }

    void TransformComponent::notifyObservers(const ETransformMessage &message)
    {
        for (auto observer : m_observers)
        {
            observer->onNotified(message);
        }
    }

    void TransformComponent::onNotified(const ETransformMessage &message)
    {
        switch (message)
        {
        case ETransformMessage::TRANSFORM_CHANGED:
            m_bLocalDirty = true;
            break;

        case ETransformMessage::TRANSFORM_DESTROYED:
            m_localPosition = m_worldPosition;
            m_localRotation = m_worldRotation;
            m_localScale = m_worldScale;
            m_bLocalDirty = true;
            break;
        }
    }

    //! Serialize
    void TransformComponent::to_json(json &j) const
    {
        Component::to_json(j);
        j["pos"] = m_localPosition;
        j["rot"] = m_localRotation;
        j["scale"] = m_localScale;
    }

    //! Deserialize
    void TransformComponent::from_json(const json &j)
    {
        setPosition(j.value("pos", Vec3()));
        setRotation(j.value("rot", Quat()));
        setScale(j.value("scale", Vec3(1.f, 1.f, 1.f)));
        Component::from_json(j);
    }
} // namespace ige::scene
