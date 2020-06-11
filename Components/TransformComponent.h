#pragma once

#include <set>

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "Component.h"

namespace ige::scene {

    //! Transform messages to childrens
    enum class ETransformMessage
    {
        TRANSFORM_CHANGED,
        TRANSFORM_DESTROYED
    };

    //! Transform observer
    class ITransformObserver: public std::enable_shared_from_this<ITransformObserver>
    {
    public:
        virtual void onNotified(const ETransformMessage &message) = 0;
    };

    //! Transform observable(as parent) and also observer(as child)
    class TransformComponent: public Component, ITransformObserver
    {
    public:
        //! Constructor
        TransformComponent(std::shared_ptr<SceneObject> owner, const Vec3& pos = Vec3(), const Quat& rot = Quat(), const Vec3& scale = Vec3());

        //! Destructor
        virtual ~TransformComponent();
        
        //! Get component name
        virtual std::string getName() const override { return "TransformComponent"; }

        //! Get parent transform component
        std::shared_ptr<TransformComponent> getParent() const { return m_parent.lock(); }

        //! Check if has parent transform component
        bool hasParent() const { return  getParent() != nullptr; }

        //! Assign parent transform component
        void setParent(std::shared_ptr<TransformComponent> parent);

        //! Remove parent transform component
        void removeParent();

        //! Translate
        virtual void translate(const Vec3& trans);

        //! Rotate
        virtual void rotate(const Quat& rot);

        //! Scale
        virtual void scale(const Vec3& scale);

        //! Set local position
        virtual void setPosition(const Vec3& pos);

        //! Get local position
        virtual Vec3 getPosition() const;

        //! Get world position
        virtual Vec3 getWorldPosition() const;

        //! Set local rotation
        virtual void setRotation(const Quat& rot);

        //! Get local rotation
        virtual Quat getRotation() const;

        //! get world rotation
        virtual Quat getWorldRotation() const;

        //! Set local scale
        virtual void setScale(const Vec3& scale);

        //! Get local scale
        virtual Vec3 getScale() const;

        //! Get world scale
        virtual Vec3 getWorldScale() const;

        //! Get local transform matrix
        virtual Mat4 getLocalMatrix() const;

        //! Get world transform matrix
        virtual Mat4 getWorldMatrix() const;

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

    protected:
        //! Update local transform matrix
        virtual void updateLocalMatrix();

        //! Update world transform matrix
        virtual void updateWorldMatrix();

        //! Add observer
        virtual void addObserver(std::shared_ptr<ITransformObserver> observer);

        //! Remove observer
        virtual void removeObserver(std::shared_ptr<ITransformObserver> observer);

        //! Notify to all observers
        virtual void notifyObservers(const ETransformMessage &message);
        
        //! Handle notification from parent
        void onNotified(const ETransformMessage &message) override;

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

        //! Parent tranformation
        std::weak_ptr<TransformComponent> m_parent;

        //! Transform observers
        std::set<std::shared_ptr<ITransformObserver>> m_observers;
    };
}

