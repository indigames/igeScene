
#include "components/Canvas.h"
#include "components/RectTransform.h"

#include "scene/SceneObject.h"
#include "scene/SceneManager.h"

namespace ige::scene 
{
    //! Constructor
    Canvas::Canvas(const std::shared_ptr<SceneObject>& owner):
        Component(owner)
    {}
    
    //! Destructor
    Canvas::~Canvas() {}

    //! Canvas to viewport matrix
    void Canvas::setCanvasToViewportMatrix(const Mat4& matrix)
    {
        if (m_canvasToViewportMatrix != matrix)
        {
            m_canvasToViewportMatrix = matrix;
            m_viewportToCanvasMatrix = m_canvasToViewportMatrix.Inverse();
            
            // Recompute viewport transform
            getOwner()->getComponent<RectTransform>()->setRecomputeFlag(RectTransform::E_Recompute::ViewportTransformOnly);
        }
    }

    const Mat4& Canvas::getCanvasToViewportMatrix() const
    {
        return m_canvasToViewportMatrix;
    }

    //! Viewport to canvas matrix
    const Mat4& Canvas::getViewportToCanvasMatrix() const
    {
        return m_viewportToCanvasMatrix;
    }       

    void Canvas::setDesignCanvasSize(const Vec2& canvasSize)
    {
        m_canvasSize = canvasSize;
    }

    void Canvas::setTargetCanvasSize(const Vec2& canvasSize)
    {
        auto oldTargetCanvasSize = m_targetCanvasSize;
        auto oldDeviceScale = m_deviceScale;

        if (SceneManager::getInstance()->isEditor())
        {
            // Editor use predefined canvas size
            m_targetCanvasSize = m_canvasSize;            
        }
        else
        {
            // Runtime use target canvas size with device scale
            m_targetCanvasSize = canvasSize;
            m_deviceScale.X(m_targetCanvasSize.X() / m_canvasSize.X());
            m_deviceScale.Y(m_targetCanvasSize.Y() / m_canvasSize.Y());
        }
        
        if (oldTargetCanvasSize != m_targetCanvasSize || oldDeviceScale != m_deviceScale)
        {
            RectTransform::E_Recompute recompute;
            if (oldTargetCanvasSize != m_targetCanvasSize)
            {
                if(oldDeviceScale != m_deviceScale)
                {
                    recompute = RectTransform::E_Recompute::RectAndTransform;
                }
                else
                {
                    recompute = RectTransform::E_Recompute::RectOnly;
                }                
            }
            else
            {
                recompute = RectTransform::E_Recompute::TransformOnly;
            }

            // Recompute transform
            getOwner()->getComponent<RectTransform>()->setRecomputeFlag(recompute);
        }
    }
}
