
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"

#include "scene/SceneObject.h"

namespace ige::scene
{
    // get width of rect with left-top-right-bottom
    inline float getRectWidth(const Vec4& vec)
    {
        return vec[2] - vec[0];
    }

    // get height of rect with left-top-right-bottom
    inline float getRectHeight(const Vec4& vec)
    {
        return vec[3] - vec[1];
    }

    // get size of rect with left-top-right-bottom
    inline Vec2 getRectSize(const Vec4& vec)
    {
        return Vec2(vec[2] - vec[0], vec[3] - vec[1]);
    }

    // get center of rect with left-top-right-bottom
    inline Vec2 getRectCenter(const Vec4& vec)
    {
        return Vec2((vec[2] + vec[0]) * 0.5f, (vec[3] + vec[1]) * 0.5f);
    }

    //! Translation
    inline void translateRect(Vec4& vec, const Vec2& offset)
    {
        vec[0] += offset.X();
        vec[2] += offset.X();
        vec[1] += offset.Y();
        vec[3] += offset.Y();
    }

    //! Claim values in range of 0.0f - 1.0f
    void clampRectZeroToOne(Vec4& vec)
    {
        vec[0] = std::clamp(vec[0], 0.0f, 1.0f);
        vec[1] = std::clamp(vec[1], 0.0f, 1.0f);
        vec[2] = std::clamp(vec[2], 0.0f, 1.0f);
        vec[3] = std::clamp(vec[3], 0.0f, 1.0f);
    }

    RectTransform::RectTransform(const std::shared_ptr<SceneObject>& owner, const Vec3& pos, const Vec2& size)
        : TransformComponent(owner)
    {
        m_posZ = 0.f;
        m_offset = Vec4(0.f, 0.f, 0.f, 0.f);
        m_anchor = Vec4(0.5f, 0.5f, 0.5f, 0.5f);
        m_pivot = Vec2(0.5f, 0.5f);
        setSize(size);
        setDirty();
    }

    RectTransform::~RectTransform()
    {
    }

    Vec2 RectTransform::getPivotInCanvasSpace()
    {
        auto rect = getRect();
        float x = rect[0] + getRectWidth(rect) * m_pivot.X();
        float y = rect[1] + getRectHeight(rect) * m_pivot.Y();
        return Vec2(x, y);
    }

    Vec2 RectTransform::getPivotInViewportSpace()
    {
        auto pivot = getPivotInCanvasSpace();
        auto pivotVec4 = Vec4(pivot.X(), pivot.Y(), m_posZ, 1.f);

        auto canvas = getOwner()->getRoot()->getComponent<Canvas>();
        if (canvas)
        {
            const auto& canvasToViewportMatrix = canvas->getCanvasToViewportMatrix();
            pivotVec4 = canvasToViewportMatrix * pivotVec4;
        }
        return Vec2(pivotVec4.X(), pivotVec4.Y());
    }

    Vec2 RectTransform::getAnchorCenterInCanvasSpace()
    {
        auto parent = getOwner()->getParent();
        if (!parent) return Vec2(0.f, 0.f);

        auto parentTransform = std::dynamic_pointer_cast<RectTransform>(parent->getTransform());
        auto parentRect = parentTransform->getRect();

        Vec4 anchorRect;
        anchorRect[0] = parentRect[0] + getRectWidth(parentRect) * m_anchor[0];
        anchorRect[2] = parentRect[0] + getRectWidth(parentRect) * m_anchor[2];
        anchorRect[1] = parentRect[1] + getRectHeight(parentRect) * m_anchor[1];
        anchorRect[3] = parentRect[1] + getRectHeight(parentRect) * m_anchor[3];

        return getRectCenter(anchorRect);
    }

    const Mat4& RectTransform::getLocalTransform()
    {
        if (m_bLocalDirty)
        {
            m_localMatrix.Identity();
            auto pivot = getPivotInViewportSpace();
            auto transformToPivotSpace = Mat4::Translate(Vec3(-pivot.X(), -pivot.Y(), m_posZ));
            auto scaleMat = Mat4::Scale(m_localScale);
            auto rotMat = Mat4::Rotation(m_localRotation);
            auto transMat = Mat4::Translate(m_localPosition);
            auto transformFromPivotSpace = Mat4::Translate(Vec3(pivot.X(), pivot.Y(), m_posZ));
            m_localMatrix = transformFromPivotSpace * rotMat * scaleMat * transMat * transformToPivotSpace;
            m_bLocalDirty = false;
        }
        return m_localMatrix;
    }

    //! Get canvas space transform
    const Mat4& RectTransform::getCanvasSpaceTransform()
    {
        if (m_canvasTransformDirty)
        {
            m_canvasTransform.Identity();
            auto parent = getOwner()->getParent();
            if (parent)
            {
                auto parentTransform = std::dynamic_pointer_cast<RectTransform>(parent->getTransform());
                m_canvasTransform = parentTransform->getCanvasSpaceTransform();
                const auto& transformToParent = getLocalTransform();
                m_canvasTransform = m_canvasTransform * transformToParent;
            }
            else
            {
                m_canvasTransform = getLocalTransform();
            }

            m_canvasTransformDirty = false;
        }
        return m_canvasTransform;
    }

    //! Get viewport transform
    const Mat4& RectTransform::getViewportTransform()
    {
        if (m_viewportTransformDirty)
        {
            m_viewportTransform.Identity();
            auto canvas = getOwner()->getRoot()->getComponent<Canvas>();
            if (canvas)
            {
                const auto& canvasToViewportMatrix = canvas->getCanvasToViewportMatrix();
                m_viewportTransform = canvasToViewportMatrix * getCanvasSpaceTransform();
            }

            // Update world matrix
            m_worldMatrix = m_viewportTransform;

            m_worldPosition.X(m_worldMatrix[3][0]);
            m_worldPosition.Y(m_worldMatrix[3][1]);
            m_worldPosition.Z(m_worldMatrix[3][2]);

            Vec3 columns[3] =
            {
                { m_worldMatrix[0][0], m_worldMatrix[0][1], m_worldMatrix[0][2]},
                { m_worldMatrix[1][0], m_worldMatrix[1][1], m_worldMatrix[1][2]},
                { m_worldMatrix[2][0], m_worldMatrix[2][1], m_worldMatrix[2][2]},
            };

            // Update world scale
            m_worldScale.X(columns[0].Length());
            m_worldScale.Y(columns[1].Length());
            m_worldScale.Z(columns[2].Length());

            if (m_worldScale.X()) columns[0] /= m_worldScale.X();
            if (m_worldScale.Y()) columns[1] /= m_worldScale.Y();
            if (m_worldScale.Z()) columns[2] /= m_worldScale.Z();

            // Update world rotation
            Mat3 rotationMatrix(columns[0], columns[1], columns[2]);
            m_worldRotation = Quat(rotationMatrix);

            // Update aabb
            updateAabb();

            m_viewportTransformDirty = false;
        }
        return m_viewportTransform;
    }

    void RectTransform::setRotation(const Quat& rot)
    {
        if (m_localRotation != rot)
        {
            m_localRotation = rot;
            setDirty();
        }
    }

    void RectTransform::setScale(const Vec3& scale)
    {
        if (m_localScale != scale)
        {
            m_localScale = scale;
            setDirty();
        }
    }

    //! OnUpdate
    void RectTransform::onUpdate(float dt)
    {
        getRect();
        getLocalTransform();
        getCanvasSpaceTransform();
        getViewportTransform();

        // Allow object picking
        TransformComponent::onUpdate(dt);
    }

    void RectTransform::setPosition(const Vec3& pos)
    {
        auto currPos = getPosition();
        if (currPos != pos)
        {
            auto offsetVec3 = (pos - currPos);
            translateRect(m_offset, Vec2(offsetVec3.X(), offsetVec3.Y()));
            m_posZ = pos.Z();
            setDirty();
        }
    }

    void RectTransform::setDirty()
    {
        // Recursive update flag of all child object
        for (auto& child : getOwner()->getChildren())
        {
            if (child)
            {
                auto childTransform = child->getComponent<RectTransform>();
                if (childTransform)
                {
                    childTransform->setDirty();
                }
            }
        }

        m_rectDirty = true;
        m_bLocalDirty = true;
        m_viewportTransformDirty = true;
        m_canvasTransformDirty = true;
    }

    void RectTransform::setAnchor(const Vec4& anchor)
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
            auto parentRectTransform = std::dynamic_pointer_cast<RectTransform>(parent->getTransform());
            auto parentRect = parentRectTransform->getRect();
            m_offset[0] -= getRectWidth(parentRect) * (m_anchor[0] - lastAnchor[0]);
            m_offset[2] -= getRectWidth(parentRect) * (m_anchor[2] - lastAnchor[2]);
            m_offset[1] -= getRectHeight(parentRect) * (m_anchor[1] - lastAnchor[1]);
            m_offset[3] -= getRectHeight(parentRect) * (m_anchor[3] - lastAnchor[3]);
        }

        // Avoid negative width and height
        if (m_anchor[0] == m_anchor[2] && m_offset[0] > m_offset[2])
            m_offset[0] = m_offset[2] = (m_offset[0] + m_offset[2]) * 0.5f;

        if (m_anchor[1] == m_anchor[3] && m_offset[1] > m_offset[3])
            m_offset[1] = m_offset[3] = (m_offset[1] + m_offset[3]) * 0.5f;

        // Recompute
        if (lastAnchor != m_anchor || lastOffset != m_offset)
            setDirty();
    }

    void RectTransform::setPivot(const Vec2& pivot)
    {
        if (m_pivot == pivot)
            return;
        m_pivot = pivot;
        setDirty();
    }

    void RectTransform::setOffset(const Vec4& offset)
    {
        auto parent = getOwner()->getParent();
        if (!parent) return;

        auto parentRectTransform = std::dynamic_pointer_cast<RectTransform>(parent->getTransform());
        if (!parentRectTransform) return;

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
            setDirty();
        }
    }

    Vec2 RectTransform::getSize()
    {
        return getRectSize(getRect());
    }

    void RectTransform::setSize(const Vec2& size)
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


    const Vec4& RectTransform::getRect()
    {
        if (m_rectDirty)
        {
            Vec4 rect;
            rect[0] = rect[1] = 0.f;

            auto parent = getOwner()->getParent();
            if (parent)
            {
                auto parentRectTransform = std::dynamic_pointer_cast<RectTransform>(parent->getTransform());
                auto parentRect = parentRectTransform->getRect();
                rect[0] = parentRect[0] + getRectWidth(parentRect) * m_anchor[0] + m_offset[0];
                rect[2] = parentRect[0] + getRectWidth(parentRect) * m_anchor[2] + m_offset[2];
                rect[1] = parentRect[1] + getRectHeight(parentRect) * m_anchor[1] + m_offset[1];
                rect[3] = parentRect[1] + getRectHeight(parentRect) * m_anchor[3] + m_offset[3];
            }
            else
            {
                auto canvas = getOwner()->getRoot()->getComponent<Canvas>();
                if (canvas)
                {
                    auto size = canvas->getDesignCanvasSize();
                    rect[2] = size.X();
                    rect[3] = size.Y();
                }
            }

            auto center = getRectCenter(rect);
            // Avoid flipped rect
            if(rect[0] > rect[2])
                rect[0] = rect[2] = center[0];

            if (rect[1] > rect[3])
                rect[1] = rect[3] = center[1];

            m_rect = rect;

            auto canvasToViewport = getOwner()->getRoot()->getComponent<Canvas>()->getCanvasToViewportMatrix();
            // Get the rect points
            auto leftTop = Vec4(rect[0], rect[1], 0.0f, 1.0f);
            auto rightTop = Vec4(rect[2], rect[1], 0.0f, 1.0f);
            auto leftBottom = Vec4(rect[0], rect[3], 0.0f, 1.0f);
            auto rightBottom = Vec4(rect[2], rect[3], 0.0f, 1.0f);

            // Apply transformation
            leftTop = canvasToViewport * leftTop;
            rightBottom = canvasToViewport * rightBottom;

            m_rectViewport = Vec4(leftTop[0], leftTop[1], rightBottom[0], rightBottom[1]);
            
            m_rectDirty = false;

            auto centerPoint = getRectCenter(m_rect);
            auto posVec2 = centerPoint - getAnchorCenterInCanvasSpace();

            // Adjust center point to fit the edges of parent
            auto anchorCenter = getRectCenter(m_anchor);
            posVec2.X(posVec2.X() - (0.5f - anchorCenter[0]) * getRectWidth(rect));
            posVec2.Y(posVec2.Y() - (0.5f - anchorCenter[1]) * getRectHeight(rect));

            // Update local position
            m_localPosition = Vec3(posVec2.X(), posVec2.Y(), m_posZ);

            // Recompute world transform
            m_viewportTransformDirty = true;
        }
        return m_rect;
    }

    bool RectTransform::hasScale() const
    {
        return m_localScale.X() != 1.0f || m_localScale.Y() != 1.0f || m_localScale.Z() != 1.0f;
    }

    bool RectTransform::hasRotation() const
    {
        return m_localRotation.X() != 0.0f || m_localRotation.Y() != 0.f || m_localRotation.Z() != 0.f || m_localRotation.W() != 1.f;
    }

    bool RectTransform::hasScaleOrRotation() const
    {
        auto parent = getOwner()->getParent();
        auto parentHasScaleOrRotation = false;
        if (parent)
        {
            auto parentTransform = std::dynamic_pointer_cast<RectTransform>(parent->getTransform());
            parentHasScaleOrRotation = parentTransform->hasScale() || parentTransform->hasRotation();
        }
        return parentHasScaleOrRotation || hasScale() || hasRotation();
    }

    void RectTransform::setWorldPosition(const Vec3& pos)
    {
        auto worldPos = getWorldPosition();
        auto delta = pos - worldPos;

        auto localPos = getPosition() + delta;
        setPosition(Vec3(localPos.X(), localPos.Y(), m_posZ));
    }

    void RectTransform::setWorldRotation(const Quat& rot)
    {
        setRotation(rot);
    }

    void RectTransform::setWorldScale(const Vec3& scale)
    {
        setScale(scale);
    }

    bool RectTransform::isPointInside(const Vec2& point) const
    {
        return (point.X() >= m_rectViewport[0] && point.X() <= m_rectViewport[2] && point.Y() >= m_rectViewport[1] && point.Y() <= m_rectViewport[3]);
    }

    //! Serialize
    void RectTransform::to_json(json& j) const
    {
        auto size = Vec2(m_rect[2] - m_rect[0], m_rect[3] - m_rect[1]);
        j = json{
            {"anchor", m_anchor},
            {"offset", m_offset},
            {"pivot", m_pivot},
            {"posz", m_posZ},
            {"size", size},
        };
    }

    //! Deserialize
    void RectTransform::from_json(const json& j)
    {
        m_posZ = 0.f;
        setOffset(j.value("offset", Vec4(0.f, 0.f, 0.f, 0.f)));
        setAnchor(j.value("anchor", Vec4(0.5f, 0.5f, 0.5f, 0.5f)));
        setPivot(j.value("pivot", Vec2(0.5f, 0.5f)));
        setSize(j.value("size", Vec2(128.f, 128.f)));
        setDirty();
    }
}
