#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "components/physic/collider/MeshCollider.h"
#include "components/physic/Rigidbody.h"
#include "components/FigureComponent.h"
#include "scene/SceneObject.h"
#include "utils/PhysicHelper.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Constructor
    MeshCollider::MeshCollider(SceneObject& owner)
        : Collider(owner)
    {
    }

    //! Destructor
    MeshCollider::~MeshCollider()
    {
        destroyShape();
    }

    //! Set mesh index
    void MeshCollider::setMeshIndex(int idx)
    {
        if (m_meshIndex != idx) {
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
        // Dynamic body -> force to convex
        auto body = getOwner()->getComponent<Rigidbody>();
        if (body && !body->isKinematic() && !convex) {
            convex = true;
        }
        m_bIsConvex = convex;
        recreateShape();
    }

    //! Create single shape
    void MeshCollider::createSingleShape(int index) {
        if (index < 0 || index >= getMeshCount())
            return;
        std::vector<Vec3> positions;
        auto* figure = getOwner()->getComponent<FigureComponent>()->getFigure();
        if (figure->NumMeshes() > 0 && index >= 0 && index < figure->NumMeshes())
        {
            int offset = 0;
            int size = 100000000;
            int space = Space::LocalSpace;

            auto mesh = figure->GetMesh(index);
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
                    float* palettebuffer = nullptr;
                    float* inbindSkinningMatrices = nullptr;
                    figure->AllocTransformBuffer(space, palettebuffer, inbindSkinningMatrices);
                    figure->ReadPositions(index, offset, size, space, palettebuffer, inbindSkinningMatrices, &positions);
                    if (inbindSkinningMatrices)
                        PYXIE_FREE_ALIGNED(inbindSkinningMatrices);
                    if (palettebuffer)
                        PYXIE_FREE_ALIGNED(palettebuffer);
                }
            }
            auto numPoints = (int)positions.size();
            auto btPositions = new btVector3[numPoints];
            m_btPositions.push_back(btPositions);
            for (auto i = 0; i < numPoints; ++i)
                btPositions[i] = PhysicHelper::to_btVector3(positions[i]);
            positions.clear();

            if (m_bIsConvex)
            {
                if (m_meshIndex == -1 && m_numMesh > 1) {
                    auto* compoundShape = (btCompoundShape*)m_shape.get();
                    if (compoundShape) {
                        auto convexHull = std::make_shared<btConvexHullShape>((btScalar*)btPositions, numPoints);
                        convexHull->optimizeConvexHull();
                        btTransform transform;
                        transform.setIdentity();
                        compoundShape->addChildShape(transform, convexHull.get());
                        m_shapes.push_back(convexHull);
                    }
                }
                else {
                    auto convexHull = std::make_unique<btConvexHullShape>((btScalar*)btPositions, numPoints);
                    convexHull->optimizeConvexHull();
                    m_shape = std::move(convexHull);                    
                }
            }
            else
            {
                auto mesh = figure->GetMesh(index);
                auto indices = new int[mesh->numIndices];
                for (uint32_t i = 0; i < mesh->numIndices; ++i)
                    indices[i] = (int)mesh->indices[i];
                m_indices.push_back(indices);

                auto indexVertexArrays = new btTriangleIndexVertexArray(mesh->numIndices / 3, indices, 3 * 4, numPoints, (btScalar*)btPositions, sizeof(btVector3));
                m_indexVertexArrays.push_back(indexVertexArrays);

                bool useQuantizedAabbCompression = true;
                if (m_meshIndex == -1 && m_numMesh > 1) {
                    auto* compoundShape = (btCompoundShape*)m_shape.get();
                    if (compoundShape) {
                        auto triangleMeshShape = std::make_shared<btBvhTriangleMeshShape>(indexVertexArrays, useQuantizedAabbCompression);
                        btTransform transform;
                        transform.setIdentity();
                        compoundShape->addChildShape(transform, triangleMeshShape.get());
                        m_shapes.push_back(triangleMeshShape);
                    }
                }
                else {
                    auto triangleMeshShape = std::make_unique<btBvhTriangleMeshShape>(indexVertexArrays, useQuantizedAabbCompression);
                    m_shape = std::move(triangleMeshShape);
                }
            }
        }
    }

    //! Destroy collision shape
    void MeshCollider::destroyShape() {
        Collider::destroyShape();

        for (auto* indexArray : m_indexVertexArrays)
            if (indexArray) delete indexArray;
        m_indexVertexArrays.clear();

        for (auto* index : m_indices)
            if (index) delete[] index;
        m_indices.clear();

        for (auto* pos : m_btPositions)
            if (pos) delete[] pos;
        m_btPositions.clear();
    }

    //! Create collision shape
    void MeshCollider::createShape()
    {
        // Destroy old instance
        destroyShape();

        // Load figure for meshes
        Figure* figure = nullptr;
        auto figureComp = getOwner()->getComponent<FigureComponent>();
        if (figureComp) {
            figure = figureComp->getFigure();
        }           
        
        // Load mesh from figure
        if (figure != nullptr)
        {
            figure->WaitInitialize();
            getOwner()->onUpdate(0.f); // force update transform

            m_numMesh = figure->NumMeshes();
            if (m_numMesh <= 0) return;

            if (m_meshIndex == -1 && m_numMesh > 1) {
                auto compoundShape = std::make_unique<btCompoundShape>();
                m_shape = std::move(compoundShape);
                for (int i = 0; i < getMeshCount(); ++i) {
                    createSingleShape(i);
                }
            }
            else {
                createSingleShape(m_meshIndex >= 0 ? m_meshIndex : 0);
            }
        }

        if (m_shape != nullptr) {
            setScale(m_scale);
            setMargin(m_margin);
        }
    }

    void MeshCollider::setScale(const Vec3& scale) {
        m_scale = scale;
        if(m_shape) m_shape->setLocalScaling(PhysicHelper::to_btVector3(m_scale));
    }

    //! Serialize
    void MeshCollider::to_json(json &j) const
    {
        Collider::to_json(j);
        j["meshIdx"] = getMeshIndex();
        j["convex"] = isConvex();
        j["maxIdx"] = getMeshCount();
    }

    //! Serialize finished event
    void MeshCollider::onSerializeFinished(Scene* scene)
    {
        Collider::onSerializeFinished(scene);
        setMeshIndex(m_json.value("meshIdx", 0));
        setConvex(m_json.value("convex", false));
        m_json.clear();
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
