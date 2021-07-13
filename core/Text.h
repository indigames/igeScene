#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
#define TEXT_TRUETYPE   0
#define TEXT_BITMAP     1

    class Text
    {
    public:
        Text(const std::string& text = "", const std::string& font = "", int fontSize = 11, const Vec4& color = {1.f, 1.f, 1.f, 1.f}, int fonttype = TEXT_TRUETYPE);
        virtual ~Text();

        //! Text
        const std::string getText() const { return m_text; }
        void setText(const std::string& text);

        //! Font Path
        const std::string& getFontPath() const { return m_fontPath; }
        void setFontPath(const std::string& path);

        //! Font Size
        int getFontSize() const { return m_fontSize; }
        void setFontSize(int size);

        //! Color
        const Vec4& getColor() const { return m_color; }
        void setColor( const Vec4& color);

        //! Get size
        const Vec2& getSize() const;

        //! Font Type
        int getFontType() const { return m_fontType; }
        //! Figure
        EditableFigure* getFigure() { return m_figure; }

    protected:
        void updateFigure();

        //! UIText
        std::string m_text;

        //! Font path
        std::string m_fontPath;

        //! Font size
        int m_fontSize;

        //! Color
        Vec4 m_color;

        //! Cached Size
        Vec2 m_size;

        int m_fontType; 

        //! Internal figure
        EditableFigure* m_figure;
    };
}
