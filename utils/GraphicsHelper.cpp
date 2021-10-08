#include <algorithm>
#include <vmath.h>
#include <bitmapHelper.h>

#include "utils/GraphicsHelper.h"
#include "core/BitmapFontHelper.h"

namespace ige::scene
{
    EditableFigure* GraphicsHelper::createSprite(const Vec2& size, const std::string& texture, const Vec2& uv_top_left, const Vec2& uv_bottom_right, Vec3* normal, uint32_t pivot, ShaderDescriptor* shader)
    {
        const std::vector<uint32_t> _tris = {0, 2, 1, 1, 2, 3};
        const std::vector<int32_t> _pivotoffset = {1,-1, 0,-1, -1,-1, 1,0, 0,0, -1,0, 1,1, 0,1, -1,1};

        auto hw = size.X()/2;
        auto hh = size.Y()/2;

        auto px = _pivotoffset[pivot*2+0] * hw;
        auto py = _pivotoffset[pivot*2+1] * hh;
        std::vector<Vec3> points = {{-hw+px,hh+py,0.0}, {hw+px,hh+py,0.0}, {-hw+px,-hh+py,0.0}, {hw+px,-hh+py,0.0}};

        if (normal != nullptr)
        {
            std::vector<Vec3> newpoints = {};
            const float nom0[3] = {0.f, 0.f, 1.f};
            Mat3 mat;
            vmath_quat_rotation(nom0, (const float*)normal, (float*)&mat);
            for (auto p: points)
                newpoints.push_back(mat * p);
            points = newpoints;
        }

        std::vector<float> uvs = { uv_top_left.X(), uv_bottom_right.Y(),
                                uv_bottom_right.X(), uv_bottom_right.Y(),
                                uv_top_left.X(), uv_top_left.Y(),
                                uv_bottom_right.X(), uv_top_left.Y()};

        std::vector<float> _points = {};
        for(auto p: points)
        {
            _points.push_back(p.X());
            _points.push_back(p.Y());
            _points.push_back(p.Z());
        }
        return createMesh(_points, _tris, texture, uvs, shader);
    }
    
    EditableFigure* GraphicsHelper::createMesh(const std::vector<float>& points, const std::vector<uint32_t>& trianglesIndices, Texture* texture, const std::vector<float>& uvs, ShaderDescriptor* shader, Vec3* normals, const Vec4& color)
    {
        if (shader == nullptr)
        {
            shader = ResourceCreator::Instance().NewShaderDescriptor();
            if (texture != nullptr) shader->SetColorTexture(true);
            shader->SetBoneCondition(1, 1);
        }

        auto efig = ResourceCreator::Instance().NewEditableFigure("sprite", true);
        efig->Initialize();
        efig->Build();

        efig->AddMaterial("mate", *shader);
        efig->AddMesh("mesh", "mate");

        auto meshIdx = efig->GetMeshIndex(GenerateNameHash("mesh"));
        if (meshIdx == -1) return nullptr;

        //efig->SetVertexElements("mesh", ATTRIBUTE_ID_POSITION, points);
        efig->SetMeshVertexValues(meshIdx, (const void*)points.data(), (uint32_t)(points.size() / 3), ATTRIBUTE_ID_POSITION, 0);

        // if (uvs) efig->SetVertexElements("mesh", ATTRIBUTE_ID_UV0, uvs);
        if (uvs.size() > 0) efig->SetMeshVertexValues(meshIdx, (const void*)uvs.data(), (uint32_t)(uvs.size() / 2), ATTRIBUTE_ID_UV0, 0);

        // if (normals) efig->SetVertexElements("mesh", ATTRIBUTE_ID_NORMAL, normals);
        if (normals) efig->SetMeshVertexValues(meshIdx, (const void*)normals, 1, ATTRIBUTE_ID_NORMAL, 0);

        // efig->SetTriangles("mesh", trianglesIndices);
        efig->SetMeshIndices(meshIdx, 0, (const uint32_t*)trianglesIndices.data(), (uint32_t)(trianglesIndices.size()), 4);

        //efig->AddJoint("joint");
        Joint joint;
        int parentIndex = -1;
        efig->AddJoint(parentIndex, joint, false, "joint");

        //efig->SetMaterialParam("mate", "DiffuseColor", Vec4(1.f, 1.f, 1.f, 1.f));
        int materialIdx = efig->GetMaterialIndex(GenerateNameHash("mate"));
        float _color[4] = { color[0], color[1], color[2], color[3] };
        efig->SetMaterialParam(materialIdx, GenerateNameHash("DiffuseColor"), _color, ParamTypeFloat4);

        // efig->SetMaterialRenderState("mate", "cull_face_enable", false);

        if (texture != nullptr)
        {
            Sampler sampler;
            sampler.samplerSlotNo = 0;
            sampler.samplerState.wrap_s = SamplerState::WRAP;
            sampler.samplerState.wrap_t = SamplerState::WRAP;
            sampler.samplerState.minfilter = SamplerState::LINEAR;
            sampler.samplerState.magfilter = SamplerState::LINEAR;
            sampler.tex = texture;
            sampler.tex->WaitInitialize();
            sampler.tex->WaitBuild();

            TextureSource texSrc;
            strncpy(texSrc.path, texture->ResourceName(), MAX_PATH);
            texSrc.normal = false;
            texSrc.wrap = false;
            sampler.textureNameIndex = efig->SetTextureSource(texSrc);
            efig->SetMaterialParam(materialIdx, GenerateNameHash("ColorSampler"), &sampler);

            // efig->SetMaterialRenderState("mate", "blend_enable", true);
            const ShaderParameterInfo* paramInfo = RenderContext::Instance().GetShaderParameterInfoByName("blend_enable");
            uint32_t blendVal[4] = { 1,0,0,0 };
            efig->SetMaterialState(materialIdx, (ShaderParameterKey)paramInfo->key, blendVal);

            const ShaderParameterInfo* blendOpParam = RenderContext::Instance().GetShaderParameterInfoByName("blend_func");
            uint32_t blendOp[4] = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,0,0 };
            efig->SetMaterialState(materialIdx, (ShaderParameterKey)blendOpParam->key, blendOp);

            const ShaderParameterInfo* blendEqParam = RenderContext::Instance().GetShaderParameterInfoByName("blend_equation");
            uint32_t blendEq[4] = { GL_FUNC_ADD, 0,0,0 };
            efig->SetMaterialState(materialIdx, (ShaderParameterKey)blendEqParam->key, blendEq);
        }
        return efig;
    }

    EditableFigure* GraphicsHelper::createMesh(const std::vector<float>& points, const std::vector<uint32_t>& trianglesIndices, const std::string& texture, const std::vector<float>& uvs, ShaderDescriptor* shader, Vec3* normals, const Vec4& color)
    {
        if (shader == nullptr)
        {
            shader = ResourceCreator::Instance().NewShaderDescriptor();
            if (texture.length() > 0) shader->SetColorTexture(true);
            shader->SetBoneCondition(1, 1);
        }

        auto efig = ResourceCreator::Instance().NewEditableFigure("sprite", true);
        efig->Initialize();
        efig->Build();

        efig->AddMaterial("mate", *shader);
        efig->AddMesh("mesh", "mate");

        auto meshIdx = efig->GetMeshIndex(GenerateNameHash("mesh"));
        if (meshIdx == -1) return nullptr;

        //efig->SetVertexElements("mesh", ATTRIBUTE_ID_POSITION, points);
        efig->SetMeshVertexValues(meshIdx, (const void*)points.data(), (uint32_t)(points.size() / 3), ATTRIBUTE_ID_POSITION, 0);

        // if (uvs) efig->SetVertexElements("mesh", ATTRIBUTE_ID_UV0, uvs);
        if (uvs.size() > 0) efig->SetMeshVertexValues(meshIdx, (const void*)uvs.data(), (uint32_t)(uvs.size() / 2), ATTRIBUTE_ID_UV0, 0);

        // if (normals) efig->SetVertexElements("mesh", ATTRIBUTE_ID_NORMAL, normals);
        if (normals) efig->SetMeshVertexValues(meshIdx, (const void*)normals, 1, ATTRIBUTE_ID_NORMAL, 0);

        // efig->SetTriangles("mesh", trianglesIndices);
        efig->SetMeshIndices(meshIdx, 0, (const uint32_t*)trianglesIndices.data(), (uint32_t)(trianglesIndices.size()), 4);

        //efig->AddJoint("joint");
        Joint joint;
        int parentIndex = -1;
        efig->AddJoint(parentIndex, joint, false, "joint");

        //efig->SetMaterialParam("mate", "DiffuseColor", Vec4(1.f, 1.f, 1.f, 1.f));
        int materialIdx = efig->GetMaterialIndex(GenerateNameHash("mate"));
        float _color[4] = { color[0], color[1], color[2], color[3] };
        efig->SetMaterialParam(materialIdx, GenerateNameHash("DiffuseColor"), _color, ParamTypeFloat4);

        // efig->SetMaterialRenderState("mate", "cull_face_enable", false);

        if (texture.length() > 0)
        {
            // efig->SetMaterialParamTexture("mate", "ColorSampler", texture, SAMPLERSTATE_BORDER, SAMPLERSTATE_BORDER, SAMPLERSTATE_LINEAR, SAMPLERSTATE_LINEAR);
            Sampler sampler;
            sampler.samplerSlotNo = 0;
            sampler.samplerState.wrap_s = SamplerState::WRAP;
            sampler.samplerState.wrap_t = SamplerState::WRAP;
            sampler.samplerState.minfilter = SamplerState::LINEAR;
            sampler.samplerState.magfilter = SamplerState::LINEAR;
            sampler.tex = ResourceCreator::Instance().NewTexture(texture.c_str());
            sampler.tex->WaitInitialize();
            sampler.tex->WaitBuild();

            TextureSource texSrc;
            strncpy(texSrc.path, texture.c_str(), MAX_PATH);
            texSrc.normal = false;
            texSrc.wrap = false;
            sampler.textureNameIndex = efig->SetTextureSource(texSrc);
            efig->SetMaterialParam(materialIdx, GenerateNameHash("ColorSampler"), &sampler);

            // efig->SetMaterialRenderState("mate", "blend_enable", true);
            const ShaderParameterInfo* paramInfo = RenderContext::Instance().GetShaderParameterInfoByName("blend_enable");
            uint32_t blendVal[4] = { 1,0,0,0 };
            efig->SetMaterialState(materialIdx, (ShaderParameterKey)paramInfo->key, blendVal);

            const ShaderParameterInfo* blendOpParam = RenderContext::Instance().GetShaderParameterInfoByName("blend_func");
            uint32_t blendOp[4] = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,0,0 };
            efig->SetMaterialState(materialIdx, (ShaderParameterKey)blendOpParam->key, blendOp);

            const ShaderParameterInfo* blendEqParam = RenderContext::Instance().GetShaderParameterInfoByName("blend_equation");
            uint32_t blendEq[4] = { GL_FUNC_ADD, 0,0,0 };
            efig->SetMaterialState(materialIdx, (ShaderParameterKey)blendEqParam->key, blendEq);

        }
        return efig;
    }

    EditableFigure* GraphicsHelper::createGridMesh(const Vec2& size, const std::string& texture)
    {
        auto w = size.X()/2;
        auto h = size.Y()/2;
        std::vector<float> points = {-w,h,0.f, w,h,0.f, -w,-h,0.f, w,-h,0.f};
        const std::vector<uint32_t> _tris = {0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1};
        std::vector<float> uvs = {0.0, size.Y(), size.X(), size.Y(), 0.0, 0.0, size.X(), 0.0};

        auto efig = createMesh(points, _tris, texture, uvs);
        int materialIdx = efig->GetMaterialIndex(GenerateNameHash("mate"));

        Sampler sampler;
        sampler.samplerSlotNo = 0;
        sampler.samplerState.wrap_s = SamplerState::WRAP;
        sampler.samplerState.wrap_t = SamplerState::WRAP;
        sampler.samplerState.minfilter = SamplerState::LINEAR;
        sampler.samplerState.magfilter = SamplerState::LINEAR;
        sampler.tex = ResourceCreator::Instance().NewTexture(texture.c_str());
        sampler.tex->WaitInitialize();
        sampler.tex->WaitBuild();

        TextureSource texSrc;
        strncpy(texSrc.path, texture.c_str(), MAX_PATH);
        texSrc.normal = false;
        texSrc.wrap = true;
        sampler.textureNameIndex = efig->SetTextureSource(texSrc);
        efig->SetMaterialParam(materialIdx, GenerateNameHash("ColorSampler"), &sampler);
        return efig;
    }

    EditableFigure* GraphicsHelper::createText(const std::string& words, const std::string& fontPath, int fontSize, const Vec4& color, float scale, uint32_t pivot)
    {
        int w, h;
        calcTextSize(words.c_str(), fontPath.c_str(), fontSize, w, h, 1.f);

        auto texture =  ResourceCreator::Instance().NewTexture(unique("text").c_str(), nullptr, w, h, GL_RED);
        texture_setText(texture, words, fontPath, fontSize);

        auto shader = ResourceCreator::Instance().NewShaderDescriptor();
        shader->SetColorTexture(true);
        shader->SetBoneCondition(1, 1);
        shader->DiscardColorMapRGB(true);

        const std::vector<uint32_t> _tris = {0, 2, 1, 1, 2, 3};
        const std::vector<int32_t> _pivotoffset = {1,-1, 0,-1, -1,-1, 1,0, 0,0, -1,0, 1,1, 0,1, -1,1};

        auto hw = w/2 * scale;
        auto hh = h/2 * scale;
        auto px = _pivotoffset[pivot*2+0] * hw;
        auto py = _pivotoffset[pivot*2+1] * hh;

        std::vector<float> points = {-hw+px,hh+py,0.0, hw+px,hh+py,0.0, -hw+px,-hh+py,0.0, hw+px,-hh+py,0.0};
        std::vector<float> uvs = {0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0};

        auto efig = createMesh(points, _tris, "", uvs, shader);
        auto meshIdx = efig->GetMeshIndex(GenerateNameHash("mesh"));
        int materialIdx = efig->GetMaterialIndex(GenerateNameHash("mate"));
        efig->SetMaterialParam(materialIdx, GenerateNameHash("DiffuseColor"), color.P(), ParamTypeFloat4);

        Sampler sampler;
        sampler.samplerSlotNo = 0;
        sampler.samplerState.wrap_s = SamplerState::WRAP;
        sampler.samplerState.wrap_t = SamplerState::WRAP;
        sampler.samplerState.minfilter = SamplerState::LINEAR;
        sampler.samplerState.magfilter = SamplerState::LINEAR;
        sampler.tex = texture;

        TextureSource texSrc;
        strncpy(texSrc.path, texture->ResourceName(), MAX_PATH);
        texSrc.normal = false;
        texSrc.wrap = false;
        sampler.textureNameIndex = efig->SetTextureSource(texSrc);
        efig->SetMaterialParam(materialIdx, GenerateNameHash("ColorSampler"), &sampler);

        const ShaderParameterInfo* paramInfo = RenderContext::Instance().GetShaderParameterInfoByName("blend_enable");
        uint32_t blendVal[4] = { 1,0,0,0 };
        efig->SetMaterialState(materialIdx, (ShaderParameterKey)paramInfo->key, blendVal);

        const ShaderParameterInfo* blendOpParam = RenderContext::Instance().GetShaderParameterInfoByName("blend_func");
        uint32_t blendOp[4] = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,0,0 };
        efig->SetMaterialState(materialIdx, (ShaderParameterKey)blendOpParam->key, blendOp);

        const ShaderParameterInfo* blendEqParam = RenderContext::Instance().GetShaderParameterInfoByName("blend_equation");
        uint32_t blendEq[4] = { GL_FUNC_ADD, 0,0,0 };
        efig->SetMaterialState(materialIdx, (ShaderParameterKey)blendEqParam->key, blendEq);
        
        return efig;
    }

    EditableFigure* GraphicsHelper::createBitmapText(const std::string& words, const std::string& fontPath, int fontSize, const Vec4& color, uint32_t pivot, float scale)
    {
        //int w = 0, h = 0;

        /*auto id = BitmapFontHelper::getInstance()->createBitmapFont(fontPath, 0, 0, 0);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, "T", 18, 14, 363, 331, 10, 11, 0, 3, 10);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, "h", 18, 14, 225, 382, 8, 11, 1, 3, 10);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, "i", 18, 14, 478, 377, 2, 11, 1, 3, 4);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, "s", 18, 14, 199, 455, 7, 8, 1, 6, 9);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, " ", 18, 14, 66, 185, 1, 1, 0, 14, 5);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, "a", 18, 14, 18, 459, 8, 8, 1, 6, 10);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, "b", 18, 14, 198, 383, 8, 11, 1, 3, 10);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, "t", 18, 14, 436, 377, 5, 11, 1, 3, 6);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, "m", 18, 14, 494, 429, 12, 8, 2, 6, 16);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, "p", 18, 14, 436, 353, 8, 11, 1, 6, 10);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, "f", 18, 14, 442, 377, 5, 11, 1, 3, 7);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, "o", 18, 14, 483, 438, 8, 8, 1, 6, 10);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, "n", 18, 14, 0, 459, 8, 8, 1, 6, 10);
        BitmapFontHelper::getInstance()->addGlyphForChar(id, ".", 18, 14, 285, 476, 2, 3, 1, 11, 6);

        auto efig = BitmapFontHelper::getInstance()->createText("This is a bitmap font.", fontPath, fontSize);*/

        auto id = BitmapFontHelper::getInstance()->createBitmapFont(fontPath);

        auto efig = BitmapFontHelper::getInstance()->createText(words.c_str(), fontPath, fontSize);

        return efig;
    }

    std::string GraphicsHelper::unique(const std::string& str)
    {
        static int uniqueNumber = 9999; // avoid conflict with Python
        uniqueNumber++;
        return str + std::to_string(uniqueNumber);
    }

    void GraphicsHelper::texture_setText(Texture* texture, const std::string& word, const std::string& font, int size, int startX, int startY, int clear)
    {
        if(texture == nullptr) return;

        int texW, texH;
        uint8_t* bmp = createTextImage(word.c_str(), font.c_str(), size, texW, texH, 1.f);

        int maxW = texture->GetTextureWidth();
        int maxH = texture->GetTextureHeight();

        int ofsX = 0;
        if (startX < 0)
        {
            ofsX = -startX;
            startX = 0;
        }

        int cpyW = (maxW < (texW - ofsX)) ? maxW : (texW - ofsX);
        if (cpyW + startX > maxW) cpyW -= ((cpyW + startX) - maxW);

        int ofsY = 0;
        int cpyH = 0;

        if (startY >= 0)
        {
            cpyH = texH;
            int baseline = startY + cpyH;
            if (baseline > maxH) cpyH -= (baseline - maxH);
        }
        else
        {
            cpyH = texH + startY;
            ofsY = -startY;
            startY = 0;
        }

        if (maxH < cpyH) cpyH = maxH;
        int posY = (maxH - cpyH) - startY;

        if (cpyW > 0 && cpyH > 0) {
            int inBufW = PYXIE_ALIGN(texW, 4);
            int outBufW = PYXIE_ALIGN(cpyW, 4);

            unsigned char* bitmap2 = (unsigned char*)pyxie::PYXIE_MALLOC(outBufW * cpyH);
            unsigned char* op = bitmap2;

            unsigned char* ip = bmp  + (cpyH - 1 + ofsY) * inBufW + ofsX;

            for (int y = 0; y < cpyH; y++)
            {
                memcpy(op, ip, outBufW);
                ip -= inBufW;
                op += outBufW;
            }
            pyxie::PYXIE_FREE(bmp);

            if (clear)
            {
                int w = texture->GetTextureWidth();
                int h = texture->GetTextureHeight();
                int format = texture->GetFormat();
                uint8_t* bmp = createColorTexture(0, 0, 0, 0, w, h, format);
                if (bmp) texture->UpdateSubImage(bmp, 0, 0, w, h);
                PYXIE_SAFE_FREE(bmp);
            }
            texture->UpdateSubImage(bitmap2, startX, posY, cpyW, cpyH);
            pyxie::PYXIE_FREE(bitmap2);
        }
    }

    Vec3 ScreenToWorld(const Vec2& screenPos, const Vec2& windowSize, Camera* cam, float wZ)
    {
        if (cam == nullptr)
            return Vec3(screenPos, wZ);

        Mat4 proj, invView;
        cam->GetProjectionMatrix(proj);
        cam->GetViewInverseMatrix(invView);
        auto invProj = proj.Inverse();

        auto w = windowSize.X();
        auto h = windowSize.Y();
        auto x = screenPos.X() / w * 2;
        auto y = screenPos.Y() / h * 2;

        auto pos = Vec4(x, y, 0.f, 1.f);
        auto npos = invProj * pos;
        npos = invView * npos;
        if (npos[3])
        {
            npos[0] /= npos[3];
            npos[1] /= npos[3];
            npos[2] /= npos[3];
            npos[3] = 1.f;
        }

        pos = Vec4(x, y, 1.f, 1.f);
        auto fpos = invProj * pos;
        fpos = invView * fpos;
        if (fpos[3])
        {
            fpos[0] /= fpos[3];
            fpos[1] /= fpos[3];
            fpos[2] /= fpos[3];
            fpos[3] = 1.f;
        }

        auto dir = fpos - npos;
        dir.Normalize();
        auto pos4 = (npos + (dir * (npos[3] - wZ)));
        return { pos4[0], pos4[1], pos4[2] };
    }

    Vec2 WorldToScreen(Vec3 world, const Vec2& windowSize, Camera* cam)
    {
        Mat4 projMat;
        cam->GetProjectionMatrix(projMat);

        Mat4 viewMat;
        cam->GetViewInverseMatrix(viewMat);
        viewMat = viewMat.Inverse();

        auto pos = projMat * viewMat * Vec4(world, 1.f);
        if (pos[3])
        {
            pos[0] /= pos[3];
            pos[1] /= pos[3];
            pos[2] /= pos[3];
        }
        auto w = windowSize.X();
        auto h = windowSize.Y();
        return Vec2(pos[0] * w * 0.5f, pos[1] * h * 0.5f);
    }

    Vec4 AabbToScreenRect(const AABBox& aabb, const Vec2& windowSize, Camera* cam)
    {
        auto cen = aabb.getCenter();
        auto ext = aabb.getExtent() * 0.5f;
        auto extentPoints = new Vec2[8]
        {
            WorldToScreen(Vec3(cen[0] - ext[0], cen[1] - ext[1], cen[2] - ext[2]), windowSize, cam),
            WorldToScreen(Vec3(cen[0] + ext[0], cen[1] - ext[1], cen[2] - ext[2]), windowSize, cam),
            WorldToScreen(Vec3(cen[0] - ext[0], cen[1] - ext[1], cen[2] + ext[2]), windowSize, cam),
            WorldToScreen(Vec3(cen[0] + ext[0], cen[1] - ext[1], cen[2] + ext[2]), windowSize, cam),
            WorldToScreen(Vec3(cen[0] - ext[0], cen[1] + ext[1], cen[2] - ext[2]), windowSize, cam),
            WorldToScreen(Vec3(cen[0] + ext[0], cen[1] + ext[1], cen[2] - ext[2]), windowSize, cam),
            WorldToScreen(Vec3(cen[0] - ext[0], cen[1] + ext[1], cen[2] + ext[2]), windowSize, cam),
            WorldToScreen(Vec3(cen[0] + ext[0], cen[1] + ext[1], cen[2] + ext[2]), windowSize, cam)
        };

        auto minX = extentPoints[0][0];
        auto minY = extentPoints[0][1];

        auto maxX = extentPoints[0][0];
        auto maxY = extentPoints[0][1];

        for (int i = 0; i < 8; ++i)
        {
            auto v = extentPoints[i];
            if (v[0] < minX) minX = v[0];
            if (v[1] < minY) minY = v[1];
            if (maxX < v[0]) maxX = v[0];
            if (maxY < v[1]) maxY = v[1];
        }

        return Vec4(minX, minY, maxX, maxY);
    }

    bool RectInside(const Vec4& r1, const Vec4& r2)
    {
        return (r2[0] >= r1[0] && r2[2] <= r1[2] && r2[1] >= r1[1] && r2[3] <= r1[3]);
    }
}