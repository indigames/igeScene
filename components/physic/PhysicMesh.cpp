#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include "components/physic/PhysicMesh.h"
#include "components/FigureComponent.h"
#include "scene/SceneObject.h"
#include "utils/PhysicHelper.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Constructor
    PhysicMesh::PhysicMesh(SceneObject &owner)
        : PhysicObject(owner)
    {
        createCollisionShape();
        init();
    }

    //! Destructor
    PhysicMesh::~PhysicMesh()
    {
        if (m_shape != nullptr)
            m_shape.reset();

        if (m_indexVertexArrays != nullptr)
            delete m_indexVertexArrays;
        m_indexVertexArrays = nullptr;

        if (m_indices != nullptr)
            delete[] m_indices;
        m_indices = nullptr;

        if (m_btPositions != nullptr)
            delete[] m_btPositions;
        m_btPositions = nullptr;
    }

    //! Path
    const std::string &PhysicMesh::getPath() const
    {
        return m_path;
    }

    void PhysicMesh::setPath(const std::string &path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        if (strcmp(m_path.c_str(), relPath.c_str()) != 0)
        {
            m_path = relPath;
            recreateCollisionShape();
        }
    }

    //! Set mesh index
    void PhysicMesh::setMeshIndex(int idx)
    {
        if (m_bIsDirty || m_meshIndex != idx)
        {
            m_meshIndex = idx;
            recreateCollisionShape();
        }
    }

    //! Convex
    float PhysicMesh::isConvex() const
    {
        return m_bIsConvex;
    }

    void PhysicMesh::setConvex(bool convex)
    {
        // Not support concave with non kinematic ridgid body
        if (!isKinematic() && !convex)
        {
            convex = true;
        }

        if (m_bIsConvex != convex)
        {
            m_bIsConvex = convex;
            recreateCollisionShape();
        }
    }

    void PhysicMesh::setIsKinematic(bool isKinematic)
    {
        if (m_bIsKinematic != isKinematic)
        {
            // Not support concave with non kinematic ridgid body
            if (!isKinematic && !m_bIsConvex)
            {
                setConvex(true);
            }
            PhysicObject::setIsKinematic(isKinematic);
        }
    }

    //! Create collision shape
    void PhysicMesh::createCollisionShape()
    {
        // Create collision shape
        if (m_shape != nullptr)
            m_shape.reset();

        Figure* figure = nullptr;
        bool figureCreated = false;
        std::vector<Vec3> positions;

        // Load from mesh file first
        if (!m_path.empty() && fs::exists(m_path) && !fs::is_directory(m_path))
        {
            figure = ResourceCreator::Instance().NewFigure(m_path.c_str());
            figureCreated = true;
        }

        // If failed, try with figure component
        if (figure == nullptr)
        {
            auto figureComp = getOwner()->getComponent<FigureComponent>();
            if (figureComp)
                figure = figureComp->getFigure();
        }

        // Load mesh from figure
        if (figure != nullptr)
        {
            figure->WaitInitialize();
            if (figure->NumMeshes() > 0 && m_meshIndex >= 0 && m_meshIndex < figure->NumMeshes())
            {
                int offset = 0;
                int size = 100000000;
                int space = Space::LocalSpace;

                auto mesh = figure->GetMesh(m_meshIndex);
                auto attIdx = -1;
                for (uint16_t i = 0; i < mesh->numVertexAttributes; ++i)
                {
                    if (mesh->vertexAttributes[i].id == AttributeID::ATTRIBUTE_ID_POSITION)
                    {
                        attIdx = i;
                        break;
                    }
                }

                if (attIdx != -1)
                {
                    if (size + offset > mesh->numVerticies)
                        size = mesh->numVerticies - offset;

                    if (size > 0)
                    {
                        float *palettebuffer = nullptr;
                        float *inbindSkinningMatrices = nullptr;
                        figure->AllocTransformBuffer(space, palettebuffer, inbindSkinningMatrices);
                        figure->ReadPositions(m_meshIndex, offset, size, space, palettebuffer, inbindSkinningMatrices, &positions);
                        if (inbindSkinningMatrices)
                            PYXIE_FREE_ALIGNED(inbindSkinningMatrices);
                        if (palettebuffer)
                            PYXIE_FREE_ALIGNED(palettebuffer);
                    }
                }
                auto numPoints = positions.size();
                if (m_btPositions != nullptr)
                    delete[] m_btPositions;
                m_btPositions = new btVector3[numPoints];
                for (size_t i = 0; i < numPoints; ++i)
                    m_btPositions[i] = PhysicHelper::to_btVector3(positions[i]);
                positions.clear();

                if (m_bIsConvex)
                {
                    auto convexHull = std::make_unique<btConvexHullShape>((btScalar *)m_btPositions, numPoints);
                    convexHull->optimizeConvexHull();
                    m_shape = std::move(convexHull);
                    convexHull = nullptr;
                }
                else
                {
                    int index = 0;
                    auto mesh = figure->GetMesh(index);

                    if (m_indices != nullptr)
                        delete[] m_indices;
                    m_indices = new int[mesh->numIndices];
                    for (uint32_t i = 0; i < mesh->numIndices; ++i)
                        m_indices[i] = (int)mesh->indices[i];

                    if (m_indexVertexArrays != nullptr)
                        delete m_indexVertexArrays;
                    m_indexVertexArrays = new btTriangleIndexVertexArray(mesh->numIndices / 3, m_indices, 3 * 4, numPoints, (btScalar *)m_btPositions, sizeof(btVector3));

                    bool useQuantizedAabbCompression = true;
                    auto triangleMeshShape = std::make_unique<btBvhTriangleMeshShape>(m_indexVertexArrays, useQuantizedAabbCompression);
                    m_shape = std::move(triangleMeshShape);
                    triangleMeshShape = nullptr;
                }
            }
            if(figureCreated)
                figure->DecReference();
            figure = nullptr;
        }

        if (m_shape == nullptr)
            m_shape = std::make_unique<btConvexHullShape>();

        setLocalScale(m_previousScale);
    }

    //! Recreate collision shape
    void PhysicMesh::recreateCollisionShape()
    {
        // Create collision shape
        if (m_shape != nullptr)
            m_shape.reset();

        // Create collision shape
        createCollisionShape();

        // Create body
        recreateBody();
    }

    //! Serialize
    void PhysicMesh::to_json(json &j) const
    {
        PhysicObject::to_json(j);
        j["meshIdx"] = getMeshIndex();
        j["convex"] = isConvex();
        j["path"] = getPath();
    }

    //! Deserialize
    void PhysicMesh::from_json(const json &j)
    {
        setMeshIndex(j.value("meshIdx", 0));
        setConvex(j.value("convex", (int)(true)));
        setPath(j.value("path", std::string()));
        PhysicObject::from_json(j);
    }

} // namespace ige::scene
