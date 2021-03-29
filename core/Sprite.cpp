#include <algorithm>

#include "core/Sprite.h"
#include "utils/GraphicsHelper.h"


namespace ige::scene
{
    Sprite::Sprite(const Vec2& size)
        : m_figure(nullptr), m_size(size), m_tiling(1, 1), m_offset(0, 0), m_wrapMode(SamplerState::CLAMP),
        m_fillMethod(FillMethod::None), m_fillOrigin(FillOrigin::Bottom), m_fillAmount(1), m_alpha(1), m_texture(nullptr),
        m_color(1.f, 1.f, 1.f, 1.f)
    {
    }

    Sprite::Sprite(Texture* texture, const Vec2& size)
        : m_figure(nullptr), m_size(size), m_tiling(1,1), m_offset(0,0), m_wrapMode(SamplerState::CLAMP),
        m_fillMethod(FillMethod::None), m_fillOrigin(FillOrigin::Bottom), m_fillAmount(1), m_alpha(1), m_texture(nullptr),
        m_color(1.f, 1.f, 1.f, 1.f)
    {
        if(texture != nullptr)
            setTexture(texture);
    }

    Sprite::~Sprite()
    {
        if(m_figure)
        {
            const auto& textureSources = m_figure->GetTextureSources();
            if (textureSources.size() > 0)
            {
                auto& textureSource = textureSources[0];
                auto texture = (Texture*)ResourceManager::Instance().GetResource(textureSource.path, TEXTURETYPE);
                if (texture)
                {
                    texture->DecReference();
                    texture = nullptr;
                }
            }
            m_texture = nullptr;

            m_figure->DecReference();
            m_figure = nullptr;
        }

        if (m_texture) 
        {
            m_texture->DecReference();
            m_texture = nullptr;
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

            if (m_figure && m_fillMethod == FillMethod::None)
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

            if (m_figure && m_fillMethod == FillMethod::None)
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

            if (m_figure && m_fillMethod == FillMethod::None)
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

    void Sprite::setFillMethod(const FillMethod& value)
    {
        if (m_fillMethod != value)
        {
            m_fillMethod = value;
            if (m_fillMethod == FillMethod::Horizontal)
                m_fillOrigin = FillOrigin::Left;
            else if (m_fillMethod == FillMethod::Vertical)
                m_fillOrigin = FillOrigin::Bottom;
            else if (m_fillMethod == FillMethod::Radial90)
                m_fillOrigin = FillOrigin::BottomLeft;
            else if (m_fillMethod == FillMethod::Radial180 || m_fillMethod == FillMethod::Radial360)
                m_fillOrigin = FillOrigin::Bottom;
            if (m_figure)
            {
                draw();
            }
        }
    }

    void Sprite::setFillOrigin(const FillOrigin& value)
    {
        if (m_fillOrigin != value)
        {
            m_fillOrigin = value;

            if (m_figure)
            {
                draw();
            }
        }
    }

    void Sprite::setFillAmount(float amount)
    {
        float value = amount > 1 ? 1 : amount < 0 ? 0 : amount;
        if (m_fillAmount != value)
        {
            m_fillAmount = value;

            if (m_figure)
            {
                draw();
            }
        }
    }

    void Sprite::setClockwise(bool value) {
        if (m_clockwise != value)
        {
            m_clockwise = value;

            if (m_figure)
            {
                draw();
            }
        }
    }

    void Sprite::setAlpha(float value) {
        if (m_alpha != value) {
            m_alpha = value > 1.f ? 1.f : value < 0 ? 0 : value;
            
            if (m_figure)
            {
                draw();
            }
        }
    }

    void Sprite::setTexture(Texture* value) {
        if (m_texture != value) 
        {
            if (m_figure != nullptr)
            {
                if (m_texture) {
                    m_texture->DecReference();
                    m_texture = nullptr;
                }
            }
            m_texture = value;
            if (value == nullptr)
                releaseSprite();
            else
            {
                m_texture->IncReference();
                draw();
                applyTexture();
            }
        }
    }

    void Sprite::setColor(float r, float g, float b, float a, bool redraw)
    {
        Vec4 newColor(r, g, b, a);
        setColor(newColor, redraw);
    }

    void Sprite::setColor(const Vec4& value, bool redraw) {
        if (m_color != value) {
            m_color[0] = MATH_CLAMP(value[0], 0.0f, 1.0f);
            m_color[1] = MATH_CLAMP(value[1], 0.0f, 1.0f);
            m_color[2] = MATH_CLAMP(value[2], 0.0f, 1.0f);
            m_color[3] = MATH_CLAMP(value[3], 0.0f, 1.0f);

            if (m_figure && redraw)
            {
                draw();
            }
        }
    }

    //!===============================================

    void Sprite::releaseSprite() {
        if (m_figure)
        {
            const auto& textureSources = m_figure->GetTextureSources();
            if (textureSources.size() > 0)
            {
                auto& textureSource = textureSources[0];
                auto texture = (Texture*)ResourceManager::Instance().GetResource(textureSource.path, TEXTURETYPE);
                if (texture)
                {
                    texture->DecReference();
                    texture = nullptr;
                }
            }
            m_texture = nullptr;

            m_figure->DecReference();
            m_figure = nullptr;
        }
    }

    void Sprite::draw() {
        switch (m_fillMethod)
        {
        case FillMethod::None:
        default:
            drawNormal();
            break;
        case FillMethod::Horizontal:
        case FillMethod::Vertical:
            drawFillBar();
            break;
        case FillMethod::Radial90:
            drawFillRadial90();
            break;
        case FillMethod::Radial180:
            drawFillRadial180();
            break;
        case FillMethod::Radial360:
            drawFillRadial360();
            break;
        }
        
    }

    void Sprite::drawNormal() {
        
        auto w = m_size.X() / 2.f;
        auto h = m_size.Y() / 2.f;

        const std::vector<float> points = { -w,h,0.f, w,h,0.f, -w,-h,0.f, w,-h,0.f };
        const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };

        auto ow = m_offset.X();
        auto oh = m_offset.Y();
        auto tw = ow + m_tiling.X();
        auto th = oh + m_tiling.Y();
        const std::vector<float> uvs = { ow, th, tw, th, ow, oh, tw, oh };

        applyMesh(points, triangles, uvs);
    }

    void Sprite::drawFillBar()
    {
        Vec2 min, max;
        if (m_fillMethod == FillMethod::Horizontal)
        {
            if (m_fillOrigin == FillOrigin::Left || m_fillOrigin == FillOrigin::Top)
            {
                min = Vec2(0, 0);
                max = Vec2(m_fillAmount, 1);
            }
            else
            {
                min = Vec2(1 - m_fillAmount, 0);
                max = Vec2(1, 1);
            }
        }
        else
        {
            if (m_fillOrigin == FillOrigin::Left || m_fillOrigin == FillOrigin::Top)
            {
                min = Vec2(0, 1 - m_fillAmount);
                max = Vec2(1, 1);
            }
            else
            {
                min = Vec2(0, 0);
                max = Vec2(1, m_fillAmount);
            }
        }

        auto w = m_size.X() / 2.f;
        auto h = m_size.Y() / 2.f;
        auto wMin = -w + m_size.X() * (min.X());
        auto wMax = w - m_size.X() * (1 - max.X());
        auto hMin = -h + (m_size.Y() * min.Y());
        auto hMax = h - m_size.Y() * (1 - max.Y());

        const std::vector<float> points = { wMin,hMax,0.f, wMax,hMax,0.f, wMin,hMin,0.f, wMax,hMin,0.f };
        const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
        auto ow = min.X();
        auto oh = min.Y();
        auto tw = max.X();
        auto th = max.Y();
        const std::vector<float> uvs = { ow, th, tw, th, ow, oh, tw, oh };

        applyMesh(points, triangles, uvs);

        if (m_wrapMode != SamplerState::WrapMode::CLAMP) {
            int materialIdx = m_figure->GetMaterialIndex(GenerateNameHash("mate"));

            const FigureMaterialParam* paramS = m_figure->GetMaterialParam(materialIdx, "ColorSampler");
            FigureMaterialParam*& param = const_cast<FigureMaterialParam*&>(paramS);
            param->sampler.samplerState.wrap_s = SamplerState::WrapMode::CLAMP;
            param->sampler.samplerState.wrap_t = SamplerState::WrapMode::CLAMP;
            m_figure->SetMaterialParam(materialIdx, "ColorSampler", &param->sampler);
        }
    }

    void Sprite::drawFillRadial90()
    {
        float angle = 2.0f * ((float)PI) * (m_clockwise ? (1.f - m_fillAmount) : m_fillAmount);
        //! Fill 90
        angle = angle / 4;
        Vec2 mid;
        Vec2 start, end, opposite;
        switch (m_fillOrigin) {
        case FillOrigin::BottomLeft:
        default:
            mid = Vec2(0, 0);
            start = Vec2(0, 1);
            end = Vec2(1, 0);
            opposite = Vec2(1, 1);
            break;
        case FillOrigin::TopLeft:
            mid = Vec2(0, 1);
            start = Vec2(1, 1);
            end = Vec2(0, 0);
            opposite = Vec2(1, 0);
            break;
        case FillOrigin::TopRight:
            mid = Vec2(1, 1);
            start = Vec2(1, 0);
            end = Vec2(0, 1);
            opposite = Vec2(0, 0);
            break;
        case FillOrigin::BottomRight:
            mid = Vec2(1, 0);
            start = Vec2(0, 0);
            end = Vec2(1, 1);
            opposite = Vec2(0, 1);
            break;
        }
        Vec2 percentagePt = rotateByAngle(mid, end, angle);
        
        int index = 0;
        Vec2 hit;
        if (m_fillAmount == 0) 
        {
            hit = start;
            index = 1;
        }
        else if (m_fillAmount == 1)
        {
            hit = end;
            index = 4;
        }
        else
        {
            index = m_fillAmount > 0.5f ? 3 : 2;
            float min_t = FLT_MAX;
            for (int i = 0; i < 4; i++) {
                int pIndex = (i + 1) % 4;

                Vec2 edgePtA = boundaryTexCoord(i);
                Vec2 edgePtB = boundaryTexCoord(pIndex);
                
                float s = 0, t = 0;

                if (isLineIntersect(edgePtA, edgePtB, mid, percentagePt, &s, &t))
                {
                    if (t > 0.f) {
                        if (t < min_t) {
                            min_t = t;
                        }
                    }
                }
            }
            hit = mid + ((percentagePt - mid) * min_t);
        }
        
        if (index == 0) return;
        
        if (index == 1) {
            //! Similar to drawNormal
            auto w = m_size.X() / 2.f;
            auto h = m_size.Y() / 2.f;
            //const std::vector<float> points = { -w,h,0.f, w,h,0.f, -w,-h,0.f, w,-h,0.f };
            const std::vector<float> points = { 0,0,0.f, 0,0,0.f, 0,0,0.f, 0,0,0.f };
            const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
            const std::vector<float> uvs = { 0, 0, 0, 0, 0, 0, 0, 0 };

            applyMesh(points, triangles, uvs);
        } 
        else if (index == 4) 
        {
            auto w = m_size.X() / 2.f;
            auto h = m_size.Y() / 2.f;
            const std::vector<float> points = { -w,h,0.f, w,h,0.f, -w,-h,0.f, w,-h,0.f };
            const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
            const std::vector<float> uvs = { 0, 1, 1, 1, 0, 0, 1, 0 };

            applyMesh(points, triangles, uvs);
        }
        else if(index == 3) 
        {
            auto w = m_size.X();
            auto h = m_size.Y();
            Vec2 btmLeft = Vec2(-m_size.X() / 2.f, -m_size.Y() / 2.f);
            if (m_clockwise) {
                //! Set 4 Point (Hit, Mid, Opposite, Start)
                const std::vector<float> points = {
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f,
                    btmLeft.X() + w * opposite.X(), btmLeft.Y() + h * opposite.Y(), 0.f,
                    btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
                const std::vector<float> uvs = { hit.X(), hit.Y() , mid.X(), mid.Y(), opposite.X(), opposite.Y(), start.X(), start.Y() };
                applyMesh(points, triangles, uvs);
            }
            else {
                //! Set 4 Point (Hit, Mid, Opposite, End)
                const std::vector<float> points = {
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f,
                    btmLeft.X() + w * opposite.X(), btmLeft.Y() + h * opposite.Y(), 0.f,
                    btmLeft.X() + w * end.X(), btmLeft.Y() + h * end.Y(), 0.f
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
                const std::vector<float> uvs = { hit.X(), hit.Y() , mid.X(), mid.Y(), opposite.X(), opposite.Y(), end.X(), end.Y() };
                applyMesh(points, triangles, uvs);
            }
        }
        else {
            auto w = m_size.X();
            auto h = m_size.Y();
            Vec2 btmLeft = Vec2(-m_size.X() / 2.f, -m_size.Y() / 2.f);
            if (m_clockwise) {
                //! Set 3 Point (Start, Hit, Mid)
                const std::vector<float> points = {
                    btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f,
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(),0.f
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 0 };
                const std::vector<float> uvs = { start.X(), start.Y(), hit.X(), hit.Y(), mid.X(), mid.Y() };

                applyMesh(points, triangles, uvs);
            }
            else {
                //! Set 3 Point (End, Hit, Mid)
                const std::vector<float> points = {
                    btmLeft.X() + w * end.X(), btmLeft.Y() + h * end.Y(), 0.f,
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(),0.f
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 0 };
                const std::vector<float> uvs = { end.X(), end.Y(), hit.X(), hit.Y(), mid.X(), mid.Y() };

                applyMesh(points, triangles, uvs);
            }

        }

        
    }

    void Sprite::drawFillRadial180()
    {
        float angle = 2.0f * ((float)PI) * (m_clockwise ? (1.f - m_fillAmount) : m_fillAmount);
        //! Fill 180
        angle = angle / 2;
        
        Vec2 mid;
        Vec2 start, end, opposite_1, opposite_2;

        switch (m_fillOrigin) {
        case FillOrigin::Bottom:
        default:
            mid = Vec2(0.5f, 0.f);
            start = Vec2(0, 0);
            end = Vec2(1, 0);
            opposite_1 = Vec2(0, 1);
            opposite_2 = Vec2(1, 1);
            break;
        case FillOrigin::Left:
            mid = Vec2(0, 0.5f);
            start = Vec2(0, 1);
            end = Vec2(0, 0);
            opposite_1 = Vec2(1, 1);
            opposite_2 = Vec2(1, 0);
            break;
        case FillOrigin::Top:
            mid = Vec2(0.5f, 1);
            start = Vec2(1, 1);
            end = Vec2(0, 1);
            opposite_1 = Vec2(1, 0);
            opposite_2 = Vec2(0, 0);
            break;
        case FillOrigin::Right:
            mid = Vec2(1, 0.5f);
            start = Vec2(1, 0);
            end = Vec2(1, 1);
            opposite_1 = Vec2(0, 0);
            opposite_2 = Vec2(0, 1);
            break;
        }
        Vec2 percentagePt = rotateByAngle(mid, end, angle);
        int index = 0;
        Vec2 hit;
        if (m_fillAmount == 0)
        {
            hit = start;
            index = 0;
        }
        else if (m_fillAmount == 1)
        {
            hit = end;
            index = 4;
        }
        else
        {
            //index = m_fillAmount > 0.75f ? 3 : m_fillAmount > 0.25f ? 2 : 1;
            float len = Vec2::Length(mid - opposite_1);
            float max_angle = len ;
            float min_t = FLT_MAX;
            for (int i = 0; i < 4; i++) {
                int pIndex = (i + 1) % 4;

                Vec2 edgePtA = boundaryTexCoord(i);
                Vec2 edgePtB = boundaryTexCoord(pIndex);

                float s = 0, t = 0;
                
                if (isLineIntersect(edgePtA, edgePtB, mid, percentagePt, &s, &t))
                {
                    if (t > 0.f) {
                        if (t < min_t) {
                            min_t = t;
                            if ((edgePtA == opposite_1 || edgePtA == opposite_2) && (edgePtB == opposite_1 || edgePtB == opposite_2))
                                index = 2;
                            else 
                                index = 0;
                        }
                    }
                }
            }
            hit = mid + ((percentagePt - mid) * min_t);
            if (index == 0) index = m_fillAmount > 0.5f ? 3 : 1;
        }

        switch (index) {
        case 0: // Set Quad to Zero
        default:
            {
                //! Similar to drawNormal
                auto w = m_size.X() / 2.f;
                auto h = m_size.Y() / 2.f;
                //const std::vector<float> points = { -w,h,0.f, w,h,0.f, -w,-h,0.f, w,-h,0.f };
                const std::vector<float> points = { 0,0,0.f, 0,0,0.f, 0,0,0.f, 0,0,0.f };
                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
                const std::vector<float> uvs = { 0, 0, 0, 0, 0, 0, 0, 0 };

                applyMesh(points, triangles, uvs);
            }
            break;
        case 1: // Draw Triangle
            {
                auto w = m_size.X();
                auto h = m_size.Y();
                Vec2 btmLeft = Vec2(-m_size.X() / 2.f, -m_size.Y() / 2.f);
                if (m_clockwise) {
                    //! Set 3 Point (Start, Hit, Mid)
                    const std::vector<float> points = {
                        btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f,
                        btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f,
                        btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(),0.f
                    };

                    const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 0 };
                    const std::vector<float> uvs = { hit.X(), hit.Y(), start.X(), start.Y(), mid.X(), mid.Y() };

                    applyMesh(points, triangles, uvs);
                }
                else {
                    //! Set 3 Point (End, Hit, Mid)
                    const std::vector<float> points = {
                        btmLeft.X() + w * end.X(), btmLeft.Y() + h * end.Y(), 0.f,
                        btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f,
                        btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(),0.f
                    };

                    const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 0 };
                    const std::vector<float> uvs = { end.X(), end.Y(), hit.X(), hit.Y(), mid.X(), mid.Y() };

                    applyMesh(points, triangles, uvs);
                }

            }
            break;
        case 2: // Draw Quadrilateral
            {
                auto w = m_size.X();
                auto h = m_size.Y();
                Vec2 btmLeft = Vec2(-m_size.X() / 2.f, -m_size.Y() / 2.f);
                if (m_clockwise) {
                    //! Set 4 Point (Opposite_1, Hit, Start, Mid)
                    const std::vector<float> points = {
                        btmLeft.X() + w * opposite_1.X(), btmLeft.Y() + h * opposite_1.Y(), 0.f,
                        btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f,
                        btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f,
                        btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f
                    };

                    const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
                    const std::vector<float> uvs = { opposite_1.X(), opposite_1.Y(),hit.X(), hit.Y(),  start.X(), start.Y(), mid.X(), mid.Y() };
                    applyMesh(points, triangles, uvs);
                }
                else {
                    //! Set 4 Point (Hit, Opposite_2, Mid, End)
                    const std::vector<float> points = {
                        btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f,
                        btmLeft.X() + w * opposite_2.X(), btmLeft.Y() + h * opposite_2.Y(), 0.f,
                        btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f,
                        btmLeft.X() + w * end.X(), btmLeft.Y() + h * end.Y(), 0.f
                    };

                    const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
                    const std::vector<float> uvs = { hit.X(), hit.Y(), opposite_2.X(), opposite_2.Y(), mid.X(), mid.Y(),  end.X(), end.Y() };
                    applyMesh(points, triangles, uvs);
                }
            }
            break;
        case 3: // draw Pentagon 
        {
            auto w = m_size.X();
            auto h = m_size.Y();
            Vec2 btmLeft = Vec2(-m_size.X() / 2.f, -m_size.Y() / 2.f);
            if (m_clockwise) {
                //! Set 5 Point (Opposite_1, Opposite_2, Start, Mid, Hit)
                const std::vector<float> points = {
                    btmLeft.X() + w * opposite_1.X(), btmLeft.Y() + h * opposite_1.Y(), 0.f,
                    btmLeft.X() + w * opposite_2.X(), btmLeft.Y() + h * opposite_2.Y(), 0.f,
                    btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f,
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 3, 4, 1, 
                                                          1, 2, 0, 3, 2, 1, 1, 4, 3 };
                const std::vector<float> uvs = { opposite_1.X(), opposite_1.Y(), opposite_2.X(), opposite_2.Y(), start.X(), start.Y(), mid.X(), mid.Y(), hit.X(), hit.Y() };
                applyMesh(points, triangles, uvs);
            }
            else {
                //! Set 5 Point (Opposite_1, Opposite_2, Hit, Mid, End)
                const std::vector<float> points = {
                    btmLeft.X() + w * opposite_1.X(), btmLeft.Y() + h * opposite_1.Y(), 0.f,
                    btmLeft.X() + w * opposite_2.X(), btmLeft.Y() + h * opposite_2.Y(), 0.f,
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f,
                    btmLeft.X() + w * end.X(), btmLeft.Y() + h * end.Y(), 0.f
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 3, 4, 1,
                                                          1, 2, 0, 3, 2, 1, 1, 4, 3 };
                const std::vector<float> uvs = { opposite_1.X(), opposite_1.Y(), opposite_2.X(), opposite_2.Y(), hit.X(), hit.Y(), mid.X(), mid.Y(),  end.X(), end.Y() };
                applyMesh(points, triangles, uvs);
            }
        }
            break;
        case 4:
            {
                auto w = m_size.X() / 2.f;
                auto h = m_size.Y() / 2.f;
                const std::vector<float> points = { -w,h,0.f, w,h,0.f, -w,-h,0.f, w,-h,0.f };
                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
                const std::vector<float> uvs = { 0, 1, 1, 1, 0, 0, 1, 0 };

                applyMesh(points, triangles, uvs);
            }
            break;
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

    void Sprite::drawFillRadial360()
    {
        float angle = 2.0f * ((float)PI) * (m_clockwise ? (1.f - m_fillAmount) : m_fillAmount);
        float m_angle = RADIANS_TO_DEGREES(angle);
        Vec2 mid(0.5f, 0.5f);
        Vec2 start, edge_1, edge_2, edge_3, edge_4;

        switch (m_fillOrigin) {
        case FillOrigin::Bottom:
        default:
            start = Vec2(0.5f, 0);
            edge_1 = Vec2(0, 0);
            edge_2 = Vec2(0, 1);
            edge_3 = Vec2(1, 1);
            edge_4 = Vec2(1, 0);
            break;
        case FillOrigin::Left:
            start = Vec2(0, 0.5f);
            edge_1 = Vec2(0, 1);
            edge_2 = Vec2(1, 1);
            edge_3 = Vec2(1, 0);
            edge_4 = Vec2(0, 0);
            break;
        case FillOrigin::Top:
            start = Vec2(0.5f, 1);
            edge_1 = Vec2(1, 1);
            edge_2 = Vec2(1, 0);
            edge_3 = Vec2(0, 0);
            edge_4 = Vec2(0, 1);
            break;
        case FillOrigin::Right:
            start = Vec2(1, 0.5f);
            edge_1 = Vec2(1, 0);
            edge_2 = Vec2(0, 0);
            edge_3 = Vec2(0, 1);
            edge_4 = Vec2(1, 1);
            break;
        }

        Vec2 percentagePt = rotateByAngle(mid, start, angle);
        int index = 0;
        Vec2 hit;
        if (m_fillAmount == 0)
        {
            hit = start;
            index = 0;
        }
        else if (m_fillAmount == 1)
        {
            hit = start;
            index = 6;
        }
        else
        {
            float min_t = FLT_MAX;
            for (int i = 0; i <= 4; i++) {
                int pIndex = (i + (4 - 1)) % 4;

                Vec2 edgePtA = boundaryTexCoord(i % 4);
                Vec2 edgePtB = boundaryTexCoord(pIndex);


                float s = 0, t = 0;

                if (isLineIntersect(edgePtA, edgePtB, mid, percentagePt, &s, &t))
                {
                    if (i == 0 || i == 4) {
                        if (0 > s || s > 1)
                            continue;
                    }

                    if (t >= 0.f) {
                        if (t < min_t) {
                            min_t = t;
                            index = i;

                            if ((edgePtA == edge_1 || edgePtA == edge_4) && (edgePtB == edge_1 || edgePtB == edge_4))
                                index = m_fillAmount > 0.5f ? 5 : 1;
                            else if ((edgePtA == edge_1 || edgePtA == edge_2) && (edgePtB == edge_1 || edgePtB == edge_2))
                                index = m_clockwise ? 2 : 4;
                            else if ((edgePtA == edge_2 || edgePtA == edge_3) && (edgePtB == edge_2 || edgePtB == edge_3))
                                index = 3;
                            else if ((edgePtA == edge_3 || edgePtA == edge_4) && (edgePtB == edge_3 || edgePtB == edge_4))
                                index = m_clockwise ? 4 : 2;
                            else index = 0;
                        }
                    }
                }

            }
            hit = mid + ((percentagePt - mid) * min_t);


        }


        switch (index) {
        case 0: // Set Quad to Zero
        default:
        {
            //! Similar to drawNormal
            auto w = m_size.X() / 2.f;
            auto h = m_size.Y() / 2.f;
            const std::vector<float> points = { 0,0,0.f, 0,0,0.f, 0,0,0.f, 0,0,0.f };
            const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
            const std::vector<float> uvs = { 0, 0, 0, 0, 0, 0, 0, 0 };

            applyMesh(points, triangles, uvs);
        }
        break;
        case 1:
        {
            auto w = m_size.X();
            auto h = m_size.Y();
            Vec2 btmLeft = Vec2(-m_size.X() / 2.f, -m_size.Y() / 2.f);
            if (m_clockwise) {
                //! Set 3 Point (Start, Mid, Hit)
                const std::vector<float> points = {
                    btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f,
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 0 };
                const std::vector<float> uvs = { start.X(), start.Y(), mid.X(), mid.Y() , hit.X(), hit.Y() };

                applyMesh(points, triangles, uvs);
            }
            else {
                //! Set 3 Point (start, Hit, Mid)
                const std::vector<float> points = {
                    btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f,
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(),0.f
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 0 };
                const std::vector<float> uvs = { start.X(), start.Y(), hit.X(), hit.Y(), mid.X(), mid.Y() };

                applyMesh(points, triangles, uvs);
            }
        }
        break;
        case 2:
        {
            auto w = m_size.X();
            auto h = m_size.Y();
            Vec2 btmLeft = Vec2(-m_size.X() / 2.f, -m_size.Y() / 2.f);
            if (m_clockwise) {
                //! Set 4 Point (Start, E1, Mid, Hit)
                const std::vector<float> points = {
                    btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f,
                    btmLeft.X() + w * edge_1.X(), btmLeft.Y() + h * edge_1.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f,
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
                const std::vector<float> uvs = { start.X(), start.Y(), edge_1.X(), edge_1.Y(), mid.X(), mid.Y(), hit.X(), hit.Y()};
                applyMesh(points, triangles, uvs);
            }
            else {
                //! Set 4 Point (E4, Start, Hit, Mid)
                const std::vector<float> points = {
                    btmLeft.X() + w * edge_4.X(), btmLeft.Y() + h * edge_4.Y(), 0.f,
                    btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f,
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f
                    
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
                const std::vector<float> uvs = { edge_4.X(), edge_4.Y() ,  start.X(), start.Y() , hit.X(), hit.Y(),  mid.X(), mid.Y()};
                applyMesh(points, triangles, uvs);
            }
        }
        break;
        case 3: 
        {
            auto w = m_size.X();
            auto h = m_size.Y();
            Vec2 btmLeft = Vec2(-m_size.X() / 2.f, -m_size.Y() / 2.f);
            if (m_clockwise) {
                //! Set 5 Point (Start, Edge1, Mid, Edge2, Hit)
                const std::vector<float> points = {
                    btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f,
                    btmLeft.X() + w * edge_1.X(), btmLeft.Y() + h * edge_1.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f,
                    btmLeft.X() + w * edge_2.X(), btmLeft.Y() + h * edge_2.Y(), 0.f,
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 2, 4, 3, 
                                                          1, 2, 0, 3, 2, 1, 3, 4, 2 };
                const std::vector<float> uvs = { start.X(), start.Y(), edge_1.X(), edge_1.Y(), mid.X(), mid.Y(), edge_2.X(), edge_2.Y(), hit.X(), hit.Y() };
                applyMesh(points, triangles, uvs);
            }
            else {
                //! Set 5 Point (edge_4, Start, edge_3, Mid, Hit)
                const std::vector<float> points = {
                    btmLeft.X() + w * edge_4.X(), btmLeft.Y() + h * edge_4.Y(), 0.f,
                    btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f,
                    btmLeft.X() + w * edge_3.X(), btmLeft.Y() + h * edge_3.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f,
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 2, 4, 3,
                                                          1, 2, 0, 3, 2, 1, 3, 4, 2 };
                const std::vector<float> uvs = { edge_4.X(), edge_4.Y(), start.X(), start.Y(), edge_3.X(), edge_3.Y(), mid.X(), mid.Y(), hit.X(), hit.Y() };
                applyMesh(points, triangles, uvs);
            }
        }
        break;
        case 4:
        {
            auto w = m_size.X();
            auto h = m_size.Y();
            Vec2 btmLeft = Vec2(-m_size.X() / 2.f, -m_size.Y() / 2.f);
            if (m_clockwise) {
                //! Set 6 Point (Start, Edge1, Mid, Edge2, Edge3, Hit)
                const std::vector<float> points = {
                    btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f,
                    btmLeft.X() + w * edge_1.X(), btmLeft.Y() + h * edge_1.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f,
                    btmLeft.X() + w * edge_2.X(), btmLeft.Y() + h * edge_2.Y(), 0.f,
                    btmLeft.X() + w * edge_3.X(), btmLeft.Y() + h * edge_3.Y(), 0.f,
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 2, 4, 3, 2, 5, 4,
                                                          1, 2, 0, 3, 2, 1, 3, 4, 2, 4, 5, 2 };
                const std::vector<float> uvs = { start.X(), start.Y(), edge_1.X(), edge_1.Y(), mid.X(), mid.Y(), edge_2.X(), edge_2.Y(), edge_3.X(), edge_3.Y(), hit.X(), hit.Y() };
                applyMesh(points, triangles, uvs);
            }
            else {
                //! Set 6 Point (edge_4, Start, edge_3, Mid, edge_2, Hit)
                const std::vector<float> points = {
                    btmLeft.X() + w * edge_4.X(), btmLeft.Y() + h * edge_4.Y(), 0.f,
                    btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f,
                    btmLeft.X() + w * edge_3.X(), btmLeft.Y() + h * edge_3.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f,
                    btmLeft.X() + w * edge_2.X(), btmLeft.Y() + h * edge_2.Y(), 0.f,
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f
                };

                const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 3, 2, 4, 3, 4, 5,
                                                          1, 2, 0, 3, 2, 1, 4, 2, 3, 5, 4, 3 };
                const std::vector<float> uvs = { edge_4.X(), edge_4.Y(), start.X(), start.Y(), edge_3.X(), edge_3.Y(), mid.X(), mid.Y(), edge_2.X(), edge_2.Y(), hit.X(), hit.Y() };
                applyMesh(points, triangles, uvs);
            }
        }
        break;
        case 5:
        {
            auto w = m_size.X();
            auto h = m_size.Y();
            Vec2 btmLeft = Vec2(-m_size.X() / 2.f, -m_size.Y() / 2.f);
            if (m_clockwise) {
                //! Set 7 Point (Edge_4, Hit, Edge_3, Mid, Edge_2, Edge_1, Start)
                const std::vector<float> points = {
                    btmLeft.X() + w * edge_4.X(), btmLeft.Y() + h * edge_4.Y(), 0.f,
                    btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f,
                    btmLeft.X() + w * edge_3.X(), btmLeft.Y() + h * edge_3.Y(), 0.f,
                    btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f,
                    btmLeft.X() + w * edge_2.X(), btmLeft.Y() + h * edge_2.Y(), 0.f,
                    btmLeft.X() + w * edge_1.X(), btmLeft.Y() + h * edge_1.Y(), 0.f,
                    btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f
                };

                const std::vector<uint32_t> triangles = { 0, 3, 1, 0, 2, 3, 3, 2, 4, 3, 4, 5, 6, 3, 5,
                                                          1, 3, 0, 3, 2, 0, 4, 2, 3, 5, 4, 3, 5, 3, 6 };
                const std::vector<float> uvs = { edge_4.X(), edge_4.Y(), hit.X(), hit.Y(), edge_3.X(), edge_3.Y(), mid.X(), mid.Y(), edge_2.X(), edge_2.Y(), edge_1.X(), edge_1.Y(), start.X(), start.Y()};
                applyMesh(points, triangles, uvs);
            }
            else {
                //! Set 7 Point (edge_4, Start, edge_3, Mid, Edge_2, Edge_1, Hit)
                const std::vector<float> points = {
                   btmLeft.X() + w * edge_4.X(), btmLeft.Y() + h * edge_4.Y(), 0.f,
                   btmLeft.X() + w * start.X(), btmLeft.Y() + h * start.Y(), 0.f,
                   btmLeft.X() + w * edge_3.X(), btmLeft.Y() + h * edge_3.Y(), 0.f,
                   btmLeft.X() + w * mid.X(), btmLeft.Y() + h * mid.Y(), 0.f,
                   btmLeft.X() + w * edge_2.X(), btmLeft.Y() + h * edge_2.Y(), 0.f,
                   btmLeft.X() + w * edge_1.X(), btmLeft.Y() + h * edge_1.Y(), 0.f,
                   btmLeft.X() + w * hit.X(), btmLeft.Y() + h * hit.Y(), 0.f
                };

                const std::vector<uint32_t> triangles = { 0, 3, 1, 0, 2, 3, 3, 2, 4, 3, 4, 5, 6, 3, 5,
                                                          1, 3, 0, 3, 2, 0, 4, 2, 3, 5, 4, 3, 5, 3, 6 };
                const std::vector<float> uvs = { edge_4.X(), edge_4.Y(), start.X(), start.Y(), edge_3.X(), edge_3.Y(), mid.X(), mid.Y(), edge_2.X(), edge_2.Y(), edge_1.X(), edge_1.Y(), hit.X(), hit.Y() };
                applyMesh(points, triangles, uvs);
            }
        }
        break;
        case 6:
        {
            auto w = m_size.X() / 2.f;
            auto h = m_size.Y() / 2.f;
            const std::vector<float> points = { -w,h,0.f, w,h,0.f, -w,-h,0.f, w,-h,0.f };
            const std::vector<uint32_t> triangles = { 0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1 };
            const std::vector<float> uvs = { 0, 1, 1, 1, 0, 0, 1, 0 };

            applyMesh(points, triangles, uvs);
        }
        break;
        }
    }

    void Sprite::applyMesh(const std::vector<float>& points, const std::vector<uint32_t>& triangles, const std::vector<float>& uvs) {
        if (m_figure == nullptr)
        {
            m_figure = GraphicsHelper::getInstance()->createMesh(points, triangles, m_texture, uvs, nullptr, nullptr, m_color);
            m_figure->WaitInitialize();
        }
        else
        {
            auto meshIdx = m_figure->GetMeshIndex(GenerateNameHash("mesh"));
            if (meshIdx == -1) return;

            m_figure->SetMeshVertexValues(meshIdx, (const void*)points.data(), (uint32_t)(points.size() / 3), ATTRIBUTE_ID_POSITION, 0);
            m_figure->SetMeshIndices(meshIdx, 0, (const uint32_t*)triangles.data(), (uint32_t)(triangles.size() / 3), 4);
            m_figure->SetMeshVertexValues(meshIdx, (const void*)uvs.data(), (uint32_t)(uvs.size() / 2), ATTRIBUTE_ID_UV0, 0);
            m_figure->SetMeshAlpha(meshIdx, m_alpha);

            int materialIdx = m_figure->GetMaterialIndex(GenerateNameHash("mate"));
            float color[4] = { m_color[0], m_color[1], m_color[2], m_color[3] };
            m_figure->SetMaterialParam(materialIdx, "DiffuseColor", color, ParamTypeFloat4);
        }
    }

    void Sprite::applyTexture() {
        if (m_figure == nullptr) return;

        Sampler sampler;
        sampler.samplerSlotNo = 0;
        sampler.samplerState.wrap_s = m_wrapMode;
        sampler.samplerState.wrap_t = m_wrapMode;
        sampler.samplerState.minfilter = SamplerState::LINEAR;
        sampler.samplerState.magfilter = SamplerState::LINEAR;
        if (m_texture) {
            sampler.tex = m_texture;
            sampler.tex->WaitInitialize();
            sampler.tex->WaitBuild();

            TextureSource texSrc;
            strncpy(texSrc.path, m_texture->ResourceName(), MAX_PATH);
            texSrc.normal = false;
            texSrc.wrap = false;
            sampler.textureNameIndex = m_figure->SetTextureSource(texSrc);
        }

        int materialIdx = m_figure->GetMaterialIndex(GenerateNameHash("mate"));
        m_figure->SetMaterialParam(materialIdx, "ColorSampler", &sampler);
        const ShaderParameterInfo* paramInfo = RenderContext::Instance().GetShaderParameterInfoByName("blend_enable");
        uint32_t blendVal[4] = { 1,0,0,0 };
        m_figure->SetMaterialState(materialIdx, (ShaderParameterKey)paramInfo->key, blendVal);

        const ShaderParameterInfo* blendOpParam = RenderContext::Instance().GetShaderParameterInfoByName("blend_func");
        uint32_t blendOp[4] = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,0,0 };
        m_figure->SetMaterialState(materialIdx, (ShaderParameterKey)blendOpParam->key, blendOp);

        const ShaderParameterInfo* blendEqParam = RenderContext::Instance().GetShaderParameterInfoByName("blend_equation");
        uint32_t blendEq[4] = { GL_FUNC_ADD, 0,0,0 };
        m_figure->SetMaterialState(materialIdx, (ShaderParameterKey)blendEqParam->key, blendEq);
    }

    Vec2 Sprite::rotateByAngle(const Vec2& pivot, const Vec2& target, float angle)
    {
        Vec2 v_angle(cosf(angle), sinf(angle));
        Vec2 v_rot = (target - pivot);
        Vec2 v_out(v_rot.X() * v_angle.X() - v_rot.Y() * v_angle.Y(), v_rot.X() * v_angle.Y() + v_rot.Y() * v_angle.X());
        return pivot + v_out;
    }

    float Sprite::crossProduct2Vector(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D)
    {
        return (D.Y() - C.Y()) * (B.X() - A.X()) - (D.X() - C.X()) * (B.Y() - A.Y());
    }

    Vec2 Sprite::boundaryTexCoord(int index)
    {
        if (m_clockwise)
        {
            return Vec2(index > 1 ? 1 : 0, index % 3 > 0 ? 1 : 0);
        }
        else
        {
            return Vec2(index % 3 > 0 ? 1 : 0, index > 1 ? 1 : 0);
        }
    }

    Vec2 Sprite::findInterpolationPoint(Vec2& target, Vec2& other, float alpha) {
        return target * (1.f - alpha) + other * alpha;
    }

    bool Sprite::isLineIntersect(const Vec2& A, const Vec2& B,
        const Vec2& C, const Vec2& D,
        float* S, float* T)
    {
        if ((A.X() == B.X() && A.Y() == B.Y()) || (C.X() == D.X() && C.Y() == D.Y()))
        {
            return false;
        }

        const float denom = crossProduct2Vector(A, B, C, D);

        if (denom == 0)
        {
            // Lines parallel or overlap
            return false;
        }

        if (S != nullptr) *S = crossProduct2Vector(C, D, C, A) / denom;
        if (T != nullptr) *T = crossProduct2Vector(A, B, C, A) / denom;

        return true;
    }
}
