#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "Component.h"

namespace ige::scene
{
    //! FigureComponent
    class EnvironmentComponent : public Component
    {
    public:
        //! Constructor
        EnvironmentComponent(std::shared_ptr<SceneObject> owner, const std::string& name, Figure* parentFigure = nullptr);

        //! Destructor
        virtual ~EnvironmentComponent();

        //! Get component name
        virtual std::string getName() const override { return "EnvironmentComponent"; }

        //! Ambient Sky Color
        const Vec3 getAmbientSkyColor() { return m_environment->GetAmbientSkyColor(); }
        void setAmbientSkyColor(const Vec3& col) { m_environment->SetAmbientSkyColor(col); }

        //! Ambient Ground Color
        const Vec3 getAmbientGroundColor() { return m_environment->GetAmbientGroundColor(); }
        void setAmbientGroundColor(const Vec3& col) { m_environment->SetAmbientGroundColor(col); }

        //! Ambient direction
        Vec3 getAmbientDirection() { return m_environment->GetAmbientDirection(); }
        void setAmbientDirection(const Vec3& dir) { m_environment->SetAmbientDirection(dir); }

        //! Directional light intensity
        float getDirectionalLightIntensity(uint32_t index) { return m_environment->GetDirectionalLampIntensity(index); }
        void setDirectionalLightIntensity(uint32_t index, float intensity) { m_environment->SetDirectionalLampIntensity(index, intensity); }

        //! Directional light color
        const Vec3 getDirectionalLightColor(uint32_t index) { return m_environment->GetDirectionalLampColor(index); }
        void setDirectionalLightColor(uint32_t index, const Vec3& col) { m_environment->SetDirectionalLampColor(index, col); }

        //! Directional light direction
        Vec3 getDirectionalLightDirection(uint32_t index) { return m_environment->GetDirectionalLampDirection(index); }
        void setDirectionalLightDirection(uint32_t index, const Vec3& dir) { m_environment->SetDirectionalLampDirection(index, dir); }

        //! Point light range
        float getPointLightRange(uint32_t index) { return m_environment->GetPointLampRange(index); }
        void setPointLightRange(uint32_t index, float range) { m_environment->SetPointLampRange(index, range); }

        //! Point light intensity
        float getPointLightIntensity(uint32_t index) { return m_environment->GetPointLampIntensity(index); }
        void setPointLightIntensity(uint32_t index, float intensity) { m_environment->SetPointLampIntensity(index, intensity); }

        //! Point light color
        const Vec3 getPointLightColor(uint32_t index) { return m_environment->GetPointLampColor(index); }
        void setPointLightColor(uint32_t index, const Vec3& col) { m_environment->SetPointLampColor(index, col); }

        //! Point light position
        Vec3 getPointLightPosition(uint32_t index) { return m_environment->GetPointLampPosition(index); }
        void setPointLightPosition(uint32_t index, const Vec3& pos) { m_environment->SetPointLampPosition(index, pos); }

        //! Fog near distance
        float getDistanceFogNear() { return m_environment->GetDistanceFogNear(); }
        void setDistanceFogNear(float f_near) { m_environment->SetDistanceFogNear(f_near); }

        //! Fog far distance
        float getDistanceFogFar() { return m_environment->GetDistanceFogFar(); }
        void setDistanceFogFar(float f_far) { m_environment->SetDistanceFogFar(f_far); }

        //! Fog alpha distance
        float getDistanceFogAlpha() { return m_environment->GetDistanceFogAlpha(); }
        void setDistanceFogAlpha(float alpha) { m_environment->SetDistanceFogAlpha(alpha); }

        //! Fog color distance
        const Vec3 getDistanceFogColor() { return m_environment->GetDistanceFogColor(); }
        void setDistanceFogColor(const Vec3& col) { m_environment->SetDistanceFogColor(col); }

        //! Shadow color
        const Vec3 getShadowColor() { return m_environment->GetShadowColor(); }
        void setShadowColor(const Vec3& col) { m_environment->SetShadowColor(col); }

        //! Shadow density
        float getShadowDensity() { return m_environment->GetShadowDensity(); }
        void setShadowDensity(float density) { m_environment->SetShadowDensity(density); }

        //! Shadow wideness
        float getShadowWideness() { return m_environment->GetShadowWideness(); }
        void setShadowWideness(float wideness) { m_environment->SetShadowWideness(wideness); }

        //! Update
        virtual void onUpdate(float dt);

        //! Serialize
        void to_json(json& j, const Component& obj);

        //! Deserialize 
        void from_json(const json& j, Component& obj);

    protected:
        std::shared_ptr<Environment> m_environment;
    };
}
