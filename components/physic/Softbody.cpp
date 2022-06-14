#include "components/physic/Softbody.h"
#include "components/physic/PhysicManager.h"
#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "scene/SceneObject.h"
#include "scene/SceneManager.h"

#include "utils/PhysicHelper.h"
#include "components/physic/PhysicConstraint.h"
#include "components/physic/constraint/FixedConstraint.h"
#include "components/physic/constraint/HingeConstraint.h"
#include "components/physic/constraint/SliderConstraint.h"
#include "components/physic/constraint/SpringConstraint.h"
#include "components/physic/constraint/Dof6SpringConstraint.h"

#include <BulletSoftBody/btSoftBodyHelpers.h>

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Constructor
    Softbody::Softbody(SceneObject& owner)
        : Rigidbody(owner)
    {
        m_mass = -1.f;
    }

    //! Destructor
    Softbody::~Softbody()
    {
        if (m_indicesMap != nullptr)
            delete[] m_indicesMap;
        m_indicesMap = nullptr;
    }

    //! Get AABB
    AABBox Softbody::getAABB()
    {
        if (!getSoftBody()) return {};
        btVector3 aabbMin, aabbMax;
        getSoftBody()->getAabb(aabbMin, aabbMax);
        auto box = AABBox(PhysicHelper::from_btVector3(aabbMin), PhysicHelper::from_btVector3(aabbMax));
        box = box.Transform(getOwner()->getTransform()->getWorldMatrix().Inverse());
        return box;
    }

    //! Set mesh index
    void Softbody::setMeshIndex(int idx)
    {
        if (m_bIsDirty || m_meshIndex != idx)
        {
            m_meshIndex = idx;
            recreateBody();
        }
    }

    //! Linear velocity
    void Softbody::setLinearVelocity(const btVector3 &velocity)
    {
        if (m_bIsDirty || m_linearVelocity != velocity)
        {
            m_linearVelocity = velocity;
            getSoftBody()->setLinearVelocity(m_linearVelocity);
        }
    }

    //! Angular velocity
    void Softbody::setAngularVelocity(const btVector3 &velocity)
    {
        if (m_bIsDirty || m_angularVelocity != velocity)
        {
            m_angularVelocity = velocity;
            getSoftBody()->setAngularVelocity(m_angularVelocity);
        }
    }

    void Softbody::setDampingCoeff(float coeff)
    {
        if (m_bIsDirty || m_dampingCoeff != coeff)
        {
            m_dampingCoeff = coeff;
            getSoftBody()->setDampingCoefficient(m_dampingCoeff);
        }
    }

    void Softbody::setPressureCoeff(float coeff)
    {
        if (m_bIsDirty || m_pressureCoeff != coeff)
        {
            m_pressureCoeff = coeff;
            getSoftBody()->m_cfg.kPR = m_pressureCoeff;
        }
    }

    void Softbody::setVolumeConvCoeff(float coeff)
    {
        if (m_bIsDirty || m_volumeConvCoeff != coeff)
        {
            m_volumeConvCoeff = coeff;
            getSoftBody()->m_cfg.kVC = m_volumeConvCoeff;
        }
    }

    void Softbody::setDynamicFrictionCoeff(float coeff)
    {
        if (m_bIsDirty || m_dynamicFrictionCoeff != coeff)
        {
            m_dynamicFrictionCoeff = coeff;
            getSoftBody()->m_cfg.kDF = m_dynamicFrictionCoeff;
        }
    }

    void Softbody::setPoseMatchCoeff(float coeff)
    {
        if (m_bIsDirty || m_poseMatchCoeff != coeff)
        {
            m_poseMatchCoeff = coeff;
            getSoftBody()->m_cfg.kMT = m_poseMatchCoeff;
        }
    }

    void Softbody::setVelocityFactor(float coeff)
    {
        if (m_bIsDirty || m_velocityFactor != coeff)
        {
            m_velocityFactor = coeff;
            getSoftBody()->m_cfg.kVCF = m_velocityFactor;
        }
    }

    void Softbody::setRepulsionStiffness(float repulsionStiffness)
    {
        if (m_bIsDirty || m_repulsionStiffness != repulsionStiffness)
        {
            m_repulsionStiffness = repulsionStiffness;
            getSoftBody()->setSpringStiffness(m_repulsionStiffness);
        }
    }

    void Softbody::setSleepingThreshold(float threshold)
    {
        if (m_bIsDirty || m_sleepingThreshold != threshold)
        {
            m_sleepingThreshold = threshold;
            getSoftBody()->m_sleepingThreshold = m_sleepingThreshold;
        }
    }

    void Softbody::setRestLengthScale(float scale)
    {
        if (m_bIsDirty || m_restLengthScale != scale)
        {
            m_restLengthScale = scale;
            getSoftBody()->setRestLengthScale(m_restLengthScale);
        }
    }

    void Softbody::setGravityFactor(float factor)
    {
        if (m_bIsDirty || m_gravityFactor != factor)
        {
            m_gravityFactor = factor;
            getSoftBody()->setGravityFactor(m_gravityFactor);
        }
    }

    void Softbody::setRigidContactHardness(float factor)
    {
        if (m_bIsDirty || m_rigidContactHardness != factor)
        {
            m_rigidContactHardness = factor;
            getSoftBody()->m_cfg.kCHR = m_rigidContactHardness;
        }
    }

    void Softbody::setKineticContactHardness(float factor)
    {
        if (m_bIsDirty || m_kineticContactHardness != factor)
        {
            m_kineticContactHardness = factor;
            getSoftBody()->m_cfg.kKHR = m_kineticContactHardness;
        }
    }

    void Softbody::setSoftContactHardness(float factor)
    {
        if (m_bIsDirty || m_softContactHardness != factor)
        {
            m_softContactHardness = factor;
            getSoftBody()->m_cfg.kSHR = m_softContactHardness;
        }
    }

    void Softbody::setAnchorHardness(float factor)
    {
        if (m_bIsDirty || m_anchorHardness != factor)
        {
            m_anchorHardness = factor;
            getSoftBody()->m_cfg.kAHR = m_anchorHardness;
        }
    }

    void Softbody::setPosIterationNumber(int number)
    {
        if (m_bIsDirty || m_positionIterNumber != number)
        {
            m_positionIterNumber = number;
            getSoftBody()->m_cfg.piterations = m_positionIterNumber;
        }
    }

    void Softbody::setAeroModel(int model)
    {
        if (model < btSoftBody::eAeroModel::V_Point || model > btSoftBody::eAeroModel::F_OneSided)
        {
            model = btSoftBody::eAeroModel::V_Point;
        }

        if (m_bIsDirty || m_aeroModel != model)
        {
            m_aeroModel = model;
            getSoftBody()->m_cfg.aeromodel = (btSoftBody::eAeroModel::_)m_aeroModel;
        }
    }

    void Softbody::setSelfCollision(bool selfCollision)
    {
        if (m_bIsDirty || m_bUseSelfCollision != selfCollision)
        {
            m_bUseSelfCollision = selfCollision;
            getSoftBody()->setSelfCollision(m_bUseSelfCollision);
        }
    }

    void Softbody::setSoftSoftCollision(bool soft)
    {
        if (m_bIsDirty || m_softSoftCollision != soft)
        {
            m_softSoftCollision = soft;
            getSoftBody()->m_softSoftCollision = m_softSoftCollision;
        }
    }

    void Softbody::setWindVelocity(const btVector3 &velocity)
    {
        if (m_bIsDirty || m_windVelocity != velocity)
        {
            m_windVelocity = velocity;
            getSoftBody()->setWindVelocity(m_windVelocity);
        }
    }

    //! Create physic body
    void Softbody::createBody()
    {
        destroyBody();
        auto world = getManager()->getDeformableWorld();
        Figure *figure = nullptr;
        std::vector<Vec3> positions;
        auto figureComp = getOwner()->getComponent<FigureComponent>();
        if (figureComp && figureComp->getFigure()) {
            figure = figureComp->getFigure();
        }

        if (figure != nullptr)
        {
            figure->WaitInitialize();
            if (figure->NumMeshes() > 0 && m_meshIndex >= 0 && m_meshIndex < figure->NumMeshes())
            {
                int offset = 0;
                int size = 100000000;
                int space = Space::WorldSpace;

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

        // Apply pre-configurated values to Rigidbody
        setMass(m_mass);
        setFriction(m_friction);
        setRestitution(m_restitution);
        setLinearVelocity(m_linearVelocity);
        setAngularVelocity(m_angularVelocity);
        setLinearFactor(m_linearFactor);
        setAngularFactor(m_angularFactor);

        // Recalculate scale
        auto scale = m_previousScale;
        m_previousScale = Vec3(1.f, 1.f, 1.f);
        setScale(scale);

        // Apply pre-configurated values
        setSelfCollision(m_bUseSelfCollision);
        setSoftSoftCollision(m_softSoftCollision);

        setRepulsionStiffness(m_repulsionStiffness);
        setRestLengthScale(m_restLengthScale);
        setPosIterationNumber(m_positionIterNumber);
        setSleepingThreshold(m_sleepingThreshold);
        setGravityFactor(m_gravityFactor);
        setVelocityFactor(m_gravityFactor);

        setDampingCoeff(m_dampingCoeff);
        setPressureCoeff(m_pressureCoeff);
        setVolumeConvCoeff(m_volumeConvCoeff);
        setDynamicFrictionCoeff(m_dynamicFrictionCoeff);
        setPoseMatchCoeff(m_poseMatchCoeff);
        setRigidContactHardness(m_rigidContactHardness);
        setKineticContactHardness(m_kineticContactHardness);
        setSoftContactHardness(m_softContactHardness);
        setAnchorHardness(m_anchorHardness);

        setAeroModel(m_aeroModel);
        setWindVelocity(m_windVelocity);

        // Apply collision filter group and mask
        setCollisionFilterGroup(m_collisionFilterGroup);
        setCollisionFilterMask(m_collisionFilterMask);

        // Continuos detection mode
        setCCD(m_bIsCCD);

        // Add custom material callback for collision events
        addCollisionFlag(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        // Update transform
        updateBtTransform();

        m_bIsDirty = false;

        // Activate
        if (isEnabled())
            activate();
    }

    //! Set local scale
    void Softbody::setScale(const Vec3 &scale)
    {
        if (m_bIsDirty || m_previousScale != scale)
        {
            auto dScale = PhysicHelper::to_btVector3(scale) / PhysicHelper::to_btVector3(m_previousScale);
            getSoftBody()->scale({std::abs(dScale[0]), std::abs(dScale[1]), std::abs(dScale[2])});
            m_previousScale = scale;
        }
    }

    //! Get nearest node
    int Softbody::getNearestNodeIndex(const btVector3 &pos)
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
    btVector3 Softbody::getNodePosition(int idx)
    {
        if (idx < 0 || idx >= getSoftBody()->m_nodes.size())
            return btVector3(FLT_MAX, FLT_MAX, FLT_MAX);
        return getSoftBody()->m_nodes[idx].m_x;
    }

    //! Get node normal
    btVector3 Softbody::getNodeNormal(int idx)
    {
        if (idx < 0 || idx >= getSoftBody()->m_nodes.size())
            return btVector3(FLT_MAX, FLT_MAX, FLT_MAX);
        return getSoftBody()->m_nodes[idx].m_n;
    }

    //! Update IGE transform
    void Softbody::updateIgeTransform()
    {
        // Update transform
        auto transform = getOwner()->getTransform();
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
                auto pos = transform->getWorldMatrix().Inverse() * (PhysicHelper::from_btVector3(nodes[idx].m_x));
                for (int j = 0; j < 3; ++j)
                {
                    switch (mesh->vertexAttributes[attIdx].type)
                    {
                    case GL_FLOAT:
                        ((float *)buffer)[j] = pos[j];
                        break;
                    case GL_SHORT:
                        ((int16_t *)buffer)[j] = F32toS16(pos[j]);
                        break;
                    case GL_HALF_FLOAT:
                        ((uint16_t *)buffer)[j] = F32toF16(pos[j]);
                        break;
                    case GL_UNSIGNED_BYTE:
                        ((uint8_t *)buffer)[j] = F32toU8(pos[j]);
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
    void Softbody::optimizeMesh(const std::vector<Vec3> &orgPoss, int *indices, int numIndeces, float *&optPoss)
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
    void Softbody::to_json(json &j) const
    {
        Rigidbody::to_json(j);
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

    //! Serialize finished event
    void Softbody::onSerializeFinished(Scene* scene) {
        init();
        auto j = m_json;
        setMass(j.value("mass", 1.f));
        setRestitution(j.value("restitution", 1.f));
        setFriction(j.value("friction", 0.5f));
        setLinearVelocity(PhysicHelper::to_btVector3(j.value("linearVelocity", Vec3())));
        setAngularVelocity(PhysicHelper::to_btVector3(j.value("angularVelocity", Vec3())));
        setLinearFactor(PhysicHelper::to_btVector3(j.value("linearFactor", Vec3(1.f, 1.f, 1.f))));
        setAngularFactor(PhysicHelper::to_btVector3(j.value("angularFactor", Vec3(1.f, 1.f, 1.f))));
        setIsKinematic(j.value("isKinematic", false));
        setIsTrigger(j.value("isTrigger", false));
        setCollisionFilterGroup(j.value("group", isKinematic() ? 2 : 1));
        setCollisionFilterMask(j.value("mask", isKinematic() ? 3 : -1));
        setCCD(j.value("ccd", false));
        setLinearSleepingThreshold(j.value("linearSleepingThreshold", 0.8f));
        setAngularSleepingThreshold(j.value("angularSleepingThreshold", 1.0f));
        setActivationState(j.value("activeState", 1));
        setPositionOffset(j.value("offset", Vec3(0.f, 0.f, 0.f)));

        auto jConstraints = j.value("consts", json());
        for (auto it : jConstraints)
        {
            auto key = (int)it.at(0);
            auto val = it.at(1);
            std::shared_ptr<PhysicConstraint> constraint = nullptr;
            switch (key)
            {
            case (int)PhysicConstraint::ConstraintType::Fixed:
                constraint = addConstraint<FixedConstraint>();
                break;
            case (int)PhysicConstraint::ConstraintType::Hinge:
                constraint = addConstraint<HingeConstraint>();
                break;
            case (int)PhysicConstraint::ConstraintType::Slider:
                constraint = addConstraint<SliderConstraint>();
                break;
            case (int)PhysicConstraint::ConstraintType::Spring:
                constraint = addConstraint<SpringConstraint>();
                break;
            case (int)PhysicConstraint::ConstraintType::Dof6Spring:
                constraint = addConstraint<Dof6SpringConstraint>();
                break;
            }
            if (constraint)
                val.get_to(*constraint);
        }

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
        m_json.clear();
    }

    //! Update property by key value
    void Softbody::setProperty(const std::string& key, const json& val)
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
            Rigidbody::setProperty(key, val);
    }

} // namespace ige::scene
