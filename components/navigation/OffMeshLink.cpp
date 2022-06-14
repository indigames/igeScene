#include "components/navigation/OffMeshLink.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{
    //! Constructor
    OffMeshLink::OffMeshLink(SceneObject& owner)
        : Component(owner)
    {
    }

    OffMeshLink::~OffMeshLink()
    {
        m_endPoint.reset();
    }

    //! Serialize
    void OffMeshLink::to_json(json &j) const
    {
        Component::to_json(j);
        j["endUuid"] = getEndPoint() ? getEndPoint()->getUUID() : "";
        j["radius"] = getRadius();
        j["2way"] = isBidirectional();
        j["mask"] = getMask();
        j["areaId"] = getAreaId();
    }

    //! Deserialize
    void OffMeshLink::from_json(const json &j)
    {
        auto uuid = j.value("endUuid", std::string());
        setEndPoint(getOwner()->getScene()->findObjectByUUID(uuid));
        setRadius(j.value("radius", 1.f));
        setBidirectional(j.value("2way", true));
        setMask(j.value("mask", 1));
        setAreaId(j.value("areaId", 63));
        Component::from_json(j);
    }

    //! Update property by key value
    void OffMeshLink::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("endUuid") == 0)
        {
            setEndPoint(getOwner()->getScene()->findObjectByUUID(val));
        }
        else if (key.compare("radius") == 0)
        {
            setRadius(val);
        }
        else if (key.compare("2way") == 0)
        {
            setBidirectional(val);
        }
        else if (key.compare("mask") == 0)
        {
            setMask(val);
        }
        else if (key.compare("areaId") == 0)
        {
            setAreaId(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }
} // namespace ige::scene
