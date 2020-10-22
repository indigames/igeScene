#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/PhysicBase.h"

namespace ige::scene
{
    //! PhysicMesh
    class PhysicMesh : public PhysicBase
    {
    public:
        //! Constructor
        PhysicMesh(SceneObject& owner, const std::string& path = {});

        //! Destructor
        virtual ~PhysicMesh();

        //! Get name
        std::string getName() const override { return "PhysicMesh"; }

        //! Path
        const std::string& getPath() const;
        void setPath(const std::string& path);

        //! Convex
        float isConvex() const;
        void setConvex(bool convex = true);

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Create collision shape
        void createCollisionShape(const std::string& path);

        //! Recreate collision shape once radius changed
        void recreateCollisionShape(const std::string& path);

        //! Set local scale of the box
        virtual void setLocalScale(const Vec3 &scale) override;

    protected:
        //! Path to the model file
        std::string m_path;

        //! Convex or Concave
        bool m_bIsConvex = true;

        //! Cache m_btPositions
        btVector3* m_btPositions = nullptr;

        //! Cache m_indexVertexArrays
        btTriangleIndexVertexArray* m_indexVertexArrays = nullptr;
    };
} // namespace ige::scene