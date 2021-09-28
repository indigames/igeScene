
#include "components/gui/Canvas.h"
#include "components/gui/RectTransform.h"

#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"

namespace ige::scene
{
    //! Constructor
    Canvas::Canvas(SceneObject &owner)
        : Component(owner)
    {
        // Canvas camera
        m_camera = ResourceCreator::Instance().NewCamera("canvas_camera", nullptr);
        m_camera->SetPosition({ 0.f, 0.f, 10.f });
        m_camera->LockonTarget(false);
        m_camera->SetAspectRate(SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH());
        m_camera->SetOrthographicProjection(true);
        m_camera->SetWidthBase(false);

        // This is raycat target
        getOwner()->setIsRaycastTarget(true);
    }

    //! Destructor
    Canvas::~Canvas() {
        if (m_camera) {
            m_camera->DecReference();
            m_camera = nullptr;
        }
    }
   
    void Canvas::setDesignCanvasSize(const Vec2 &canvasSize)
    {
        m_canvasSize = canvasSize;
        updateCanvas();
    }

    void Canvas::setTargetCanvasSize(const Vec2 &canvasSize)
    {
        if (m_canvasSize[0] == 0.f || m_canvasSize[1] == 0.f)
            m_canvasSize = canvasSize;
        m_targetCanvasSize = SceneManager::getInstance()->isEditor() ? canvasSize : getOwner()->getScene()->getWindowSize();
        m_deviceScale.X(m_targetCanvasSize.X() / m_canvasSize.X());
        m_deviceScale.Y(m_targetCanvasSize.Y() / m_canvasSize.Y());
        updateCanvas();
    }

    void Canvas::updateCanvas() {
        auto transform = getOwner()->getRectTransform();
        m_scaleFactor = std::min(m_deviceScale[0], m_deviceScale[1]);
        Vec2 resize = Vec2(m_scaleFactor * m_canvasSize[0], m_scaleFactor * m_canvasSize[1]);
        Vec3 worldPosition = Vec3(resize[0] * 0.5f, resize[1] * 0.5f, 0);
        transform->setPosition(worldPosition);
        Vec3 worldScale = Vec3(m_scaleFactor, m_scaleFactor, m_scaleFactor);
        transform->setScale(worldScale);
        transform->setSize(m_canvasSize);
        m_canvasTrueSize = m_canvasSize * m_scaleFactor;
        getOwner()->getRectTransform()->setTransformDirty();
        m_camera->SetPosition({ worldPosition.X(), worldPosition.Y(), worldPosition.Z() + 10.0f });
    }

    //! Serialize
    void Canvas::to_json(json &j) const
    {
        Component::to_json(j);
        j["size"] = m_canvasSize;
        j["targetSize"] =  m_targetCanvasSize;
    }

    //! Deserialize
    void Canvas::from_json(const json &j)
    {
        m_json = j;
        Component::from_json(j);
    }

    //! Serialize finished event
    void Canvas::onSerializeFinished(Scene* scene)
    {
        setDesignCanvasSize(m_json.value("size", Vec2(560.f, 940.f)));
        setTargetCanvasSize(m_json.value("targetSize", Vec2(560.f, 940.f)));
        Component::onSerializeFinished(scene);
        m_json.clear();
    }

} // namespace ige::scene
