#include <vmath.h>

#include <bitmapHelper.h>
#include "utils/GraphicsHelper.h"

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

    EditableFigure* GraphicsHelper::createMesh(const std::vector<float>& points, const std::vector<uint32_t>& trianglesIndices, const std::string& texture, const std::vector<float>& uvs, ShaderDescriptor* shader, Vec3* normals)
    {
        if (shader == nullptr)
        {
            shader = ResourceCreator::Instance().NewShaderDescriptor();
            if (texture.length() > 0) shader->SetColorTexture(true);
            shader->SetBoneCondition(1, 1);
        }

        auto efig = ResourceCreator::Instance().NewEditableFigure("sprite");
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
        efig->SetMeshIndices(meshIdx, 0, (const uint32_t*)trianglesIndices.data(), (uint32_t)(trianglesIndices.size() / 3), 4);

        //efig->AddJoint("joint");
        Joint joint;
        int parentIndex = -1;
        efig->AddJoint(parentIndex, joint, false, "joint");

        //efig->SetMaterialParam("mate", "DiffuseColor", Vec4(1.f, 1.f, 1.f, 1.f));
        int materialIdx = efig->GetMaterialIndex(GenerateNameHash("mate"));
        float color[4] = { 1.f, 1.f, 1.f, 1.f };
        efig->SetMaterialParam(materialIdx, "DiffuseColor", color, ParamTypeFloat4);

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
            efig->SetMaterialParam(materialIdx, "ColorSampler", &sampler);

            // efig->SetMaterialRenderState("mate", "blend_enable", true);
            const ShaderParameterInfo* paramInfo = RenderContext::Instance().GetShaderParameterInfoByName("blend_enable");
            uint32_t blendVal[4] = { 1,0,0,0 };
            efig->SetMaterialState(materialIdx, (ShaderParameterKey)paramInfo->key, blendVal);
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
        efig->SetMaterialParam(materialIdx, "ColorSampler", &sampler);
        return efig;
    }

    EditableFigure* GraphicsHelper::createText(const std::string& words, const std::string& fontPath, int fontSize, const Vec4& color, uint32_t pivot, float scale)
    {
        int w, h;
        calcTextSize(words.c_str(), fontPath.c_str(), fontSize, w, h);

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
        efig->SetMaterialParam(materialIdx, "DiffuseColor", color.P(), ParamTypeFloat4);

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
        efig->SetMaterialParam(materialIdx, "ColorSampler", &sampler);

        const ShaderParameterInfo* paramInfo = RenderContext::Instance().GetShaderParameterInfoByName("blend_enable");
        uint32_t blendVal[4] = { 1,0,0,0 };
        efig->SetMaterialState(materialIdx, (ShaderParameterKey)paramInfo->key, blendVal);
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
        uint8_t* bmp = createTextImage(word.c_str(), font.c_str(), size, texW, texH);

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
            int inBufW = ALIGN(texW, 4);
            int outBufW = ALIGN(cpyW, 4);

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


}