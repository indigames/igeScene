#include "components/physic/BulletDebugRender.h"
#include "utils/ShapeDrawer.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    void BulletDebugRender::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
    {
        ShapeDrawer::drawLine(PhysicHelper::from_btVector3(from), PhysicHelper::from_btVector3(to), PhysicHelper::from_btVector3(color));
    }
} // namespace ige::scene