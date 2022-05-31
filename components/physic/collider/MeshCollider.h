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
        int getMeshCount() const { return m_numMesh; }
        int getMeshIndex() const { return m_meshIndex; };
        void setMeshIndex(int idx);

        //! Convex
        bool isConvex() const;
        void setConvex(bool convex = true);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

        //! Not support scale
        virtual void setScale(const Vec3& scale) override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Serialize finished event
        virtual void onSerializeFinished(Scene* scene) override;

        //! Create collision shape
        virtual void createShape() override;

        //! Destroy shape
        virtual void destroyShape() override;

    protected:
        //! Helper to create single mesh shape
        void createSingleShape(int index);

    protected:
        //! Mesh index
        int m_meshIndex = -1;

        //! Mesh counter
        int m_numMesh = 0;

        //! Convex or Concave
        bool m_bIsConvex = true;

        //! Cache btTriangleIndexVertexArray
        std::vector<btTriangleIndexVertexArray*> m_indexVertexArrays;

        //! Cache indices
        std::vector<int*> m_indices;

        //! Cache btPosition
        std::vector<btVector3*> m_btPositions;
    };
} // namespace ige::scene