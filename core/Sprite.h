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

        const Vec2& getTiling() { return m_tiling; }
        void setTiling(const Vec2& value);

        const Vec2& getOffset() { return m_offset; }
        void setOffset(const Vec2& value);

        const SamplerState::WrapMode& getWrapMode() { return m_wrapMode; }
        void setWrapMode(const SamplerState::WrapMode& value);
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

        SamplerState::WrapMode m_wrapMode;

    };
}
