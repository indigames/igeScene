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
        NavArea(SceneObject& owner);

        //! Destructor
        virtual ~NavArea();

        //! Get name
        std::string getName() const override { return "NavArea"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::NavArea; }

        //! AreaID
        uint8_t getAreaId() const { return m_areaId; }
        void setAreaId(uint8_t id);

        //! Bounding box
        const AABBox& getBoundingBox() const { return m_boundingBox; }
        void setBoundingBox(const AABBox& aabb);

        //! World bounding box
        AABBox getWorldBoundingBox() const;

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

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
    };
} // namespace ige::scene
