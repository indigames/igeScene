#pragma once

#include "utils/Singleton.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    class RayOBBChecker
    {
    public:
        static std::pair<Vec3, Vec3> screenPosToWorldRay(
            int x, int y,                       // Screen space position, in pixels
            int screenWidth, int screenHeight,  // Window size, in pixels
            const Mat4& viewInversedMatrix,     // View inversed matrix
            const Mat4& projectionMatrix        // Projection matrix
        );

        static std::pair<Vec3, Vec3> RayOBB(
            Vec3 origin,
            Vec3 direction
        );

        static bool checkIntersect(
            const AABBox& aabb,                 // AABB box
            const Mat4& modelMatrix,            // World transformation matrix
            float& intersection_distance,       // Output: intersection distance
            float max_distance = 10000.f        // Max distance
        );

        static Vec3 g_origin;
        static Vec3 g_direction;
    };
}
