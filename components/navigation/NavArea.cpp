#include "components/navigation/NavArea.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    NavArea::NavArea(SceneObject &owner)
        : Component(owner)
    {
    }

    NavArea::~NavArea()
    {
    }

    //! AreaID
    void NavArea::setAreaId(uint8_t id)
    {
        m_areaId = id;
    }

    //! Area cost
    void NavArea::setAreaCost(float cost)
    {
        m_areaCost = cost;
    }

    //! Bounding box
    void NavArea::setBoundingBox(const AABBox& aabb)
    {
        m_boundingBox = aabb;
    }

    //! World bounding box
    AABBox NavArea::getWorldBoundingBox() const
    {
        return m_boundingBox.Transform(Mat4::Translate(getOwner()->getTransform()->getWorldPosition()));
    }

    //! Serialize
    void NavArea::to_json(json &j) const
    {
        Component::to_json(j);
        j["id"] = getAreaId();
    }

    //! Deserialize
    void NavArea::from_json(const json &j)
    {
        setAreaId(j.value("id", 0));
        Component::from_json(j);
    }
} // namespace ige::scene
