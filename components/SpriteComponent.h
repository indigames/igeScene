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

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Sprite
        std::shared_ptr<Sprite> m_sprite;

        //! Billboard setting
        bool m_bIsBillboard = false;
    };
} // namespace ige::scene
