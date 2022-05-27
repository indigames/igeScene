#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "components/physic/MeshCollider.h"
#include "components/physic/Rigidbody.h"
#include "components/FigureComponent.h"
#include "scene/SceneObject.h"
#include "utils/PhysicHelper.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Constructor
    MeshCollider::MeshCollider(SceneObject &owner)
        : Collider(owner)
    {
        createShape();
    }

    //! Destructor
    MeshCollider::~MeshCollider()
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

    //! Set mesh index
    void MeshCollider::setMeshIndex(int idx)
    {
        if (m_meshIndex != idx)
        {
            m_meshIndex = idx;
            recreateShape();
        }
    }

    //! Convex
    bool MeshCollider::isConvex() const
    {
        return m_bIsConvex;
    }

    void MeshCollider::setConvex(bool convex)
    {
        if (m_bIsConvex != convex)
        {
            // Dynamic body -> force to convex
            auto body = getOwner()->getComponent<Rigidbody>();
            if (body && !body->isKinematic() && !convex) {
                convex = true;
            }
            m_bIsConvex = convex;
            recreateShape();
        }
    }

    //! Create collision shape
    void MeshCollider::createShape()
    {
        // Create collision shape
        if (m_shape != nullptr)
            m_shape.reset();

        Figure* figure = nullptr;
        auto figureComp = getOwner()->getComponent<FigureComponent>();
        if (figureComp)
            figure = figureComp->getFigure();
        

        // Load mesh from figure
        if (figure != nullptr)
        {
            std::vector<Vec3> positions;

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
            figure = nullptr;
        }

        if (m_shape == nullptr)
            m_shape = std::make_unique<btConvexHullShape>();

        setScale(m_scale);
    }

    //! Serialize
    void MeshCollider::to_json(json &j) const
    {
        Collider::to_json(j);
        j["meshIdx"] = getMeshIndex();
        j["convex"] = isConvex();
    }

    //! Deserialize
    void MeshCollider::from_json(const json &j)
    {
        Collider::from_json(j);
        setMeshIndex(j.value("meshIdx", 0));
        setConvex(j.value("convex", false));
    }

    //! Update property by key value
    void MeshCollider::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("meshIdx") == 0)
            setMeshIndex(val);
        else if (key.compare("convex") == 0)
            setConvex(val);
        else
            Collider::setProperty(key, val);
    }
} // namespace ige::scene
