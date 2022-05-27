#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/Collider.h"

namespace ige::scene
{
    //! MeshCollider
    class MeshCollider : public Collider
    {
    public:
        //! Constructor
        MeshCollider(SceneObject& owner);

        //! Destructor
        virtual ~MeshCollider();

        //! Get name
        std::string getName() const override { return "MeshCollider"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::MeshCollider; }

        //! Mesh Index
        int getMeshIndex() const { return m_meshIndex; };
        void setMeshIndex(int idx);

        //! Convex
        bool isConvex() const;
        void setConvex(bool convex = true);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Create collision shape
        virtual void createShape() override;

    protected:
        //! Mesh index
        int m_meshIndex = 0;

        //! Convex or Concave
        bool m_bIsConvex = true;

        //! Cache btTriangleIndexVertexArray
        btTriangleIndexVertexArray* m_indexVertexArrays = nullptr;

        //! Cache indices
        int* m_indices = nullptr;

        //! Cache btPosition
        btVector3* m_btPositions = nullptr;
    };
} // namespace ige::scene