
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"

#include "scene/SceneObject.h"

namespace ige::scene
{
    RectTransform::RectTransform(const std::shared_ptr<SceneObject>& owner, const Vec3& pos, const Vec2& size)
        : TransformComponent(owner)
    {
        m_posZ = 0.f;
        setAnchor(Anchor(0.5f, 0.5f, 0.5f, 0.5f));
        setPivot(Vec2(0.5f, 0.5f));
        setOffset(Offset(-50.f, -50.f, 50.f, 50.f));
        setSize(size);
        setRecomputeFlag(E_Recompute::RectAndTransform);
    }

    RectTransform::~RectTransform()
    {
    }

    Vec2 RectTransform::getPivotInCanvasSpace()
    {
        auto rect = getRect();
        float x = rect.m_left + rect.getWidth() * m_pivot.X();
        float y = rect.m_top + rect.getHeight() * m_pivot.Y();
        return Vec2(x, y);
    }

    Vec2 RectTransform::getAnchorCenterInCanvasSpace()
    {
        auto parent = getOwner()->getParent();
        if (!parent) return Vec2(0.0f, 0.0f);

        auto parentTransform = std::dynamic_pointer_cast<RectTransform>(parent->getTransform());
        auto parentRect = parentTransform->getRect();

        Rect anchorRect;
        anchorRect.m_left = parentRect.m_left + m_anchor.m_left * parentRect.getWidth();
        anchorRect.m_right = parentRect.m_left + m_anchor.m_right * parentRect.getWidth();
        anchorRect.m_top = parentRect.m_top + m_anchor.m_top * parentRect.getHeight();
        anchorRect.m_bottom = parentRect.m_top + m_anchor.m_bottom * parentRect.getHeight();

        return anchorRect.getCenter();
    }

    const Mat4& RectTransform::getLocalTransform()
    {
        if (m_bLocalDirty)
        {
            m_localMatrix.Identity();

            if (hasScaleOrRotation())
            {
                auto pivot = getPivotInCanvasSpace();
                auto transformToPivotSpace = Mat4::Translate(Vec3(-pivot.X(), -pivot.Y(), 0.f));
                auto scaleMat = Mat4::Scale(m_localScale);
                auto rotMat = Mat4::Rotation(m_localRotation);
                auto transformFromPivotSpace = Mat4::Translate(Vec3(pivot.X(), pivot.Y(), 0.f));
                m_localMatrix = transformFromPivotSpace * rotMat * scaleMat * transformToPivotSpace;
            }
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
                if (hasScaleOrRotation())
                {
                    auto transformToParent = getLocalTransform();
                    m_worldMatrix = m_canvasTransform = m_canvasTransform * transformToParent;

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
                }
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
                auto canvasToViewportMatrix = canvas->getCanvasToViewportMatrix();
                m_viewportTransform = canvasToViewportMatrix * getCanvasSpaceTransform();
            }
            m_viewportTransformDirty = false;
        }
        return m_viewportTransform;
    }

    void RectTransform::setRotation(const Quat& rot)
    {
        if (m_localRotation != rot)
        {
            m_localRotation = rot;
            m_worldRotation = m_localRotation;
            setRecomputeFlag(E_Recompute::TransformOnly);
        }
    }

    void RectTransform::setScale(const Vec3& scale)
    {
        if (m_localScale != scale)
        {
            m_localScale = scale;
            setRecomputeFlag(E_Recompute::TransformOnly);
        }
    }

    const Vec3& RectTransform::getPosition() const
    {
        return m_localPosition;
    }

    const Mat4& RectTransform::getLocalMatrix() const
    {
        return m_localMatrix;
    }

    const Mat4& RectTransform::getWorldMatrix() const
    {
        return m_viewportTransform;
    }

    //! OnUpdate
    void RectTransform::onUpdate(float dt)
    {
        getRect();
        getLocalTransform();
        getCanvasSpaceTransform();
        getViewportTransform();
    }

    void RectTransform::setPosition(const Vec3& pos)
    {
        auto currPos = getPosition();
        if (currPos != pos)
        {
            auto offsetVec3 = (pos - currPos);
            m_offset += Vec2(offsetVec3.X(), offsetVec3.Y());
            m_posZ = pos.Z();
            setRecomputeFlag(E_Recompute::RectOnly);
        }
    }

    void RectTransform::setRecomputeFlag(E_Recompute flag)
    {
        if (flag == E_Recompute::RectOnly && hasScaleOrRotation())
        {
            // If has scale or rotation, need recalculate transform
            flag = E_Recompute::RectAndTransform;
        }

        // Recursive update flag of all child object
        for (auto& child : getOwner()->getChildren())
        {
            if (child)
            {
                auto childTransform = child->getComponent<RectTransform>();
                if (childTransform)
                {
                    childTransform->setRecomputeFlag(flag);
                }
            }
        }

        // Now update flags
        switch (flag)
        {
        case E_Recompute::RectOnly:
            m_rectDirty = true;
            break;
        case E_Recompute::TransformOnly:
            m_bLocalDirty = true;
            m_viewportTransformDirty = true;
            m_canvasTransformDirty = true;
            break;
        case E_Recompute::RectAndTransform:
            m_rectDirty = true;
            m_bLocalDirty = true;
            m_viewportTransformDirty = true;
            m_canvasTransformDirty = true;
            break;
        }
    }

    void RectTransform::setAnchor(const Anchor& anchor)
    {
        auto lastAnchor = m_anchor;
        auto lastOffset = m_offset;
        m_anchor = anchor;

        // Correct anchor left & right
        if (m_anchor.m_right < m_anchor.m_left)
        {
            if (m_anchor.m_right != lastAnchor.m_right)
                m_anchor.m_right = m_anchor.m_left;
            else
                m_anchor.m_left = m_anchor.m_right;
        }

        // Correct anchor top & bottom
        if (m_anchor.m_bottom < m_anchor.m_top)
        {
            if (m_anchor.m_bottom != lastAnchor.m_bottom)
                m_anchor.m_bottom = m_anchor.m_top;
            else
                m_anchor.m_top = m_anchor.m_bottom;
        }

        // Update offsets
        auto parent = getOwner()->getParent();
        if (parent)
        {
            auto parentRectTransform = std::dynamic_pointer_cast<RectTransform>(parent->getTransform());
            auto parentRect = parentRectTransform->getRect();
            m_offset.m_left -= parentRect.getWidth() * (m_anchor.m_left - lastAnchor.m_left);
            m_offset.m_right -= parentRect.getWidth() * (m_anchor.m_right - lastAnchor.m_right);
            m_offset.m_top -= parentRect.getHeight() * (m_anchor.m_top - lastAnchor.m_top);
            m_offset.m_bottom -= parentRect.getHeight() * (m_anchor.m_bottom - lastAnchor.m_bottom);
        }

        // Avoid negative width and height
        if (m_anchor.m_left == m_anchor.m_right && m_offset.m_left > m_offset.m_right)
            m_offset.m_left = m_offset.m_right = (m_offset.m_left + m_offset.m_right) * 0.5f;

        if (m_anchor.m_top == m_anchor.m_bottom && m_offset.m_top > m_offset.m_bottom)
            m_offset.m_top = m_offset.m_bottom = (m_offset.m_top + m_offset.m_bottom) * 0.5f;

        // Recompute
        if (lastAnchor != m_anchor || lastOffset != m_offset)
            setRecomputeFlag(E_Recompute::RectOnly);
    }

    void RectTransform::setPivot(const Vec2& pivot)
    {
        if (m_pivot == pivot)
            return;

        // If has scale or rotation, need calculate offset in transformed space
        if (hasScaleOrRotation())
        {
            auto rect = getRect();
            auto localTransform = getLocalTransform();

            // Get the rect points
            auto leftTop = Vec4(rect.m_left, rect.m_top, 0.0f, 1.0f);
            auto rightTop = Vec4(rect.m_right, rect.m_top, 0.0f, 1.0f);
            auto leftBottom = Vec4(rect.m_left, rect.m_bottom, 0.0f, 1.0f);
            auto rightBottom = Vec4(rect.m_right, rect.m_bottom, 0.0f, 1.0f);

            // Apply transformation
            leftTop = localTransform * leftTop;
            rightTop = localTransform * rightTop;
            leftBottom = localTransform * leftBottom;
            rightBottom = localTransform * rightBottom;

            // Set new pivot
            m_pivot = pivot;
            setRecomputeFlag(E_Recompute::TransformOnly);

            // Calculate pivot in transformed space
            auto rightVec4 = rightTop - leftTop;
            auto downVec4 = leftBottom - leftTop;
            auto leftTopVec = Vec2(leftTop.X(), leftTop.Y());
            auto rightVec = Vec2(rightVec4.X(), rightVec4.Y());
            auto downVec = Vec2(downVec4.X(), downVec4.Y());
            auto transPivot = leftTopVec + (rightVec * pivot.X()) + (downVec * pivot.Y());

            // Adjust the size based on transformed pivot
            float newLeft = transPivot.X() - rect.getWidth() * pivot.X();
            float newTop = transPivot.Y() - rect.getHeight() * pivot.Y();
            float deltaX = newLeft - rect.m_left;
            float deltaY = newTop - rect.m_top;

            // Adjust the offset values
            m_offset.m_left += deltaX;
            m_offset.m_right += deltaX;
            m_offset.m_top += deltaY;
            m_offset.m_bottom += deltaY;

            // Recalculate rect
            setRecomputeFlag(E_Recompute::RectOnly);
        }
        else
        {
            // No scale or rotation, just update pivot
            m_pivot = pivot;
            setRecomputeFlag(E_Recompute::TransformOnly);
        }
    }

    void RectTransform::setOffset(const Offset& offset)
    {
        auto parent = getOwner()->getParent();
        if (!parent) return;

        auto parentRectTransform = std::dynamic_pointer_cast<RectTransform>(parent->getTransform());
        if (!parentRectTransform) return;

        auto lastOffset = m_offset;
        auto newOffset = offset;

        auto parentRect = parentRectTransform->getRect();
        float left = parentRect.m_left + parentRect.getWidth() * m_anchor.m_left + offset.m_left;
        float right = parentRect.m_left + parentRect.getWidth() * m_anchor.m_right + offset.m_right;
        float top = parentRect.m_top + parentRect.getHeight() * m_anchor.m_top + offset.m_top;
        float bottom = parentRect.m_top + parentRect.getHeight() * m_anchor.m_bottom + offset.m_bottom;

        if (left > right)
        {
            // left - right offsets are flipped
            bool leftChanged = newOffset.m_left != lastOffset.m_left;
            bool rightChanged = newOffset.m_right != lastOffset.m_right;

            if (leftChanged && rightChanged)
            {
                float newValue = left * (1.0f - m_pivot.X()) + right * m_pivot.X();
                newOffset.m_left = newValue - (parentRect.m_left + parentRect.getWidth() * m_anchor.m_left);
                newOffset.m_right = newValue - (parentRect.m_left + parentRect.getWidth() * m_anchor.m_right);
            }
            else if (rightChanged)
            {
                newOffset.m_right = left - (parentRect.m_left + parentRect.getWidth() * m_anchor.m_right);
            }
            else if (leftChanged)
            {
                newOffset.m_left = right - (parentRect.m_left + parentRect.getWidth() * m_anchor.m_left);
            }
        }

        if (top > bottom)
        {
            // top - bottom offsets are flipped
            bool topChanged = newOffset.m_top != lastOffset.m_top;
            bool bottomChanged = newOffset.m_bottom != lastOffset.m_bottom;

            if (topChanged && bottomChanged)
            {
                float newValue = top * (1.0f - m_pivot.Y()) + bottom * m_pivot.Y();
                newOffset.m_top = newValue - (parentRect.m_top + parentRect.getHeight() * m_anchor.m_top);
                newOffset.m_bottom = newValue - (parentRect.m_top + parentRect.getHeight() * m_anchor.m_bottom);
            }
            else if (bottomChanged)
            {
                newOffset.m_bottom = top - (parentRect.m_top + parentRect.getHeight() * m_anchor.m_bottom);
            }
            else if (topChanged)
            {
                newOffset.m_top = bottom - (parentRect.m_top + parentRect.getHeight() * m_anchor.m_top);
            }
        }

        if (m_offset != newOffset)
        {
            m_offset = newOffset;
            setRecomputeFlag(E_Recompute::RectOnly);
        }
    }

    Vec2 RectTransform::getSize()
    {
        return getRect().getSize();
    }

    void RectTransform::setSize(const Vec2& size)
    {
        auto offset = getOffset();

        // Adjust width
        if (m_anchor.m_left == m_anchor.m_right)
        {
            auto curWidth = m_offset.m_right - m_offset.m_left;
            auto diff = size.X() - curWidth;
            offset.m_left -= diff * m_pivot.X();
            offset.m_right += diff * (1.0f - m_pivot.X());
        }

        // Adjust height
        if (m_anchor.m_top == m_anchor.m_bottom)
        {
            auto curHeight = m_offset.m_bottom - m_offset.m_top;
            auto diff = size.Y() - curHeight;
            offset.m_top -= diff * m_pivot.Y();
            offset.m_bottom += diff * (1.0f - m_pivot.Y());
        }

        // Set new offset
        setOffset(offset);
    }


    const Rect& RectTransform::getRect()
    {
        if (m_rectDirty)
        {
            Rect rect;
            rect.m_left = rect.m_top = 0.f;

            auto parent = getOwner()->getParent();
            if (parent)
            {
                auto parentRectTransform = std::dynamic_pointer_cast<RectTransform>(parent->getTransform());
                auto parentRect = parentRectTransform->getRect();
                rect.m_left = parentRect.m_left + parentRect.getWidth() * m_anchor.m_left + m_offset.m_left;
                rect.m_right = parentRect.m_left + parentRect.getWidth() * m_anchor.m_right + m_offset.m_right;
                rect.m_top = parentRect.m_top + parentRect.getHeight() * m_anchor.m_top + m_offset.m_top;
                rect.m_bottom = parentRect.m_top + parentRect.getHeight() * m_anchor.m_bottom + m_offset.m_bottom;
            }
            else
            {
                auto canvas = getOwner()->getRoot()->getComponent<Canvas>();
                if (canvas)
                {
                    auto size = canvas->getDesignCanvasSize();
                    rect.m_right = size.X();
                    rect.m_bottom = size.Y();
                }
            }

            // Avoid flipped rect
            if(rect.m_left > rect.m_right)
                rect.m_left = rect.m_right = rect.getCenterX();

            if (rect.m_top > rect.m_bottom)
                rect.m_top = rect.m_bottom = rect.getCenterY();

            m_rect = rect;
            m_rectDirty = false;

            auto pivot = getPivotInCanvasSpace();
            auto posVec2 = pivot - getAnchorCenterInCanvasSpace();
            m_localPosition = Vec3(posVec2.X(), posVec2.Y(), m_posZ);

            auto pivotPosition = Vec4(pivot.X(), pivot.Y(), m_posZ, 1.f);
            if (parent)
            {
                auto parentRectTransform = std::dynamic_pointer_cast<RectTransform>(parent->getTransform());
                auto transform = parentRectTransform->getViewportTransform();
                pivotPosition = transform * pivotPosition;
            }
            m_worldPosition = Vec3(pivotPosition.X(), pivotPosition.Y(), pivotPosition.Z());
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
        auto parentHasScaleOrRotation = false;
        auto parent = getOwner()->getParent();
        if (parent)
        {
            auto parentTransform = std::dynamic_pointer_cast<RectTransform>(parent->getTransform());
            parentHasScaleOrRotation = parentTransform->hasScaleOrRotation();
        }
        return parentHasScaleOrRotation || hasScale() || hasRotation();
    }
}
