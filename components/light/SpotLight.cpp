#include "components/light/SpotLight.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{
    //! Constructor
    SpotLight::SpotLight(SceneObject &owner)
        : LightComponent(owner)
    {
        m_index = getOwner()->getScene()->acquireSpotLight();

        // Set default color
        setColor({1.f, 1.f, 1.f});

        // Set default intensity
        setIntensity(1.f);
    }

    //! Destructor
    SpotLight::~SpotLight()
    {
        getOwner()->getScene()->releaseSpotLight(m_index);
    }

    //! Range
    float SpotLight::getRange() const
    {
        return getOwner()->getScene()->getEnvironment()->GetSpotLampRange(m_index);
    }

    void SpotLight::setRange(float range)
    {
        getOwner()->getScene()->getEnvironment()->SetSpotLampRange(m_index, range);
    }

    //! Intensity
    float SpotLight::getIntensity() const
    {
        return getOwner()->getScene()->getEnvironment()->GetSpotLampIntensity(m_index);
    }

    void SpotLight::setIntensity(float intensity)
    {
        getOwner()->getScene()->getEnvironment()->SetSpotLampIntensity(m_index, intensity);
    }

    //! Color
    Vec3 SpotLight::getColor() const
    {
        return getOwner()->getScene()->getEnvironment()->GetSpotLampColor(m_index);
    }

    void SpotLight::setColor(const Vec3 &col)
    {
        getOwner()->getScene()->getEnvironment()->SetSpotLampColor(m_index, col);
    }

    //! Direction
    Vec3 SpotLight::getPosition() const
    {
        return getOwner()->getScene()->getEnvironment()->GetSpotLampPosition(m_index);
    }

    //! Direction
    Vec3 SpotLight::getDirection() const
    {
        return getOwner()->getScene()->getEnvironment()->GetSpotLampDirection(m_index);
    }

    //! Angle
    float SpotLight::getAngle() const
    {
        return getOwner()->getScene()->getEnvironment()->GetSpotLampAngle(m_index);
    }

    void SpotLight::setAngle(float angle)
    {
        getOwner()->getScene()->getEnvironment()->SetSpotLampAngle(m_index, angle);
    }

    //! Update position from transform component
    void SpotLight::onUpdate(float dt)
    {
        auto transform = getOwner()->getTransform();
        getOwner()->getScene()->getEnvironment()->SetSpotLampPosition(m_index, transform->getWorldPosition());
        getOwner()->getScene()->getEnvironment()->SetSpotLampDirection(m_index, transform->getWorldForward().Negative());
    }

    //! Serialize
    void SpotLight::to_json(json &j) const
    {
        Component::to_json(j);
        j["its"] = getIntensity();
        j["col"] = getColor();
        j["rng"] = getRange();
        j["angle"] = getAngle();
    }

    //! Deserialize
    void SpotLight::from_json(const json &j)
    {
        setIntensity(j.value("its", 1.f));
        setColor(j.value("col", Vec3(0.5f, 0.5f, 0.5f)));
        setRange(j.value("rng", 100.0f));
        setAngle(j.value("angle", 30.f));
        Component::from_json(j);
    }

} // namespace ige::scene