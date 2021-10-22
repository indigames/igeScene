#include "utils/PhysicHelper.h"

namespace ige::scene
{
    // Convert igeVMath to bullet
    btTransform PhysicHelper::to_btTransform(const TransformComponent& transform)
    {
        return btTransform(to_btQuaternion(transform.getRotation()), to_btVector3(transform.getPosition()));
    }

    btTransform PhysicHelper::to_btTransform(const Quat& rot, const Vec3& pos)
    {
        return btTransform(to_btQuaternion(rot), to_btVector3(pos));
    }

    btVector3 PhysicHelper::to_btVector3(const Vec3& vec)
    {
        return btVector3(vec[0], vec[1], vec[2]);
    }

    btQuaternion PhysicHelper::to_btQuaternion(const Quat& quat)
    {
        return btQuaternion(quat[0], quat[1], quat[2], quat[3]);
    }

    // Convert igeVMath from bullet
    Vec3 PhysicHelper::from_btVector3(const btVector3& vec)
    {
        return Vec3(vec.getX(), vec.getY(), vec.getZ());
    }

    Quat PhysicHelper::from_btQuaternion(const btQuaternion& quat)
    {
        return Quat(quat.getX(), quat.getY(), quat.getZ(), quat.getW());
    }
}
