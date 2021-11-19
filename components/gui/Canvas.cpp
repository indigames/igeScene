
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
        m_camera = ResourceCreator::Instance().NewCamera((owner.getUUID() + "canvas_camera").c_str(), nullptr);
        m_camera->LockonTarget(false);
        m_camera->SetOrthographicProjection(true);
        m_camera->SetWidthBase(false);

        // This is raycat target
        getOwner()->setIsRaycastTarget(true);
        updateCanvas();
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
        if (m_canvasSize != canvasSize) {
            m_canvasSize = canvasSize;
            updateCanvas();
        }
    }

    void Canvas::setTargetCanvasSize(const Vec2 &canvasSize)
    {
        if (m_targetCanvasSize != canvasSize)
        {   
            m_targetCanvasSize = canvasSize;
            updateCanvas();
        }
    }

    void Canvas::setMatchWidthOrHeight(float value)
    {
        float _value = MATH_CLAMP(0, 1, value);
        if (_value != m_MatchWidthOrHeight) {
            m_MatchWidthOrHeight = _value;
            updateCanvas();
        }
    }

    void Canvas::updateCanvas() {
        
        if (m_canvasSize[0] < 0 || m_canvasSize[1] < 0 || (m_canvasSize[0] == 0 && m_canvasSize[1] == 0)) return;
        m_deviceScale.X(m_targetCanvasSize.X() / m_canvasSize.X());
        m_deviceScale.Y(m_targetCanvasSize.Y() / m_canvasSize.Y());
        switch (m_ScreenMatchMode) {
        case ScreenMatchMode::MatchWidthOrHeight:
        {
            float logWidth = std::log2(m_deviceScale[0]);
            float logHeight = std::log2(m_deviceScale[1]);
            float logWeightedAverage = MATH_LERP(logWidth, logHeight, m_MatchWidthOrHeight);
            m_scaleFactor = std::pow(2, logWeightedAverage);
        }
            break;
        case ScreenMatchMode::Expand:
            m_scaleFactor = std::min(m_deviceScale[0], m_deviceScale[1]);
            break;
        case ScreenMatchMode::Shrink:
            m_scaleFactor = std::max(m_deviceScale[0], m_deviceScale[1]);
            break;
        }
        if (m_scaleFactor <= 0) m_scaleFactor = 0.001f;
        auto transform = getOwner()->getRectTransform();
        if (transform) {
            transform->lockMove(false);
            transform->lockRotate(false);
            transform->lockScale(false);

            m_canvasTrueSize = m_canvasSize * m_scaleFactor;
            transform->setSize(m_canvasSize);

            Vec2 resize = Vec2(m_scaleFactor * m_canvasSize[0], m_scaleFactor * m_canvasSize[1]);
            Vec3 worldPosition = Vec3(m_targetCanvasSize[0] * 0.5f, m_targetCanvasSize[1] * 0.5f, 0);
            transform->setLocalPosition(worldPosition);

            Vec3 worldScale = Vec3(m_scaleFactor, m_scaleFactor, m_scaleFactor);
            transform->setLocalScale(worldScale);

            m_camera->SetPosition({ worldPosition.X(), worldPosition.Y(), worldPosition.Z() + 100.0f });
            m_camera->SetAspectRate(m_targetCanvasSize.X() / m_targetCanvasSize.Y());
            m_camera->SetOrthoWidth(m_targetCanvasSize.X() * 0.5f);
            m_camera->SetOrthoHeight(m_targetCanvasSize.Y() * 0.5f);

            transform->setSize(m_targetCanvasSize / m_scaleFactor);

            transform->lockMove(true);
            transform->lockRotate(true);
            transform->lockScale(true);
        }
    }

    void Canvas::setScreenMatchMode(ScreenMatchMode mode)
    {
        if (mode != m_ScreenMatchMode) {
            m_ScreenMatchMode = mode;
            updateCanvas();
        }
    }

    void Canvas::setScreenMatchMode(int mode)
    {
        int _mode = MATH_CLAMP(0, 2, mode);
        setScreenMatchMode((ScreenMatchMode)mode);
    }

    //! Serialize
    void Canvas::to_json(json &j) const
    {
        Component::to_json(j);
        j["size"] = m_canvasSize;
        j["targetSize"] =  m_targetCanvasSize;
        j["screenmatchmode"] = m_ScreenMatchMode;
        j["matchwithorheight"] = m_MatchWidthOrHeight;
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
        m_canvasSize = m_json.value("size", Vec2(560.f, 940.f));
        m_ScreenMatchMode = (ScreenMatchMode)m_json.value("screenmatchmode", (int)ScreenMatchMode::Expand);
        m_MatchWidthOrHeight = m_json.value("matchwithorheight", 0.5f);

    #if EDITOR_MODE
        m_targetCanvasSize = m_json.value("targetSize", Vec2(560.f, 940.f));
    #else
        m_targetCanvasSize = Vec2(SystemInfo::Instance().GetGameW(), SystemInfo::Instance().GetGameH());
    #endif

        updateCanvas();

        Component::onSerializeFinished(scene);
        m_json.clear();
    }

} // namespace ige::scene
