#include "components/EnvironmentComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{
    //! Constructor
    EnvironmentComponent::EnvironmentComponent(SceneObject& owner)
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

    //! Shadow bias
    float EnvironmentComponent::getShadowBias() const
    {
        return RenderContext::Instance().GetShadowBias();
    }

    void EnvironmentComponent::setShadowBias(float bias)
    {
        RenderContext::Instance().SetShadowBias(bias);
    }

    const Vec2 &EnvironmentComponent::getShadowTextureSize() const
    {
        return getOwner()->getScene()->getShadowTextureSize();
    }

    void EnvironmentComponent::setShadowTextureSize(const Vec2 &size)
    {
        getOwner()->getScene()->setShadowTextureSize(size);
    }

    Environment* EnvironmentComponent::getEnvironment()
    {
        return getOwner()->getScene()->getEnvironment();
    }

    //! Serialize
    void EnvironmentComponent::to_json(json &j) const
    {
        Component::to_json(j);
        j["fogNear"] = getDistanceFogNear();
        j["fogFar"] = getDistanceFogFar();
        j["fogAlpha"] = getDistanceFogAlpha();
        j["fogCol"] = getDistanceFogColor();
        j["shadowCol"] = getShadowColor();
        j["shadowSize"] = getShadowTextureSize();
        j["shadowDensity"] = getShadowDensity();
        j["shadowWideness"] = getShadowWideness();
        j["shadowBias"] = getShadowBias();
    }

    //! Deserialize
    void EnvironmentComponent::from_json(const json &j)
    {
        setDistanceFogNear(j.value("fogNear", 0.1f));
        setDistanceFogFar(j.value("fogFar", 50.f));
        setDistanceFogAlpha(j.value("fogAlpha", 1.f));
        setDistanceFogColor(j.value("fogCol", Vec3(1.f, 1.f, 1.f)));
        setShadowColor(j.value("shadowCol", Vec3(1.f, 1.f, 1.f)));
        setShadowTextureSize(j.value("shadowSize", Vec2(2048, 2048)));
        setShadowDensity(j.value("shadowDensity", 0.5f));
        setShadowWideness(j.value("shadowWideness", 1000.f));
        setShadowBias(j.value("shadowBias", 0.005f));
        Component::from_json(j);
    }
} // namespace ige::scene
