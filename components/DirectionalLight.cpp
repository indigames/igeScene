#include "components/DirectionalLight.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{
    //! Constructor
    DirectionalLight::DirectionalLight(SceneObject &owner)
        : Component(owner)
    {
        m_index = getOwner()->getScene()->acquireDirectionalLight();

        // Create actual light
        setColor({ 1.f, 1.f, 1.f });
    }

    //! Destructor
    DirectionalLight::~DirectionalLight()
    {
        getOwner()->getScene()->releaseDirectionalLight(m_index);
    }

    //! Intensity
    float DirectionalLight::getIntensity() const
    {
        return getOwner()->getScene()->getEnvironment()->GetDirectionalLampIntensity(m_index);
    }

    void DirectionalLight::setIntensity(float intensity)
    {
        getOwner()->getScene()->getEnvironment()->SetDirectionalLampIntensity(m_index, intensity);
    }

    //! Color
    Vec3 DirectionalLight::getColor() const
    {
        return getOwner()->getScene()->getEnvironment()->GetDirectionalLampColor(m_index);
    }

    void DirectionalLight::setColor(const Vec3 &col)
    {
        getOwner()->getScene()->getEnvironment()->SetDirectionalLampColor(m_index, col);
    }

    //! Direction
    Vec3 DirectionalLight::getDirection() const
    {
        return getOwner()->getScene()->getEnvironment()->GetDirectionalLampDirection(m_index);
    }

    //! Update direction from transform component
    void DirectionalLight::onUpdate(float dt)
    {
        auto transform = getOwner()->getTransform();
        getOwner()->getScene()->getEnvironment()->SetDirectionalLampDirection(m_index, transform->getWorldForward());
    }

    //! Serialize
    void DirectionalLight::to_json(json &j) const
    {
        j = json{
            {"its", getIntensity()},
            {"col", getColor()},
        };
    }

    //! Deserialize
    void DirectionalLight::from_json(const json &j)
    {
        setIntensity(j.value("its", 1.f));
        setColor(j.value("col", Vec3()));
    }

} // namespace ige::scene