#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"

namespace ige::scene
{
    //! CameraComponent
    class CameraComponent : public Component
    {
    public:
        //! Constructor
        CameraComponent(const std::shared_ptr<SceneObject>& owner, const std::string& name);

        //! Destructor
        virtual ~CameraComponent();

        //! Get component name
        virtual std::string getName() const override { return "CameraComponent"; }

        //! Position
        void setPosition(const Vec3& pos);
        Vec3 getPosition() const;

        //! Rotation
        void setRotation(const Quat& rot);
        Quat getRotation() const;

        //! Rotation
        void setScale(const Vec3& scale);
        Vec3 getScale() const;

        //! Aspect ratio
        float getAspectRatio() const { return m_camera->GetAspectRate(); }
        void setAspectRatio(float val) { m_camera->SetAspectRate(val); }

        //! Width/Height based
        inline void setWidthBase(bool enable) { m_camera->SetWidthBase(enable); }
        inline bool isWidthBase() const { return m_camera->IsWidthBase(); }

        //! FOV
        float getFieldOfView() const { return m_camera->GetFieldOfView(); }
        void setFieldOfView(float val) { m_camera->SetFieldOfView(val);}

        //! Near plane
        float getNearPlane() const { return m_camera->GetNearPlane(); }
        void setNearPlane(float val) { m_camera->SetNearPlane(val); }

        //! Far plane
        float getFarPlane() const { return m_camera->GetFarPlane(); }
        void setFarPlane(float val) { m_camera->SetFarPlane(val); }

        //! Tilt (X-axis)
        float getTilt() const { return m_camera->GetTilt(); }
        void setTilt(float tilt);

        //! Pan (Y-axis)
        float getPan() const { return m_camera->GetPan(); }
        void setPan(float pan);

        //! Roll (Z-axis)
        float getRoll() const { return m_camera->GetRoll(); }
        void setRoll(float roll);

        //! Targets
        void setTarget(const Vec3& tar);
        Vec3 getTarget() const { return m_camera->GetTarget(); }
        Vec3 getWorldTarget() const { return m_camera->GetWorldTarget(); }

        //! Lock on target
        void lockOnTarget(bool lockOn = true);
        bool getLockOn() const { return m_camera->GetLockon(); }

        //! Ortho Graphic
        void setOrthoProjection(bool ortho) { m_camera->SetOrthographicProjection(ortho);}
        bool isOrthoProjection() const { return m_camera->IsOrthographicProjection(); }

        //! Ortho width
        float getOrthoWidth() const { return m_camera->GetOrthoWidth(); }
        void setOrthoWidth(float val) { m_camera->SetOrthoWidth(val); }

        //! Ortho height
        float getOrthoHeight() const { return m_camera->GetOrthoHeight(); }
        void setOrthoHeight(float val) { m_camera->SetOrthoHeight(val); }

        //! Get projection matrix
        Mat4& getProjectionMatrix(Mat4& out, bool direct3D = false) { return m_camera->GetProjectionMatrix(out, direct3D); }

        //! Get view inverse matrix
        Mat4& getViewInverseMatrix(Mat4& out) { return m_camera->GetViewInverseMatrix(out); }

        //! get screen matrix
        Mat4& getScreenMatrix(Mat4& out) { return m_camera->GetScreenMatrix(out); }

        //! Screen scale
        void setScreenScale(const Vec2& v) { m_camera->SetScreenScale(v);}
        const Vec2&	getScreenScale() const { return m_camera->GetScreenScale(); }

        //! Screen offset
        void setScreenOffset(const Vec2& v) { m_camera->SetScreenOffset(v); }
        const Vec2&	getScreenOffset() const { return m_camera->GetScreenOffset(); }

        //! Screen radian
        void setScreenRadian(float r) { m_camera->SetScreenRadian(r); }
        float getScreenRadian() const { return m_camera->GetScreenRadian(); }

        //! Up-axis
        void setUpAxis(int ax) { m_camera->SetUpAxis(ax); }
        int getUpAxis() const { return m_camera->GetUpAxis(); }

        //! Update
        virtual void onUpdate(float dt) override;

        //! Update
        virtual void onRender() override;

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Get camera
        Camera* getCamera() { return m_camera; }

        //! Get shooting target
        SceneObject* getShootTarget() { return m_shootTarget; }

        //! Set shooting target
        void setShootTarget(SceneObject* target);

        //! Get target id
        uint64_t getTargetId() { return m_targetId; }

    protected:
        //! Internal camera
        Camera* m_camera = nullptr;

        //! Camera name
        std::string m_name;

        //! Target to shoot
        SceneObject* m_shootTarget;
        uint64_t m_targetId = 0xffffffff;
    };
}
