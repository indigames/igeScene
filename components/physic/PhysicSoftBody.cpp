#include "components/physic/PhysicSoftBody.h"
#include "components/physic/PhysicManager.h"
#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "scene/SceneObject.h"
#include "scene/SceneManager.h"

#include "utils/PhysicHelper.h"

#include <BulletSoftBody/btSoftBodyHelpers.h>

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Constructor
    PhysicSoftBody::PhysicSoftBody(SceneObject &owner)
        : PhysicObject(owner)
    {
        m_mass = -1.f;
        init();
    }

    //! Destructor
    PhysicSoftBody::~PhysicSoftBody()
    {
        if (m_indicesMap != nullptr)
            delete[] m_indicesMap;
        m_indicesMap = nullptr;
    }

    //! Get AABB
    AABBox PhysicSoftBody::getAABB()
    {
        btVector3 aabbMin, aabbMax;
        getSoftBody()->getAabb(aabbMin, aabbMax);
        return AABBox(PhysicHelper::from_btVector3(aabbMin), PhysicHelper::from_btVector3(aabbMax));
    }


    //! Set mesh index
    void PhysicSoftBody::setMeshIndex(int idx)
    {
        if (m_bIsDirty || m_meshIndex != idx)
        {
            m_meshIndex = idx;
            recreateBody();
        }
    }

    //! Linear velocity
    void PhysicSoftBody::setLinearVelocity(const btVector3 &velocity)
    {
        if (m_bIsDirty || m_linearVelocity != velocity)
        {
            m_linearVelocity = velocity;
            getSoftBody()->setLinearVelocity(m_linearVelocity);
        }
    }

    //! Angular velocity
    void PhysicSoftBody::setAngularVelocity(const btVector3 &velocity)
    {
        if (m_bIsDirty || m_angularVelocity != velocity)
        {
            m_angularVelocity = velocity;
            getSoftBody()->setAngularVelocity(m_angularVelocity);
        }
    }

    void PhysicSoftBody::setDampingCoeff(float coeff)
    {
        if (m_bIsDirty || m_dampingCoeff != coeff)
        {
            m_dampingCoeff = coeff;
            getSoftBody()->setDampingCoefficient(m_dampingCoeff);
        }
    }

    void PhysicSoftBody::setPressureCoeff(float coeff)
    {
        if (m_bIsDirty || m_pressureCoeff != coeff)
        {
            m_pressureCoeff = coeff;
            getSoftBody()->m_cfg.kPR = m_pressureCoeff;
        }
    }

    void PhysicSoftBody::setVolumeConvCoeff(float coeff)
    {
        if (m_bIsDirty || m_volumeConvCoeff != coeff)
        {
            m_volumeConvCoeff = coeff;
            getSoftBody()->m_cfg.kVC = m_volumeConvCoeff;
        }
    }

    void PhysicSoftBody::setDynamicFrictionCoeff(float coeff)
    {
        if (m_bIsDirty || m_dynamicFrictionCoeff != coeff)
        {
            m_dynamicFrictionCoeff = coeff;
            getSoftBody()->m_cfg.kDF = m_dynamicFrictionCoeff;
        }
    }

    void PhysicSoftBody::setPoseMatchCoeff(float coeff)
    {
        if (m_bIsDirty || m_poseMatchCoeff != coeff)
        {
            m_poseMatchCoeff = coeff;
            getSoftBody()->m_cfg.kMT = m_poseMatchCoeff;
        }
    }

    void PhysicSoftBody::setVelocityFactor(float coeff)
    {
        if (m_bIsDirty || m_velocityFactor != coeff)
        {
            m_velocityFactor = coeff;
            getSoftBody()->m_cfg.kVCF = m_velocityFactor;
        }
    }

    void PhysicSoftBody::setRepulsionStiffness(float repulsionStiffness)
    {
        if (m_bIsDirty || m_repulsionStiffness != repulsionStiffness)
        {
            m_repulsionStiffness = repulsionStiffness;
            getSoftBody()->setSpringStiffness(m_repulsionStiffness);
        }
    }

    void PhysicSoftBody::setSleepingThreshold(float threshold)
    {
        if (m_bIsDirty || m_sleepingThreshold != threshold)
        {
            m_sleepingThreshold = threshold;
            getSoftBody()->m_sleepingThreshold = m_sleepingThreshold;
        }
    }

    void PhysicSoftBody::setRestLengthScale(float scale)
    {
        if (m_bIsDirty || m_restLengthScale != scale)
        {
            m_restLengthScale = scale;
            getSoftBody()->setRestLengthScale(m_restLengthScale);
        }
    }

    void PhysicSoftBody::setGravityFactor(float factor)
    {
        if (m_bIsDirty || m_gravityFactor != factor)
        {
            m_gravityFactor = factor;
            getSoftBody()->setGravityFactor(m_gravityFactor);
        }
    }

    void PhysicSoftBody::setRigidContactHardness(float factor)
    {
        if (m_bIsDirty || m_rigidContactHardness != factor)
        {
            m_rigidContactHardness = factor;
            getSoftBody()->m_cfg.kCHR = m_rigidContactHardness;
        }
    }

    void PhysicSoftBody::setKineticContactHardness(float factor)
    {
        if (m_bIsDirty || m_kineticContactHardness != factor)
        {
            m_kineticContactHardness = factor;
            getSoftBody()->m_cfg.kKHR = m_kineticContactHardness;
        }
    }

    void PhysicSoftBody::setSoftContactHardness(float factor)
    {
        if (m_bIsDirty || m_softContactHardness != factor)
        {
            m_softContactHardness = factor;
            getSoftBody()->m_cfg.kSHR = m_softContactHardness;
        }
    }

    void PhysicSoftBody::setAnchorHardness(float factor)
    {
        if (m_bIsDirty || m_anchorHardness != factor)
        {
            m_anchorHardness = factor;
            getSoftBody()->m_cfg.kAHR = m_anchorHardness;
        }
    }

    void PhysicSoftBody::setPosIterationNumber(int number)
    {
        if (m_bIsDirty || m_positionIterNumber != number)
        {
            m_positionIterNumber = number;
            getSoftBody()->m_cfg.piterations = m_positionIterNumber;
        }
    }

    void PhysicSoftBody::setAeroModel(int model)
    {
        if (model < btSoftBody::eAeroModel::V_Point || model > btSoftBody::eAeroModel::F_OneSided)
        {
            model = btSoftBody::eAeroModel::V_TwoSided;
        }

        if (m_bIsDirty || m_aeroModel != model)
        {
            m_aeroModel = model;
            getSoftBody()->m_cfg.aeromodel = (btSoftBody::eAeroModel::_)m_aeroModel;
        }
    }

    void PhysicSoftBody::setSelfCollision(bool selfCollision)
    {
        if (m_bIsDirty || m_bUseSelfCollision != selfCollision)
        {
            m_bUseSelfCollision = selfCollision;
            getSoftBody()->setSelfCollision(m_bUseSelfCollision);
        }
    }

    void PhysicSoftBody::setSoftSoftCollision(bool soft)
    {
        if (m_bIsDirty || m_softSoftCollision != soft)
        {
            m_softSoftCollision = soft;
            getSoftBody()->m_softSoftCollision = m_softSoftCollision;
        }
    }

    void PhysicSoftBody::setWindVelocity(const btVector3 &velocity)
    {
        if (m_bIsDirty || m_windVelocity != velocity)
        {
            m_windVelocity = velocity;
            getSoftBody()->setWindVelocity(m_windVelocity);
        }
    }

    //! Create physic body
    void PhysicSoftBody::createBody()
    {
        // Create collision shape
        if (m_body)
            m_body.reset();

        auto world = getManager()->getDeformableWorld();

        Figure *figure = nullptr;
        std::vector<Vec3> positions;
        auto figureComp = getOwner()->getComponent<FigureComponent>();
        if (figureComp && figureComp->getFigure())
            figure = figureComp->getFigure();
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

                auto indices = new int[mesh->numIndices];
                for (uint32_t i = 0; i < mesh->numIndices; ++i)
                    indices[i] = (int)mesh->indices[i];

                float *optPoss;
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
        m_bIsDirty = true;

        // Set this to SoftBody
        setSoftBody(true);
        getSoftBody()->generateBendingConstraints(2);

        // Apply pre-configurated values to PhysicObject
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
        setDampingCoeff(m_dampingCoeff);
        setPressureCoeff(m_pressureCoeff);
        setVolumeConvCoeff(m_volumeConvCoeff);
        setDynamicFrictionCoeff(m_dynamicFrictionCoeff);
        setPoseMatchCoeff(m_poseMatchCoeff);
        setRepulsionStiffness(m_repulsionStiffness);
        setSleepingThreshold(m_sleepingThreshold);
        setRestLengthScale(m_restLengthScale);
        setGravityFactor(m_gravityFactor);
        setVelocityFactor(m_gravityFactor);
        setSelfCollision(m_bUseSelfCollision);
        setSoftSoftCollision(m_softSoftCollision);
        setWindVelocity(m_windVelocity);
        setAeroModel(m_aeroModel);
        setPosIterationNumber(m_positionIterNumber);
        setRigidContactHardness(m_rigidContactHardness);
        setKineticContactHardness(m_kineticContactHardness);
        setSoftContactHardness(m_softContactHardness);
        setAnchorHardness(m_anchorHardness);

        // Apply collision filter group and mask
        setCollisionFilterGroup(m_collisionFilterGroup);
        setCollisionFilterMask(m_collisionFilterMask);

        // Continuos detection mode
        setCCD(m_bIsCCD);

        // Add custom material callback for collision events
        addCollisionFlag(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        m_bIsDirty = false;

        // Activate
        if (m_bIsEnabled)
            activate();
    }

    //! Set local scale
    void PhysicSoftBody::setLocalScale(const Vec3 &scale)
    {
        if (m_bIsDirty || m_previousScale != scale)
        {
            auto dScale = PhysicHelper::to_btVector3(scale) / PhysicHelper::to_btVector3(m_previousScale);
            getSoftBody()->scale({std::abs(dScale[0]), std::abs(dScale[1]), std::abs(dScale[2])});
            m_previousScale = scale;
        }
    }

    //! Get nearest node
    int PhysicSoftBody::getNearestNodeIndex(const btVector3 &pos)
    {
        int nearestNo = -1;
        float dist = FLT_MAX;
        btVector3 nearestPos(0, 0, 0);

        int nNodes = getSoftBody()->m_nodes.size();
        for (int i = 0; i < nNodes; i++)
        {
            float l = pos.distance2(getSoftBody()->m_nodes[i].m_x);
            if (dist > l)
            {
                dist = l;
                nearestNo = i;
                nearestPos = getSoftBody()->m_nodes[i].m_x;
            }
        }
        return nearestNo;
    }

    //! Get node position
    btVector3 PhysicSoftBody::getNodePosition(int idx)
    {
        if (idx < 0 || idx >= getSoftBody()->m_nodes.size())
            return btVector3(FLT_MAX, FLT_MAX, FLT_MAX);
        return getSoftBody()->m_nodes[idx].m_x;
    }

    //! Get node normal
    btVector3 PhysicSoftBody::getNodeNormal(int idx)
    {
        if (idx < 0 || idx >= getSoftBody()->m_nodes.size())
            return btVector3(FLT_MAX, FLT_MAX, FLT_MAX);
        return getSoftBody()->m_nodes[idx].m_n;
    }

    //! Update Bullet transform
    void PhysicSoftBody::updateBtTransform()
    {
        if (!SceneManager::getInstance()->isEditor())
            return;

        auto newTrans = PhysicHelper::to_btTransform(getOwner()->getTransform()->getWorldRotation(), getOwner()->getTransform()->getWorldPosition());
        getSoftBody()->transformTo(newTrans);

        Vec3 scale = getOwner()->getTransform()->getWorldScale();
        Vec3 dScale = {scale[0] - m_previousScale[0], scale[1] - m_previousScale[1], scale[2] - m_previousScale[2]};
        float scaleDelta = vmath_lengthSqr(dScale.P(), 3);
        if (scaleDelta >= 0.01f)
        {
            setLocalScale(scale);
        }
    }

    //! Update IGE transform
    void PhysicSoftBody::updateIgeTransform()
    {
        // Update transform
        if (getOwner()->getTransform())
        {
            getOwner()->getTransform()->setPosition(PhysicHelper::from_btVector3(getSoftBody()->m_pose.m_com));

            // Update rotation
            auto xform = btTransform(getSoftBody()->m_pose.m_rot * getSoftBody()->m_pose.m_scl);
            getOwner()->getTransform()->setRotation(PhysicHelper::from_btQuaternion(xform.getRotation()));

            getOwner()->getTransform()->onUpdate(0.f);
        }

        // Update position and normal
        auto figureComp = getOwner()->getComponent<FigureComponent>();
        if (!figureComp || !figureComp->getFigure())
            return;

        int offset = 0;

        auto figure = figureComp->getFigure();
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
            int elemSize = 0;
            switch (mesh->vertexAttributes[attIdx].type)
            {
            case GL_FLOAT:
                elemSize = 4;
                break;
            case GL_SHORT:
                elemSize = 2;
                break;
            case GL_HALF_FLOAT:
                elemSize = 2;
                break;
            case GL_UNSIGNED_BYTE:
                elemSize = 1;
                break;
            }

            const auto &nodes = getSoftBody()->m_nodes;
            for (int i = 0; i < mesh->numVerticies; ++i)
            {
                int idx = m_indicesMap[i];
                auto *buffer = (uint8_t *)PYXIE_MALLOC(3 * elemSize);
                MemoryCleaner cleaner(buffer);

                for (int j = 0; j < 3; ++j)
                {
                    switch (mesh->vertexAttributes[attIdx].type)
                    {
                    case GL_FLOAT:
                        ((float *)buffer)[j] = nodes[idx].m_x[j];
                        break;
                    case GL_SHORT:
                        ((int16_t *)buffer)[j] = F32toS16(nodes[idx].m_x[j]);
                        break;
                    case GL_HALF_FLOAT:
                        ((uint16_t *)buffer)[j] = F32toF16(nodes[idx].m_x[j]);
                        break;
                    case GL_UNSIGNED_BYTE:
                        ((uint8_t *)buffer)[j] = F32toU8(nodes[idx].m_x[j]);
                        break;
                    }
                }
                uint8_t *location = ((uint8_t *)mesh->vertices) + (i + offset) * mesh->vertexFormatSize + mesh->vertexAttributes[attIdx].offset;
                memcpy(location, buffer, elemSize * mesh->vertexAttributes[attIdx].size);
            }
            figure->ResetMeshBuffer(m_meshIndex, true, false, true);
        }

        attIdx = -1;
        for (uint16_t i = 0; i < mesh->numVertexAttributes; ++i)
        {
            if (mesh->vertexAttributes[i].id == AttributeID::ATTRIBUTE_ID_NORMAL)
            {
                attIdx = i;
                break;
            }
        }
        if (attIdx != -1)
        {
            int elemSize = 0;
            switch (mesh->vertexAttributes[attIdx].type)
            {
            case GL_FLOAT:
                elemSize = 4;
                break;
            case GL_SHORT:
                elemSize = 2;
                break;
            case GL_HALF_FLOAT:
                elemSize = 2;
                break;
            case GL_UNSIGNED_BYTE:
                elemSize = 1;
                break;
            }

            const auto &nodes = getSoftBody()->m_nodes;
            for (int i = 0; i < mesh->numVerticies; ++i)
            {
                int idx = m_indicesMap[i];
                auto *buffer = (uint8_t *)PYXIE_MALLOC(3 * elemSize);
                MemoryCleaner cleaner(buffer);

                for (int j = 0; j < 3; ++j)
                {
                    switch (mesh->vertexAttributes[attIdx].type)
                    {
                    case GL_FLOAT:
                        ((float *)buffer)[j] = nodes[idx].m_n[j];
                        break;
                    case GL_SHORT:
                        ((int16_t *)buffer)[j] = F32toS16(nodes[idx].m_n[j]);
                        break;
                    case GL_HALF_FLOAT:
                        ((uint16_t *)buffer)[j] = F32toF16(nodes[idx].m_n[j]);
                        break;
                    case GL_UNSIGNED_BYTE:
                        ((uint8_t *)buffer)[j] = F32toU8(nodes[idx].m_n[j]);
                        break;
                    }
                }
                uint8_t *location = ((uint8_t *)mesh->vertices) + (i + offset) * mesh->vertexFormatSize + mesh->vertexAttributes[attIdx].offset;
                memcpy(location, buffer, elemSize * mesh->vertexAttributes[attIdx].size);
            }
            figure->ResetMeshBuffer(m_meshIndex, true, false, true);
        }
    }

    //! Optimize mesh
    void PhysicSoftBody::optimizeMesh(const std::vector<Vec3> &orgPoss, int *indices, int numIndeces, float *&optPoss)
    {
        if (m_indicesMap != nullptr)
            delete[] m_indicesMap;
        m_indicesMap = new int[orgPoss.size()];

        std::map<Vec3, int> vecList;
        int numOptVertex = 0;
        for (int i = 0; i < numIndeces; i++)
        {
            int idx = indices[i];
            const Vec3 &v = orgPoss[idx];
            if (vecList.count(v) == 0)
            {
                vecList[v] = numOptVertex;
                numOptVertex++;
            }
            m_indicesMap[idx] = indices[i] = vecList[v];
        }
        std::map<uint32_t, Vec3> vecListRV;
        for (const auto &it : vecList)
            vecListRV[it.second] = it.first;

        optPoss = new float[vecListRV.size() * 3];
        int numf = 0;
        for (const auto &it : vecListRV)
        {
            for (int i = 0; i < 3; i++)
            {
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
        PhysicObject::to_json(j);
        j["meshIdx"] = getMeshIndex();
        j["dampCoeff"] = getDampingCoeff();
        j["presCoeff"] = getPressureCoeff();
        j["volCoeff"] = getVolumeConvCoeff();
        j["friCoeff"] = getDynamicFrictionCoeff();
        j["poseCoeff"] = getPoseMatchCoeff();
        j["repStiff"] = getRepulsionStiffness();
        j["sleepThr"] = getSleepingThreshold();
        j["restLS"] = getRestLengthScale();
        j["graF"] = getGravityFactor();
        j["velF"] = getVelocityFactor();
        j["pItrNum"] = getPosIterationNumber();
        j["aero"] = getAeroModel();
        j["isSelfCol"] = isSelfCollision();
        j["isSoftCol"] = isSoftSoftCollision();
        j["windVel"] = PhysicHelper::from_btVector3(getWindVelocity());
        j["rch"] = getRigidContactHardness();
        j["kch"] = getKineticContactHardness();
        j["sch"] = getSoftContactHardness();
        j["ahr"] = getAnchorHardness();
    }

    //! Deserialize
    void PhysicSoftBody::from_json(const json &j)
    {
        setMeshIndex(j.value("meshIdx", 0));
        setDampingCoeff(j.value("dampCoeff", 0.4f));
        setPressureCoeff(j.value("presCoeff", 0.f));
        setVolumeConvCoeff(j.value("volCoeff", 0.f));
        setDynamicFrictionCoeff(j.value("friCoeff", 0.2f));
        setPoseMatchCoeff(j.value("poseCoeff", 0.f));
        setRepulsionStiffness(j.value("repStiff", 0.5f));
        setSleepingThreshold(j.value("sleepThr", 0.f));
        setRestLengthScale(j.value("restLS", 0.f));
        setGravityFactor(j.value("graF", 1.f));
        setVelocityFactor(j.value("velF", 1.f));
        setPosIterationNumber(j.value("pItrNum", 1));
        setAeroModel(j.value("aero", 1));
        setSelfCollision(j.value("isSelfCol", false));
        setSoftSoftCollision(j.value("isSoftCol", false));
        setWindVelocity(PhysicHelper::to_btVector3(j.value("windVel", Vec3())));
        setRigidContactHardness(j.value("rch", 1.f));
        setKineticContactHardness(j.value("kch", 0.1f));
        setSoftContactHardness(j.value("sch", 1.f));
        setAnchorHardness(j.value("ahr", 1.f));
        PhysicObject::from_json(j);
    }

    //! Update property by key value
    void PhysicSoftBody::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("meshIdx") == 0)
            setMeshIndex(val);
        else if (key.compare("dampCoeff") == 0)
            setDampingCoeff(val);
        else if (key.compare("presCoeff") == 0)
            setPressureCoeff(val);
        else if (key.compare("volCoeff") == 0)
            setVolumeConvCoeff(val);
        else if (key.compare("friCoeff") == 0)
            setDynamicFrictionCoeff(val);
        else if (key.compare("poseCoeff") == 0)
            setPoseMatchCoeff(val);
        else if (key.compare("repStiff") == 0)
            setRepulsionStiffness(val);
        else if (key.compare("sleepThr") == 0)
            setSleepingThreshold(val);
        else if (key.compare("restLS") == 0)
            setRestLengthScale(val);
        else if (key.compare("graF") == 0)
            setGravityFactor(val);
        else if (key.compare("velF") == 0)
            setVelocityFactor(val);
        else if (key.compare("pItrNum") == 0)
            setPosIterationNumber(val);
        else if (key.compare("aero") == 0)
            setAeroModel(val);
        else if (key.compare("isSelfCol") == 0)
            setSelfCollision(val);
        else if (key.compare("isSoftCol") == 0)
            setSoftSoftCollision(val);
        else if (key.compare("windVel") == 0)
            setWindVelocity(PhysicHelper::to_btVector3(val));
        else if (key.compare("rch") == 0)
            setRigidContactHardness(val);
        else if (key.compare("kch") == 0)
            setKineticContactHardness(val);
        else if (key.compare("sch") == 0)
            setSoftContactHardness(val);
        else if (key.compare("ahr") == 0)
            setAnchorHardness(val);
        else
            PhysicObject::setProperty(key, val);
    }

} // namespace ige::scene
