#pragma once

#include "utils/PyxieHeaders.h"
#include "core/igeSceneMacros.h"
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

    enum class SpriteType {
        Simple = 0,
        Sliced = 1,
    };

    class Sprite
    {
    public:
        Sprite(const Vec2& size = { 0.f, 0.f });
        Sprite(Texture* texture, const Vec2& size = {0.f, 0.f});
        virtual ~Sprite();

        const Vec2& getSize() const { return m_size; }
        void setSize(const Vec2& size);

        const Vec2& getTexSize() const { return m_texSize; }

        EditableFigure* getFigure() { return m_figure; }

        const Vec2& getTiling() { return m_tiling; }
        void setTiling(const Vec2& value);

        const Vec2& getOffset() { return m_offset; }
        void setOffset(const Vec2& value);

        const SamplerState::WrapMode& getWrapMode() { return m_wrapMode; }
        void setWrapMode(const SamplerState::WrapMode& value);

        const FillMethod& getFillMethod() { return m_fillMethod; }
        void setFillMethod(const FillMethod& value);

        const FillOrigin& getFillOrigin() { return m_fillOrigin; }
        void setFillOrigin(const FillOrigin& value);

        const SpriteType& getSpriteType() { return m_spriteType; }
        void setSpriteType(const SpriteType& value);

        const float getFillAmount() const { return m_fillAmount; }
        void setFillAmount(float amount);

        const bool getClockwise() const { return m_clockwise; }
        void setClockwise(bool value);

        const Vec4& getColor() const { return m_color; }
        void setColor(float r, float g, float b, float a = 1, bool redraw = true);
        void setColor(const Vec4& value, bool redraw = true);

        Texture* getTexture() const { return m_texture; }
        void setTexture(Texture* texture);

        //! Border
        void setBorder(float left, float right, float top, float bottom);
        const Vec4& getBorder() const { return m_border; }

        const float getBorderLeft() const { return m_border[0]; }
        void setBorderLeft(float value);

        const float getBorderRight() const { return m_border[1]; }
        void setBorderRight(float value);

        const float getBorderTop() const { return m_border[2]; }
        void setBorderTop(float value);

        const float getBorderBottom() const { return m_border[3]; }
        void setBorderBottom(float value);

        const bool isScaleBorder() const { return m_bIsScaleBorder; }
        void setIsScaleBorder(bool value);


    protected:
        virtual void draw();
        
        virtual void drawNormal();
        //! Draw Fill Horizontal or Vertical
        virtual void drawFillBar();
        //! Draw Fill Radial
        virtual void drawFillRadial90();
        virtual void drawFillRadial180();
        virtual void drawFillRadial360();

        //! Draw Nine Sliced
        virtual void drawNineSliced();

        virtual void applyMesh(const std::vector<float> &points, const std::vector<uint32_t> &triangles, const std::vector<float> &uvs);
        virtual void applyTexture();

        Vec2 boundaryTexCoord(int index);

        void releaseSprite();

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
        //std::string m_path;
        Vec2 m_texSize;
        Vec2 m_size;

        //! UV Map Loop
        Vec2 m_tiling;

        //! UV Map Offset
        Vec2 m_offset;

        //! Color
        Vec4 m_color;

        //! Border
        Vec4 m_border;
        bool m_bIsScaleBorder;

        EditableFigure* m_figure;
        Texture* m_texture;

        //! Wrap Mode
        SamplerState::WrapMode m_wrapMode;
        
        SpriteType m_spriteType;
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
