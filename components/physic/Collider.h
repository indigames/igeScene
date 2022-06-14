#pragma once

#include "components/Component.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <btBulletCollisionCommon.h>

namespace ige::scene
{
    class Rigidbody;
    class CompoundCollider;

    //! Collider
    class Collider : public Component
    {
    public:
        //! Constructor
        Collider(SceneObject& owner);

        //! Destructor
        virtual ~Collider();

        //! Init
        virtual void init();

        //! Local scale
        virtual const Vec3& getScale() const { return m_scale; }
        virtual void setScale(const Vec3& scale);

        //! Collision Margin
        float getMargin() const { return m_margin; }
        void setMargin(float margin);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;
    
        //! Set/get compound collider
        virtual std::shared_ptr<CompoundCollider> getCompoundCollider() const { return m_compoundCollider.expired() ? nullptr : m_compoundCollider.lock(); }
        virtual void setCompoundCollider(std::shared_ptr<CompoundCollider> body);

        //! Get collision shape
        std::unique_ptr<btCollisionShape>& getShape();

        //! Recreate shape, also recreate body
        virtual void recreateShape();

        //! Transform changed event
        void onTransformChanged();

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Serialize finished event
        virtual void onSerializeFinished(Scene* scene) override;

        //! Create shape
        virtual void createShape() = 0;

        //! Destroy shape
        virtual void destroyShape();

    protected:
        //! Collision shape
        std::unique_ptr<btCollisionShape> m_shape = nullptr;

        //! Collision shapes for compound collider
        std::vector<std::shared_ptr<btCollisionShape>> m_shapes;

        //! Cache scale value
        Vec3 m_scale = {1.f, 1.f, 1.f};

        //! Collision margin
        float m_margin = 0.025f;

        //! Cache compound collider instance
        std::weak_ptr<CompoundCollider> m_compoundCollider;

        //! Cache serialize data
        json m_json;
    };

} // namespace ige::scene
