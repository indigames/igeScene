#include "components/PointLight.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{
    //! Constructor
    PointLight::PointLight(SceneObject &owner)
        : Component(owner)
    {
        m_index = getOwner()->getScene()->acquirePointLight();

        // Create actual light
        setColor({ 1.f, 1.f, 1.f });
    }

    //! Destructor
    PointLight::~PointLight()
    {
        getOwner()->getScene()->releasePointLight(m_index);
    }

    //! Range
    float PointLight::getRange() const
    {
        return getOwner()->getScene()->getEnvironment()->GetPointLampRange(m_index);
    }

    void PointLight::setRange(float range)
    {
        getOwner()->getScene()->getEnvironment()->SetPointLampRange(m_index, range);
    }

    //! Intensity
    float PointLight::getIntensity() const
    {
        return getOwner()->getScene()->getEnvironment()->GetPointLampIntensity(m_index);
    }

    void PointLight::setIntensity(float intensity)
    {
        getOwner()->getScene()->getEnvironment()->SetPointLampIntensity(m_index, intensity);
    }

    //! Color
    Vec3 PointLight::getColor() const
    {
        return getOwner()->getScene()->getEnvironment()->GetPointLampColor(m_index);
    }

    void PointLight::setColor(const Vec3 &col)
    {
        getOwner()->getScene()->getEnvironment()->SetPointLampColor(m_index, col);
    }

    //! Direction
    Vec3 PointLight::getPosition() const
    {
        return getOwner()->getScene()->getEnvironment()->GetPointLampPosition(m_index);
    }

    //! Update position from transform component
    void PointLight::onUpdate(float dt)
    {
        auto transform = getOwner()->getTransform();
        getOwner()->getScene()->getEnvironment()->SetPointLampPosition(m_index, transform->getWorldPosition());
    }

    //! Serialize
    void PointLight::to_json(json &j) const
    {
        j = json{
            {"its", getIntensity()},
            {"col", getColor()},
            {"rng", getRange()},
        };
    }

    //! Deserialize
    void PointLight::from_json(const json &j)
    {
        setIntensity(j.value("its", 1.f));
        setColor(j.value("col", Vec3()));
        setRange(j.value("rng", 1.f));
    }

} // namespace ige::scene