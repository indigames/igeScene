#pragma once
#include <bullet/btBulletCollisionCommon.h>

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/TransformComponent.h"

namespace ige::scene
{
    class PhysicHelper
    {
    public:
        PhysicHelper() = delete;

        // Convert igeVMath to bullet
        static btTransform to_btTransform(const TransformComponent& transform);
        static btVector3 to_btVector3(const Vec3& vec);
        static btQuaternion to_btQuaternion(const Quat& quat);

        // Convert bullet to igeVMath
        static Vec3 from_btVector3(const btVector3& vec);
        static Quat from_btQuaternion(const btQuaternion& quat);
    };
}
