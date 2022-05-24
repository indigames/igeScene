#include "core/Text.h"
#include "utils/GraphicsHelper.h"

#include <bitmapHelper.h>
using namespace pyxie;

#include <algorithm>

namespace ige::scene
{
    //! Constructor
    Text::Text(const std::string& text, const std::string& font, int fontSize, const Vec4& color, int fonttype, bool worldSpace)
        : m_text(text), m_fontPath(font), m_fontSize(fontSize), m_color(color), m_figure(nullptr), m_fontType(fonttype), m_worldSpace(worldSpace)
    {
        updateFigure();
    }

    //! Destructor
    Text::~Text()
    {
        if(m_figure)
        {
            m_figure->DecReference();
            m_figure = nullptr;
        }
    }

    //! Text
    void Text::setText(const std::string& text, int type)
    {
        if(m_text != text)
        {
            m_text = text;
            m_fontType = type;
            updateFigure();
        }
    }

    //! Font Path
    void Text::setFontPath(const std::string& path)
    {
        auto tmpPath = path;
        std::replace(tmpPath.begin(), tmpPath.end(), '\\', '/');

        if(m_fontPath != tmpPath)
        {
            m_fontPath = tmpPath;
            updateFigure();
        }
    }

    //! Font Size
    void Text::setFontSize(int size)
    {
        if(m_fontSize != size)
        {
            m_fontSize = size;
            updateFigure();
        }
    }

    //! Color
    void Text::setColor( const Vec4& color)
    {
        if(m_color != color)
        {
            m_color = color;
            if(m_figure)
            {
                int materialIdx = m_figure->GetMaterialIndex(GenerateNameHash("mate"));
                m_figure->SetMaterialParam(materialIdx, GenerateNameHash("DiffuseColor"), color.P(), ParamTypeFloat4);
            }
        }
    }

    const Vec2& Text::getSize() const
    {
        return m_size;
    }

    void Text::updateFigure()
    {
        if (m_fontPath != "")
        {
            Vec3 pos, scale(1.f, 1.f, 1.f);
            Quat rot;
            if (m_figure)
            {
                pos = m_figure->GetPosition(); 
                rot = m_figure->GetRotation();
                scale = m_figure->GetScale();
                m_figure->DecReference();
                m_figure = nullptr;
            }

            if (m_fontType == 1)
            {
                int w = 0, h = 0;
                m_figure = GraphicsHelper::getInstance()->createBitmapText(m_text, m_fontPath, m_fontSize, m_color, w, h);
                m_size = Vec2(w, h);
            }
            else
            {
                int w = 0, h = 0;
                calcTextSize(m_text.c_str(), m_fontPath.c_str(), m_fontSize, w, h, 1.f);
                m_size = Vec2(w, h);
                m_figure = GraphicsHelper::getInstance()->createText(m_text, m_fontPath, m_fontSize, m_color, m_worldSpace ? 0.01f : 1.0f);
            }

            if (m_figure == nullptr) return;

            m_figure->SetPosition(pos);
            m_figure->SetRotation(rot);
            m_figure->SetScale(scale);
            m_figure->WaitBuild();
        }
    }
}
