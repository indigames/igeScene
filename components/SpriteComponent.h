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
        SpriteComponent(SceneObject &owner, const std::string &texture = "", const Vec2 &size = {32.f, 32.f}, bool isBillboard = false);

        //! Destructor
        virtual ~SpriteComponent();

        //! Get component name
        virtual std::string getName() const override { return "Sprite"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::Sprite; }

        //! Enable
        virtual void onEnable() override;

        //! Disable
        virtual void onDisable() override;

        //! Update
        virtual void onUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

        //! Get associated figure
        EditableFigure *getFigure() { return m_sprite->getFigure(); }

        //! Path
        virtual void setPath(const std::string &path);
        virtual const std::string &getPath() const { return m_path; }

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

        //! SpriteType
        void setSpriteType(int value);
        const SpriteType& getSpriteType() const { return m_sprite->getSpriteType(); }

        void setBorderLeft(float value);
        const float getBorderLeft() const { return m_sprite->getBorderLeft(); }

        void setBorderRight(float value);
        const float getBorderRight() const { return m_sprite->getBorderRight(); }

        void setBorderTop(float value);
        const float getBorderTop() const { return m_sprite->getBorderTop(); }

        void setBorderBottom(float value);
        const float getBorderBottom() const { return m_sprite->getBorderBottom(); }

        //! Alpha
        void setAlpha(float value);
        const float getAlpha() const;

        //! Color
        virtual void setColor(float r, float g, float b, float a = 1);
        virtual void setColor(const Vec4& value);
        virtual const Vec4 getColor()  const { return m_sprite->getColor(); }

        //! FillMethod
        virtual void setFillMethod(int value);
        virtual const FillMethod& getFillMethod() const { return m_sprite->getFillMethod(); }

        //! FillOrigin
        virtual void setFillOrigin(int value);
        virtual const FillOrigin& getFillOrigin() const { return m_sprite->getFillOrigin(); }

        //! FillAmount
        virtual void setFillAmount(float value);
        virtual const float getFillAmount()  const { return m_sprite->getFillAmount(); }

        //! Clockwise
        virtual void setClockwise(bool value);
        virtual const bool getClockwise() const { return m_sprite->getClockwise(); }



        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Utils to add/remove resouce from showcase
        void onResourceAdded(Resource* res);
        void onResourceRemoved(Resource* res);
        bool m_bResAdded = false;

        const Vec4& getBorder() const { return m_sprite->getBorder(); }
        void setBorder(const Vec4& value);

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

    protected:

        //! Sprite
        std::shared_ptr<Sprite> m_sprite;

        std::string m_path;
        Texture* m_texture;

        //! Billboard setting
        bool m_bIsBillboard = false;

        friend class UIImage;
    };
} // namespace ige::scene
