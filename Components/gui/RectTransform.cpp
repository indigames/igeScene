
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
        auto size = getSize();
        float x = rect.m_left + size.X() * m_pivot.X();
        float y = rect.m_top + size.Y() * m_pivot.Y();
        return Vec2(x, y);
    }

    Vec2 RectTransform::getAnchorCenterInCanvasSpace()
    {
        auto parent = getOwner()->getParent();
        if (!parent) return Vec2(0.0f, 0.0f);

        auto parentTransform = parent->getComponent<RectTransform>();
        if (!parentTransform) return Vec2(0.0f, 0.0f);

        auto parentRect = parentTransform->getRect();
        auto parentSize = parentTransform->getSize();

        Rect anchorRect;
        anchorRect.m_left = parentRect.m_left + m_anchor.m_left * parentSize.X();
        anchorRect.m_right = parentRect.m_left + m_anchor.m_right * parentSize.X();
        anchorRect.m_top = parentRect.m_top + m_anchor.m_top * parentSize.Y();
        anchorRect.m_bottom = parentRect.m_top + m_anchor.m_bottom * parentSize.Y();

        return anchorRect.getCenter();
    }

    const Mat4& RectTransform::getLocalTransform()
    {
        if (m_bLocalDirty)
        {
            m_localMatrix.Identity();

            //if (hasScaleOrRotation())
            {
                auto pivot = getPivotInCanvasSpace();

                Mat4 transformToPivotSpace;
                auto pivotVec4 = Vec4(pivot.X(), pivot.Y(), 0.f, 0.f);
                vmath_mat4_translation((pivotVec4 * (-1.f)).P(), transformToPivotSpace.P());

                Mat4 translate;
                vmath_mat4_translation(m_localPosition.P(), translate.P());

                Mat4 scaleMat;
                auto scaleVec4 = Vec4(m_localScale.X(), m_localScale.Y(), m_localScale.Z(), 0.f);
                vmath_mat_scale(scaleVec4.P(), 4, scaleMat.P());

                Mat4 rotMat;
                vmath_mat_from_quat(m_localRotation.P(), 4, rotMat.P());

                Mat4 transformFromPivotSpace;
                vmath_mat4_translation(pivotVec4.P(), transformFromPivotSpace.P());

                m_localMatrix = transformFromPivotSpace * rotMat * scaleMat * translate * transformToPivotSpace;
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
                auto parentTransform = parent->getComponent<RectTransform>();
                if (parentTransform)
                {
                    m_canvasTransform = parentTransform->getCanvasSpaceTransform();

                   // if (hasScaleOrRotation())
                    {
                        auto transformToParent = getLocalTransform();
                        m_canvasTransform = m_canvasTransform * transformToParent;
                    }
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
                m_worldMatrix = m_viewportTransform = canvasToViewportMatrix * getCanvasSpaceTransform();

                // Update world position
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

                if (m_worldScale.X())
                {
                    columns[0] /= m_worldScale.X();
                }

                if (m_worldScale.Y())
                {
                    columns[1] /= m_worldScale.Y();
                }

                if (m_worldScale.Z())
                {
                    columns[2] /= m_worldScale.Z();
                }

                // Update world rotation
                Mat3 rotationMatrix(columns[0], columns[1], columns[2]);
                m_worldRotation = Quat(rotationMatrix);
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
        if (flag == E_Recompute::RectOnly /*&& hasScaleOrRotation()*/)
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
            auto parentRectTransform = parent->getComponent<RectTransform>();
            if (parentRectTransform)
            {
                auto parentSize = parentRectTransform->getSize();
                m_offset.m_left -= parentSize.X() * (m_anchor.m_left - lastAnchor.m_left);
                m_offset.m_right -= parentSize.X() * (m_anchor.m_right - lastAnchor.m_right);
                m_offset.m_top -= parentSize.Y() * (m_anchor.m_top - lastAnchor.m_top);
                m_offset.m_bottom -= parentSize.Y() * (m_anchor.m_bottom - lastAnchor.m_bottom);
            }
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
        //if (hasScaleOrRotation())
        {
            auto rect = getRect();
            auto localTransform = getLocalTransform();

            // Get the rect points
            auto leftTop = Vec4(rect.m_left, rect.m_top, 0.0f, 0.f);
            auto rightTop = Vec4(rect.m_right, rect.m_top, 0.0f, 0.f);
            auto leftBottom = Vec4(rect.m_left, rect.m_bottom, 0.0f, 0.f);
            auto rightBottom = Vec4(rect.m_right, rect.m_bottom, 0.0f, 0.f);

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
            auto oldSize = rect.getSize();
            float newLeft = transPivot.X() - oldSize.X() * pivot.X();
            float newTop = transPivot.Y() - oldSize.Y() * pivot.Y();
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
        /*else
        {
            // No scale or rotation, just update pivot
            m_pivot = pivot;
            setRecomputeFlag(E_Recompute::TransformOnly);
        }    */    
    }

    void RectTransform::setOffset(const Offset& offset) 
    {
        auto parent = getOwner()->getParent();
        if (!parent) return;

        auto parentRectTransform = parent->getComponent<RectTransform>();
        if (!parentRectTransform) return;
        
        auto lastOffset = m_offset;
        auto newOffset = offset;

        auto parentRect = parentRectTransform->getRect();
        auto parentSize = parentRectTransform->getSize();

        float left = parentRect.m_left + parentSize.X() * m_anchor.m_left + offset.m_left;
        float right = parentRect.m_left + parentSize.X() * m_anchor.m_right + offset.m_right;
        float top = parentRect.m_top + parentSize.Y() * m_anchor.m_top + offset.m_top;
        float bottom = parentRect.m_top + parentSize.Y() * m_anchor.m_bottom + offset.m_bottom;

        if (left > right)
        {
            // left - right offsets are flipped
            bool leftChanged = newOffset.m_left != lastOffset.m_left;
            bool rightChanged = newOffset.m_right != lastOffset.m_right;

            if (leftChanged && rightChanged)
            {               
                float newValue = left * (1.0f - m_pivot.X()) + right * m_pivot.X();
                newOffset.m_left = newValue - (parentRect.m_left + parentSize.X() * m_anchor.m_left);
                newOffset.m_right = newValue - (parentRect.m_left + parentSize.X() * m_anchor.m_right);
            }
            else if (rightChanged)
            {
                newOffset.m_right = left - (parentRect.m_left + parentSize.X() * m_anchor.m_right);
            }
            else if (leftChanged)
            {
                newOffset.m_left = right - (parentRect.m_left + parentSize.X() * m_anchor.m_left);
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
                newOffset.m_top = newValue - (parentRect.m_top + parentSize.Y() * m_anchor.m_top);
                newOffset.m_bottom = newValue - (parentRect.m_top + parentSize.Y() * m_anchor.m_bottom);
            }
            else if (bottomChanged)
            {
                newOffset.m_bottom = top - (parentRect.m_top + parentSize.Y() * m_anchor.m_bottom);
            }
            else if (topChanged)
            {
                newOffset.m_top = bottom - (parentRect.m_top + parentSize.Y() * m_anchor.m_top);
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

        if (m_anchor.m_left == m_anchor.m_right)
        {            
            auto curWidth = m_offset.m_right - m_offset.m_left;
            auto diff = size.X() - curWidth;
            offset.m_left -= diff * m_pivot.X();
            offset.m_right += diff * (1.0f - m_pivot.X());            
        }

        if (m_anchor.m_top == m_anchor.m_bottom)
        {            
            auto curHeight = m_offset.m_bottom - m_offset.m_top;
            auto diff = size.Y() - curHeight;
            offset.m_top -= diff * m_pivot.Y();
            offset.m_bottom += diff * (1.0f - m_pivot.Y());            
        }

        if(m_offset != offset)
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
                auto parentRectTransform = parent->getComponent<RectTransform>();
                if (parentRectTransform)
                {
                    auto parentRect = parentRectTransform->getRect();
                    auto parentSize = parentRectTransform->getSize();
                    rect.m_left = parentRect.m_left + parentSize.X() * m_anchor.m_left + m_offset.m_left;
                    rect.m_right = parentRect.m_left + parentSize.X() * m_anchor.m_right + m_offset.m_right;
                    rect.m_top = parentRect.m_top + parentSize.Y() * m_anchor.m_top + m_offset.m_top;
                    rect.m_bottom = parentRect.m_top + parentSize.Y() * m_anchor.m_bottom + m_offset.m_bottom;
                }
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

            auto posVec2 = getPivotInCanvasSpace() - getAnchorCenterInCanvasSpace();
            m_localPosition = Vec3(posVec2.X(), posVec2.Y(), m_posZ);
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
        return hasScale() || hasRotation();
    }    
}
