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
            int x, int y,                       // Screen Position, in pixels, from bottom-left corner of the window
            int screenWidth, int screenHeight,  // Window size, in pixels
            const Mat4& viewMatrix,             // Camera position and orientation
            const Mat4& projectionMatrix        // Camera parameters (ratio, field of view, near and far planes)            
        );

        static bool checkIntersect(            
            const Vec3& aabb_min,               // Minimum X,Y,Z coords of the mesh when not transformed at all.
            const Vec3& aabb_max,               // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
            const Mat4& modelMatrix,            // Transformation applied to the mesh (which will thus be also applied to its bounding box)
            float& intersection_distance        // Output : distance between ray_origin and the intersection with the OBB
        );
        
        static void setChecking(bool checking) { g_isChecking = checking; }
        static bool isChecking() { return g_isChecking; }

        static Vec3 g_origin;
        static Vec3 g_direction;
        static bool g_isChecking;
    };
}
