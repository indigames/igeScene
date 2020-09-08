#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include "components/physic/PhysicBox.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Constructor
    PhysicBox::PhysicBox(const std::shared_ptr<SceneObject> &owner, const Vec3& size)
        : PhysicBase(owner)
    {
        createCollisionShape(size);
    }

    //! Destructor
    PhysicBox::~PhysicBox()
    {
        if (m_shape != nullptr)
            m_shape.reset();
    }

    //! Get size
    Vec3 &PhysicBox::getSize()
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
    }

    //! Recreate collision shape
    void PhysicBox::recreateCollisionShape(const Vec3 &size)
    {
        // Create collision shape
        if (m_shape != nullptr)
            m_shape.reset();
        createCollisionShape(size);

        // Create body
        recreateBody();
    }

    //! Set local scale of the box
    void PhysicBox::setLocalScale(const Vec3 &scale)
    {
        if (m_shape)
            m_shape->setLocalScaling(PhysicHelper::to_btVector3(scale));
    }
} // namespace ige::scene