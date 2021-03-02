#pragma once

#include "utils/PyxieHeaders.h"
#include "core/Macros.h"

using namespace pyxie;



namespace ige::scene
{
    enum class FillMethod
    {
        None,
        Horizontal,
        Vertical,
        Radial90,
        Radial180,
        Radial360,
    };

    enum class FillOrigin
    {
        Bottom,         //!Horizontal | Radial180 | Radial360
        Top,            //!Horizontal | Radial180 | Radial360
        Left,           //!Vertical | Radial180 | Radial360
        Right,          //!Vertical | Radial180 | Radial360
        BottomLeft,     //!Radial90
        TopLeft,        //!Radial90     
        TopRight,       //!Radial90
        BottomRight,    //!Radial90
    };

    class Sprite
    {
    public:
        Sprite(const std::string& path, const Vec2& size = {64.f, 64.f});
        virtual ~Sprite();

        const Vec2& getSize() const { return m_size; }
        void setSize(const Vec2& size);

        const std::string& getPath() const { return m_path; }
        void setPath(const std::string& path);

        const Vec2& getTexSize() const { return m_texSize; }

        EditableFigure* getFigure() { return m_figure; }

        const Vec2& getTiling() { return m_tiling; }
        void setTiling(const Vec2& value);

        const Vec2& getOffset() { return m_offset; }
        void setOffset(const Vec2& value);

        const SamplerState::WrapMode& getWrapMode() { return m_wrapMode; }
        void setWrapMode(const SamplerState::WrapMode& value);

        //! Enable alpha blending
        const bool isAlphaBlendingEnable() const { return m_bIsAlphaBlendingEnable; }
        void setAlphaBlendingEnable(bool enable = true);

        //! Alpha blending operation
        const int getAlphaBlendingOp() const { return m_alphaBlendingOp; }
        void setAlphaBlendingOp(int op);

        const FillMethod& getFillMethod() { return m_fillMethod; }
        void setFillMethod(const FillMethod& value);

        const FillOrigin& getFillOrigin() { return m_fillOrigin; }
        void setFillOrigin(const FillOrigin& value);

        const float getFillAmount() const { return m_fillAmount; }
        void setFillAmount(float amount);

        const bool getClockwise() const { return m_clockwise; }
        void setClockwise(bool value);


    protected:
        virtual void draw();
        
        virtual void drawNormal();
        //! Draw Fill Horizontal or Vertical
        virtual void drawFillBar();
        //! Draw Fill Radial
        virtual void drawFillRadial90();
        virtual void drawFillRadial180();
        virtual void drawFillRadial360();

        virtual void applyMesh(const std::vector<float> &points, const std::vector<uint32_t> &triangles, const std::vector<float> &uvs);
        virtual void applyTexture();
        
        Vec2 boundaryTexCoord(int index);

    protected:
        static Vec2 rotateByAngle(const Vec2& pivot, const Vec2& target, float angle);
        static float crossProduct2Vector(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D);
        static Vec2 findInterpolationPoint(Vec2& target, Vec2& other, float alpha);

        /** A general line-line intersection test
         @param A   the startpoint for the first line L1 = (A - B)
         @param B   the endpoint for the first line L1 = (A - B)
         @param C   the startpoint for the second line L2 = (C - D)
         @param D   the endpoint for the second line L2 = (C - D)
         @param S   the range for a hitpoint in L1 (p = A + S*(B - A))
         @param T   the range for a hitpoint in L2 (p = C + T*(D - C))
         @return    whether these two lines intersects.

         Note that to truly test intersection for segments we have to make
         sure that S & T lie within [0..1] and for rays, make sure S & T > 0
         the hit point is        C + T * (D - C);
         the hit point also is   A + S * (B - A);
         */
        static bool isLineIntersect(const Vec2& A, const Vec2& B,
            const Vec2& C, const Vec2& D,
            float* S = nullptr, float* T = nullptr);

    protected:
        std::string m_path;
        Vec2 m_texSize;
        Vec2 m_size;

        //! UV Map Loop
        Vec2 m_tiling;

        //! UV Map Offset
        Vec2 m_offset;

        EditableFigure* m_figure;
        Texture* m_texture;

        //! Wrap Mode
        SamplerState::WrapMode m_wrapMode;
        
        FillMethod m_fillMethod;
        FillOrigin m_fillOrigin;
        float m_fillAmount;
        bool m_clockwise;

        //! Cache alpha blending state
        bool m_bIsAlphaBlendingEnable = true;

        //! Cache alpha blending operation (COL = 0, ADD = 1, SUB = 2, MUL = 3)
        int m_alphaBlendingOp = 2;
    };
}
