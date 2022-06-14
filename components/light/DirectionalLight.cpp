#include "components/light/DirectionalLight.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{
    //! Constructor
    DirectionalLight::DirectionalLight(SceneObject& owner)
        : Component(owner)
    {
        m_index = getOwner()->getScene()->acquireDirectionalLight();

        // Set default color
        setColor({1.f, 1.f, 1.f});

        // Set default intensity
        setIntensity(0.75f);
    }

    //! Destructor
    DirectionalLight::~DirectionalLight()
    {
        if(getOwner()->getScene())
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
        getOwner()->getScene()->getEnvironment()->SetDirectionalLampDirection(m_index, transform->getWorldForward().Negative());
    }

    //! Serialize
    void DirectionalLight::to_json(json &j) const
    {
        Component::to_json(j);
        j["its"] = getIntensity();
        j["col"] = getColor();
    }

    //! Deserialize
    void DirectionalLight::from_json(const json &j)
    {
        setIntensity(j.value("its", 1.f));
        setColor(j.value("col", Vec3()));
        Component::from_json(j);
    }

    //! Update property by key value
    void DirectionalLight::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("its") == 0)
        {
            setIntensity(val);
        }
        else if (key.compare("col") == 0)
        {
            setColor(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }
} // namespace ige::scene