#include "components/physic/PhysicSoftBody.h"
#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "scene/SceneObject.h"
#include "scene/SceneManager.h"
#include "physic/PhysicManager.h"

#include "utils/PhysicHelper.h"

#include <BulletSoftBody/btSoftBodyHelpers.h>

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Constructor
    PhysicSoftBody::PhysicSoftBody(SceneObject &owner)
        : PhysicBase(owner)
    {
        init();
    }

    //! Destructor
    PhysicSoftBody::~PhysicSoftBody()
    {
        if (m_indicesMap != nullptr)
            delete[] m_indicesMap;
        m_indicesMap = nullptr;
    }

    //! Create physic body
    void PhysicSoftBody::createBody()
    {
        // Create collision shape
        if (m_body)
            m_body.reset();

        auto world = PhysicManager::getInstance()->getDeformableWorld();

        Figure* figure = nullptr;
        std::vector<Vec3> positions;
        auto figureComp = getOwner()->getComponent<FigureComponent>();
        if(figureComp && figureComp->getFigure())
            figure = figureComp->getFigure();
        if (figure != nullptr)
        {
            figure->WaitInitialize();
            if (figure->NumMeshes() > 0)
            {
                int index = 0;
                int offset = 0;
                int size = 100000000;
                int space = Space::WorldSpace;

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

                auto indices = new int[mesh->numIndices];
                for (uint32_t i = 0; i < mesh->numIndices; ++i)
                    indices[i] = (int)mesh->indices[i];

                float* optPoss;
                optimizeMesh(positions, indices, mesh->numIndices, optPoss);
                m_body = std::unique_ptr<btSoftBody>(btSoftBodyHelpers::CreateFromTriMesh(world->getWorldInfo(), optPoss, indices, mesh->numIndices / 3));

                positions.clear();
                delete[] indices;
                delete[] optPoss;
            }
        }

        if (m_body == nullptr)
            m_body = std::make_unique<btSoftBody>(&world->getWorldInfo());
        m_body->setUserPointer(this);
        
        // Set this to SoftBody
        setSoftBody(true);

        // Apply pre-configurated values to PhysicBase
        if(m_mass > 0) // mass based on total mass
            setMass(m_mass);
        setFriction(m_friction);
        setRestitution(m_restitution);
        setLinearVelocity(m_linearVelocity);
        setAngularVelocity(m_angularVelocity);
        setLinearFactor(m_linearFactor);
        setAngularFactor(m_angularFactor);
        setCollisionMargin(m_collisionMargin);

        // Recalculate scale
        auto scale = m_previousScale;
        m_previousScale = Vec3(1.f, 1.f, 1.f);
        setLocalScale(scale);

        // Apply pre-configurated values
        setDampingCoefficient(m_dampingCoefficient);
        setRepulsionStiffness(m_repulsionStiffness);

        if(m_sleepingThreshold > 0)
            setSleepingThreshold(m_sleepingThreshold);

        if(m_restLengthScale > 0)
            setRestLengthScale(m_restLengthScale);

        setSelfCollision(m_bUseSelfCollision);
        setSoftSoftCollision(m_softSoftCollision);
        setWindVelocity(m_windVelocity);

        getSoftBody()->generateBendingConstraints(2);
        btSoftBodyHelpers::ReoptimizeLinkOrder(getSoftBody());

        getSoftBody()->m_materials[0]->m_kLST = 0.5f; // Linear stiffness coefficient [0,1]
        getSoftBody()->m_cfg.kMT = 0.5f;  // Pose matching coefficient [0,1]
        getSoftBody()->m_cfg.kVC = 0.5f;  // Volume conservation coefficient [0,+inf]
        getSoftBody()->m_cfg.kPR = 1.f;  // Pressure coefficient [-inf,+inf]

        // Apply collision filter group and mask
        setCollisionFilterGroup(m_collisionFilterGroup);
        setCollisionFilterMask(m_collisionFilterMask);

        // Continuos detection mode
        setCCD(m_bIsCCD);

        // Add custom material callback for collision events
        addCollisionFlag(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        // Activate
        if (m_bIsEnabled)
            activate();
    }

    //! Set local scale
    void PhysicSoftBody::setLocalScale(const Vec3& scale)
    {
        auto dScale = PhysicHelper::to_btVector3(scale) / PhysicHelper::to_btVector3(m_previousScale);
        getSoftBody()->scale({ std::abs(dScale[0]), std::abs(dScale[1]), std::abs(dScale[2]) });
        m_previousScale = scale;
    }

    //! Update Bullet transform
    void PhysicSoftBody::updateBtTransform()
    {
        if (!SceneManager::getInstance()->isEditor())
            return;

        auto newTrans = PhysicHelper::to_btTransform(getOwner()->getTransform()->getWorldRotation(), getOwner()->getTransform()->getWorldPosition());
        getSoftBody()->transformTo(newTrans);

        Vec3 scale = getOwner()->getTransform()->getWorldScale();
        Vec3 dScale = { scale[0] - m_previousScale[0], scale[1] - m_previousScale[1], scale[2] - m_previousScale[2] };
        float scaleDelta = vmath_lengthSqr(dScale.P(), 3);
        if (scaleDelta >= 0.01f)
        {
            setLocalScale(scale);
        }
    }

    //! Update IGE transform
    void PhysicSoftBody::updateIgeTransform()
    {
        auto figureComp = getOwner()->getComponent<FigureComponent>();
        if (!figureComp || !figureComp->getFigure())
            return;

        int index = 0;
        int offset = 0;
                
        auto figure = figureComp->getFigure();
        auto mesh = figure->GetMesh(index);
        auto attIdx = -1;
        for (uint16_t i = 0; i < mesh->numVertexAttributes; ++i) {
            if (mesh->vertexAttributes[i].id == AttributeID::ATTRIBUTE_ID_POSITION) {
                attIdx = i;
                break;
            }
        }

        if (attIdx != -1)
        {
            int elemSize = 0;
            switch (mesh->vertexAttributes[attIdx].type) {
            case GL_FLOAT: elemSize = 4; break;
            case GL_SHORT: elemSize = 2; break;
            case GL_HALF_FLOAT: elemSize = 2; break;
            case GL_UNSIGNED_BYTE: elemSize = 1; break;
            }

            const auto& nodes = getSoftBody()->m_nodes;
            for (int i = 0; i < mesh->numVerticies; ++i) {
                int idx = m_indicesMap[i];
                auto* buffer = (uint8_t*)PYXIE_MALLOC(3 * elemSize);
                MemoryCleaner cleaner(buffer);

                for (int j = 0; j < 3; ++j) {
                    switch (mesh->vertexAttributes[attIdx].type) {
                    case GL_FLOAT: ((float*)buffer)[j] = nodes[idx].m_x[j]; break;
                    case GL_SHORT: ((int16_t*)buffer)[j] = F32toS16(nodes[idx].m_x[j]); break;
                    case GL_HALF_FLOAT: ((uint16_t*)buffer)[j] = F32toF16(nodes[idx].m_x[j]); break;
                    case GL_UNSIGNED_BYTE: ((uint8_t*)buffer)[j] = F32toU8(nodes[idx].m_x[j]); break;
                    }
                }
                uint8_t* location = ((uint8_t*)mesh->vertices) + (i + offset) * mesh->vertexFormatSize + mesh->vertexAttributes[attIdx].offset;
                memcpy(location, buffer, elemSize * mesh->vertexAttributes[attIdx].size);
            }
            figure->ResetMeshBuffer(index, true, false, true);
        }
  
        attIdx = -1;
        for (uint16_t i = 0; i < mesh->numVertexAttributes; ++i) {
            if (mesh->vertexAttributes[i].id == AttributeID::ATTRIBUTE_ID_NORMAL) {
                attIdx = i;
                break;
            }
        }
        if (attIdx != -1)
        {
            int elemSize = 0;
            switch (mesh->vertexAttributes[attIdx].type) {
            case GL_FLOAT: elemSize = 4; break;
            case GL_SHORT: elemSize = 2; break;
            case GL_HALF_FLOAT: elemSize = 2; break;
            case GL_UNSIGNED_BYTE: elemSize = 1; break;
            }

            const auto& nodes = getSoftBody()->m_nodes;
            for (int i = 0; i < mesh->numVerticies; ++i) {
                int idx = m_indicesMap[i];
                auto* buffer = (uint8_t*)PYXIE_MALLOC(3 * elemSize);
                MemoryCleaner cleaner(buffer);

                for (int j = 0; j < 3; ++j) {
                    switch (mesh->vertexAttributes[attIdx].type) {
                    case GL_FLOAT: ((float*)buffer)[j] = nodes[idx].m_n[j]; break;
                    case GL_SHORT: ((int16_t*)buffer)[j] = F32toS16(nodes[idx].m_n[j]); break;
                    case GL_HALF_FLOAT: ((uint16_t*)buffer)[j] = F32toF16(nodes[idx].m_n[j]); break;
                    case GL_UNSIGNED_BYTE: ((uint8_t*)buffer)[j] = F32toU8(nodes[idx].m_n[j]); break;
                    }
                }
                uint8_t* location = ((uint8_t*)mesh->vertices) + (i + offset) * mesh->vertexFormatSize + mesh->vertexAttributes[attIdx].offset;
                memcpy(location, buffer, elemSize * mesh->vertexAttributes[attIdx].size);
            }
            figure->ResetMeshBuffer(index, true, false, true);
        }
    }

    //! Optimize mesh
    void PhysicSoftBody::optimizeMesh(const std::vector<Vec3>& orgPoss, int* indices, int numIndeces, float*& optPoss) 
    {
        if (m_indicesMap != nullptr)
            delete[] m_indicesMap;
        m_indicesMap = new int[orgPoss.size()];

        std::map<Vec3, int> vecList;
        int numOptVertex = 0;
        for (int i = 0; i < numIndeces; i++) {
            int idx = indices[i];
            const Vec3& v = orgPoss[idx];
            if (vecList.count(v) == 0) {
                vecList[v] = numOptVertex;
                numOptVertex++;
            }
            m_indicesMap[idx] = indices[i] = vecList[v];
        }
        std::map<uint32_t, Vec3> vecListRV;
        for (const auto& it : vecList)
            vecListRV[it.second] = it.first;

        optPoss = new float[vecListRV.size() * 3];
        int numf = 0;
        for (const auto& it : vecListRV) {
            for (int i = 0; i < 3; i++) {
                optPoss[numf] = it.second[i];
                numf++;
            }
        }
        vecList.clear();
        vecListRV.clear();
    }

    //! Serialize
    void PhysicSoftBody::to_json(json &j) const
    {
        PhysicBase::to_json(j);
        j["dampCoeff"] = getDampingCoefficient();
        j["repStiff"] = getRepulsionStiffness();
        j["sleepThr"] = getSleepingThreshold();
        j["restLS"] = getRestLengthScale();
        j["isSelfCol"] = isSelfCollision();
        j["isSoftCol"] = isSoftSoftCollision();
        j["windVel"] = PhysicHelper::from_btVector3(getWindVelocity());
    }

    //! Deserialize
    void PhysicSoftBody::from_json(const json &j)
    {
        PhysicBase::from_json(j);
        setDampingCoefficient(j.value("dampCoeff", 0.4f));
        setRepulsionStiffness(j.value("repStiff", 0.5f));
        setSleepingThreshold(j.value("sleepThr", 0.f));
        setRestLengthScale(j.value("restLS", 0.f));
        setSelfCollision(j.value("isSelfCol", false));
        setSoftSoftCollision(j.value("isSoftCol", false));
        setWindVelocity(PhysicHelper::to_btVector3(j.value("windVel", Vec3())));
    }
} // namespace ige::scene
