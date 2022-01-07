#pragma once

#include "utils/Singleton.h"
#include "utils/PyxieHeaders.h"

namespace ige::scene
{
    class GraphicsHelper : public Singleton<GraphicsHelper>
    {
    public:
        EditableFigure* createSprite(const Vec2& size = {128, 128}, const std::string& texture = "", const Vec2& uv_top_left = {0.f, 0.f}, const Vec2& uv_bottom_right = {1.f, 1.f}, Vec3* normal = nullptr, uint32_t pivot = 4, ShaderDescriptor* shader = nullptr);
        EditableFigure* createMesh(const std::vector<float>& points, const std::vector<uint32_t>& trianglesIndices, const std::string& texture = "", const std::vector<float>& uvs = {}, ShaderDescriptor* shader = nullptr, Vec3* normal = nullptr, const Vec4& color = { 1.f, 1.f, 1.f, 1.f });
        EditableFigure* createMesh(const std::vector<float>& points, const std::vector<uint32_t>& trianglesIndices, Texture* texture, const std::vector<float>& uvs = {}, ShaderDescriptor* shader = nullptr, Vec3* normal = nullptr, const Vec4& color = { 1.f, 1.f, 1.f, 1.f });
        EditableFigure* createText(const std::string& words, const std::string& fontPath, int fontSize, const Vec4& color = {1.f, 1.f, 1.f, 1.f}, float scale = 1.0f, uint32_t pivot = 4);

        EditableFigure* createBitmapText(const std::string& words, const std::string& fontPath, int fontSize, int& outW, int& outH);
        EditableFigure* createBitmapText(const std::string& words, const std::string& fontPath, int fontSize, const Vec4& color, int& outW, int& outH);
        EditableFigure* createBitmapText(const std::string& words, const std::string& fontPath, int fontSize, const Vec4& color, uint32_t pivot, int& outW, int& outH);
        EditableFigure* createBitmapText(const std::string& words, const std::string& fontPath, int fontSize, const Vec4& color, uint32_t pivot, float scale, int& outW, int& outH);
        
        EditableFigure* createGridMesh(const Vec2& size = {128, 128}, const std::string& texture = "");
        
        //! Texture utilities
        void texture_setText(Texture* texture, const std::string& word, const std::string& font, int size, int startX = 0, int startY = 0, int clear = 0);

    protected:
        std::string unique(const std::string& str);
    };

    Vec3 ScreenToWorld(const Vec2& screenPos, const Vec2& windowSize, Camera* cam = nullptr, float wZ = 0.f);
    Vec2 WorldToScreen(Vec3 world, const Vec2& windowSize, Camera* cam);
    Vec4 AabbToScreenRect(const AABBox& aabb, const Vec2& windowSize, Camera* cam);
    bool RectInside(const Vec4& r1, const Vec4& r2);

    std::string generateUUID(unsigned int len = 16);
}
