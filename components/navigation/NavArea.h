#pragma once

#include "components/Component.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    // Class NavArea
    class NavArea : public Component
    {
    public:
        //! Constructor
        NavArea(SceneObject &owner);

        //! Destructor
        virtual ~NavArea();

        //! Get name
        std::string getName() const override { return "NavArea"; }

        //! AreaID
        uint8_t getAreaId() const { return m_areaId; }
        void setAreaId(uint8_t id);

        //! Area cost
        float getAreaCost() const { return m_areaCost; }
        void setAreaCost(float cost);

        //! Bounding box
        const AABBox& getBoundingBox() const { return m_boundingBox; }
        void setBoundingBox(const AABBox& aabb);

        //! World bounding box
        AABBox getWorldBoundingBox() const;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

    protected:
        //! Bounding box
        AABBox m_boundingBox;

        //! AreaID
        uint8_t m_areaId = 0;

        //! Area Cost
        float m_areaCost = 1.f;
    };
} // namespace ige::scene
