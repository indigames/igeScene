#pragma once

#include "utils/Singleton.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    class RayOBBChecker
    {
    public:
        static void screenPosToWorldRay(
            int x, int y,                       // Screen space position, in pixels
            int screenWidth, int screenHeight,  // Window size, in pixels
            const Mat4& viewInversedMatrix,     // View inversed matrix
            const Mat4& projectionMatrix        // Projection matrix
        );

        static bool checkIntersect(            
            const Vec3& aabb_min,               // Min AABB coords
            const Vec3& aabb_max,               // Maximum AABB coords
            const Mat4& modelMatrix,            // World transformation matrix
            float& intersection_distance        // Output: intersection distance
        );
        
        static void setChecking(bool checking) { g_isChecking = checking; }
        static bool isChecking() { return g_isChecking; }

        static Vec3 g_origin;
        static Vec3 g_direction;
        static bool g_isChecking;
    };
}
