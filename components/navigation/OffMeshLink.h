#pragma once

#include "components/Component.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    // Class OffMeshLink: Navigation offmesh link
    class OffMeshLink : public Component
    {
    public:
        //! Constructor
        OffMeshLink(SceneObject &owner);

        //! Destructor
        virtual ~OffMeshLink();

        //! Get name
        std::string getName() const override { return "OffMeshLink"; }

        //! Endpoint object
        std::shared_ptr<SceneObject> getEndPoint() const { return m_endPoint.expired() ? nullptr : m_endPoint.lock(); }
        void setEndPoint(std::shared_ptr<SceneObject> endpoint) { m_endPoint = endpoint; }

        //! Radius
        float getRadius() const { return m_radius; }
        void setRadius(float radius) { m_radius = radius; }

        //! Bidirectional
        bool isBidirectional() const { return m_bIsBidirectional; }
        void setBidirectional(bool directional) { m_bIsBidirectional = directional; }

        //! Mask
        uint32_t getMask() const { return m_mask; }
        void setMask(uint32_t mask) { m_mask = mask; }

        //! Area ID
        uint32_t getAreaId() const { return m_areaId; }
        void setAreaId(uint32_t id) { m_areaId = id; }

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

    protected:
        //! Endpoint object
        std::weak_ptr<SceneObject> m_endPoint;

        //! Radius
        float m_radius = 1.f;

        //! Bidirectional flag
        bool m_bIsBidirectional = true;

        //! Flags mask to represent properties of this mesh
        uint32_t m_mask = 1;

        //! Area id to be used for this off mesh connection's internal poly
        uint32_t m_areaId = 1;

        //! Cache endpoint changed flag
        bool m_bEndPointDirty = false;
    };
} // namespace ige::scene
