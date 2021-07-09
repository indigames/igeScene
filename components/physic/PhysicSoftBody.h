#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftBody.h>

#include "event/Event.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/PhysicObject.h"

namespace ige::scene
{
    //! PhysicSoftBody
    class PhysicSoftBody : public PhysicObject
    {
    public:
        //! Constructor
        PhysicSoftBody(SceneObject& owner);

        //! Destructor
        virtual ~PhysicSoftBody();

        // Inherited via Component
        virtual std::string getName() const override { return "PhysicSoftBody"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::PhysicSoftBody; }

        //! Mesh Index
        int getMeshIndex() const { return m_meshIndex; };
        void setMeshIndex(int idx);

        //! Mass
        virtual void setMass(float mass) override {
            m_mass = mass;
            if(m_mass > 0.f)
                getSoftBody()->setTotalMass(m_mass);
        }

        //! Indicate object is a kinematic object
        virtual bool isKinematic() const override { return false; }
        virtual void setIsKinematic(bool isKinematic = true) override {};

        //! Linear velocity
        virtual void setLinearVelocity(const btVector3& velocity) override;

        //! Angular velocity
        virtual void setAngularVelocity(const btVector3& velocity) override;

        //! Get AABB
        virtual AABBox getAABB() override;

        //! Damping coefficient
        virtual float getDampingCoeff() const { return m_dampingCoeff; }
        virtual void setDampingCoeff(float coeff);

        //! Pressure coefficient
        virtual float getPressureCoeff() const { return m_pressureCoeff; }
        virtual void setPressureCoeff(float coeff);

        //! Volume conversation coefficient
        virtual float getVolumeConvCoeff() const { return m_volumeConvCoeff; }
        virtual void setVolumeConvCoeff(float coeff);

        //! Dynamic friction coefficient
        virtual float getDynamicFrictionCoeff() const { return m_dynamicFrictionCoeff; }
        virtual void setDynamicFrictionCoeff(float coeff);

        //! Pose matching coefficient
        virtual float getPoseMatchCoeff() const { return m_poseMatchCoeff; }
        virtual void setPoseMatchCoeff(float coeff);

        //! Repulsion Stiffness
        virtual float getRepulsionStiffness() const { return m_repulsionStiffness; }
        virtual void setRepulsionStiffness(float repulsionStiffness);

        //! Sleeping Threshold
        virtual float getSleepingThreshold() const { return m_sleepingThreshold; }
        virtual void setSleepingThreshold(float threshold);

        //! Rest Length Scale
        virtual float getRestLengthScale() const { return m_restLengthScale; }
        virtual void setRestLengthScale(float scale);

        //! Gravity factor
        virtual float getGravityFactor() const { return m_gravityFactor; }
        virtual void setGravityFactor(float factor);

        //! Velocities correction factor
        virtual float getVelocityFactor() const { return m_velocityFactor; }
        virtual void setVelocityFactor(float coeff);

        //! Rigid contacts hardness
        virtual float getRigidContactHardness() const { return m_rigidContactHardness; }
        virtual void setRigidContactHardness(float factor);

        //! Kinetic contacts hardness
        virtual float getKineticContactHardness() const { return m_kineticContactHardness; }
        virtual void setKineticContactHardness(float factor);

        //! Soft contacts hardness
        virtual float getSoftContactHardness() const { return m_softContactHardness; }
        virtual void setSoftContactHardness(float factor);

        //! Anchor hardness
        virtual float getAnchorHardness() const { return m_anchorHardness; }
        virtual void setAnchorHardness(float factor);

        //! Position interation number
        virtual int getPosIterationNumber() const { return m_positionIterNumber; }
        virtual void setPosIterationNumber(int number);

        //! Aero Model
        virtual int getAeroModel() const { return m_aeroModel; }
        virtual void setAeroModel(int model);

        //! Self collision
        virtual bool isSelfCollision() const { return m_bUseSelfCollision; }
        virtual void setSelfCollision(bool selfCollision = true);

        //! Soft soft-collision
        virtual bool isSoftSoftCollision() const { return m_softSoftCollision; }
        virtual void setSoftSoftCollision(bool soft = true);

        //! Wind velocity
        virtual const btVector3& getWindVelocity() const { return m_windVelocity; }
        virtual void setWindVelocity(const btVector3& velocity);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    public:
        //! Update Bullet transform
        virtual void updateBtTransform() override;

        //! Update IGE transform
        virtual void updateIgeTransform() override;

        //! Apply force
        virtual void applyForce(const btVector3& force) override { getSoftBody()->addForce(force); }
        virtual void applyForce(const btVector3& force, const btVector3& pos) override { getSoftBody()->addForce(force); }

        //! Apply impulse
        virtual void applyImpulse(const btVector3& impulse) override { addVelocity(impulse); }
        virtual void applyImpulse(const btVector3& impulse, const btVector3& pos) override { addVelocity(impulse); }

        //! Apply Repulsion Force
        virtual void applyRepulsionForce(float timeStep, bool applySpringForce) { getSoftBody()->applyRepulsionForce(timeStep, applySpringForce); }

        //! Add Velocity
        virtual void addVelocity(const btVector3& velocity, int nodeIdx = -1)
        {
            if (nodeIdx >= 0) {
                getSoftBody()->addVelocity(velocity, nodeIdx);
            }
            else {
                getSoftBody()->addVelocity(velocity);
            }
        }

        //! Get Volume
        virtual float getVolume() const { return getSoftBody()->getVolume(); }

        //! Get Center of mass
        virtual btVector3 getCenterOfMass() const { return getSoftBody()->getCenterOfMass(); }

        //! Append deformable anchor
        virtual void appendDeformableAnchor(int node, btRigidBody* body) {
            getSoftBody()->appendDeformableAnchor(node, body);
        }

        //! Append anchor
        virtual void appendAnchor(int node, btRigidBody* body, bool disableLinkedCollission = false, btScalar influence = 1) {
            getSoftBody()->appendAnchor(node, body, disableLinkedCollission, influence);
        }

        //! Remove anchor
        virtual void removeAnchor(int node) {
            getSoftBody()->removeAnchor(node);
        }

        //! Get nearest node index
        virtual int getNearestNodeIndex(const btVector3& pos);

        //! Get node position
        virtual btVector3 getNodePosition(int idx);

        //! Get node normal
        virtual btVector3 getNodeNormal(int idx);

    protected:
        //! Serialize
        virtual void to_json(json &j) const override;

        //! Deserialize
        virtual void from_json(const json &j) override;

        //! Create physic body
        virtual void createBody() override;

        //! Set local scale
        virtual void setLocalScale(const Vec3& scale) override;

        //! Optimize mesh
        void optimizeMesh(const std::vector<Vec3>&, int* indices, int numIndeces, float*& optPoss);

    protected:
        //! Mesh index
        int m_meshIndex = 0;

        //! Damping Coefficient
        float m_dampingCoeff = 1.f;

        //! Repulsion Stiffness
        float m_repulsionStiffness = 0.5f;

        //! Pressure coefficient
        float m_pressureCoeff = 0.f;

        //! Velocity correction factor
        float m_velocityFactor = 1.f;

        //! Volume conversation coefficient
        float m_volumeConvCoeff = 0.f;

        //! Dynamic friction coefficient
        float m_dynamicFrictionCoeff = 0.2f;

        //! Pose matching coefficient
        float m_poseMatchCoeff = 0.f;

        //! Rigid contacts hardness
        float m_rigidContactHardness = 1.f;

        //! Kinetic contacts hardness
        float m_kineticContactHardness = 0.1f;

        //! Soft contacts hardness
        float m_softContactHardness = 1.f;

        //! Anchors hardness
        float m_anchorHardness = 1.f;

        //! Sleeping Threshold
        float m_sleepingThreshold = 0.04f;

        //! Rest Length Scale
        float m_restLengthScale = 1.f;

        //! Gravity factor
        float m_gravityFactor = 1.f;

        //! Position iteration number
        int m_positionIterNumber = 1;

        //! Aero model
        int m_aeroModel = (int)btSoftBody::eAeroModel::V_TwoSided;

        //! Use Self Collision
        bool m_bUseSelfCollision = false;

        //! Soft Soft-Collision
        bool m_softSoftCollision = false;

        //! Wind Velocity
        btVector3 m_windVelocity = {0.f, 0.f, 0.f};

        //! Cache indices map
        int* m_indicesMap = nullptr;
};
} // namespace ige::scene
