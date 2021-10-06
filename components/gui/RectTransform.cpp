
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"

#include "scene/SceneObject.h"

namespace ige::scene
{
    // get width of rect with left-top-right-bottom
    inline float getRectWidth(const Vec4 &vec)
    {
        return vec[2] - vec[0];
    }

    // get height of rect with left-top-right-bottom
    inline float getRectHeight(const Vec4 &vec)
    {
        return vec[3] - vec[1];
    }

    // get size of rect with left-top-right-bottom
    inline Vec2 getRectSize(const Vec4 &vec)
    {
        return Vec2(vec[2] - vec[0], vec[3] - vec[1]);
    }

    // get center of rect with left-top-right-bottom
    inline Vec2 getRectCenter(const Vec4 &vec)
    {
        return Vec2((vec[2] + vec[0]) * 0.5f, (vec[1] + vec[3]) * 0.5f);
    }

    inline Vec2 getRectOffsetCenter(const Vec4& vec, const Vec2& size, const Vec4& anchor)
    {
        Vec4 rect;
        rect[0] = size[0] * (anchor[0] - 0.5f);
        rect[1] = size[1] * (anchor[1] - 0.5f);
        rect[2] = size[0] * (anchor[2] - 0.5f);
        rect[3] = size[1] * (anchor[3] - 0.5f);
        Vec4 rectOff = rect;
        rectOff[0] += vec[0];
        rectOff[1] += vec[1];
        rectOff[2] -= vec[2];
        rectOff[3] -= vec[3];
        Vec2 centerRect = getRectCenter(rect);
        Vec2 centerOff = getRectCenter(rectOff);
        return centerOff - centerRect;
    }

    //! Translation
    inline void translateRect(Vec4 &vec, const Vec2 &offset)
    {
        vec[0] += offset.X();
        vec[2] += offset.X();
        vec[1] += offset.Y();
        vec[3] += offset.Y();
    }

    //! Claim values in range of 0.0f - 1.0f
    void clampRectZeroToOne(Vec4 &vec)
    {
        vec[0] = std::clamp(vec[0], 0.0f, 1.0f);
        vec[1] = std::clamp(vec[1], 0.0f, 1.0f);
        vec[2] = std::clamp(vec[2], 0.0f, 1.0f);
        vec[3] = std::clamp(vec[3], 0.0f, 1.0f);
    }

    RectTransform::RectTransform(SceneObject &owner, const Vec3 &pos, const Vec2 &size)
        : TransformComponent(owner, pos)
    {
        m_offset = Vec4(0.f, 0.f, 0.f, 0.f);
        m_anchor = Vec4(0.5f, 0.5f, 0.5f, 0.5f);
        m_pivot = Vec2(0.5f, 0.5f);
        m_size = size;
        m_anchoredPosition = Vec2(0, 0);
        setAnchor(m_anchor);
        setRectDirty();
        setTransformDirty();
        m_SelectedListenerID = SceneObject::getSelectedEvent().addListener(std::bind(&RectTransform::onSceneObjectSelected, this, std::placeholders::_1));
    }

    RectTransform::~RectTransform()
    {
        SceneObject::getSelectedEvent().removeListener(m_SelectedListenerID);
        m_SelectedListenerID = 0;
    }

    Vec2 RectTransform::getPivotInCanvasSpace()
    {
        auto rect = m_rect;
        float x = (m_localPosition[0] - getRectWidth(rect) * 0.5f) + getRectWidth(rect) * m_pivot.X();
        float y = (m_localPosition[1] - getRectHeight(rect) * 0.5f) + getRectHeight(rect) * m_pivot.Y();
        return Vec2(x, y);
    }

    Vec2 RectTransform::getAnchorCenterInCanvasSpace()
    {
        auto parent = getOwner()->getParent();
        if (!parent)
            return Vec2(0.f, 0.f);

        auto parentTransform = parent->getRectTransform();
        if (!parentTransform)
            return Vec2(0.f, 0.f);

        auto parentRect = parentTransform->getRect();

        Vec4 anchorRect;
        anchorRect[0] = parentRect[0] + getRectWidth(parentRect) * m_anchor[0];
        anchorRect[2] = parentRect[0] + getRectWidth(parentRect) * m_anchor[2];
        anchorRect[1] = parentRect[1] + getRectHeight(parentRect) * m_anchor[1];
        anchorRect[3] = parentRect[1] + getRectHeight(parentRect) * m_anchor[3];

        return getRectCenter(anchorRect);
    }

    const Mat4 &RectTransform::getLocalTransform()
    {
        if (m_bLocalDirty)
        {
            m_worldMatrix.Identity();
            auto pivot = getPivotInCanvasSpace();
            auto transformToPivotSpace = Mat4::Translate(Vec3(-pivot.X(), -pivot.Y(), m_localPosition.Z()));
            auto transformFromPivotSpace = Mat4::Translate(Vec3(pivot.X(), pivot.Y(), m_localPosition.Z()));
            m_worldMatrix = transformFromPivotSpace * Mat4::Rotation(m_localRotation) * Mat4::Scale(m_localScale) * transformToPivotSpace * Mat4::Translate(m_localPosition);
            m_bLocalDirty = false;
        }
        return m_worldMatrix;
    }

    //! Get canvas space transform
    const Mat4 &RectTransform::getCanvasSpaceTransform()
    {
        if (m_canvasTransformDirty)
        {
            m_canvasTransform = getLocalTransform();
            auto parent = getOwner()->getParent();
            if (parent)
            {
                auto parentTransform = parent->getRectTransform();
                if (parentTransform)
                {
                    m_canvasTransform = parentTransform->getCanvasSpaceTransform() * m_canvasTransform;
                }
            }
            m_canvasTransformDirty = false;
        }
        return m_canvasTransform;
    }

    //! Get viewport transform
    const Mat4 &RectTransform::getViewportTransform()
    {
        if (m_viewportTransformDirty)
        {
            m_viewportTransform.Identity();

            // Update world matrix
            m_worldMatrix = m_viewportTransform;

            m_worldPosition.X(m_worldMatrix[3][0]);
            m_worldPosition.Y(m_worldMatrix[3][1]);
            m_worldPosition.Z(m_worldMatrix[3][2]);

            Vec3 columns[3] = {
                {m_worldMatrix[0][0], m_worldMatrix[0][1], m_worldMatrix[0][2]},
                {m_worldMatrix[1][0], m_worldMatrix[1][1], m_worldMatrix[1][2]},
                {m_worldMatrix[2][0], m_worldMatrix[2][1], m_worldMatrix[2][2]},
            };

            // Update world scale
            m_worldScale.X(columns[0].Length());
            m_worldScale.Y(columns[1].Length());
            m_worldScale.Z(columns[2].Length());

            if (m_worldScale.X())
                columns[0] /= m_worldScale.X();
            if (m_worldScale.Y())
                columns[1] /= m_worldScale.Y();
            if (m_worldScale.Z())
                columns[2] /= m_worldScale.Z();

            // Update world rotation
            Mat3 rotationMatrix(columns[0], columns[1], columns[2]);
            m_worldRotation = Quat(rotationMatrix);

            // Fire transform changed event
            getOwner()->getTransformChangedEvent().invoke(*getOwner());
            m_viewportTransformDirty = false;

            // Notify all children
            notifyObservers(ETransformMessage::TRANSFORM_CHANGED);
        }
        return m_viewportTransform;
    }

    void RectTransform::onNotified(const ETransformMessage &message)
    {
        setTransformDirty();

        // Update new transform
        onUpdate(0.f);
    }

    void RectTransform::setParent(std::shared_ptr<TransformComponent> comp)
    {
        bool flag = false;
        if (comp != nullptr) {
            auto oldParent = getOwner()->getParent();
            if (oldParent)
            {
                auto oldParentRect = oldParent->getRectTransform();
                auto parentRect = comp->getOwner()->getRectTransform();
                if (parentRect && oldParentRect && !m_rectDirty) {
                    flag = true;
                }
            }
        }
        TransformComponent::setParent(comp);
        if(flag) setLocalToRectDirty();
    }

    void RectTransform::worldTranslate(const Vec3 &trans)
    {
        if (trans.Length() <= 0) return;

        // Reformat the delta pos to parent direction
        auto deltaPos4 = Vec4(trans[0], trans[1], trans[2], 1.f);
        auto parent = getOwner()->getParent();
        if (parent)
        {
            auto parentTransform = parent->getRectTransform();
            if (parentTransform)
            {
                auto mat = parentTransform->getWorldMatrix().Inverse();
                mat[3][0] = mat[3][1] = mat[3][2] = 0.f; // Eliminate translation
                deltaPos4 = mat * deltaPos4;
            }
        }
        translate(Vec3(deltaPos4[0], deltaPos4[1], deltaPos4[2]));
    }

    void RectTransform::setPosition(const Vec3 &pos)
    {
        if (m_localPosition != pos)
        {
            m_localPosition = pos;
            setTransformDirty();

            onUpdate(0.f);
            setLocalToRectDirty();
        }
    }

    void RectTransform::setRotation(const Quat &rot)
    {
        if (m_localRotation != rot)
        {
            m_localRotation = rot;
            setTransformDirty();
        }
    }

    void RectTransform::setScale(const Vec3 &scale)
    {
        if (m_localScale != scale)
        {
            m_localScale = scale;
            setTransformDirty();
        }
    }

    //! OnUpdate
    void RectTransform::onUpdate(float dt)
    {
        // Ensure rect updated
        getRect();

        // Ensure transform updated
        getViewportTransform();
        
        // Allow object picking
        TransformComponent::onUpdate(dt);

        updateLocalToRect();
    }

    void RectTransform::setRectDirty()
    {
        m_rectDirty = true;

        // Recursive update flag of all child object
        for (auto& child : getOwner()->getChildren())
        {
            if (!child.expired())
            {
                auto childTransform = child.lock()->getComponent<RectTransform>();
                if (childTransform)
                {
                    childTransform->setRectDirty();
                }
            }
        }
    }

    void RectTransform::setTransformDirty()
    {
        m_bLocalDirty = true;
        m_viewportTransformDirty = true;
        m_canvasTransformDirty = true;
        
        // Recursive update flag of all child object
        for (auto &child : getOwner()->getChildren())
        {
            if (!child.expired())
            {
                auto childTransform = child.lock()->getComponent<RectTransform>();
                if (childTransform)
                {
                    childTransform->setTransformDirty();
                }
            }
        }
        getOwner()->getTransformChangedEvent().invoke(*getOwner());
    }

    void RectTransform::setLocalToRectDirty()
    {
        m_bLocalToRectDirty = true;

        // Recursive update flag of all child object
        for (auto& child : getOwner()->getChildren())
        {
            if (!child.expired())
            {
                auto childTransform = child.lock()->getComponent<RectTransform>();
                if (childTransform)
                {
                    childTransform->setLocalToRectDirty();
                }
            }
        }

    }

    void RectTransform::setAnchor(const Vec4 &anchor)
    {
        auto lastAnchor = m_anchor;
        m_anchor = anchor;
        clampRectZeroToOne(m_anchor);

        // Correct anchor left & right
        if (m_anchor[2] < m_anchor[0])
        {
            if (m_anchor[2] != lastAnchor[2])
                m_anchor[2] = m_anchor[0];
            else
                m_anchor[0] = m_anchor[2];
        }

        // Correct anchor top & bottom
        if (m_anchor[3] < m_anchor[1])
        {
            if (m_anchor[3] != lastAnchor[3])
                m_anchor[3] = m_anchor[1];
            else
                m_anchor[1] = m_anchor[3];
        }
                
        // Recompute
        if (lastAnchor != m_anchor) {
            updateRect();
        }
    }

    void RectTransform::setPivot(const Vec2 &pivot)
    {
        if (m_pivot == pivot)
            return;

        // Set new pivot
        m_pivot = pivot;

        updateRect();
    }

    void RectTransform::setOffset(const Vec4 &offset)
    {
        auto parent = getOwner()->getParent();
        if (!parent)
            return;

        auto parentRectTransform = parent->getRectTransform();
        if (!parentRectTransform)
            return;

        auto newOffset = offset;

        if (m_offset != newOffset) {
            m_offset = newOffset;

            if (parentRectTransform)
            {
                auto parentSize = parentRectTransform->getSize();
                Vec4 rect;
                rect[0] = parentSize[0] * (m_anchor[0] - 0.5f) + m_offset[0];
                rect[1] = parentSize[1] * (m_anchor[1] - 0.5f) + m_offset[1];
                rect[2] = parentSize[0] * (m_anchor[2] - 0.5f) - m_offset[2];
                rect[3] = parentSize[1] * (m_anchor[3] - 0.5f) - m_offset[3];
                auto rectSize = getRectSize(rect);
                if (rectSize != m_size)
                    m_size = rectSize;
                auto centerOffset = getRectOffsetCenter(m_offset, parentSize, m_anchor);
                centerOffset[0] += (m_pivot[0] - 0.5f) * m_size[0];
                centerOffset[1] += (m_pivot[1] - 0.5f) * m_size[1];
                setAnchoredPosition(centerOffset);
            }

            for (auto& child : getOwner()->getChildren())
            {
                if (!child.expired())
                {
                    auto childTransform = child.lock()->getComponent<RectTransform>();
                    if (childTransform)
                    {
                        childTransform->updateSize();
                    }
                }
            }
        }
    }

    Vec2 RectTransform::getSize()
    {
        return m_size;
    }

    void RectTransform::setSize(const Vec2 &size)
    {      
        if (m_size != size) {
            float diffW = size[0] - m_size[0];
            float diffH = size[1] - m_size[1];
            m_localPosition[0] -= (m_pivot[0] - 0.5f) * diffW;
            m_localPosition[1] -= (m_pivot[1] - 0.5f) * diffH;

            m_size = size;

            //! update child Size
            updateRect(true);

            for (auto& child : getOwner()->getChildren())
            {
                if (!child.expired())
                {
                    auto childTransform = child.lock()->getComponent<RectTransform>();
                    if (childTransform)
                    {
                        childTransform->updateSize();
                    }
                }
            }
            
            onUpdate(0.f);
        }
    }

    void RectTransform::updateSize()
    {
        auto parent = getOwner()->getParent();
        if (parent == nullptr) return;
        auto parentRect = parent->getRectTransform();
        if (parentRect == nullptr) return;
        auto parentSize = parentRect->getSize();
        Vec4 rect;
        auto centerParentSize = parentSize * 0.5f;

        Vec4 anchorRect;
        anchorRect[0] = parentSize[0] * m_anchor[0];
        anchorRect[1] = parentSize[1] * m_anchor[1];
        anchorRect[2] = parentSize[0] * m_anchor[2];
        anchorRect[3] = parentSize[1] * m_anchor[3];

        auto anchorCenter = getRectCenter(anchorRect);
        auto diffW = anchorCenter[0] - centerParentSize[0];
        auto diffH = anchorCenter[1] - centerParentSize[1];
        //! Determine anchor
        //! If Same : change offset 
        //! If Diff : change size 
        if (m_anchor[0] == m_anchor[2])
        {
            m_offset[0] = m_anchoredPosition[0] - m_size[0] * m_pivot[0] + (m_anchor[2] - m_anchor[0]) * parentSize[0] * 0.5f;
            m_offset[2] = -m_anchoredPosition[0] + (m_pivot[0] - 1) * m_size[0] + (m_anchor[2] - m_anchor[0]) * parentSize[0] * 0.5f;
        }

        if (m_anchor[1] == m_anchor[3])
        {
            m_offset[1] = m_anchoredPosition[1] - m_size[1] * m_pivot[1] + (m_anchor[3] - m_anchor[1]) * parentSize[1] * 0.5f;
            m_offset[3] = -m_anchoredPosition[1] + (m_pivot[1] - 1) * m_size[1] + (m_anchor[3] - m_anchor[1]) * parentSize[1] * 0.5f;
        }

        rect[0] = parentSize[0] * (m_anchor[0] - 0.5f) + m_offset[0];
        rect[1] = parentSize[1] * (m_anchor[1] - 0.5f) + m_offset[1];
        rect[2] = parentSize[0] * (m_anchor[2] - 0.5f) - m_offset[2];
        rect[3] = parentSize[1] * (m_anchor[3] - 0.5f) - m_offset[3];
        
        auto rectSize = getRectSize(rect);
        if (rectSize != m_size) {
            m_size = rectSize;
        }

        auto centerOffset = getRectOffsetCenter(m_offset, parentSize, m_anchor);
        centerOffset[0] += (m_pivot[0] - 0.5f) * m_size[0];
        centerOffset[1] += (m_pivot[1] - 0.5f) * m_size[1];
        setAnchoredPosition(centerOffset);
        
        m_anchorOffset[0] = parentRect->getWorldPosition()[0] + parentSize[0] * (m_anchor[0] - 0.5f);
        m_anchorOffset[1] = parentRect->getWorldPosition()[1] + parentSize[1] * (m_anchor[1] - 0.5f);
        m_anchorOffset[2] = parentRect->getWorldPosition()[0] + parentSize[0] * (m_anchor[2] - 0.5f);
        m_anchorOffset[3] = parentRect->getWorldPosition()[1] + parentSize[1] * (m_anchor[3] - 0.5f);

        //! Update Rect
        m_rect[0] = -m_size[0] * 0.5f + (m_pivot[0] - 0.5f) * m_size[0];
        m_rect[1] = -m_size[1] * 0.5f + (m_pivot[1] - 0.5f) * m_size[1];
        m_rect[2] = m_size[0];
        m_rect[3] = m_size[1];

        for (auto& child : getOwner()->getChildren())
        {
            if (!child.expired())
            {
                auto childTransform = child.lock()->getComponent<RectTransform>();
                if (childTransform)
                {
                    childTransform->updateSize();
                }
            }
        }
    }

    void RectTransform::updateRect(bool only)
    {
        auto parent = getOwner()->getParent();
        if (parent == nullptr) return;
        auto parentRect = parent->getRectTransform();
        if (parentRect == nullptr) return;
        auto parentSize = parentRect->getSize();

        //! Update AnchorOffset
        m_anchorOffset[0] = parentRect->getWorldPosition()[0] + parentSize[0] * (m_anchor[0] - 0.5f);
        m_anchorOffset[1] = parentRect->getWorldPosition()[1] + parentSize[1] * (m_anchor[1] - 0.5f);
        m_anchorOffset[2] = parentRect->getWorldPosition()[0] + parentSize[0] * (m_anchor[2] - 0.5f);
        m_anchorOffset[3] = parentRect->getWorldPosition()[1] + parentSize[1] * (m_anchor[3] - 0.5f);

        //! Update Offset
        m_offset[0] = m_localPosition[0] - m_size[0] * 0.5f - (m_anchor[0] - 0.5f) * parentSize[0];
        m_offset[1] = m_localPosition[1] - m_size[1] * 0.5f - (m_anchor[1] - 0.5f) * parentSize[1];
        m_offset[2] = -(m_localPosition[0] + m_size[0] * 0.5f) + (m_anchor[2] - 0.5f) * parentSize[0];
        m_offset[3] = -(m_localPosition[1] + m_size[1] * 0.5f) + (m_anchor[3] - 0.5f) * parentSize[1];
        //! Update AnchoredPosition
        auto centerOffset = getRectOffsetCenter(m_offset, parentSize, m_anchor);
        centerOffset[0] += (m_pivot[0] - 0.5f) * m_size[0];
        centerOffset[1] += (m_pivot[1] - 0.5f) * m_size[1];        
        setAnchoredPosition(centerOffset);
        
        //! Update Rect
        m_rect[0] = -m_size[0] * 0.5f + (m_pivot[0] - 0.5f) * m_size[0];
        m_rect[1] = -m_size[1] * 0.5f + (m_pivot[1] - 0.5f) * m_size[1];
        m_rect[2] = m_size[0];
        m_rect[3] = m_size[1];

        if (!only) {
            for (auto& child : getOwner()->getChildren())
            {
                if (!child.expired())
                {
                    auto childTransform = child.lock()->getComponent<RectTransform>();
                    if (childTransform)
                    {
                        childTransform->updateRect();
                    }
                }
            }
        }
    }

    const Vec4 &RectTransform::getRect()
    {
        if (m_rectDirty && !m_bLocalToRectDirty)
        {
            Vec4 rect;
            rect[0] = rect[1] = 0.f;
            bool is_Canvas = false;

            auto parent = getOwner()->getParent();
            std::shared_ptr<RectTransform> parentRectTransform = nullptr;
            Vec2 parentSize;
            
            if (parent)
            {
                parentRectTransform = parent->getRectTransform();
                is_Canvas = parentRectTransform == nullptr;
            }
            else
            {
                rect[2] = m_size[0];
                rect[3] = m_size[1];
            }

            if (parentRectTransform)
            {
                parentSize = parentRectTransform->getSize();
                m_anchorOffset[0] = parentRectTransform->getWorldPosition()[0] + parentSize[0] * (m_anchor[0] - 0.5f);
                m_anchorOffset[1] = parentRectTransform->getWorldPosition()[1] + parentSize[1] * (m_anchor[1] - 0.5f);
                m_anchorOffset[2] = parentRectTransform->getWorldPosition()[0] + parentSize[0] * (m_anchor[2] - 0.5f);
                m_anchorOffset[3] = parentRectTransform->getWorldPosition()[1] + parentSize[1] * (m_anchor[3] - 0.5f);
            }

            Vec2 posVec2;
            if (is_Canvas)
            {
                auto centerPoint = getRectCenter(rect);
                m_rect = rect;
                posVec2 = Vec2(m_localPosition[0], m_localPosition[1]);
            }
            else
            {
                auto centerSize = parentSize * 0.5f;

                Vec4 anchorRect;
                anchorRect[0] = parentSize[0] * m_anchor[0];
                anchorRect[1] = parentSize[1] * m_anchor[1];
                anchorRect[2] = parentSize[0] * m_anchor[2];
                anchorRect[3] = parentSize[1] * m_anchor[3];

                auto anchorCenter = getRectCenter(anchorRect);
                auto diffW = anchorCenter[0] - centerSize[0];
                auto diffH = anchorCenter[1] - centerSize[1];

                posVec2[0] = m_anchoredPosition[0] + diffW - (m_pivot[0] - 0.5f) * m_size[0];
                posVec2[1] = m_anchoredPosition[1] + diffH - (m_pivot[1] - 0.5f) * m_size[1];

                m_rect[0] = -m_size[0] * 0.5f + (m_pivot[0] - 0.5f) * m_size[0];
                m_rect[1] = -m_size[1] * 0.5f + (m_pivot[1] - 0.5f) * m_size[1];
                m_rect[2] = m_size[0];
                m_rect[3] = m_size[1];
            }
            m_localPosition = Vec3(posVec2.X(), posVec2.Y(), m_localPosition.Z());
            m_rectDirty = false;
            setTransformDirty();
        }
        else
        {
            m_rect[0] = -m_size[0] * 0.5f + (m_pivot[0] - 0.5f) * m_size[0];
            m_rect[1] = -m_size[1] * 0.5f + (m_pivot[1] - 0.5f) * m_size[1];
            m_rect[2] = m_size[0];
            m_rect[3] = m_size[1];
        }
        return m_rect;
    }

    void RectTransform::updateLocalToRect()
    {
        //! Prevent loop set
        if (m_rectDirty && m_bLocalToRectDirty) 
            m_bLocalToRectDirty = false;
        if (m_bLocalToRectDirty)
        {
            updateWorldToLocal();

            m_bLocalToRectDirty = false; 
            //! change m_offset, m_anchoredPosition, m_rect
            auto parent = getOwner()->getParent();
            std::shared_ptr<RectTransform> parentRectTransform = nullptr;
            if (parent)
            {
                parentRectTransform = parent->getRectTransform();
            }

            if (parentRectTransform)
            {
                auto parentSize = parentRectTransform->getSize();
                m_offset[0] = m_localPosition[0] - m_size[0] * 0.5f - (m_anchor[0] - 0.5f) * parentSize[0];
                m_offset[1] = m_localPosition[1] - m_size[1] * 0.5f - (m_anchor[1] - 0.5f) * parentSize[1];
                m_offset[2] = -(m_localPosition[0] + m_size[0] * 0.5f) + (m_anchor[2] - 0.5f) * parentSize[0];
                m_offset[3] = -(m_localPosition[1] + m_size[1] * 0.5f) + (m_anchor[3] - 0.5f) * parentSize[1];
                
                //Update anchored Pos
                auto centerOffset = getRectOffsetCenter(m_offset, parentSize, m_anchor);
                centerOffset[0] += (m_pivot[0] - 0.5f) * m_size[0];
                centerOffset[1] += (m_pivot[1] - 0.5f) * m_size[1];
                m_anchoredPosition = centerOffset;

                //Update m_rect
                auto parentRect = parentRectTransform->getRect();
                auto centerParent = getRectCenter(parentRect);
                Vec2 centerParentSize(parentSize[0] / 2, parentSize[1] / 2);
                Vec4 rect;
                rect[0] = (centerParentSize[0] + m_offset[0] + (m_anchor[0] - 0.5f) * parentSize[0]);
                rect[1] = (centerParentSize[1] + m_offset[1] + (m_anchor[1] - 0.5f) * parentSize[1]);
                rect[2] = (centerParentSize[0] + m_offset[2] - (m_anchor[2] - 0.5f) * parentSize[0]);
                rect[3] = (centerParentSize[1] + m_offset[3] - (m_anchor[3] - 0.5f) * parentSize[1]);

                m_rect[0] = -m_size[0] * 0.5f + (m_pivot[0] - 0.5f) * m_size[0];
                m_rect[1] = -m_size[1] * 0.5f + (m_pivot[1] - 0.5f) * m_size[1];
                m_rect[2] = m_size[0];
                m_rect[3] = m_size[1];

                auto center = getRectCenter(rect);
                if (rect[0] > rect[2])
                    rect[0] = rect[2] = center[0];

                if (rect[1] > rect[3])
                    rect[1] = rect[3] = center[1];
                m_rect = rect;
            }
            updateAnchorOffset();
        }
    }
    
    void RectTransform::setAnchoredPosition(const Vec2& value)
    {
        //! Convert AnchoredPosition to Local Position
        auto parent = getOwner()->getParent();
        if (parent == nullptr)
            return;

        auto parentRectTransform = parent->getRectTransform();
        if (parentRectTransform == nullptr)
            return;
        m_anchoredPosition = value;
        
        auto parentSize = parentRectTransform->getSize();
        auto centerSize = parentSize * 0.5f;

        Vec4 anchorRect;
        anchorRect[0] = parentSize[0] * m_anchor[0];
        anchorRect[1] = parentSize[1] * m_anchor[1];
        anchorRect[2] = parentSize[0] * m_anchor[2];
        anchorRect[3] = parentSize[1] * m_anchor[3];

        auto anchorCenter = getRectCenter(anchorRect);
        auto diffW = anchorCenter[0] - centerSize[0];
        auto diffH = anchorCenter[1] - centerSize[1];
        
        m_localPosition[0] = m_anchoredPosition[0] - (m_pivot[0] - 0.5f) * m_size[0] + diffW;
        m_localPosition[1] = m_anchoredPosition[1] - (m_pivot[1] - 0.5f) * m_size[1] + diffH;

        //!Update Offset
        m_offset[0] = m_localPosition[0] - m_size[0] * 0.5f - (m_anchor[0] - 0.5f) * parentSize[0];
        m_offset[1] = m_localPosition[1] - m_size[1] * 0.5f - (m_anchor[1] - 0.5f) * parentSize[1];
        m_offset[2] = -(m_localPosition[0] + m_size[0] * 0.5f) + (m_anchor[2] - 0.5f) * parentSize[0];
        m_offset[3] = -(m_localPosition[1] + m_size[1] * 0.5f) + (m_anchor[3] - 0.5f) * parentSize[1];

        //! Update Rect 
        m_rect[0] = -m_size[0] * 0.5f + (m_pivot[0] - 0.5f) * m_size[0];
        m_rect[1] = -m_size[1] * 0.5f + (m_pivot[1] - 0.5f) * m_size[1];
        m_rect[2] = m_size[0];
        m_rect[3] = m_size[1];

        setTransformDirty();
    }

    void RectTransform::updateAnchorOffset()
    {
        auto parent = getOwner()->getParent();
        if (!parent)
            return;

        auto parentRectTransform = parent->getRectTransform();
        if (!parentRectTransform)
            return;

        if (parentRectTransform)
        {
            auto parentSize = parentRectTransform->getSize();

            m_anchorOffset[0] = parentRectTransform->getWorldPosition()[0] + parentSize[0] * (m_anchor[0] - 0.5f);
            m_anchorOffset[1] = parentRectTransform->getWorldPosition()[1] + parentSize[1] * (m_anchor[1] - 0.5f);
            m_anchorOffset[2] = parentRectTransform->getWorldPosition()[0] + parentSize[0] * (m_anchor[2] - 0.5f);
            m_anchorOffset[3] = parentRectTransform->getWorldPosition()[1] + parentSize[1] * (m_anchor[3] - 0.5f);
        }
    }

    void RectTransform::onSceneObjectSelected(SceneObject& sceneObject)
    {
        auto rectTransform = sceneObject.getComponent<RectTransform>();
        if (rectTransform) {
            rectTransform->setLocalToRectDirty();
        }
    }

    //! Serialize
    void RectTransform::to_json(json &j) const
    {
        TransformComponent::to_json(j);
        j["anchor"] = m_anchor;
        j["offset"] = m_offset;
        j["pivot"] = m_pivot;
        j["size"] = m_size;
        j["anchorposition"] = m_anchoredPosition;
        j["anchoroffset"] = m_anchorOffset;
    }

    //! Deserialize
    void RectTransform::from_json(const json &j)
    {
        m_offset = j.value("offset", Vec4(0.f, 0.f, 0.f, 0.f));
        m_anchor = j.value("anchor", Vec4(0.5f, 0.5f, 0.5f, 0.5f));
        m_pivot = j.value("pivot", Vec2(0.5f, 0.5f));
        m_size = j.value("size", Vec2(128.f, 128.f));
        m_anchoredPosition = j.value("anchorposition", Vec2(0.f, 0.f));
        m_anchorOffset = j.value("anchoroffset", Vec4(0.f, 0.f, 0.f, 0.f));
        m_bLocalToRectDirty = false;
        TransformComponent::from_json(j);
        setRectDirty();
        //setTransformDirty();
    }

    //! Update property by key value
    void RectTransform::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("offset") == 0)
        {
            setOffset(val);
        }
        else if (key.compare("anchor") == 0)
        {
            setAnchor(val);
        }
        else if (key.compare("pivot") == 0)
        {
            setPivot(val);
        }
        else if (key.compare("size") == 0)
        {
            setSize(val);
        }
        else if (key.compare("anchorposition") == 0)
        {
            setAnchoredPosition(val);
        }
        else if (key.compare("anchoroffset") == 0)
        {
            setAnchorOffset(val);
        }
        TransformComponent::setProperty(key, val);
    }
} // namespace ige::scene
