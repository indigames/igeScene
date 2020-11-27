#include "components/navigation/OffMeshLink.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{
    //! Constructor
    OffMeshLink::OffMeshLink(SceneObject &owner)
        : Component(owner)
    {
    }

    OffMeshLink::~OffMeshLink()
    {
    }

    //! Serialize
    void OffMeshLink::to_json(json &j) const
    {
        Component::to_json(j);
        j["endUuid"] = getEndPoint() ? getEndPoint()->getUUID() : std::string();
        j["radius"] = getRadius();
        j["2way"] = isBidirectional();
        j["mask"] = getMask();
        j["areaId"] = getAreaId();
    }

    //! Deserialize
    void OffMeshLink::from_json(const json &j)
    {
        auto uuid = j.value("endUuid", std::string());
        setEndPoint(getOwner()->getScene()->findObjectByUUID(uuid).get());

        setRadius(j.value("radius", 1.f));
        setBidirectional(j.value("2way", true));
        setMask(j.value("mask", 0));
        setAreaId(j.value("areaId", 0));
        Component::from_json(j);
    }
} // namespace ige::scene
