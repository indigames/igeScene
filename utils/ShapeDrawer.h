#pragma once
#include "utils/PyxieHeaders.h"

namespace ige::scene
{
    //! Debug rendering line.
    struct DebugLine
    {
        //! Construct with start and end positions and color.
        DebugLine(const Vec3& start, const Vec3& end, const Vec3& color) :
            start(start),
            end(end),
            color(color)
        {
        }

        //! Start position.
        Vec3 start;

        //! End position.
        Vec3 end;

        //! Color.
        Vec3 color;
    };
    
    //! ShapeDrawer
    class ShapeDrawer
    {
    public:
        static void initialize();
        static uint32_t loadShaders(const char* vShader, const char* fShader);
        static void setViewProjectionMatrix(const Mat4 &viewProjection);
        static void drawLine(const Vec3 &start, const Vec3 &end, const Vec3 &color = { 0.f, 1.f, 0.f });
        static void flush();
    };
} // namespace ige::scene