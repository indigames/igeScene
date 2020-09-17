#include "components/EnvironmentComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    EnvironmentComponent::EnvironmentComponent(const std::shared_ptr<SceneObject>& owner, const std::string& name)
        : Component(owner), m_name(name)
    {
        m_environment = ResourceCreator::Instance().NewEnvironmentSet(name.c_str(), nullptr);
        m_environment->WaitBuild();

        getOwner()->getRoot()->getResourceAddedEvent().invoke(m_environment);
    }

    //! Destructor
    EnvironmentComponent::~EnvironmentComponent()
    {
        if (m_environment)
        {
            if(hasOwner()) getOwner()->getRoot()->getResourceRemovedEvent().invoke(m_environment);
            m_environment->DecReference();
            m_environment = nullptr;
        }
        ResourceManager::Instance().DeleteDaemon();
    }

    //! Update
    void EnvironmentComponent::onUpdate(float dt)
    {
        if (m_environment == nullptr) return;
    }

    //! Render
    void EnvironmentComponent::onRender()
    {
        if (m_environment == nullptr) return;

        // Render
        m_environment->Render();
    }

    //! Serialize
    void EnvironmentComponent::to_json(json& j) const
    {
        j = json {
            {"name", m_name},
            {"ambSkyCol", getAmbientSkyColor()},
            {"ambGndCol", getAmbientGroundColor()},
            {"ambDir", getAmbientDirection()},
            {"dirInt_0", getDirectionalLightIntensity(0)},
            {"dirCol_0", getDirectionalLightColor(0)},
            {"dirDir_0", getDirectionalLightDirection(0)},
            {"dirInt_1", getDirectionalLightIntensity(1)},
            {"dirCol_1", getDirectionalLightColor(1)},
            {"dirDir_1", getDirectionalLightDirection(1)},
            {"dirInt_2", getDirectionalLightIntensity(2)},
            {"dirCol_2", getDirectionalLightColor(2)},
            {"dirDir_2", getDirectionalLightDirection(2)},
            {"pointRng_0", getPointLightRange(0)},
            {"pointInt_0", getPointLightIntensity(0)},
            {"pointCol_0", getPointLightColor(0)},
            {"pointPos_0", getPointLightPosition(0)},
            {"pointRng_1", getPointLightRange(1)},
            {"pointInt_1", getPointLightIntensity(1)},
            {"pointCol_1", getPointLightColor(1)},
            {"pointPos_1", getPointLightPosition(1)},
            {"pointRng_2", getPointLightRange(2)},
            {"pointInt_2", getPointLightIntensity(2)},
            {"pointCol_2", getPointLightColor(2)},
            {"pointPos_2", getPointLightPosition(2)},
            {"pointRng_3", getPointLightRange(3)},
            {"pointInt_3", getPointLightIntensity(3)},
            {"pointCol_3", getPointLightColor(3)},
            {"pointPos_3", getPointLightPosition(3)},
            {"pointRng_4", getPointLightRange(4)},
            {"pointInt_4", getPointLightIntensity(4)},
            {"pointCol_4", getPointLightColor(4)},
            {"pointPos_4", getPointLightPosition(4)},
            {"pointRng_5", getPointLightRange(5)},
            {"pointInt_5", getPointLightIntensity(5)},
            {"pointCol_5", getPointLightColor(5)},
            {"pointPos_5", getPointLightPosition(5)},
            {"pointRng_6", getPointLightRange(6)},
            {"pointInt_6", getPointLightIntensity(6)},
            {"pointCol_6", getPointLightColor(6)},
            {"pointPos_6", getPointLightPosition(6)},
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
    void EnvironmentComponent::from_json(const json& j)
    {
        setAmbientSkyColor(j.at("ambSkyCol"));
        setAmbientGroundColor(j.at("ambGndCol"));
        setAmbientDirection(j.at("ambGndCol"));
        setAmbientDirection(j.at("ambDir"));
        setDirectionalLightIntensity(0, j.at("dirInt_0"));
        setDirectionalLightColor(0, j.at("dirCol_0"));
        setDirectionalLightDirection(0, j.at("dirDir_0"));
        setDirectionalLightIntensity(1, j.at("dirInt_1"));
        setDirectionalLightColor(1, j.at("dirCol_1"));
        setDirectionalLightDirection(1, j.at("dirDir_1"));
        setDirectionalLightIntensity(2, j.at("dirInt_2"));
        setDirectionalLightColor(2, j.at("dirCol_2"));
        setDirectionalLightDirection(2, j.at("dirDir_2"));
        setPointLightRange(0, j.at("pointRng_0"));
        setPointLightIntensity(0, j.at("pointInt_0"));
        setPointLightColor(0, j.at("pointCol_0"));
        setPointLightPosition(0, j.at("pointPos_0"));
        setPointLightRange(1, j.at("pointRng_1"));
        setPointLightIntensity(1, j.at("pointInt_1"));
        setPointLightColor(1, j.at("pointCol_1"));
        setPointLightPosition(1, j.at("pointPos_1"));
        setPointLightRange(2, j.at("pointRng_2"));
        setPointLightIntensity(2, j.at("pointInt_2"));
        setPointLightColor(2, j.at("pointCol_2"));
        setPointLightPosition(2, j.at("pointPos_2"));
        setPointLightRange(3, j.at("pointRng_3"));
        setPointLightIntensity(3, j.at("pointInt_3"));
        setPointLightColor(3, j.at("pointCol_3"));
        setPointLightPosition(3, j.at("pointPos_3"));
        setPointLightRange(4, j.at("pointRng_4"));
        setPointLightIntensity(4, j.at("pointInt_4"));
        setPointLightColor(4, j.at("pointCol_4"));
        setPointLightPosition(4, j.at("pointPos_4"));
        setPointLightRange(5, j.at("pointRng_5"));
        setPointLightIntensity(5, j.at("pointInt_5"));
        setPointLightColor(5, j.at("pointCol_5"));
        setPointLightPosition(5, j.at("pointPos_5"));
        setPointLightRange(6, j.at("pointRng_6"));
        setPointLightIntensity(6, j.at("pointInt_6"));
        setPointLightColor(6, j.at("pointCol_6"));
        setPointLightPosition(6, j.at("pointPos_6"));
        setDistanceFogNear(j.at("fogNear"));
        setDistanceFogFar(j.at("fogFar"));
        setDistanceFogAlpha(j.at("fogAlpha"));
        setDistanceFogColor(j.at("fogCol"));
        setShadowColor(j.at("shadowCol"));
        setShadowDensity(j.at("shadowDensity"));
        setShadowWideness(j.at("shadowWideness"));
    }
}
