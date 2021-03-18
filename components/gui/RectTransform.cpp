
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
        return Vec2((vec[2] + vec[0]) * 0.5f, (vec[3] + vec[1]) * 0.5f);
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
        setSize(size);
        setRectDirty();
        setTransformDirty();
    }

    RectTransform::~RectTransform()
    {
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
            auto canvas = getOwner()->getCanvas();
            if (canvas)
            {
                m_viewportTransform = canvas->getCanvasToViewportMatrix() * getCanvasSpaceTransform();
            }

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

    void RectTransform::worldTranslate(const Vec3 &trans)
    {
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
            auto deltaPos = pos - m_localPosition;
            auto newOffset = m_offset;
            translateRect(newOffset, Vec2(deltaPos[0], deltaPos[1]));
            setOffset(newOffset);

            if (m_localPosition[3] != pos.Z())
            {
                m_localPosition.Z(pos.Z());
                setTransformDirty();
            }

            onUpdate(0.f);
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
    }

    void RectTransform::setRectDirty()
    {
        m_rectDirty = true;

        // Recursive update flag of all child object
        for (auto &child : getOwner()->getChildren())
        {
            if (child)
            {
                auto childTransform = child->getComponent<RectTransform>();
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
        //m_bWorldDirty = true;
        m_viewportTransformDirty = true;
        m_canvasTransformDirty = true;

        // Recursive update flag of all child object
        for (auto &child : getOwner()->getChildren())
        {
            if (child)
            {
                auto childTransform = child->getComponent<RectTransform>();
                if (childTransform)
                {
                    childTransform->setTransformDirty();
                }
            }
        }
    }

    void RectTransform::setAnchor(const Vec4 &anchor)
    {
        auto lastAnchor = m_anchor;
        auto lastOffset = m_offset;
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

        // Update offsets
        auto parent = getOwner()->getParent();
        if (parent)
        {
            auto parentRectTransform = parent->getRectTransform();
            if (parentRectTransform)
            {
                auto parentRect = parentRectTransform->getRect();
                m_offset[0] -= getRectWidth(parentRect) * (m_anchor[0] - lastAnchor[0]);
                m_offset[2] -= getRectWidth(parentRect) * (m_anchor[2] - lastAnchor[2]);
                m_offset[1] -= getRectHeight(parentRect) * (m_anchor[1] - lastAnchor[1]);
                m_offset[3] -= getRectHeight(parentRect) * (m_anchor[3] - lastAnchor[3]);
            }
        }

        // Avoid negative width and height
        if (m_anchor[0] == m_anchor[2] && m_offset[0] > m_offset[2])
            m_offset[0] = m_offset[2] = (m_offset[0] + m_offset[2]) * 0.5f;

        if (m_anchor[1] == m_anchor[3] && m_offset[1] > m_offset[3])
            m_offset[1] = m_offset[3] = (m_offset[1] + m_offset[3]) * 0.5f;

        // Recompute
        if (lastAnchor != m_anchor || lastOffset != m_offset)
            setRectDirty();
    }

    void RectTransform::setPivot(const Vec2 &pivot)
    {
        if (m_pivot == pivot)
            return;

        // Calculate rect by pivot point
        auto rect = getRect();
        float x = rect[0] + getRectWidth(rect) * m_pivot.X();
        float y = rect[1] + getRectHeight(rect) * m_pivot.Y();
        auto curPivot = Vec2(x, y);

        auto transformToPivotSpace = Mat4::Translate(Vec3(-curPivot.X(), -curPivot.Y(), m_localPosition.Z()));
        auto transformFromPivotSpace = Mat4::Translate(Vec3(curPivot.X(), curPivot.Y(), m_localPosition.Z()));
        auto localTransform = transformFromPivotSpace * Mat4::Rotation(m_localRotation) * Mat4::Scale(m_localScale) * transformToPivotSpace;

        // Get the rect points
        auto leftTop = Vec4(rect[0], rect[1], m_localPosition.Z(), 1.f);
        auto rightTop = Vec4(rect[2], rect[1], m_localPosition.Z(), 1.f);
        auto leftBottom = Vec4(rect[0], rect[3], m_localPosition.Z(), 1.f);
        auto rightBottom = Vec4(rect[2], rect[3], m_localPosition.Z(), 1.f);

        // Apply transformation
        leftTop = localTransform * leftTop;
        rightTop = localTransform * rightTop;
        leftBottom = localTransform * leftBottom;
        rightBottom = localTransform * rightBottom;

        // Set new pivot
        m_pivot = pivot;
        setTransformDirty();

        // Calculate pivot in transformed space
        auto rightVec4 = rightTop - leftTop;
        auto downVec4 = leftBottom - leftTop;
        auto leftTopVec = Vec2(leftTop.X(), leftTop.Y());
        auto rightVec = Vec2(rightVec4.X(), rightVec4.Y());
        auto downVec = Vec2(downVec4.X(), downVec4.Y());
        auto transPivot = leftTopVec + (rightVec * pivot.X()) + (downVec * pivot.Y());

        // Adjust the size based on transformed pivot
        auto oldSize = getRectSize(rect);
        float newLeft = transPivot.X() - oldSize.X() * pivot.X();
        float newTop = transPivot.Y() - oldSize.Y() * pivot.Y();

        float deltaX = newLeft - rect[0];
        float deltaY = newTop - rect[1];

        // Adjust the offset values
        m_offset[0] += deltaX;
        m_offset[2] += deltaX;
        m_offset[1] += deltaY;
        m_offset[3] += deltaY;

        // Recalculate rect
        setRectDirty();
    }

    void RectTransform::setOffset(const Vec4 &offset)
    {
        auto parent = getOwner()->getParent();
        if (!parent)
            return;

        auto parentRectTransform = parent->getRectTransform();
        if (!parentRectTransform)
            return;

        auto lastOffset = m_offset;
        auto newOffset = offset;

        auto parentRect = parentRectTransform->getRect();
        float left = parentRect[0] + getRectWidth(parentRect) * m_anchor[0] + offset[0];
        float right = parentRect[0] + getRectWidth(parentRect) * m_anchor[2] + offset[2];
        float top = parentRect[1] + getRectHeight(parentRect) * m_anchor[1] + offset[1];
        float bottom = parentRect[1] + getRectHeight(parentRect) * m_anchor[3] + offset[3];

        if (left > right)
        {
            // left - right offsets are flipped
            bool leftChanged = newOffset[0] != lastOffset[0];
            bool rightChanged = newOffset[2] != lastOffset[2];

            if (leftChanged && rightChanged)
            {
                float newValue = left * (1.0f - m_pivot.X()) + right * m_pivot.X();
                newOffset[0] = newValue - (parentRect[0] + getRectWidth(parentRect) * m_anchor[0]);
                newOffset[2] = newValue - (parentRect[0] + getRectWidth(parentRect) * m_anchor[2]);
            }
            else if (rightChanged)
            {
                newOffset[2] = left - (parentRect[0] + getRectWidth(parentRect) * m_anchor[2]);
            }
            else if (leftChanged)
            {
                newOffset[0] = right - (parentRect[0] + getRectWidth(parentRect) * m_anchor[0]);
            }
        }

        if (top > bottom)
        {
            // top - bottom offsets are flipped
            bool topChanged = newOffset[1] != lastOffset[1];
            bool bottomChanged = newOffset[3] != lastOffset[3];

            if (topChanged && bottomChanged)
            {
                float newValue = top * (1.0f - m_pivot.Y()) + bottom * m_pivot.Y();
                newOffset[1] = newValue - (parentRect[1] + getRectHeight(parentRect) * m_anchor[1]);
                newOffset[3] = newValue - (parentRect[1] + getRectHeight(parentRect) * m_anchor[3]);
            }
            else if (bottomChanged)
            {
                newOffset[3] = top - (parentRect[1] + getRectHeight(parentRect) * m_anchor[3]);
            }
            else if (topChanged)
            {
                newOffset[1] = bottom - (parentRect[1] + getRectHeight(parentRect) * m_anchor[1]);
            }
        }

        if (m_offset != newOffset)
        {
            m_offset = newOffset;
            setRectDirty();
        }
    }

    Vec2 RectTransform::getSize()
    {
        return getRectSize(getRect());
    }

    void RectTransform::setSize(const Vec2 &size)
    {
        auto offset = getOffset();

        // Adjust width
        if (m_anchor[0] == m_anchor[2])
        {
            auto curWidth = m_offset[2] - m_offset[0];
            auto diff = size.X() - curWidth;
            offset[0] -= diff * m_pivot.X();
            offset[2] += diff * (1.0f - m_pivot.X());
        }

        // Adjust height
        if (m_anchor[1] == m_anchor[3])
        {
            auto curHeight = m_offset[3] - m_offset[1];
            auto diff = size.Y() - curHeight;
            offset[1] -= diff * m_pivot.Y();
            offset[3] += diff * (1.0f - m_pivot.Y());
        }

        // Set new offset
        setOffset(offset);
    }

    const Vec4 &RectTransform::getRect()
    {
        if (m_rectDirty)
        {
            Vec4 rect;
            rect[0] = rect[1] = 0.f;

            auto canvas = getOwner()->getCanvas();
            if (canvas)
            {
                auto size = canvas->getDesignCanvasSize();
                rect[2] = size.X();
                rect[3] = size.Y();
            }

            auto parent = getOwner()->getParent();
            if (parent)
            {
                auto parentRectTransform = parent->getRectTransform();
                if (parentRectTransform)
                {
                    auto parentRect = parentRectTransform->getRect();
                    rect[0] = parentRect[0] + getRectWidth(parentRect) * m_anchor[0] + m_offset[0];
                    rect[2] = parentRect[0] + getRectWidth(parentRect) * m_anchor[2] + m_offset[2];
                    rect[1] = parentRect[1] + getRectHeight(parentRect) * m_anchor[1] + m_offset[1];
                    rect[3] = parentRect[1] + getRectHeight(parentRect) * m_anchor[3] + m_offset[3];
                }
            }

            auto center = getRectCenter(rect);

            // Avoid flipped rect
            if (rect[0] > rect[2])
                rect[0] = rect[2] = center[0];

            if (rect[1] > rect[3])
                rect[1] = rect[3] = center[1];

            m_rect = rect;

            auto centerPoint = getRectCenter(m_rect);
            auto posVec2 = centerPoint - getAnchorCenterInCanvasSpace();

            // Adjust center point to fit the edges of parent
            auto anchorCenter = getRectCenter(m_anchor);
            posVec2.X(posVec2.X() - (0.5f - anchorCenter[0]) * getRectWidth(rect));
            posVec2.Y(posVec2.Y() - (0.5f - anchorCenter[1]) * getRectHeight(rect));

            // Update local position
            m_localPosition = Vec3(posVec2.X(), posVec2.Y(), m_localPosition.Z());

            m_rectDirty = false;

            // Recompute world transform
            setTransformDirty();
        }
        return m_rect;
    }

    //! Serialize
    void RectTransform::to_json(json &j) const
    {
        Component::to_json(j);
        j["anchor"] = m_anchor;
        j["offset"] = m_offset;
        j["pivot"] = m_pivot;
        j["size"] = Vec2(m_rect[2] - m_rect[0], m_rect[3] - m_rect[1]);
    }

    //! Deserialize
    void RectTransform::from_json(const json &j)
    {
        m_offset = j.value("offset", Vec4(0.f, 0.f, 0.f, 0.f));
        m_anchor = j.value("anchor", Vec4(0.5f, 0.5f, 0.5f, 0.5f));
        m_pivot = j.value("pivot", Vec2(0.5f, 0.5f));
        setSize(j.value("size", Vec2(128.f, 128.f)));
        Component::from_json(j);

        setRectDirty();
        setTransformDirty();
    }
} // namespace ige::scene
