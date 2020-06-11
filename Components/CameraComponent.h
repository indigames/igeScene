#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "Component.h"

namespace ige::scene
{
    //! CameraComponent
    class CameraComponent : public Component
    {
    public:
        //! Constructor
        CameraComponent(std::shared_ptr<SceneObject> owner, const std::string& name);

        //! Destructor
        virtual ~CameraComponent();

        //! Get component name
        std::string getName() const override { return "CameraComponent"; }

        //! Get camera
        Camera* getCamera() const { return m_camera.get(); }

        //! Position
        void setPosition(const Vec3& pos) { m_camera->SetPosition(pos);  }
        const Vec3& getPosition() const { return m_camera->GetPosition(); }

        //! Rotation
        void setRotation(const Quat& rot) { m_camera->SetRotation(rot);  }
        const Quat& getRotation() const { return m_camera->GetRotation(); }

        //! Aspect ratio
        float getAspectRatio() const { return m_camera->GetAspectRate(); }
        void setAspectRatio(float val) { m_camera->SetAspectRate(val); }

        //! Width/Height based
        inline void setWidthBase(bool enable) { m_camera->SetWidthBase(enable); }
        inline bool isWidthBase() { return m_camera->IsWidthBase(); }

        //! FOV
        float getFieldOfView() const { return m_camera->GetFieldOfView(); }
        void setFieldOfView(float val) { m_camera->SetFieldOfView(val);}

        //! Near plane
        float getNearPlane() const { return m_camera->GetNearPlane(); }
        void setNearPlane(float val) { m_camera->SetNearPlane(val); }

        //! Far plane
        float getFarPlane() const { return m_camera->GetFarPlane(); }
        void setFarPlane(float val) { m_camera->SetFarPlane(val); }

        //! Ortho Graphic
        void setOrthoProjection(bool ortho) { m_camera->SetOrthographicProjection(ortho);}
        bool isOrthoProjection() { return m_camera->IsOrthographicProjection(); }

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
		void setScreenScale(Vec2& v) { m_camera->SetScreenScale(v);}
        const Vec2&	getScreenScale() const { return m_camera->GetScreenScale(); }

        //! Screen offset
		void setScreenOffset(Vec2& v) { m_camera->SetScreenOffset(v); }
        const Vec2&	getScreenOffset() const { return m_camera->GetScreenOffset(); }

        //! Screen radian
		void setScreenRadian(float r) { m_camera->SetScreenRadian(r); }
		float getScreenRadian() const { return m_camera->GetScreenRadian(); }

        //! Up-axis
		void setUpAxis(int ax) { m_camera->SetUpAxis(ax); }

        //! Update
        virtual void onUpdate(float dt);

        //! Serialize
        void to_json(json& j, const Component& obj);

        //! Deserialize 
        void from_json(const json& j, Component& obj);

    protected:
        //! Internal camera
        std::shared_ptr<Camera> m_camera;
    };
}
