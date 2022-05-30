#pragma once

#include "components/Component.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <btBulletCollisionCommon.h>

namespace ige::scene
{

    //! Collider
    class Collider : public Component
    {
    public:
        //! Constructor
        Collider(SceneObject &owner);

        //! Destructor
        virtual ~Collider();

        //! Local scale
        virtual const Vec3& getScale() const { return m_scale; }
        virtual void setScale(const Vec3& scale);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;
    
        //! Get collision shape
        std::unique_ptr<btCollisionShape>& getShape();

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Create shape
        virtual void createShape() = 0;

        //! Recreate shape, also recreate body
        virtual void recreateShape();

    protected:
        //! Collision shape
        std::unique_ptr<btCollisionShape> m_shape = nullptr;

        //! Collision shapes for compound collider
        std::vector<std::shared_ptr<btCollisionShape>> m_shapes;

        //! Cache scale value
        Vec3 m_scale = {1.f, 1.f, 1.f};
    };

} // namespace ige::scene
