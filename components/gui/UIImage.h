#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/SpriteComponent.h"
#include "components/gui/UIMaskable.h"


namespace ige::scene
{
    class UIEventContext;
    class EventContext;

    //! UIImage
    class UIImage : public SpriteComponent, public UIMaskable
    {
    public:
        //! Constructor
        UIImage(SceneObject& owner, const std::string& texture = "", const Vec2& size = { 128.f, 128.f }, const bool isSliced = false, const Vec4& border = { 0.f, 0.f, 0.f, 0.f });

        //! Destructor
        virtual ~UIImage();

        //! Get component name
        virtual std::string getName() const override { return "UIImage"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::UIImage; }

        //! Update
        virtual void onUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

        //! Enable
        virtual void onEnable() override;

        //! Disable
        virtual void onDisable() override;
            
        void setInteractable(bool value);
        const bool isInteractable() const { return m_bIsInteractable; }

        Event<>& getOnClickedEvent() { return m_onClickedEvent; }
        Event<>& getOnPressedEvent() { return m_onPressedEvent; }
        Event<>& getOnDragEvent() { return m_onDragEvent; }
        Event<>& getOnReleasedEvent() { return m_onReleasedEvent; }
        Event<>& getOnSelectedEvent() { return m_onSelectedEvent; }

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

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

        //! Resource added event
        virtual void onResourceAdded(Resource* res) override;

        //! Overide function UIMaskable
        virtual EditableFigure* getCurrentFigure() override;
        virtual SceneObject* getSceneObjectOwner() override;

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
