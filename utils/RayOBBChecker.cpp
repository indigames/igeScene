#include <vmath.h>

#include "utils/RayOBBChecker.h"

namespace ige::scene
{
    Vec3 RayOBBChecker::g_origin = Vec3();
    Vec3 RayOBBChecker::g_direction= Vec3();
    bool RayOBBChecker::g_isChecking = false;

    void RayOBBChecker::screenPosToWorldRay(int mouseX, int mouseY, int screenWidth, int screenHeight,
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
    }

    bool RayOBBChecker::checkIntersect(const Vec3& aabb_min, const Vec3& aabb_max, const Mat4& modelMatrix, float& intersection_distance)
    {
        float tMin = 0.0f;
        float tMax = 100000.0f;

        Vec3 OBBposition_worldspace(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
        Vec3 delta = OBBposition_worldspace - g_origin;

        // Test intersection with the 2 planes perpendicular to the OBB's X axis
        {
            Vec3 xaxis(modelMatrix[0][0], modelMatrix[0][1], modelMatrix[0][2]);
            float e = Vec3::Dot(xaxis, delta);
            float f = Vec3::Dot(g_direction, xaxis);

            if ( fabs(f) > 0.001f ){ // Standard case

                float t1 = (e + aabb_min.X()) / f; // Intersection with the "left" plane
                float t2 = (e + aabb_max.X()) / f; // Intersection with the "right" plane
                // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

                // We want t1 to represent the nearest intersection, 
                // so if it's not the case, invert t1 and t2
                if (t1 > t2){
                    float w = t1; t1 = t2; t2 = w; // swap t1 and t2
                }

                // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
                if ( t2 < tMax )
                    tMax = t2;

                // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
                if ( t1 > tMin )
                    tMin = t1;

                // And here's the trick :
                // If "far" is closer than "near", then there is NO intersection.
                // See the images in the tutorials for the visual explanation.
                if (tMax < tMin )
                    return false;

            }else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
                if(-e+aabb_min.X() > 0.0f || -e+aabb_max.X() < 0.0f)
                    return false;
            }
        }


        // Test intersection with the 2 planes perpendicular to the OBB's Y axis
        // Exactly the same thing than above.
        {
            Vec3 yaxis(modelMatrix[1][0], modelMatrix[1][1], modelMatrix[1][2]);
            float e = Vec3::Dot(yaxis, delta);
            float f = Vec3::Dot(g_direction, yaxis);

            if ( fabs(f) > 0.001f ){

                float t1 = (e+aabb_min.Y())/f;
                float t2 = (e+aabb_max.Y())/f;

                if (t1>t2){float w=t1;t1=t2;t2=w;}

                if ( t2 < tMax )
                    tMax = t2;
                if ( t1 > tMin )
                    tMin = t1;
                if (tMin > tMax)
                    return false;

            }else{
                if(-e+aabb_min.Y() > 0.0f || -e+aabb_max.Y() < 0.0f)
                    return false;
            }
        }


        // Test intersection with the 2 planes perpendicular to the OBB's Z axis
        // Exactly the same thing than above.
        {
            Vec3 zaxis(modelMatrix[2][0], modelMatrix[2][1], modelMatrix[2][2]);
            float e = Vec3::Dot(zaxis, delta);
            float f = Vec3::Dot(g_direction, zaxis);

            if ( fabs(f) > 0.001f ){

                float t1 = (e+aabb_min.Z())/f;
                float t2 = (e+aabb_max.Z())/f;

                if (t1>t2){float w=t1;t1=t2;t2=w;}

                if ( t2 < tMax )
                    tMax = t2;
                if ( t1 > tMin )
                    tMin = t1;
                if (tMin > tMax)
                    return false;

            }else{
                if(-e+aabb_min.Z() > 0.0f || -e+aabb_max.Z() < 0.0f)
                    return false;
            }
        }

        intersection_distance = tMin;
        return true;
    }
}
