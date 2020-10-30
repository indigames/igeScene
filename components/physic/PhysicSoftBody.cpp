#include "components/physic/PhysicSoftBody.h"
#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "scene/SceneObject.h"
#include "physic/PhysicManager.h"

#include "utils/PhysicHelper.h"

#include <BulletSoftBody/btSoftBodyHelpers.h>

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Constructor
    PhysicSoftBody::PhysicSoftBody(SceneObject &owner, const std::string& path)
        : PhysicBase(owner), m_path(path)
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

    //! Set path
    void PhysicSoftBody::setPath(const std::string& path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        if (strcmp(m_path.c_str(), relPath.c_str()) != 0)
        {
            m_path = relPath;
            recreateBody();
        }
    }

    //! Create physic body
    void PhysicSoftBody::createBody()
    {
        // Create collision shape
        if (m_body)
            m_body.reset();

        if (m_indicesMap != nullptr)
            delete[] m_indicesMap;
        m_indicesMap = nullptr;
        m_numIndices = 0;

        auto world = PhysicManager::getInstance()->getDeformableWorld();

        Figure* figure = nullptr;
        bool figureCreated = false;

        std::vector<Vec3> positions;
        auto figureComp = getOwner()->getComponent<FigureComponent>();
        if(figureComp && figureComp->getFigure())
            figure = figureComp->getFigure();
        else
        {
            figure = ResourceCreator::Instance().NewFigure(m_path.c_str());
            figureCreated = true;
        }

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
                optimizeMesh(positions, indices, mesh->numIndices, optPoss, m_numIndices, m_indicesMap);
                m_body = std::unique_ptr<btSoftBody>(btSoftBodyHelpers::CreateFromTriMesh(world->getWorldInfo(), optPoss, indices, mesh->numIndices / 3));

                positions.clear();
                delete[] indices;
                delete[] optPoss;
            }

            if (figureCreated)
            {
                figure->DecReference();
                figure = nullptr;
            }
        }

        if (m_body == nullptr)
            m_body = std::make_unique<btSoftBody>(&world->getWorldInfo());
        m_body->setUserPointer(this);
        
        // Set this to SoftBody
        setSoftBody(true);

        // Apply pre-configurated values to PhysicBase
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
        setSleepingThreshold(m_sleepingThreshold);
        setRestLengthScale(m_restLengthScale);
        setSelfCollision(m_bUseSelfCollision);
        setSoftSoftCollision(m_softSoftCollision);
        setWindVelocity(m_windVelocity);
        setDensity(m_density);

        // Set collision config
        getSoftBody()->m_cfg.kKHR = 1;
        getSoftBody()->m_cfg.kCHR = 1;
        getSoftBody()->m_cfg.kDF = 1;
        getSoftBody()->m_cfg.collisions = btSoftBody::fCollision::CL_SS;
        getSoftBody()->m_cfg.collisions |= btSoftBody::fCollision::CL_RS;
        getSoftBody()->generateClusters(64);

        getSoftBody()->m_materials[0]->m_kLST = 0.5f; // Linear stiffness coefficient [0,1]
        getSoftBody()->m_cfg.kMT = 0.5f;  // Pose matching coefficient [0,1]
        getSoftBody()->m_cfg.kVC = 0.5f;  // Volume conservation coefficient [0,+inf]
        getSoftBody()->m_cfg.kPR = 1.f;  // Pressure coefficient [-inf,+inf]
        getSoftBody()->generateBendingConstraints(2);
        getSoftBody()->setPose(true, true);

        // Apply collision filter group and mask
        if (m_bIsKinematic)
        {
            addCollisionFlag(btCollisionObject::CF_KINEMATIC_OBJECT);
            m_collisionFilterGroup = 2;
            m_collisionFilterMask = 3;
        }
        else
        {
            m_collisionFilterGroup = 1;
            m_collisionFilterMask = -1;
        }

        if (m_body->getBroadphaseHandle())
        {
            m_body->getBroadphaseHandle()->m_collisionFilterGroup = m_collisionFilterGroup;
            m_body->getBroadphaseHandle()->m_collisionFilterMask = m_collisionFilterMask;
        }

        // Continuos detection mode
        setCCD(m_bIsCCD);

        // Add custom material callback for collision events
        addCollisionFlag(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        // Add trigger flag
        if (m_bIsTrigger)
            addCollisionFlag(btCollisionObject::CF_NO_CONTACT_RESPONSE);

        if (m_bIsEnabled)
            activate();
    }

    //! Set local scale
    void PhysicSoftBody::setLocalScale(const Vec3& scale)
    {
        auto dScale = PhysicHelper::to_btVector3(scale) / PhysicHelper::to_btVector3(m_previousScale);
        getSoftBody()->scale({ std::abs(dScale[0]), std::abs(dScale[1]), std::abs(dScale[2]) });
        m_previousScale = scale;

        if (m_body->getCollisionShape())
            m_body->getCollisionShape()->setLocalScaling(PhysicHelper::to_btVector3(m_previousScale));
    }

    //! Update Bullet transform
    void PhysicSoftBody::updateBtTransform()
    {   
        const auto& currTrans = m_body->getWorldTransform();
        auto newTrans = PhysicHelper::to_btTransform(getOwner()->getTransform()->getWorldRotation(), getOwner()->getTransform()->getWorldPosition());
        getSoftBody()->transform(newTrans * currTrans.inverse());
        m_body->setWorldTransform(newTrans);

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
       // const btTransform& result = m_body->getWorldTransform();
       // getOwner()->getTransform()->setPosition(PhysicHelper::from_btVector3(result.getOrigin()));
       // getOwner()->getTransform()->setRotation(PhysicHelper::from_btQuaternion(result.getRotation()));

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

            void* buffer = PYXIE_MALLOC(m_numIndices * 3 * elemSize);
            MemoryCleaner cleaner(buffer);

            auto totalCount = 0;
            const auto& nodes = getSoftBody()->m_nodes;
            for (int i = 0; i < m_numIndices; ++i) {
                int idx = m_indicesMap[i];
                for (int j = 0; j < 3; ++j) {
                    switch (mesh->vertexAttributes[attIdx].type) {
                    case GL_FLOAT: ((float*)buffer)[totalCount++] = nodes[idx].m_x[j]; break;
                    case GL_SHORT: ((int16_t*)buffer)[totalCount++] = F32toS16(nodes[idx].m_x[j]); break;
                    case GL_HALF_FLOAT: ((uint16_t*)buffer)[totalCount++] = F32toF16(nodes[idx].m_x[j]); break;
                    case GL_UNSIGNED_BYTE: ((uint8_t*)buffer)[totalCount++] = F32toU8(nodes[idx].m_x[j]); break;
                    }
                }
            }

            int size = totalCount / mesh->vertexAttributes[attIdx].size;
            if (offset + size > mesh->numVerticies)
                size = mesh->numVerticies - offset;

            uint8_t* p = (uint8_t*)buffer;
            for (int i = 0; i < size; i++) {
                uint8_t* location = ((uint8_t*)mesh->vertices) + (i + offset) * mesh->vertexFormatSize + mesh->vertexAttributes[attIdx].offset;
                memcpy(location, p, elemSize * mesh->vertexAttributes[attIdx].size);
                p += elemSize * mesh->vertexAttributes[attIdx].size;
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

            void* buffer = PYXIE_MALLOC(m_numIndices * 3 * elemSize);
            MemoryCleaner cleaner(buffer);

            auto totalCount = 0;
            const auto& nodes = getSoftBody()->m_nodes;
            for (int i = 0; i < m_numIndices; ++i) {
                int idx = m_indicesMap[i];
                for (int j = 0; j < 3; ++j) {
                    switch (mesh->vertexAttributes[attIdx].type) {
                    case GL_FLOAT: ((float*)buffer)[totalCount++] = nodes[idx].m_n[j]; break;
                    case GL_SHORT: ((int16_t*)buffer)[totalCount++] = F32toS16(nodes[idx].m_n[j]); break;
                    case GL_HALF_FLOAT: ((uint16_t*)buffer)[totalCount++] = F32toF16(nodes[idx].m_n[j]); break;
                    case GL_UNSIGNED_BYTE: ((uint8_t*)buffer)[totalCount++] = F32toU8(nodes[idx].m_n[j]); break;
                    }
                }
            }

            int size = totalCount / mesh->vertexAttributes[attIdx].size;
            if (offset + size > mesh->numVerticies)
                size = mesh->numVerticies - offset;

            uint8_t* p = (uint8_t*)buffer;
            for (int i = 0; i < size; i++) {
                uint8_t* location = ((uint8_t*)mesh->vertices) + (i + offset)*mesh->vertexFormatSize + mesh->vertexAttributes[attIdx].offset;
                memcpy(location, p, elemSize * mesh->vertexAttributes[attIdx].size);
                p += elemSize * mesh->vertexAttributes[attIdx].size;
            }
            figure->ResetMeshBuffer(index, true, false, true);
        }
    }

    //! Optimize mesh
    void PhysicSoftBody::optimizeMesh(const std::vector<Vec3>& orgPoss, int* indices, int numIndeces, float*& optPoss, int& numOptPoss, int*& orgMap) 
    {
        if (orgMap != nullptr)
            delete[] orgMap;

        orgMap = new int[orgPoss.size()];
        std::map<Vec3, int> vecList;
        int numOptVertex = 0;
        for (int i = 0; i < numIndeces; i++) {
            int idx = indices[i];
            const Vec3& v = orgPoss[idx];
            if (vecList.count(v)) {
                indices[i] = vecList[v];
            }
            else {
                vecList[v] = numOptVertex;
                indices[i] = numOptVertex;
                numOptVertex++;
            }
            orgMap[idx] = indices[i];
        }
        std::map<uint32_t, Vec3> vecListRV;
        for (auto& it : vecList)
            vecListRV[it.second] = it.first;

        numOptPoss = vecListRV.size();
        optPoss = new float[numOptPoss * 3];
        int numf = 0;
        for (auto& it : vecListRV) {
            for (int i = 0; i < 3; i++) {
                optPoss[numf] = it.second[i];
                numf++;
            }
        }
    }

    //! Serialize
    void PhysicSoftBody::to_json(json &j) const
    {
        PhysicBase::to_json(j);
        j["path"] = getPath();
        j["dampCoeff"] = getDampingCoefficient();
        j["repStiff"] = getRepulsionStiffness();
        j["sleepThr"] = getSleepingThreshold();
        j["restLS"] = getRestLengthScale();
        j["isSelfCol"] = isSelfCollision();
        j["isSoftCol"] = isSoftSoftCollision();
        j["windVel"] = PhysicHelper::from_btVector3(getWindVelocity());
        j["density"] = getDensity();
    }

    //! Deserialize
    void PhysicSoftBody::from_json(const json &j)
    {
        PhysicBase::from_json(j);
        setPath(j.value("path",std::string()));
        setDampingCoefficient(j.value("dampCoeff", 0.4f));
        setRepulsionStiffness(j.value("repStiff", 0.5f));
        setSleepingThreshold(j.value("sleepThr", 0.f));
        setRestLengthScale(j.value("restLS", 0.f));
        setSelfCollision(j.value("isSelfCol", false));
        setSoftSoftCollision(j.value("isSoftCol", false));
        setSoftSoftCollision(j.value("isSoftCol", false));
        setWindVelocity(PhysicHelper::to_btVector3(j.value("windVel", Vec3())));
        setDensity(j.value("density", 1.f));
    }
} // namespace ige::scene
