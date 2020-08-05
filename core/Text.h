#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    class Text
    {
    public:
        Text(const std::string& text = "", const std::string& font = "", int fontSize = 11, const Vec4& color = {1.f, 1.f, 1.f, 1.f});
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

        //! Internal figure
        EditableFigure* m_figure;
    };
}
