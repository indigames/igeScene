#pragma once

#include "utils/Singleton.h"
#include "utils/PyxieHeaders.h"

using namespace pyxie;

namespace ige::scene
{
    class GraphicsHelper : public Singleton<GraphicsHelper>
    {
    public:
        EditableFigure* createSprite(const Vec2& size = {128, 128}, const std::string& texture = "", const Vec2& uv_top_left = {0.f, 0.f}, const Vec2& uv_bottom_right = {1.f, 1.f}, Vec3* normal = nullptr, uint32_t pivot = 4, ShaderDescriptor* shader = nullptr);
        EditableFigure* createMesh(const std::vector<float>& points, const std::vector<uint32_t>& trianglesIndices, const std::string& texture = "", const std::vector<float>& uvs = {}, ShaderDescriptor* shader = nullptr, Vec3* normal = nullptr);
        EditableFigure* createText(const std::string& words, const std::string& fontPath, int fontSize, const Vec4& color = {1.f, 1.f, 1.f, 1.f}, uint32_t pivot = 4, float scale = 1.0f);
        EditableFigure* createGridMesh(const Vec2& size = {128, 128}, const std::string& texture = "");

        //! Texture utilities
        void texture_setText(Texture* texture, const std::string& word, const std::string& font, int size, int startX = 0, int startY = 0, int clear = 0);

    protected:
        std::string unique(const std::string& str);
    };
}
