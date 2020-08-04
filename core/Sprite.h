#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
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

    protected:
        void adjustTextureUVs();

        std::string m_path;
        Vec2 m_texSize;
        Vec2 m_size;
        EditableFigure* m_figure;
    };
}
