#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "components/gui/UIImage.h"
#include "components/gui/UIText.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"

#include <functional>


namespace ige::scene
{
    TransformComponent::TransformComponent(SceneObject &owner, const Vec3 &pos, const Quat &rot, const Vec3 &scale)
        : Component(owner), m_localPosition(pos), m_localRotation(rot), m_localScale(scale)
    {
        m_bLocalDirty = true;

        m_localMatrix.Identity();
        m_worldMatrix.Identity();

        m_worldPosition = pos;
        m_worldRotation = rot;
        m_worldScale = scale;

        if (getOwner()->getParent())
        {
            m_parent = getOwner()->getParent()->getTransform();
            if (getParent()) getParent()->addObserver(this);
        }
    }

    TransformComponent::~TransformComponent()
    {
        if (getParent()) getParent()->removeObserver(this);
        m_parent.reset();
        m_observers.clear();
        notifyObservers(ETransformMessage::TRANSFORM_DESTROYED);
    }

    std::shared_ptr<TransformComponent> TransformComponent::getParent() const
    {
        return m_parent.expired() ? nullptr : m_parent.lock();
    }

    //! Set parent transform
    void TransformComponent::setParent(std::shared_ptr<TransformComponent> comp)
    {
        if (getParent())
            getParent()->removeObserver(this);
        m_parent.reset();

        if (comp != nullptr) {
            m_parent = comp;
            if (getParent()) getParent()->addObserver(this);
        }

        if(!getOwner()->isGUIObject()) {
            updateWorldToLocal();
        }
    }

    void TransformComponent::onUpdate(float dt)
    {
        if (m_bLocalDirty)
        {
            updateLocalToWorld();
            m_bLocalDirty = false;
        }
    }

    void TransformComponent::localTranslate(const Vec3 &trans)
    {
        setLocalPosition(m_localPosition + trans);
    }

    void TransformComponent::translate(const Vec3 &trans)
    {
        setPosition(m_worldPosition + trans);
    }

    void TransformComponent::localRotate(const Quat &rot)
    {
        setLocalRotation(m_localRotation * rot);
    }

    void TransformComponent::rotate(const Quat &rot)
    {
        setRotation(m_worldRotation * rot);
    }

    void TransformComponent::localScale(const Vec3 &scale)
    {
        setLocalScale(Vec3(m_localScale.X() * scale.X(), m_localScale.Y() * scale.Y(), m_localScale.Z() * scale.Z()));
    }

    void TransformComponent::scale(const Vec3 &scale)
    {
        setScale(Vec3(m_worldScale.X() * scale.X(), m_worldScale.Y() * scale.Y(), m_worldScale.Z() * scale.Z()));
    }

    void TransformComponent::setLocalPosition(const Vec3 &pos)
    {
        if (m_localPosition != pos && !isLockMove())
        {
            m_localPosition = pos;
            m_bLocalDirty = true;
        }
    }

    const Vec3 &TransformComponent::getLocalPosition() const
    {
        return m_localPosition;
    }

    void TransformComponent::setPosition(const Vec3 &pos)
    {
        if (m_worldPosition != pos && !isLockMove())
        {
            m_worldPosition = pos;
            updateWorldToLocal();
        }
    }

    const Vec3 &TransformComponent::getPosition() const
    {
        return m_worldPosition;
    }

    void TransformComponent::setLocalRotation(const Quat &rot)
    {
        if (m_localRotation != rot && !isLockRotate())
        {
            m_localRotation = rot;
            m_bLocalDirty = true;
        }
    }

    void TransformComponent::setLocalRotation(const Vec3 &rot)
    {
        Quat rotQuat;
        vmath_eulerToQuat(rot.P(), rotQuat.P());

        if (m_localRotation != rotQuat && !isLockRotate())
        {
            m_localRotation = rotQuat;
            m_bLocalDirty = true;
        }
    }

    const Quat &TransformComponent::getLocalRotation() const
    {
        return m_localRotation;
    }

    void TransformComponent::setRotation(const Vec3& rot)
    {
        Quat rotQuat;
        vmath_eulerToQuat(rot.P(), rotQuat.P());

        if (m_worldRotation != rotQuat && !isLockRotate())
        {
            m_worldRotation = rotQuat;
            updateWorldToLocal();
        }
    }

    void TransformComponent::setRotation(const Quat &rot)
    {
        if (m_worldRotation != rot && !isLockRotate())
        {
            m_worldRotation = rot;
            updateWorldToLocal();
        }
    }

    const Quat &TransformComponent::getRotation() const
    {
        return m_worldRotation;
    }

    void TransformComponent::setLocalScale(const Vec3 &scale)
    {
        if (m_localScale != scale && !isLockScale())
        {
            m_localScale = scale;
            m_bLocalDirty = true;
        }
    }

    const Vec3 &TransformComponent::getLocalScale() const
    {
        return m_localScale;
    }

    void TransformComponent::setScale(const Vec3 &scale)
    {
        if (m_worldScale != scale && !isLockScale())
        {
            m_worldScale = scale;
            updateWorldToLocal();
        }
    }

    const Vec3 &TransformComponent::getScale() const
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
        m_worldMatrix = (getParent()) ? getParent()->getWorldMatrix() * m_localMatrix : m_localMatrix;

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
    }

    void TransformComponent::updateWorldToLocal()
    {
        // Update world matrix
        m_worldMatrix.Identity();
        vmath_mat4_from_rottrans(m_worldRotation.P(), m_worldPosition.P(), m_worldMatrix.P());
        vmath_mat_appendScale(m_worldMatrix.P(), m_worldScale.P(), 4, 4, m_worldMatrix.P());

        // Update local matrix
        m_localMatrix = getParent() ? getParent()->getWorldMatrix().Inverse() * m_worldMatrix : m_worldMatrix;

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
    }

    Vec3 TransformComponent::localToGlobal(Vec3 point) const
    {
        Mat4 localMatrix;
        localMatrix.Identity();
        vmath_mat4_from_rottrans(m_localRotation.P(), point.P(), localMatrix.P());
        vmath_mat_appendScale(localMatrix.P(), m_localScale.P(), 4, 4, localMatrix.P());

        auto worldMatrix = (getParent()) ? getParent()->getWorldMatrix() * localMatrix : localMatrix;
        Vec3 lpoint;
        lpoint.X(worldMatrix[3][0]);
        lpoint.Y(worldMatrix[3][1]);
        lpoint.Z(worldMatrix[3][2]);
        return lpoint;
    }

    Vec3 TransformComponent::globalToLocal(Vec3 point) const
    {
        // Update world matrix
        Mat4 worldMatrix;
        worldMatrix.Identity();
        vmath_mat4_from_rottrans(m_worldRotation.P(), point.P(), worldMatrix.P());
        vmath_mat_appendScale(worldMatrix.P(), m_worldScale.P(), 4, 4, worldMatrix.P());

        auto localMatrix = getParent() ? getParent()->getWorldMatrix().Inverse() * worldMatrix : worldMatrix;
        Vec3 lpoint;
        lpoint.X(localMatrix[3][0]);
        lpoint.Y(localMatrix[3][1]);
        lpoint.Z(localMatrix[3][2]);
        return lpoint;
    }

    void TransformComponent::makeDirty() 
    {
        m_bLocalDirty = true;
    }

    void TransformComponent::lockMove(bool active)
    {
        if (active)
            m_lockTransform |= (1 << ((1) - 1));
        else
            m_lockTransform ^= (1 << ((1) - 1));
    }
    void TransformComponent::lockRotate(bool active)
    {
        if (active)
            m_lockTransform |= (1 << ((2) - 1));
        else
            m_lockTransform ^= (1 << ((2) - 1));
    }
    void TransformComponent::lockScale(bool active)
    {
        if (active)
            m_lockTransform |= (1 << ((3) - 1));
        else
            m_lockTransform ^= (1 << ((3) - 1));
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
            if(observer != nullptr)
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
        Vec3 lRotEuler;
        vmath_quatToEuler(m_localRotation.P(), lRotEuler.P());

        Vec3 wRotEuler;
        vmath_quatToEuler(m_worldRotation.P(), wRotEuler.P());

        Component::to_json(j);
        j["pos"] = m_localPosition;
        j["rot"] = lRotEuler;
        j["scale"] = m_localScale;
        j["wpos"] = m_worldPosition;
        j["wrot"] = wRotEuler;
        j["wscale"] = m_worldScale;
    }

    //! Deserialize
    void TransformComponent::from_json(const json &j)
    {
        setLocalPosition(j.value("pos", Vec3()));
        setLocalRotation(j.value("rot", Vec3(0.f, 0.f, 0.f)));
        setLocalScale(j.value("scale", Vec3(1.f, 1.f, 1.f)));
        Component::from_json(j);
        onUpdate(0.f); // pre-warm
    }

    //! Update property by key value
    void TransformComponent::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("pos") == 0)
        {
            setLocalPosition(val);
        }
        else if (key.compare("rot") == 0)
        {
            setLocalRotation((Vec3)val);
        }
        else if (key.compare("scale") == 0)
        {
            setLocalScale(val);
        }
        else if (key.compare("wpos") == 0)
        {
            setPosition(val);
        }
        else if (key.compare("wrot") == 0)
        {
            setRotation((Vec3)val);
        }
        else if (key.compare("wscale") == 0)
        {
            setScale(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
        onUpdate(0.f);
    }
} // namespace ige::scene
