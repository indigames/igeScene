#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"

namespace ige::scene
{
    //! Constructor
    CameraComponent::CameraComponent(SceneObject &owner, const std::string &name)
        : Component(owner), m_name(name)
    {
        static int name_count = 0;
        m_camera = ResourceCreator::Instance().NewCamera((name + std::to_string(name_count++)).c_str(), nullptr);
        owner.setLockedFrameAABB(true);
    }

    //! Destructor
    CameraComponent::~CameraComponent()
    {
        if (m_camera)
            m_camera->DecReference();
        m_camera = nullptr;
    }

    //! Pan (Y-axis)
    void CameraComponent::setPan(float pan)
    {
        if (m_camera == nullptr)
            return;
        m_camera->SetPan(pan);
    }

    //! Tilt (X-axis)
    void CameraComponent::setTilt(float tilt)
    {
        if (m_camera == nullptr)
            return;
        m_camera->SetTilt(tilt);
    }

    //! Roll (Z-axis)
    void CameraComponent::setRoll(float roll)
    {
        if (m_camera == nullptr)
            return;
        m_camera->SetRoll(roll);
    }

    // Set lock target vector
    void CameraComponent::setTarget(const Vec3 &tar)
    {
        if (m_camera == nullptr)
            return;
        m_camera->SetTarget(tar);
    }

    // Set lock target
    void CameraComponent::lockOnTarget(bool lockOn)
    {
        if (m_camera == nullptr)
            return;
        m_camera->LockonTarget(lockOn);
    }

    //! Update
    void CameraComponent::onUpdate(float dt)
    {
        if (m_camera == nullptr)
            return;

        auto transCmp = getOwner()->getTransform();
        if (transCmp)
        {
            m_camera->SetPosition(transCmp->getPosition());
            m_camera->SetRotation(transCmp->getRotation());

            // Update
            m_camera->Step(dt);
        }
    }

    //! Render
    void CameraComponent::onRender()
    {
        if (m_camera == nullptr)
            return;
        m_camera->Render();
    }

    //! Position
    void CameraComponent::setPosition(const Vec3 &pos)
    {
        auto transCmp = getOwner()->getTransform();
        transCmp->setPosition(pos);
    }

    Vec3 CameraComponent::getPosition() const
    {
        auto transCmp = getOwner()->getTransform();
        return transCmp->getPosition();
    }

    //! Rotation
    void CameraComponent::setRotation(const Quat &rot)
    {
        auto transCmp = getOwner()->getTransform();
        transCmp->setRotation(rot);
    }

    //! Scale
    void CameraComponent::setScale(const Vec3 &scale)
    {
        auto transCmp = getOwner()->getTransform();
        transCmp->setScale(scale);
    }

    Vec3 CameraComponent::getScale() const
    {
        auto transCmp = getOwner()->getTransform();
        return transCmp->getScale();
    }

    Quat CameraComponent::getRotation() const
    {
        auto transCmp = getOwner()->getTransform();
        return transCmp->getRotation();
    }

    //! Serialize
    void CameraComponent::to_json(json &j) const
    {
        Component::to_json(j);
        j["name"] = m_name;
        j["wBase"] = isWidthBase();
        j["fov"] = getFieldOfView();
        j["near"] = getNearPlane();
        j["far"] = getFarPlane();
        j["lock"] = getLockOn();
        j["ortho"] = isOrthoProjection();
        j["orthoW"] = getOrthoWidth();
        j["orthoH"] = getOrthoHeight();
        j["scrScale"] = getScreenScale();
        j["scrOff"] = getScreenOffset();
        j["scrRad"] = getScreenRadian();
        j["up"] = getUpAxis();
        j["pos"] = getPosition();
        j["rot"] = getRotation();
        j["scale"] = getScale();
        j["aspect"] = getAspectRatio();
        j["clearColor"] = getClearColor();

        if (getLockOn())
        {
            j["target"] = getTarget();
        }
        else
        {
            j["pan"] = getPan();
            j["tilt"] = getTilt();
            j["roll"] = getRoll();
        }
    }

    //! Deserialize
    void CameraComponent::from_json(const json &j)
    {
        if(j.contains("name")) m_name = j.at("name");
        setWidthBase(j.at("wBase"));
        setFieldOfView(j.at("fov"));
        setNearPlane(j.at("near"));
        setFarPlane(j.at("far"));
        lockOnTarget(j.at("lock"));        
        setOrthoProjection(j.at("ortho"));
        setOrthoWidth(j.at("orthoW"));
        setOrthoHeight(j.at("orthoH"));
        setScreenScale(j.at("scrScale"));
        setScreenOffset(j.at("scrOff"));
        setScreenRadian(j.at("scrRad"));
        setUpAxis(j.at("up"));

    #ifdef EDITOR_MODE
        setAspectRatio(j.value("aspect", -1.f));
    #else
        setAspectRatio(-1.f); // Auto detect based on screen size
    #endif

        setClearColor(j.value("clearColor", Vec4(1.f, 1.f, 1.f, 1.f)));

        if (getLockOn())
        {
            setTarget(j.value("target", Vec3(0.f, 0.f, 0.f)));
        }
        else
        {
            setPan(j.value("pan", 0.f));
            setTilt(j.value("tilt", 0.f));
            setRoll(j.value("roll", 0.f));
        }
        
        Component::from_json(j);

        auto transCmp = getOwner()->getRectTransform();
        if (transCmp)
        {
            setPosition(j.value("pos", Vec3()));
            setRotation(j.value("rot", Quat()));
            setScale(j.value("scale", Vec3(1.f, 1.f, 1.f)));
        }

        if (getOwner()->isActive())
        {
            getOwner()->getScene()->setActiveCamera(std::dynamic_pointer_cast<CameraComponent>(getOwner()->getComponent(getInstanceId())));
        }

        if (getCamera()) getCamera()->SetResourceName(m_name.c_str());
    }

    //! Update property by key value
    void CameraComponent::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("wBase") == 0)
        {
            setWidthBase(val);
        }
        else if (key.compare("name") == 0)
        {
            if (m_name.compare(val) != 0)
            {
                m_name = val;
                if (getCamera()) getCamera()->SetResourceName(m_name.c_str());
            }
        }
        else if (key.compare("fov") == 0)
        {
            setFieldOfView(val);
        }
        else if (key.compare("near") == 0)
        {
            setNearPlane(val);
        }
        else if (key.compare("far") == 0)
        {
            setFarPlane(val);
        }
        else if (key.compare("lock") == 0)
        {
            lockOnTarget(val);
        }
        else if (key.compare("target") == 0)
        {
            setTarget(val);
        }
        else if (key.compare("ortho") == 0)
        {
            setOrthoProjection(val);
        }
        else if (key.compare("orthoW") == 0)
        {
            setOrthoWidth(val);
        }
        else if (key.compare("orthoH") == 0)
        {
            setOrthoHeight(val);
        }
        else if (key.compare("scrScale") == 0)
        {
            setScreenScale(val);
        }
        else if (key.compare("scrOff") == 0)
        {
            setScreenOffset(val);
        }
        else if (key.compare("scrRad") == 0)
        {
            setScreenRadian(val);
        }
        else if (key.compare("up") == 0)
        {
            setUpAxis(val);
        }
        else if (key.compare("pos") == 0)
        {
            setPosition(val);
        }
        else if (key.compare("rot") == 0)
        {
            setRotation(val);
        }
        else if (key.compare("scale") == 0)
        {
            setScale(val);
        }
        else if (key.compare("pan") == 0)
        {
            setPan(val);
        }
        else if (key.compare("tilt") == 0)
        {
            setTilt(val);
        }
        else if (key.compare("roll") == 0)
        {
            setRoll(val);
        }
        else if (key.compare("clearColor") == 0)
        {
            setClearColor(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }
} // namespace ige::scene
