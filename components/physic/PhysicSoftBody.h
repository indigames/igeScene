#pragma once

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletSoftBody/btSoftBody.h>

#include "event/Event.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/PhysicBase.h"

namespace ige::scene
{
    //! PhysicSoftBody
    class PhysicSoftBody : public PhysicBase
    {
    public:
        //! Constructor
        PhysicSoftBody(SceneObject& owner);

        //! Destructor
        virtual ~PhysicSoftBody();

        // Inherited via Component
        virtual std::string getName() const override { return "PhysicSoftBody"; }

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
        virtual void getAABB(btVector3& aabbMin, btVector3 aabbMax) override { 
            getSoftBody()->getAabb(aabbMin, aabbMax);
        }

        //! Damping coefficient
        virtual float getDampingCoefficient() const { return m_dampingCoefficient; }
        virtual void setDampingCoefficient(float coeff);

        //! Repulsion Stiffness
        virtual float getRepulsionStiffness() const { return m_repulsionStiffness; }
        virtual void setRepulsionStiffness(float repulsionStiffness);

        //! Sleeping Threshold
        virtual float getSleepingThreshold() const { return m_sleepingThreshold; }
        virtual void setSleepingThreshold(float threshold);

        //! Rest Length Scale
        virtual float getRestLengthScale() const { return m_restLengthScale; }
        virtual void setRestLengthScale(float scale);

        //! Self collision
        virtual bool isSelfCollision() const { return m_bUseSelfCollision; }
        virtual void setSelfCollision(bool selfCollision = true);

        //! Soft soft collision
        virtual bool isSoftSoftCollision() const { return m_softSoftCollision; }
        virtual void setSoftSoftCollision(bool soft = true);

        //! Wind velocity
        virtual const btVector3& getWindVelocity() const { return m_windVelocity; }
        virtual void setWindVelocity(const btVector3& velocity);

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
        virtual void addVelocity(const btVector3& velocity) { getSoftBody()->addVelocity(velocity); }

        //! Get Volume
        virtual float getVolume() const { return getSoftBody()->getVolume(); }

        //! Get Center of mass
        virtual btVector3 getCenterOfMass() const { return getSoftBody()->getCenterOfMass(); }

        //! Append deformable anchor
        virtual void appendDeformableAnchor(int node, btRigidBody* body) { getSoftBody()->appendDeformableAnchor(node, body); }

        //! Append anchor
        virtual void appendAnchor(int node, btRigidBody* body, bool disableLinkedColission = false, btScalar influence = 1) {
            getSoftBody()->appendAnchor(node, body, disableLinkedColission, influence);
        }

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
        //! Damping Coefficient
        float m_dampingCoefficient = 0.4f;

        //! Repulsion Stiffness
        float m_repulsionStiffness = 0.5f;

        //! Sleeping Threshold
        float m_sleepingThreshold = 0.f;

        //! Rest Length Scale
        float m_restLengthScale = 0.f;

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
