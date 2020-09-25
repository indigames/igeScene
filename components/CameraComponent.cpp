#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"

namespace ige::scene
{
    //! Constructor
    CameraComponent::CameraComponent(const std::shared_ptr<SceneObject>& owner, const std::string& name)
        : Component(owner), m_name(name), m_shootTarget(nullptr)
    {
        static int name_count = 0;
        m_camera = ResourceCreator::Instance().NewCamera((name + std::to_string(name_count++)).c_str(), nullptr);
    }

    //! Destructor
    CameraComponent::~CameraComponent() 
    {
        m_shootTarget = nullptr;
        if (m_camera) m_camera->DecReference();
        m_camera = nullptr;
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
        if (transCmp)
        {
            m_camera->SetPosition(transCmp->getWorldPosition());
            m_camera->SetRotation(transCmp->getWorldRotation());

            // Update
            m_camera->Step(dt);
        }
    }

    //! Render
    void CameraComponent::onRender()
    {
        if (m_camera == nullptr) return;
        m_camera->Render();
    }

    //! Position
    void CameraComponent::setPosition(const Vec3& pos)
    { 
        auto transCmp = getOwner()->getTransform();
        transCmp->setWorldPosition(pos);
    }

    Vec3 CameraComponent::getPosition() const 
    { 
        auto transCmp = getOwner()->getTransform();
        return transCmp->getWorldPosition();
    }

    //! Rotation
    void CameraComponent::setRotation(const Quat& rot)
    { 
        auto transCmp = getOwner()->getTransform();
        transCmp->setWorldRotation(rot);
    }

    //! Scale
    void CameraComponent::setScale(const Vec3& scale)
    {
        auto transCmp = getOwner()->getTransform();
        transCmp->setWorldScale(scale);
    }

    Vec3 CameraComponent::getScale() const
    {
        auto transCmp = getOwner()->getTransform();
        return transCmp->getWorldScale();
    }

    Quat CameraComponent::getRotation() const
    { 
        auto transCmp = getOwner()->getTransform();
        return transCmp->getWorldRotation();
    }

    void CameraComponent::setShootTarget(SceneObject* target) {
        m_shootTarget = target;
        m_targetId = target ? target->getId() : 0xffffffff;
    }

    //! Serialize
    void CameraComponent::to_json(json& j) const
    {
        j = json {
            {"name", m_name},
            {"wBase", isWidthBase()},
            {"fov", getFieldOfView()},
            {"near", getNearPlane()},
            {"far", getFarPlane()},
            {"lock", getLockOn()},
            {"target", getTarget()},
            {"ortho", isOrthoProjection()},
            {"orthoW", getOrthoWidth()},
            {"orthoH", getOrthoHeight()},
            {"scrScale", getScreenScale()},
            {"scrOff", getScreenOffset()},
            {"scrRad", getScreenRadian()},
            {"up", getUpAxis()},
            {"targetId", m_targetId},
            {"pos", getPosition()},
            {"rot", getRotation()},
            {"scale", getScale()},
        };
    }

    //! Deserialize
    void CameraComponent::from_json(const json& j)
    {
        setWidthBase(j.at("wBase"));
        setFieldOfView(j.at("fov"));
        setNearPlane(j.at("near"));
        setFarPlane(j.at("far"));
        lockOnTarget(j.at("lock"));
        setTarget(j.at("target"));
        setOrthoProjection(j.at("ortho"));
        setOrthoWidth(j.at("orthoW"));
        setOrthoHeight(j.at("orthoH"));
        setScreenScale(j.at("scrScale"));
        setScreenOffset(j.at("scrOff"));
        setScreenRadian(j.at("scrRad"));
        setUpAxis(j.at("up"));
        m_targetId = j.at("targetId");

        auto transCmp = getOwner()->getRectTransform();
        if (transCmp)
        {
            setPosition(j.value("pos", Vec3()));
            setRotation(j.value("rot", Quat()));
            setScale(j.value("scale", Vec3(1.f, 1.f, 1.f)));
        }
    }
}
