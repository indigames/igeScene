
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
        setCanvasToViewportMatrix(Mat4::IdentityMat());
        getOwner()->setIsRaycastTarget(true);
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
            getOwner()->getRectTransform()->setRectDirty();
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
        m_canvasTrueSize = canvasSize * m_scaleFactor;
        // Recompute transform
        auto transform = getOwner()->getRectTransform();
        Vec3 worldPosition = Vec3(m_canvasSize[0] / 2, m_canvasSize[1] / 2, 0);
        transform->setPosition(worldPosition);
        transform->setSize(m_canvasSize);
        transform->setTransformDirty();
        //updateCanvas();
    }

    void Canvas::setTargetCanvasSize(const Vec2 &canvasSize)
    {
        auto oldTargetCanvasSize = m_targetCanvasSize;
        auto oldDeviceScale = m_deviceScale;
        bool updated = false;
        if (SceneManager::getInstance()->isEditor())
        {
            // Editor use predefined canvas size
            m_targetCanvasSize = canvasSize;
            m_deviceScale.X(m_targetCanvasSize.X() / m_canvasSize.X());
            m_deviceScale.Y(m_targetCanvasSize.Y() / m_canvasSize.Y());
            updated = true;
        }
        else
        {
            // Runtime use target canvas size with device scale
            m_targetCanvasSize = canvasSize;
            m_deviceScale.X(m_targetCanvasSize.X() / m_canvasSize.X());
            m_deviceScale.Y(m_targetCanvasSize.Y() / m_canvasSize.Y());
        }

        if (oldTargetCanvasSize != m_targetCanvasSize || oldDeviceScale != m_deviceScale || updated)
        {   
            updateCanvas();
        }
    }

    void Canvas::updateCanvas() {
        auto transform = getOwner()->getRectTransform();
        m_scaleFactor = std::max(m_deviceScale[0], m_deviceScale[1]);
        Vec2 resize = Vec2(m_scaleFactor * m_canvasSize[0], m_scaleFactor * m_canvasSize[1]);
        Vec3 worldPosition = Vec3(resize[0] / 2, resize[1] / 2, 0);
        transform->setPosition(worldPosition);
        Vec3 worldScale = Vec3(m_scaleFactor, m_scaleFactor, m_scaleFactor);
        transform->setScale(worldScale);
        transform->setSize(m_canvasSize);
        m_canvasTrueSize = m_canvasSize * m_scaleFactor;
        getOwner()->getRectTransform()->setTransformDirty();
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
