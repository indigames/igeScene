
#include "components/gui/Canvas.h"
#include "components/gui/RectTransform.h"

#include "scene/SceneObject.h"
#include "scene/SceneManager.h"

namespace ige::scene
{
    //! Constructor
    Canvas::Canvas(SceneObject &owner)
        : Component(owner)
    {
    }

    //! Destructor
    Canvas::~Canvas() {}

    //! Canvas to viewport matrix
    void Canvas::setCanvasToViewportMatrix(const Mat4 &matrix)
    {
        if (m_canvasToViewportMatrix != matrix)
        {
            m_canvasToViewportMatrix = matrix;
            m_viewportToCanvasMatrix = m_canvasToViewportMatrix.Inverse();

            // Recompute viewport transform
            getOwner()->getRectTransform()->setTransformDirty();
        }
    }

    const Mat4 &Canvas::getCanvasToViewportMatrix() const
    {
        return m_canvasToViewportMatrix;
    }

    //! Viewport to canvas matrix
    const Mat4 &Canvas::getViewportToCanvasMatrix() const
    {
        return m_viewportToCanvasMatrix;
    }

    void Canvas::setDesignCanvasSize(const Vec2 &canvasSize)
    {
        m_canvasSize = canvasSize;

        // Recompute transform
        auto transform = getOwner()->getRectTransform();
        transform->setTransformDirty();
    }

    void Canvas::setTargetCanvasSize(const Vec2 &canvasSize)
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
            // Recompute transform
            getOwner()->getRectTransform()->setTransformDirty();
        }
    }

    //! Serialize
    void Canvas::to_json(json &j) const
    {
        Component::to_json(j);
        j["size"] = m_canvasSize;
        j["targetSize"] =  m_targetCanvasSize;
        j["scale"] =  m_deviceScale;
        j["viewport"] =  m_canvasToViewportMatrix;
    }

    //! Deserialize
    void Canvas::from_json(const json &j)
    {
        setDesignCanvasSize(j.value("size", Vec2(560.f, 940.f)));
        setTargetCanvasSize(j.value("targetSize", Vec2(560.f, 940.f)));
        m_deviceScale = (j.value("targetSize", Vec2(560.f, 940.f)));
        setCanvasToViewportMatrix(j.value("viewport", Mat4::IdentityMat()));
        Component::from_json(j);
    }
} // namespace ige::scene
