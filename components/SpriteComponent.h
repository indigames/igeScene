#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/Component.h"
#include "core/Sprite.h"

namespace ige::scene
{
    //! SpriteComponent
    class SpriteComponent : public Component
    {
    public:
        //! Constructor
        SpriteComponent(SceneObject &owner, const std::string &texture = "", const Vec2 &size = {32.f, 32.f}, bool isBillboard = false, bool isGUI = false);

        //! Destructor
        virtual ~SpriteComponent();

        //! Get component name
        virtual std::string getName() const override { return "SpriteComponent"; }

        //! Update
        virtual void onUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

        //! Get associated figure
        EditableFigure *getFigure() { return m_sprite->getFigure(); }

        //! Path
        void setPath(const std::string &path);
        const std::string &getPath() const { return m_sprite->getPath(); }

        //! Size
        void setSize(const Vec2 &size);
        const Vec2 &getSize() const { return m_sprite->getSize(); }

        //! Billboard
        void setBillboard(bool isBillboard = true);
        const bool isBillboard() const { return m_bIsBillboard; }

        //! Tiling
        void setTiling(const Vec2& value);
        const Vec2& getTiling() const { return m_sprite->getTiling(); }

        //! Offset
        void setOffset(const Vec2& value);
        const Vec2& getOffset() const { return m_sprite->getOffset(); }

        //! WrapMode
        void setWrapMode(int value);
        const SamplerState::WrapMode& getWrapMode() const { return m_sprite->getWrapMode(); }

        //! Alpha blending
        const bool isAlphaBlendingEnable() const { return m_sprite && m_sprite->isAlphaBlendingEnable(); }
        void setAlphaBlendingEnable(bool enable = true) { if (m_sprite) m_sprite->setAlphaBlendingEnable(enable); }

        //! Alpha blending operation
        const int getAlphaBlendingOp() const { return m_sprite ? m_sprite->getAlphaBlendingOp() : 0; }
        void setAlphaBlendingOp(int op) { if (m_sprite) m_sprite->setAlphaBlendingOp(op); }


        //! Alpha
        void setAlpha(float value);
        const float getAlpha()  const { return m_sprite->getAlpha(); }


    protected:

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

    protected:

        //! Sprite
        std::shared_ptr<Sprite> m_sprite;

        //! Billboard setting
        bool m_bIsBillboard = false;

        bool m_bIsGUI = false;

        friend class UIImage;
    };
} // namespace ige::scene
