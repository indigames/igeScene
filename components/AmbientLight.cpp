#include "components/AmbientLight.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{
    //! Constructor
    AmbientLight::AmbientLight(SceneObject &owner)
        : Component(owner)
    {
    }

    //! Destructor
    AmbientLight::~AmbientLight()
    {
    }

    //! Sky Color
    Vec3 AmbientLight::getSkyColor() const
    {
        return getOwner()->getScene()->getEnvironment()->GetAmbientSkyColor();
    }

    void AmbientLight::setSkyColor(const Vec3 &col)
    {
        getOwner()->getScene()->getEnvironment()->SetAmbientSkyColor(col);
    }

    //! Ground Color
    Vec3 AmbientLight::getGroundColor() const
    {
        return getOwner()->getScene()->getEnvironment()->GetAmbientGroundColor();
    }

    void AmbientLight::setGroundColor(const Vec3 &col)
    {
        getOwner()->getScene()->getEnvironment()->SetAmbientGroundColor(col);
    }

    //! Direction
    Vec3 AmbientLight::getDirection() const
    {
        return getOwner()->getScene()->getEnvironment()->GetAmbientDirection();
    }

    void AmbientLight::setDirection(const Vec3 &direction)
    {
        getOwner()->getScene()->getEnvironment()->SetAmbientDirection(direction);
    }

    //! Serialize
    void AmbientLight::to_json(json &j) const
    {
        j = json{
            {"sky", getSkyColor()},
            {"gnd", getGroundColor()},
            {"dir", getDirection()},
        };
    }

    //! Deserialize
    void AmbientLight::from_json(const json &j)
    {
        setSkyColor(j.value("sky", Vec3()));
        setGroundColor(j.value("gnd", Vec3()));
        setDirection(j.value("dir", Vec3()));
    }
} // namespace ige::scene