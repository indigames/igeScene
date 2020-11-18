#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include "components/physic/PhysicBox.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    PhysicBox::PhysicBox(SceneObject &owner, const Vec3 &size)
        : PhysicObject(owner)
    {
        createCollisionShape(size);
        init();
    }

    //! Destructor
    PhysicBox::~PhysicBox()
    {
        if (m_shape != nullptr)
            m_shape.reset();
    }

    //! Get size
    const Vec3 &PhysicBox::getSize() const
    {
        return m_size;
    }

    //! Set size
    void PhysicBox::setSize(const Vec3 &size)
    {
        if (m_size != size)
            recreateCollisionShape(size);
    }

    //! Create collision shape
    void PhysicBox::createCollisionShape(const Vec3 &size)
    {
        // Create collision shape
        if (m_shape != nullptr)
            m_shape.reset();
        m_shape = std::make_unique<btBoxShape>(PhysicHelper::to_btVector3(size));
        m_size = size;

        m_bIsDirty = true;
        setLocalScale(m_previousScale);
        m_bIsDirty = false;
    }

    //! Recreate collision shape
    void PhysicBox::recreateCollisionShape(const Vec3 &size)
    {
        // Create collision shape
        createCollisionShape(size);

        // Create body
        recreateBody();
    }

    //! Serialize
    void PhysicBox::to_json(json &j) const
    {
        PhysicObject::to_json(j);
        j["size"] = getSize();
    }

    //! Deserialize
    void PhysicBox::from_json(const json &j)
    {
        setSize(j.value("size", Vec3(1.f, 1.f, 1.f)));
        PhysicObject::from_json(j);
    }

} // namespace ige::scene