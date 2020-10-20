#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"

namespace ige::scene
{
    //! FigureComponent
    class EnvironmentComponent : public Component
    {
    public:
        //! Constructor
        EnvironmentComponent(SceneObject &owner, const std::string &name);

        //! Destructor
        virtual ~EnvironmentComponent();

        //! Get component name
        virtual std::string getName() const override { return "EnvironmentComponent"; }

        //! Ambient Sky Color
        Vec3 getAmbientSkyColor() const { return m_environment->GetAmbientSkyColor(); }
        void setAmbientSkyColor(const Vec3 &col) const { m_environment->SetAmbientSkyColor(col); }

        //! Ambient Ground Color
        Vec3 getAmbientGroundColor() const { return m_environment->GetAmbientGroundColor(); }
        void setAmbientGroundColor(const Vec3 &col) { m_environment->SetAmbientGroundColor(col); }

        //! Ambient direction
        Vec3 getAmbientDirection() const { return m_environment->GetAmbientDirection(); }
        void setAmbientDirection(const Vec3 &dir) { m_environment->SetAmbientDirection(dir); }

        //! Directional light intensity
        float getDirectionalLightIntensity(uint32_t index) const { return m_environment->GetDirectionalLampIntensity(index); }
        void setDirectionalLightIntensity(uint32_t index, float intensity) { m_environment->SetDirectionalLampIntensity(index, intensity); }

        //! Directional light color
        Vec3 getDirectionalLightColor(uint32_t index) const { return m_environment->GetDirectionalLampColor(index); }
        void setDirectionalLightColor(uint32_t index, const Vec3 &col) { m_environment->SetDirectionalLampColor(index, col); }

        //! Directional light direction
        Vec3 getDirectionalLightDirection(uint32_t index) const { return m_environment->GetDirectionalLampDirection(index); }
        void setDirectionalLightDirection(uint32_t index, const Vec3 &dir) { m_environment->SetDirectionalLampDirection(index, dir); }

        //! Point light range
        float getPointLightRange(uint32_t index) const { return m_environment->GetPointLampRange(index); }
        void setPointLightRange(uint32_t index, float range) { m_environment->SetPointLampRange(index, range); }

        //! Point light intensity
        float getPointLightIntensity(uint32_t index) const { return m_environment->GetPointLampIntensity(index); }
        void setPointLightIntensity(uint32_t index, float intensity) { m_environment->SetPointLampIntensity(index, intensity); }

        //! Point light color
        Vec3 getPointLightColor(uint32_t index) const { return m_environment->GetPointLampColor(index); }
        void setPointLightColor(uint32_t index, const Vec3 &col) { m_environment->SetPointLampColor(index, col); }

        //! Point light position
        Vec3 getPointLightPosition(uint32_t index) const { return m_environment->GetPointLampPosition(index); }
        void setPointLightPosition(uint32_t index, const Vec3 &pos) { m_environment->SetPointLampPosition(index, pos); }

        //! Fog near distance
        float getDistanceFogNear() const { return m_environment->GetDistanceFogNear(); }
        void setDistanceFogNear(float f_near) { m_environment->SetDistanceFogNear(f_near); }

        //! Fog far distance
        float getDistanceFogFar() const { return m_environment->GetDistanceFogFar(); }
        void setDistanceFogFar(float f_far) { m_environment->SetDistanceFogFar(f_far); }

        //! Fog alpha distance
        float getDistanceFogAlpha() const { return m_environment->GetDistanceFogAlpha(); }
        void setDistanceFogAlpha(float alpha) { m_environment->SetDistanceFogAlpha(alpha); }

        //! Fog color distance
        Vec3 getDistanceFogColor() const { return m_environment->GetDistanceFogColor(); }
        void setDistanceFogColor(const Vec3 &col) { m_environment->SetDistanceFogColor(col); }

        //! Shadow color
        Vec3 getShadowColor() const { return m_environment->GetShadowColor(); }
        void setShadowColor(const Vec3 &col) { m_environment->SetShadowColor(col); }

        //! Shadow density
        float getShadowDensity() const { return m_environment->GetShadowDensity(); }
        void setShadowDensity(float density) { m_environment->SetShadowDensity(density); }

        //! Shadow wideness
        float getShadowWideness() const { return m_environment->GetShadowWideness(); }
        void setShadowWideness(float wideness) { m_environment->SetShadowWideness(wideness); }

        //! Get internal environment
        Environment *getEnvironment() { return m_environment; }

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Environment instance
        Environment *m_environment;

        //! Environment name
        std::string m_name;
    };
} // namespace ige::scene
