#include <vmath.h>

#include "utils/RayOBBChecker.h"

namespace ige::scene
{
    Vec3 RayOBBChecker::g_origin = Vec3();
    Vec3 RayOBBChecker::g_direction= Vec3();

    std::pair<Vec3, Vec3> RayOBBChecker::screenPosToWorldRay(int mouseX, int mouseY, int screenWidth, int screenHeight,
            const Mat4& viewInversedMatrix, const Mat4& projectionMatrix)
    {
        Vec4 lRayStart_NDC(
            (float)mouseX / (float)screenWidth  * 2.0f,
            (float)mouseY / (float)screenHeight * 2.0f,
            -1.0f,
            1.0f
        );

        Vec4 lRayEnd_NDC(
            (float)mouseX / (float)screenWidth * 2.0f,
            (float)mouseY / (float)screenHeight * 2.0f,
            0.f,
            1.0f
        );

        // The Projection matrix goes from Camera Space to NDC.
        Mat4 projectionInversedMatrix = projectionMatrix.Inverse();

        Vec4 lRayStart_camera = projectionInversedMatrix * lRayStart_NDC; lRayStart_camera /= lRayStart_camera.W();
        Vec4 lRayStart_world = viewInversedMatrix * lRayStart_camera; lRayStart_world /= lRayStart_world.W();
        Vec4 lRayEnd_camera = projectionInversedMatrix * lRayEnd_NDC; lRayEnd_camera /= lRayEnd_camera.W();
        Vec4 lRayEnd_world = viewInversedMatrix * lRayEnd_camera; lRayEnd_world /= lRayEnd_world.W();
        g_origin = Vec3(lRayStart_world.X(), lRayStart_world.Y(), lRayStart_world.Z());

        auto lRayDir_world = lRayEnd_world - lRayStart_world;
        g_direction = Vec3(lRayDir_world.X(), lRayDir_world.Y(), lRayDir_world.Z());
        g_direction.Normalize();

        return { g_origin, g_direction };
    }


    std::pair<Vec3, Vec3> RayOBBChecker::RayOBB(Vec3 origin, Vec3 direction) {
        g_origin = origin;
        g_direction = direction;
        g_direction.Normalize();

        return { g_origin, g_direction };
    }

    bool RayOBBChecker::checkIntersect(const AABBox& aabb, const Mat4& modelMatrix, float& intersection_distance, float max_distance)
    {
        float tMin = 0.0f;
        float tMax = max_distance;

        Vec3 OBBposition_worldspace(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
        Vec3 delta = OBBposition_worldspace - g_origin;

        // for each axis X, Y and Z
        for(int i = 0; i < 3; ++i)
        {
            Vec3 axis(modelMatrix[i][0], modelMatrix[i][1], modelMatrix[i][2]);
            float e = Vec3::Dot(axis, delta);
            float f = Vec3::Dot(g_direction, axis);

            if (fabs(f) > 0.0001f)
            { 
                // Standard case
                float t1 = (e + aabb.MinEdge[i]) / f; // Intersection with the "min" plane
                float t2 = (e + aabb.MaxEdge[i]) / f; // Intersection with the "max" plane
                // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

                // We want t1 to represent the nearest intersection, 
                // so if it's not the case, invert t1 and t2
                if (t1 > t2)
                {
                    float w = t1; t1 = t2; t2 = w; // swap t1 and t2
                }

                // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
                if ( t2 < tMax ) tMax = t2;

                // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
                if ( t1 > tMin ) tMin = t1;

                // And here's the trick :
                // If "far" is closer than "near", then there is NO intersection.
                // See the images in the tutorials for the visual explanation.
                if (tMax < tMin )
                    return false;
            }
            else
            {
                // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
                if (-e + aabb.MinEdge[i] > 0.0f || -e + aabb.MaxEdge[i] < 0.0f)
                    return false;
            }
        }
        intersection_distance = tMin;
        return true;
    }
}
