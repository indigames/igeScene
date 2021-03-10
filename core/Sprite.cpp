#include <algorithm>

#include "core/Sprite.h"
#include "utils/GraphicsHelper.h"

namespace ige::scene
{
    Sprite::Sprite(const std::string& path, const Vec2& size)
        : m_figure(nullptr), m_size(size), m_tiling(1,1), m_offset(0,0), m_wrapMode(SamplerState::CLAMP)
    {
        setPath(path);
    }

    Sprite::~Sprite()
    {
        if(m_figure)
        {
            m_figure->DecReference();
            m_figure = nullptr;
        }
    }

    void Sprite::setSize(const Vec2& size)
    {
        if(m_size != size)
        {
            m_size = size;

            if (m_figure)
            {
                auto w = m_size.X() / 2.f;
                auto h = m_size.Y() / 2.f;
                const std::vector<float> points = { -w,h,0.f, w,h,0.f, -w,-h,0.f, w,-h,0.f };

                auto meshIdx = m_figure->GetMeshIndex(GenerateNameHash("mesh"));
                if (meshIdx == -1) return;
                m_figure->SetMeshVertexValues(meshIdx, (const void*)points.data(), (uint32_t)(points.size() / 3), ATTRIBUTE_ID_POSITION, 0);
            }
        }
    }

    void Sprite::setTiling(const Vec2& value)
    {
        if (m_tiling != value)
        {
            m_tiling = value;

            if (m_figure)
            {
                auto ow = m_offset.X();
                auto oh = m_offset.Y();
                auto tw = ow + m_tiling.X();
                auto th = oh + m_tiling.Y();
                const std::vector<float> uvs = { ow, th, tw, th, ow, oh, tw, oh };

                auto meshIdx = m_figure->GetMeshIndex(GenerateNameHash("mesh"));
                if (meshIdx == -1) return;
                m_figure->SetMeshVertexValues(meshIdx, (const void*)uvs.data(), (uint32_t)(uvs.size() / 2), ATTRIBUTE_ID_UV0, 0);
            }
        }
    }

    void Sprite::setOffset(const Vec2& value)
    {
        if (m_offset != value)
        {
            m_offset = value;

            if (m_figure)
            {
                auto ow = m_offset.X();
                auto oh = m_offset.Y();
                auto tw = ow + m_tiling.X();
                auto th = oh + m_tiling.Y();
                const std::vector<float> uvs = { ow, th, tw, th, ow, oh, tw, oh };

                auto meshIdx = m_figure->GetMeshIndex(GenerateNameHash("mesh"));
                if (meshIdx == -1) return;
                m_figure->SetMeshVertexValues(meshIdx, (const void*)uvs.data(), (uint32_t)(uvs.size() / 2), ATTRIBUTE_ID_UV0, 0);
            }
        }
    }

    void Sprite::setWrapMode(const SamplerState::WrapMode& value) 
    {
        if (m_wrapMode != value)
        {
            m_wrapMode = value;

            if (m_figure)
            {
                
                int materialIdx = m_figure->GetMaterialIndex(GenerateNameHash("mate"));
                
                const FigureMaterialParam* paramS = m_figure->GetMaterialParam(materialIdx, "ColorSampler");
                FigureMaterialParam*& param = const_cast<FigureMaterialParam*&>(paramS);
                param->sampler.samplerState.wrap_s = m_wrapMode;
                param->sampler.samplerState.wrap_t = m_wrapMode;
                m_figure->SetMaterialParam(materialIdx, "ColorSampler", &param->sampler);
            }
        }
    }

    void Sprite::setPath(const std::string& path)
    {
        if(strcmp(m_path.c_str(), path.c_str()) != 0)
        {
            m_path = path;
            std::replace(m_path.begin(), m_path.end(), '\\', '/');

            if (m_path.length() > 0)
            {
                // Free old texture
                if(m_figure != nullptr)
                {
                    const auto& textureSources = m_figure->GetTextureSources();
                    if(textureSources.size() > 0)
                    {
                        auto& textureSource = textureSources[0];
                        auto texture = (Texture*)ResourceManager::Instance().GetResource(textureSource.path, TEXTURETYPE);
                        if(texture)
                        {
                            texture->DecReference();
                            texture = nullptr;
                        }
                    }

                    m_texture = nullptr;
                }

                // Create figure if needed
                if (m_figure == nullptr)
                {
                    auto w = m_size.X() / 2.f;
                    auto h = m_size.Y() / 2.f;

                    const std::vector<float> points = { -w,h,0.f, w,h,0.f, -w,-h,0.f, w,-h,0.f };
                    const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
                    //const std::vector<float> uvs = { 0.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f };
                    auto ow = m_offset.X();
                    auto oh = m_offset.Y();
                    auto tw = ow + m_tiling.X();
                    auto th = oh + m_tiling.Y();
                    const std::vector<float> uvs = { ow, th, tw, th, ow, oh, tw, oh };
                    m_figure = GraphicsHelper::getInstance()->createMesh(points, triangles, path, uvs);
                    m_figure->WaitInitialize();
                }
                m_texture = ResourceCreator::Instance().NewTexture(m_path.c_str());
                Sampler sampler;
                sampler.samplerSlotNo = 0;
                sampler.samplerState.wrap_s = m_wrapMode;
                sampler.samplerState.wrap_t = m_wrapMode;
                sampler.samplerState.minfilter = SamplerState::LINEAR;
                sampler.samplerState.magfilter = SamplerState::LINEAR;
                sampler.tex = m_texture;//ResourceCreator::Instance().NewTexture(m_path.c_str());
                sampler.tex->WaitInitialize();
                sampler.tex->WaitBuild();

                TextureSource texSrc;
                strncpy(texSrc.path, m_path.c_str(), MAX_PATH);
                texSrc.normal = false;
                texSrc.wrap = false;
                sampler.textureNameIndex = m_figure->SetTextureSource(texSrc);

                int materialIdx = m_figure->GetMaterialIndex(GenerateNameHash("mate"));
                m_figure->SetMaterialParam(materialIdx, "ColorSampler", &sampler);
                const ShaderParameterInfo* blendParam = RenderContext::Instance().GetShaderParameterInfoByName("blend_enable");
                uint32_t blendVal[4] = { 1,0,0,0 };
                m_figure->SetMaterialState(materialIdx, (ShaderParameterKey)blendParam->key, blendVal);

                const ShaderParameterInfo* blendOpParam = RenderContext::Instance().GetShaderParameterInfoByName("blend_func");
                uint32_t blendOp[4] = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,0,0 };
                m_figure->SetMaterialState(materialIdx, (ShaderParameterKey)blendOpParam->key, blendOp);

                const ShaderParameterInfo* blendEqParam = RenderContext::Instance().GetShaderParameterInfoByName("blend_equation");
                uint32_t blendEq[4] = { GL_FUNC_ADD, 0,0,0 };
                m_figure->SetMaterialState(materialIdx, (ShaderParameterKey)blendEqParam->key, blendEq);
            }
        }
    }

    //! Enable alpha blending
    void Sprite::setAlphaBlendingEnable(bool enable)
    {
        if (m_bIsAlphaBlendingEnable != enable)
        {
            m_bIsAlphaBlendingEnable = enable;
            if (m_figure)
            {
                // Setup point lights shader
                for (int i = 0; i < m_figure->NumMaterials(); ++i)
                {
                    auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                    shaderDesc->SetValue(m_figure->GetShaderName(i));
                    shaderDesc->SetAlphaBlend(m_bIsAlphaBlendingEnable);
                    m_figure->SetShaderName(i, shaderDesc->GetValue());
                }
            }
        }
    }

    //! Alpha blending operation
    void Sprite::setAlphaBlendingOp(int op)
    {
        // Only 4 operations supported now
        if (op < ShaderDescriptor::AlphaBlendOP::COL || op > ShaderDescriptor::AlphaBlendOP::MUL)
            op = m_alphaBlendingOp;

        if (m_alphaBlendingOp != op)
        {
            m_alphaBlendingOp = op;
            if (m_figure)
            {
                for (int i = 0; i < m_figure->NumMaterials(); ++i)
                {
                    auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                    shaderDesc->SetValue(m_figure->GetShaderName(i));
                    shaderDesc->SetAlphaBlendOP(m_alphaBlendingOp);
                    m_figure->SetShaderName(i, shaderDesc->GetValue());
                }
            }
        }
    }
}
