#pragma once

#include <set>
#include <algorithm>

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/TransformComponent.h"

#include "event/Event.h"

namespace ige::scene
{
    //! RectTransform: transform of UI elements
    class RectTransform : public TransformComponent
    {
    public:
        //! Constructor
        RectTransform(const std::shared_ptr<SceneObject>& owner, const Vec3& pos = Vec3(), const Vec2& size = Vec2{ 128.f, 128.f });

        //! Destructor
        virtual ~RectTransform();

        //! Get component name
        virtual std::string getName() const override { return "RectTransform"; }

        //! Get local transform
        const Mat4& getLocalTransform();

        //! Get canvas space transform
        const Mat4& getCanvasSpaceTransform();

        //! Get viewport transform
        const Mat4& getViewportTransform();

        //! Anchor
        const Vec4& getAnchor() const { return m_anchor; }
        void setAnchor(const Vec4& anchor);

        //! Pivot
        const Vec2& getPivot() const { return m_pivot; }
        void setPivot(const Vec2& pivot);

        //! Offset
        const Vec4& getOffset() const { return m_offset; }
        void setOffset(const Vec4& offset);

        //! Size
        Vec2 getSize();
        void setSize(const Vec2& size);

        //! Position
        void setPosition(const Vec3& pos) override;
        void setWorldPosition(const Vec3& pos) override;

        //! Rotation
        void setRotation(const Quat& rot) override;
        void setWorldRotation(const Quat& rot) override;

        //! Scale
        void setScale(const Vec3& scale) override;
        void setWorldScale(const Vec3& scale) override;

        //! Get rect in canvas space (no scale, no rotate)
        const Vec4& getRect();

        //! Get rect in viewport space
        const Vec4& getRectInViewportSpace() { return m_rectViewport; }

        //! Dirty flag
        void setDirty();

        //! OnUpdate
        void onUpdate(float dt) override;

        //! Serialize
        void to_json(json& j) const override { }

        //! Deserialize
        void from_json(const json& j) override { }

        //! Add observer: just do nothing
        void addObserver(TransformComponent* observer) override { }

        //! Remove observer: just do nothing
        void removeObserver(TransformComponent* observer) override { }

        //! Notify to all observers: just do nothing
        void notifyObservers(const ETransformMessage& message) override { }

        //! Handle notification from parent: just do nothing
        void onNotified(const ETransformMessage& message) override { }

        //! Check point inside rect
        bool isPointInside(const Vec2& point) const;

    protected:
        bool hasScale() const;
        bool hasRotation() const;
        bool hasScaleOrRotation() const;

        Vec2 getPivotInCanvasSpace();
        Vec2 getPivotInViewportSpace();

        Vec2 getAnchorCenterInCanvasSpace();

    protected:
        //! Anchor
        Vec4 m_anchor;

        // Offset
        Vec4 m_offset;

        //! Pivot: center of the content
        Vec2 m_pivot;

        //! Position Z
        float m_posZ;

        //! Cached rect in canvas space (no scale, no rotate)
        Vec4 m_rect;
        Vec4 m_rectViewport;
        bool m_rectDirty = true;

        //! Cached transform to viewport space
        Mat4 m_viewportTransform;
        bool m_viewportTransformDirty = true;

        //! Cached transform to canvas space
        Mat4 m_canvasTransform;
        bool m_canvasTransformDirty = true;

        //! Associated EditableFigure object
        EditableFigure* m_figure;

        //! Events
        Event<EditableFigure*> m_onFigureCreatedEvent;
        Event<EditableFigure*> m_onFigureDestroyedEvent;

    };
}
