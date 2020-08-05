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
        SpriteComponent(const std::shared_ptr<SceneObject>& owner, const std::string& texture = "", const Vec2& size = {32.f, 32.f});

        //! Destructor
        virtual ~SpriteComponent();

        //! Get component name
        virtual std::string getName() const override { return "SpriteComponent"; }

        //! Update
        virtual void onUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Get associated figure
        EditableFigure* getFigure() { return m_sprite->getFigure(); }

        //! Path
        void setPath(const std::string& path);
        const std::string& getPath() const { return m_sprite->getPath(); }

        //! Size
        void setSize(const Vec2& size);
        const Vec2& getSize() const { return m_sprite->getSize(); }

        //! Events
        Event<EditableFigure*>& getOnFigureCreatedEvent() { return m_onFigureCreatedEvent; }
        Event<EditableFigure*>& getOnFigureDestroyedEvent() { return m_onFigureDestroyedEvent; }

    protected:
        //! Sprite
        std::shared_ptr<Sprite> m_sprite;

        //! Events
        Event<EditableFigure*> m_onFigureCreatedEvent;
        Event<EditableFigure*> m_onFigureDestroyedEvent;
    };
}
