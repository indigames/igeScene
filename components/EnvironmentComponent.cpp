#include "components/EnvironmentComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{
    //! Constructor
    EnvironmentComponent::EnvironmentComponent(SceneObject &owner)
        : Component(owner)
    {
    }

    //! Destructor
    EnvironmentComponent::~EnvironmentComponent()
    {
    }

    //! Fog near distance
    float EnvironmentComponent::getDistanceFogNear() const
    {
        return getOwner()->getScene()->getEnvironment()->GetDistanceFogNear();
    }

    void EnvironmentComponent::setDistanceFogNear(float f_near)
    {
        getOwner()->getScene()->getEnvironment()->SetDistanceFogNear(f_near);
    }

    //! Fog far distance
    float EnvironmentComponent::getDistanceFogFar() const
    {
        return getOwner()->getScene()->getEnvironment()->GetDistanceFogFar();
    }

    void EnvironmentComponent::setDistanceFogFar(float f_far)
    {
        getOwner()->getScene()->getEnvironment()->SetDistanceFogFar(f_far);
    }

    //! Fog alpha distance
    float EnvironmentComponent::getDistanceFogAlpha() const
    {
        return getOwner()->getScene()->getEnvironment()->GetDistanceFogAlpha();
    }

    void EnvironmentComponent::setDistanceFogAlpha(float alpha)
    {
        getOwner()->getScene()->getEnvironment()->SetDistanceFogAlpha(alpha);
    }

    //! Fog color distance
    Vec3 EnvironmentComponent::getDistanceFogColor() const
    {
        return getOwner()->getScene()->getEnvironment()->GetDistanceFogColor();
    }

    void EnvironmentComponent::setDistanceFogColor(const Vec3 &col)
    {
        getOwner()->getScene()->getEnvironment()->SetDistanceFogColor(col);
    }

    //! Shadow color
    Vec3 EnvironmentComponent::getShadowColor() const
    {
        return getOwner()->getScene()->getEnvironment()->GetShadowColor();
    }

    void EnvironmentComponent::setShadowColor(const Vec3 &col)
    {
        getOwner()->getScene()->getEnvironment()->SetShadowColor(col);
    }

    //! Shadow density
    float EnvironmentComponent::getShadowDensity() const
    {
        return getOwner()->getScene()->getEnvironment()->GetShadowDensity();
    }

    void EnvironmentComponent::setShadowDensity(float density)
    {
        getOwner()->getScene()->getEnvironment()->SetShadowDensity(density);
    }

    //! Shadow wideness
    float EnvironmentComponent::getShadowWideness() const
    {
        return getOwner()->getScene()->getEnvironment()->GetShadowWideness();
    }

    void EnvironmentComponent::setShadowWideness(float wideness)
    {
        getOwner()->getScene()->getEnvironment()->SetShadowWideness(wideness);
    }

    //! Serialize
    void EnvironmentComponent::to_json(json &j) const
    {
        j = json{
            {"fogNear", getDistanceFogNear()},
            {"fogFar", getDistanceFogFar()},
            {"fogAlpha", getDistanceFogAlpha()},
            {"fogCol", getDistanceFogColor()},
            {"shadowCol", getShadowColor()},
            {"shadowDensity", getShadowDensity()},
            {"shadowWideness", getShadowWideness()},
        };
    }

    //! Deserialize
    void EnvironmentComponent::from_json(const json &j)
    {
        setDistanceFogNear(j.at("fogNear"));
        setDistanceFogFar(j.at("fogFar"));
        setDistanceFogAlpha(j.at("fogAlpha"));
        setDistanceFogColor(j.at("fogCol"));
        setShadowColor(j.at("shadowCol"));
        setShadowDensity(j.at("shadowDensity"));
        setShadowWideness(j.at("shadowWideness"));
    }
} // namespace ige::scene
