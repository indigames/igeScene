#include <algorithm>

#include "core/Sprite.h"
#include "utils/GraphicsHelper.h"

namespace ige::scene
{
    Sprite::Sprite(const std::string& path, const Vec2& size)
        : m_figure(nullptr), m_size(size)
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

    void Sprite::setPath(const std::string& path)
    {
        if(m_path != path)
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
                }

                // Create figure if needed
                if (m_figure == nullptr)
                {
                    auto w = m_size.X() / 2.f;
                    auto h = m_size.Y() / 2.f;

                    const std::vector<float> points = { -w,h,0.f, w,h,0.f, -w,-h,0.f, w,-h,0.f };
                    const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
                    const std::vector<float> uvs = { 0.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f };

                    m_figure = GraphicsHelper::getInstance()->createMesh(points, triangles, path, uvs);
                    m_figure->WaitInitialize();
                }

                Sampler sampler;
                sampler.samplerSlotNo = 0;
                sampler.samplerState.wrap_s = SamplerState::CLAMP;
                sampler.samplerState.wrap_t = SamplerState::CLAMP;
                sampler.samplerState.minfilter = SamplerState::LINEAR;
                sampler.samplerState.magfilter = SamplerState::LINEAR;
                sampler.tex = ResourceCreator::Instance().NewTexture(m_path.c_str());
                sampler.tex->WaitInitialize();
                sampler.tex->WaitBuild();

                TextureSource texSrc;
                strncpy(texSrc.path, m_path.c_str(), MAX_PATH);
                texSrc.normal = false;
                texSrc.wrap = false;
                sampler.textureNameIndex = m_figure->SetTextureSource(texSrc);

                int materialIdx = m_figure->GetMaterialIndex(GenerateNameHash("mate"));
                m_figure->SetMaterialParam(materialIdx, "ColorSampler", &sampler);

                const ShaderParameterInfo* paramInfo = RenderContext::Instance().GetShaderParameterInfoByName("blend_enable");
                uint32_t blendVal[4] = { 1,0,0,0 };
                m_figure->SetMaterialState(materialIdx, (ShaderParameterKey)paramInfo->key, blendVal);
            }
        }
    }
}
