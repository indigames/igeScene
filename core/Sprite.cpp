#include "core/Sprite.h"
#include "utils/GraphicsHelper.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

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

                adjustTextureUVs();
            }
        }
    }

    void Sprite::setPath(const std::string& path)
    {
        if(m_path != path)
        {
            m_path = path;
            if (m_path.length() > 0)
            {
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
                sampler.samplerState.wrap_s = SamplerState::WRAP;
                sampler.samplerState.wrap_t = SamplerState::WRAP;
                sampler.samplerState.minfilter = SamplerState::LINEAR;
                sampler.samplerState.magfilter = SamplerState::LINEAR;
                sampler.samplerState.mipfilter = SamplerState::LINEAR_MIPMAP_LINEAR;
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

                adjustTextureUVs();
            }
        }
    }

    void Sprite::adjustTextureUVs()
    {
        if(!m_figure) return;

        auto textureSources = m_figure->GetTextureSources();
        if(textureSources.size() > 0)
        {
            auto textureSource = textureSources[0];
            auto texture = (Texture*)ResourceManager::Instance().GetResource(textureSource.path, TEXTURETYPE);
            if(texture)
            {
                m_texSize = Vec2(texture->GetTextureWidth(), texture->GetTextureHeight());
                auto textureRatio = m_texSize.X() / m_texSize.Y();
                auto imageRatio = m_size.X() / m_size.Y();

                auto uvOffset = Vec2(1.f, 1.f);
                if(textureRatio > imageRatio) // width is bigger
                    uvOffset.X(textureRatio / imageRatio);
                else // height is bigger
                    uvOffset.Y(textureRatio / imageRatio);

                uvOffset.X((1.f - uvOffset.X()) / 2.f);
                uvOffset.Y((1.f - uvOffset.Y()) / 2.f);

                const std::vector<float> uvs = {
                    0.f + uvOffset.X(), 1.f - uvOffset.Y(),
                    1.f - uvOffset.X(), 1.f - uvOffset.Y(),
                    0.f + uvOffset.X(), 0.f + uvOffset.Y(),
                    1.f - uvOffset.X(), 0.f + uvOffset.Y()
                };

                auto meshIdx = m_figure->GetMeshIndex(GenerateNameHash("mesh"));
                m_figure->SetMeshVertexValues(meshIdx, (const void*)uvs.data(), (uint32_t)(uvs.size() / 2), ATTRIBUTE_ID_UV0, 0);
            }
        }
    }
}
