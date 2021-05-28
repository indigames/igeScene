#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/SpriteComponent.h"


namespace ige::scene
{
    class UIEventContext;
    class EventContext;

    //! UIImage
    class UIImage : public SpriteComponent
    {
    public:
        //! Constructor
        UIImage(SceneObject& owner, const std::string& texture = "", const Vec2& size = { 128.f, 128.f }, const bool isSliced = false, const Vec4& border = { 0.f, 0.f, 0.f, 0.f });

        //! Destructor
        virtual ~UIImage();

        //! Get component name
        virtual std::string getName() const override { return "UIImage"; }

        //! Update
        virtual void onUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

        //! FillMethod
        void setFillMethod(int value);
        const FillMethod& getFillMethod() const { return m_sprite->getFillMethod(); }

        //! FillOrigin
        void setFillOrigin(int value);
        const FillOrigin& getFillOrigin() const { return m_sprite->getFillOrigin(); }
        
        //! FillAmount
        void setFillAmount(float value);
        const float getFillAmount()  const { return m_sprite->getFillAmount(); }
        
        //! Clockwise
        void setClockwise(bool value);
        const bool getClockwise() const { return m_sprite->getClockwise(); }
        
        void setInteractable(bool value);
        const bool isInteractable() const { return m_bIsInteractable; }

        Event<>& getOnClickedEvent() { return m_onClickedEvent; }
        Event<>& getOnPressedEvent() { return m_onPressedEvent; }
        Event<>& getOnDragEvent() { return m_onDragEvent; }
        Event<>& getOnReleasedEvent() { return m_onReleasedEvent; }
        Event<>& getOnSelectedEvent() { return m_onSelectedEvent; }

        void setStencilMask(int value = -1);
    protected:
        virtual void _onTouchPress(EventContext* context);
        virtual void _onTouchDrag(EventContext* context);
        virtual void _onTouchRelease(EventContext* context);
        virtual void _onSelected(EventContext* context);
        virtual void _onClick(EventContext* context);
        virtual void _onExit(EventContext* context);

        //! Serialize
        virtual void to_json(json &j) const override;

        //! Deserialize
        virtual void from_json(const json &j) override;

        virtual void onSetParent(EventContext* context);

    protected:
        bool m_bIsInteractable;

        //! Events
        Event<> m_onClickedEvent;
        Event<> m_onPressedEvent;
        Event<> m_onDragEvent;
        Event<> m_onReleasedEvent;
        Event<> m_onSelectedEvent;
    };
}
