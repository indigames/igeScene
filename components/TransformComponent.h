#pragma once

#include <set>

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"

#include "core/igeSceneMacros.h"

namespace ige::scene
{

    //! Transform messages to childrens
    enum class ETransformMessage
    {
        TRANSFORM_CHANGED,
        TRANSFORM_DESTROYED
    };

    //! Transform observable(as parent) and also observer(as child)
    class TransformComponent : public Component
    {
    public:
        //! Constructor
        TransformComponent(SceneObject &owner, const Vec3 &pos = Vec3(), const Quat &rot = Quat(), const Vec3 &scale = Vec3(1.f, 1.f, 1.f));

        //! Destructor
        virtual ~TransformComponent();

        //! Get component name
        virtual std::string getName() const override { return "Transform"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::Transform; }

        //! Get parent transform component
        std::shared_ptr<TransformComponent> getParent() const;

        //! Set parent transform
        virtual void setParent(std::shared_ptr<TransformComponent> comp);

        //! Translate
        virtual void translate(const Vec3 &trans);
        virtual void worldTranslate(const Vec3 &trans);

        //! Rotate
        virtual void rotate(const Quat &rot);
        virtual void worldRotate(const Quat &rot);

        //! Scale
        virtual void scale(const Vec3 &scale);
        virtual void worldScale(const Vec3 &scale);

        //! Set local position
        virtual void setPosition(const Vec3 &pos);

        //! Get local position
        virtual const Vec3 &getPosition() const;

        //! Set world position
        virtual void setWorldPosition(const Vec3 &pos);

        //! Get world position
        virtual const Vec3 &getWorldPosition() const;

        //! Set local rotation
        virtual void setRotation(const Quat &rot);

        //! Set local rotation by xyz
        virtual void setRotation(const Vec3 &rot);

        //! Get local rotation
        virtual const Quat &getRotation() const;

        //! Set world rotation
        virtual void setWorldRotation(const Quat &rot);

        //! Set world rotation by xyz
        virtual void setWorldRotation(const Vec3& rot);

        //! get world rotation
        virtual const Quat &getWorldRotation() const;

        //! Set local scale
        virtual void setScale(const Vec3 &scale);

        //! Get local scale
        virtual const Vec3 &getScale() const;

        //! Set world scale
        virtual void setWorldScale(const Vec3 &scale);

        //! Get world scale
        virtual const Vec3 &getWorldScale() const;

        //! Get local transform matrix
        virtual const Mat4 &getLocalMatrix() const;

        //! Get world transform matrix
        virtual const Mat4 &getWorldMatrix() const;

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

        virtual void makeDirty();

        virtual Vec3 globalToLocal(Vec3 point) const;
        virtual Vec3 localToGlobal(Vec3 point) const;

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Update world transform matrix
        virtual void updateLocalToWorld();

        //! Update world transform, then adjust local transfrom based on world transform
        virtual void updateWorldToLocal();

        //! Add observer
        virtual void addObserver(TransformComponent *observer);

        //! Remove observer
        virtual void removeObserver(TransformComponent *observer);

        //! Notify to all observers
        virtual void notifyObservers(const ETransformMessage &message);

        //! Handle notification from parent
        virtual void onNotified(const ETransformMessage &message);

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
        std::weak_ptr<TransformComponent> m_parent;

        //! Dirty flag
        bool m_bLocalDirty = false;
    };
} // namespace ige::scene
