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
        RectTransform(SceneObject &owner, const Vec3 &pos = Vec3(), const Vec2 &size = Vec2{64.f, 64.f});

        //! Destructor
        virtual ~RectTransform();

        //! Get component name
        virtual std::string getName() const override { return "RectTransform"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::RectTransform; }

        //! Get local transform
        const Mat4 &getLocalTransform();

        //! Get canvas space transform
        const Mat4 &getCanvasSpaceTransform();

        //! Get viewport transform
        const Mat4 &getViewportTransform();

        //! Anchor
        const Vec4 &getAnchor() const { return m_anchor; }
        void setAnchor(const Vec4 &anchor);

        //! Pivot
        const Vec2 &getPivot() const { return m_pivot; }
        void setPivot(const Vec2 &pivot);

        //! Offset
        const Vec4 &getOffset() const { return m_offset; }
        void setOffset(const Vec4 &offset);

        //! Anchor Offset
        const Vec4& getAnchorOffset() const { return m_anchorOffset; }
        void setAnchorOffset(const Vec4& value) { m_anchorOffset = value; }

        //! Anchored Position
        const Vec2& getAnchoredPosition() const { return m_anchoredPosition; }
        void setAnchoredPosition(const Vec2& value);

        //! Size
        Vec2 getSize();
        void setSize(const Vec2 &size);

        //! Set Parent
        virtual void setParent(std::shared_ptr<TransformComponent> comp) override;

        //! Translate
        void worldTranslate(const Vec3 &trans) override;

        //! Rotate
        void worldRotate(const Quat &rot) override { rotate(rot); }

        //! Scale
        void worldScale(const Vec3 &scl) override { scale(scl); }

        //! Position
        void setPosition(const Vec3 &pos) override;
        void setWorldPosition(const Vec3 &pos) override {}

        //! Rotation
        void setRotation(const Quat &rot) override;
        void setWorldRotation(const Quat &rot) override {}

        //! Scale
        void setScale(const Vec3 &scale) override;
        void setWorldScale(const Vec3 &scale) override {}

        //! Get rect in canvas space (no scale, no rotate)
        const Vec4 &getRect();

        //! Dirty flag
        void setRectDirty();

        //! Set transform dirty
        void setTransformDirty();

        //! Set local & Size dirty
        void setLocalToRectDirty();

        //! OnUpdate
        void onUpdate(float dt) override;

        //! Handle notification from parent: just do nothing
        void onNotified(const ETransformMessage &message) override;

        //! Update rect & size
        void updateRect(bool only = false);
        void updateSize();

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        Vec2 getPivotInCanvasSpace();
        Vec2 getAnchorCenterInCanvasSpace();

        void onSceneObjectSelected(SceneObject& sceneObject);
        
        void updateLocalToRect();
        void updateAnchorOffset();
    protected:
        //! Anchor
        Vec4 m_anchor;

        //! Anchored Position
        Vec2 m_anchoredPosition;

        // Offset
        Vec4 m_offset;

        //! Anchor Offset 
        //! L,R,T,B base on anchor vs parent
        Vec4 m_anchorOffset;

        //! Pivot: center of the content
        Vec2 m_pivot;

        //! Size of Rect 
        Vec2 m_size = Vec2(0,0);

        //! Cached rect in canvas space (no scale, no rotate)
        Vec4 m_rect;
        bool m_rectDirty = true;
        
        //! Update when Parent change 
        bool m_parentDirty = false;

        //! Flag update rect from localPosition
        bool m_bLocalToRectDirty = true;

        //! Cached transform to viewport space
        Mat4 m_viewportTransform;
        bool m_viewportTransformDirty = true;

        //! Cached transform to canvas space
        Mat4 m_canvasTransform;
        bool m_canvasTransformDirty = true;

        uint64_t m_SelectedListenerID = 0;
    };
} // namespace ige::scene
