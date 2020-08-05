#pragma once

#include <set>
#include <algorithm>

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/TransformComponent.h"

#include "event/Event.h"

namespace ige::scene
{
    //! Class Rect: store the bounds of an axis-aligned rectangle
    class Rect
    {
    public:
        //! Constructors
        Rect() { m_left = m_top = m_right = m_bottom = 0.f; }
        Rect(float left, float top, float right, float bottom): m_left(left), m_top(top), m_right(right), m_bottom(bottom) {}

        //! Size
        inline float getWidth() const { return m_right - m_left; }
        inline float getHeight() const { return m_bottom - m_top; }
        inline Vec2 getSize() const { return Vec2(getWidth(), getHeight()); }

        //! Center
        inline float getCenterX() const { return (m_left + m_right) * 0.5f; }
        inline float getCenterY() const { return (m_top + m_bottom) * 0.5f; }
        inline Vec2 getCenter() const { return Vec2(getCenterX(), getCenterY()); }

        //! Translation
        inline void translate(const Vec2& offset)
        {
            m_left += offset.X();
            m_right += offset.X();
            m_top += offset.Y();
            m_bottom += offset.Y();
        }

        //! Check equal
        inline bool operator==(const Rect& rhs) const
        {
            return m_left == rhs.m_left && m_right == rhs.m_right && m_top == rhs.m_top && m_bottom == rhs.m_bottom;
        }

        //! Check not equal
        inline bool operator!=(const Rect& rhs) const
        {
            return !(*this == rhs);
        }

        //! Properties
        float m_left;
        float m_top;
        float m_right;
        float m_bottom;
    };

    //! Class Anchors: store anchor values as a Rect
    class Anchor: public Rect
    {
    public:
        //! Constructor
        Anchor(): Rect(0.5f, 0.5f, 0.5f, 0.5f) {}
        Anchor(float left, float top, float right, float bottom): Rect(left, top, right, bottom) { clampZeroToOne(); }

        //! Claim values in range of 0.0f - 1.0f
        void clampZeroToOne()
        {
            m_left = std::clamp(m_left, 0.0f, 1.0f);
            m_top = std::clamp(m_top, 0.0f, 1.0f);
            m_right = std::clamp(m_right, 0.0f, 1.0f);
            m_bottom = std::clamp(m_bottom, 0.0f, 1.0f);
        }

        //! Check if stretching by width
        inline bool isStretchWidth() const { return m_left != m_right; }

        //! Check if stretching by height
        inline bool isStretchHeight() const { return m_top != m_bottom; }
    };

    // Class Offset: represent offsets from the anchors
    class Offset: public Rect
    {
    public:
        Offset(): Rect(-50.f, -50.f, 50.f, 50.f) {}
        Offset(float left, float top, float right, float bottom): Rect(left, top, right, bottom) {}

        Offset& operator+=(const Vec2& rhs)
        {
            m_left += rhs.X();
            m_right += rhs.X();
            m_top += rhs.Y();
            m_bottom += rhs.Y();
            return *this;
        }

        Offset operator+(const Vec2& rhs) const
        {
            Offset result(*this);
            result.m_left += rhs.X();
            result.m_right += rhs.X();
            result.m_top += rhs.Y();
            result.m_bottom += rhs.Y();
            return result;
        }
    };

    //! RectTransform: transform of UI elements
    class RectTransform : public TransformComponent
    {
    public:
        //! Constructor
        RectTransform(const std::shared_ptr<SceneObject>& owner, const Vec3& pos = Vec3(), const Vec2& size = Vec2{ 64.f, 64.f });

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
        const Anchor& getAnchor() const { return m_anchor; }
        void setAnchor(const Anchor& anchor);

        //! Pivot
        const Vec2& getPivot() const { return m_pivot; }
        void setPivot(const Vec2& pivot);

        //! Offset
        const Offset& getOffset() const { return m_offset; }
        void setOffset(const Offset& offset);

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
        const Rect& getRect();

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

    protected:
        bool hasScale() const;
        bool hasRotation() const;
        bool hasScaleOrRotation() const;

        Vec2 getPivotInCanvasSpace();
        Vec2 getPivotInViewportSpace();

        Vec2 getAnchorCenterInCanvasSpace();

    protected:
        //! Anchor
        Anchor m_anchor;

        // Offset
        Offset m_offset;

        //! Pivot: center of the content
        Vec2 m_pivot;

        //! Position Z
        float m_posZ;

        //! Cached rect in canvas space (no scale, no rotate)
        Rect m_rect;
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