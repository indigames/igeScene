#pragma once

#include <set>

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"

namespace ige::scene {

    //! Transform messages to childrens
    enum class ETransformMessage
    {
        TRANSFORM_CHANGED,
        TRANSFORM_DESTROYED
    };

    //! Transform observable(as parent) and also observer(as child)
    class TransformComponent: public Component
    {
    public:
        //! Constructor
        TransformComponent(const std::shared_ptr<SceneObject>& owner, const Vec3& pos = Vec3(), const Quat& rot = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));

        //! Destructor
        virtual ~TransformComponent();

        //! Get component name
        virtual std::string getName() const override { return "TransformComponent"; }

        //! Get parent transform component
        TransformComponent* getParent() const;

        //! Set parent transform
        void setParent(TransformComponent* comp);

        //! Check if has parent transform component
        bool hasParent() const { return  getParent() != nullptr; }

        //! Translate
        virtual void translate(const Vec3& trans);

        //! Rotate
        virtual void rotate(const Quat& rot);

        //! Scale
        virtual void scale(const Vec3& scale);

        //! Set local position
        virtual void setPosition(const Vec3& pos);

        //! Get local position
        virtual const Vec3& getPosition() const;

        //! Set world position
        virtual void setWorldPosition(const Vec3& pos);

        //! Get world position
        virtual const Vec3& getWorldPosition() const;

        //! Set local rotation
        virtual void setRotation(const Quat& rot);

        //! Get local rotation
        virtual const Quat& getRotation() const;

        //! Set world rotation
        virtual void setWorldRotation(const Quat& rot);

        //! get world rotation
        virtual const Quat& getWorldRotation() const;

        //! Set local scale
        virtual void setScale(const Vec3& scale);

        //! Get local scale
        virtual const Vec3& getScale() const;

        //! Set world scale
        virtual void setWorldScale(const Vec3& scale);

        //! Get world scale
        virtual const Vec3& getWorldScale() const;

        //! Get local transform matrix
        virtual const Mat4& getLocalMatrix() const;

        //! Get world transform matrix
        virtual const Mat4& getWorldMatrix() const;

        //! Get local right vector
        virtual Vec3 getLocalRight() const;

        //! Get local up vector
        virtual Vec3 getLocalUp() const;

        //! Get local forward vector
        virtual Vec3 getLocalForward() const;

        //! Get world right vector
        virtual Vec3 getWorldRight() const;

        //! Get world up vector
        virtual Vec3 getWorldUp() const;

        //! Get world forward vector
        virtual Vec3 getWorldForward() const;

        //! Update
        virtual void onUpdate(float dt) override;

        //! Aabb min
        const Vec3& getAabbMin() const { return m_aabbMin; }
        inline void setAabbMin(const Vec3& aabb)
        {
            m_aabbMin = aabb;
            m_aabbCenter = Vec3((m_aabbMin[0] + m_aabbMax[0]), (m_aabbMin[1] + m_aabbMax[1]), (m_aabbMin[2] + m_aabbMax[2])) * 0.5f;
        }

        //! Aabb max
        const Vec3& getAabbMax() const { return m_aabbMax; }
        inline void setAabbMax(const Vec3& aabb)
        {
            m_aabbMax = aabb;
            m_aabbCenter = Vec3((m_aabbMin[0] + m_aabbMax[0]), (m_aabbMin[1] + m_aabbMax[1]), (m_aabbMin[2] + m_aabbMax[2])) * 0.5f;
        }

        const Vec3& getCenter() const { return m_aabbCenter; }

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

    protected:
        //! Update local transform matrix
        virtual void updateLocalMatrix();

        //! Update world transform matrix
        virtual void updateWorldMatrix();

        //! Update world transform, then adjust local transfrom based on world transform
        virtual void updateWorldToLocal();

        //! Add observer
        virtual void addObserver(TransformComponent* observer);

        //! Remove observer
        virtual void removeObserver(TransformComponent* observer);

        //! Notify to all observers
        virtual void notifyObservers(const ETransformMessage &message);

        //! Handle notification from parent
        virtual void onNotified(const ETransformMessage &message);

        //! Update bouding box
        virtual void updateAabb();

    protected:
        //! Local transform
        Vec3 m_localPosition;
        Quat m_localRotation;
        Vec3 m_localScale;
        Mat4 m_localMatrix;

        //! World transform
        Vec3 m_worldPosition;
        Quat m_worldRotation;
        Vec3 m_worldScale;
        Mat4 m_worldMatrix;

        //! Transform observers
        std::set<TransformComponent*> m_observers;

        //! Cached parent transform
        TransformComponent* m_parent = nullptr;

        //! Dirty flag
        bool m_bLocalDirty = false;
        bool m_bWorldDirty = false;

        //! Cached aabb
        Vec3 m_aabbMin = { 0.f, 0.f, 0.f };
        Vec3 m_aabbMax = { 0.f, 0.f, 0.f };

        //! Cached center position
        Vec3 m_aabbCenter = { 0.f, 0.f, 0.f };
    };
}

