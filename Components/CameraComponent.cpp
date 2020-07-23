#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    CameraComponent::CameraComponent(const std::shared_ptr<SceneObject>& owner, const std::string& name)
        : Component(owner), m_name(name), m_shootTarget(nullptr)
    {
        m_camera = ResourceCreator::Instance().NewCamera(name.c_str(), nullptr);
    }

    //! Destructor
    CameraComponent::~CameraComponent() 
    {
        if (m_camera)
        {
            m_camera->DecReference();
            m_camera = nullptr;
        }
        ResourceManager::Instance().DeleteDaemon();
    }
    
    //! Pan (Y-axis)
    void CameraComponent::setPan(float pan)
    {
        if (m_camera == nullptr) return;
        m_camera->SetPan(pan);
    }

    //! Tilt (X-axis)
    void CameraComponent::setTilt(float tilt)
    {
        if (m_camera == nullptr) return;
        m_camera->SetTilt(tilt);
    }

    //! Roll (Z-axis)
    void CameraComponent::setRoll(float roll)
    {
        if (m_camera == nullptr) return;
        m_camera->SetRoll(roll);
    }

    // Set lock target vector
    void CameraComponent::setTarget(const Vec3& tar)
    {
        if (m_camera == nullptr) return;
        m_camera->SetTarget(tar);
    }

    // Set lock target
    void CameraComponent::lockOnTarget(bool lockOn) 
    {
        if (m_camera == nullptr) return;
        m_camera->LockonTarget(lockOn);
    }

    //! Update
    void CameraComponent::onUpdate(float dt)
    {
        if (m_camera == nullptr) return;
        auto transCmp = getOwner()->getTransform();


        m_camera->SetPosition(transCmp->getWorldPosition());
        m_camera->SetRotation(transCmp->getWorldRotation());
        m_camera->SetScale(transCmp->getWorldScale());

        // Update
        m_camera->Step(dt);

        transCmp->setWorldPosition(m_camera->GetPosition());
        transCmp->setWorldRotation(m_camera->GetRotation());
        transCmp->setWorldScale(m_camera->GetScale());
    }

    //! Render
    void CameraComponent::onRender()
    {
        if (m_camera == nullptr) return;

        m_camera->Render();
    }

    //! Serialize
    void CameraComponent::to_json(json& j) const
    {
        j = json {
            {"name", m_name},
            {"pos", getPosition()},
            {"rot", getRotation()},
            {"aspect", getAspectRatio()},
            {"wBase", isWidthBase()},
            {"fov", getFieldOfView()},
            {"near", getNearPlane()},
            {"far", getFarPlane()},
            {"tilt", getTilt()},
            {"pan", getPan()},
            {"roll", getRoll()},
            {"lock", getLockOn()},
            {"target", getTarget()},
            {"ortho", isOrthoProjection()},
            {"orthoW", getOrthoWidth()},
            {"orthoH", getOrthoHeight()},
            {"scrScale", getScreenScale()},
            {"scrOff", getScreenOffset()},
            {"scrRad", getScreenRadian()},
            {"up", getUpAxis()},
        };
    }

    //! Deserialize
    void CameraComponent::from_json(const json& j)
    {
        setPosition(j.at("pos"));
        setRotation(j.at("rot"));
        setAspectRatio(j.at("aspect"));
        setWidthBase(j.at("wBase"));
        setFieldOfView(j.at("fov"));
        setNearPlane(j.at("near"));
        setFarPlane(j.at("far"));
        setTilt(j.at("tilt"));
        setPan(j.at("pan"));
        setRoll(j.at("roll"));
        lockOnTarget(j.at("lock"));
        setTarget(j.at("target"));
        setOrthoProjection(j.at("ortho"));
        setOrthoWidth(j.at("orthoW"));
        setOrthoHeight(j.at("orthoH"));
        setScreenScale(j.at("scrScale"));
        setScreenOffset(j.at("scrOff"));
        setScreenRadian(j.at("scrRad"));
        setUpAxis(j.at("up"));
    }
}
