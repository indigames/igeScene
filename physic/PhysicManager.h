#pragma once

#include <memory>
#include <vector>
#include <string>
#include <tuple>

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include "scene/SceneObject.h"
#include "components/physic/PhysicBase.h"
#include "components/physic/PhysicSoftBody.h"

#include "utils/Singleton.h"
#include "event/Event.h"

namespace ige::scene
{
    struct RaycastHit
    {
        const btCollisionObject* object;
        btVector3 position;
        btVector3 normal;
    };

    struct ContactTestResult
    {
        const btCollisionObject* objectA;
        const btCollisionObject* objectB;
        btVector3 localPosA;
        btVector3 localPosB;
        btVector3 worldPosA;
        btVector3 worldPosB;
        btVector3 normalB;
    };

    struct ContactResultCB : public btCollisionWorld::ContactResultCallback
    {
        int count;
        std::vector<ContactTestResult>& outResults;
        ContactResultCB(std::vector<ContactTestResult>& _outResults, 
                        int group = btBroadphaseProxy::DefaultFilter,
                        int mask = btBroadphaseProxy::AllFilter)
            : ContactResultCallback(), outResults(_outResults), count(0) 
        {
            m_collisionFilterGroup = group;
            m_collisionFilterMask = mask;
        }

        virtual btScalar addSingleResult(btManifoldPoint& cp,
            const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
            const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override;
    };

    /**
     * Class PhysicManager: manage physic simulation
     */
    class PhysicManager : public Singleton<PhysicManager>
    {
    public:
        //! Constructor
        PhysicManager();

        //! Destructor
        virtual ~PhysicManager();

        //! Initialize
        bool initialize(int numIteration = 10, bool deformable = true);

        //! Clear world
        void clear();

        //! Update
        void onUpdate(float dt);
        void preUpdate();
        void postUpdate();

        //! Ray test closest
        RaycastHit rayTestClosest(const btVector3 &rayFromWorld, const btVector3 &rayToWorld, int group = btBroadphaseProxy::DefaultFilter, int mask = btBroadphaseProxy::AllFilter);

        //! Ray test all
        std::vector<RaycastHit> rayTestAll(const btVector3 &rayFromWorld, const btVector3 &rayToWorld, int group = btBroadphaseProxy::DefaultFilter, int mask = btBroadphaseProxy::AllFilter);

        //! Get world
        btDiscreteDynamicsWorld*getWorld() { return m_world ? m_world.get() : nullptr; }

        //! Get world
        btSoftRigidDynamicsWorld* getDeformableWorld() { return m_world ? (btSoftRigidDynamicsWorld*)m_world.get() : nullptr; }

        //! Contact test
        std::vector<ContactTestResult> contactTest(btCollisionObject* object, int group = btBroadphaseProxy::DefaultFilter, int mask = btBroadphaseProxy::AllFilter);

        //! Contact pair test
        std::vector<ContactTestResult> contactPairTest(btCollisionObject* objectA, btCollisionObject* objectB, int group = btBroadphaseProxy::DefaultFilter, int mask = btBroadphaseProxy::AllFilter);

        //! Check deformable
        bool isDeformable() { return m_bDeformable; }

        //! Number of iteration
        int getNumIteration() { return m_numIteration; }
        void setNumIteration(int numIteration) { m_numIteration = numIteration; }

        //! Frame update ratio (speedup/slower effects)
        float getFrameUpdateRatio() { return m_frameUpdateRatio; };
        void setFrameUpdateRatio(float ratio) { m_frameUpdateRatio = ratio; }

        //! Frame max simulation sub step
        int getFrameMaxSubStep() { return m_frameMaxSubStep; }
        void setFrameMaxSubStep(int nSteps) { m_frameMaxSubStep = nSteps; }

        //! Fixed time steps
        float getFixedTimeStep() { return m_fixedTimeStep; }
        void setFixedTimeStep(float timeStep) { m_fixedTimeStep = timeStep; }

        //! Gravity
        const btVector3 &getGravity() { return m_gravity; }
        void setGravity(const btVector3 &gravity) { m_gravity = gravity; }

        //! Deformable
        bool isDeformable() const { return m_bDeformable; }
        void setDeformable(bool deformable = true);

    protected:
        //! Collision callback
        void setCollisionCallback();
        static bool collisionCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2);

        //! Create/Destroy event
        void onObjectCreated(PhysicBase* object);
        void onObjectDestroyed(PhysicBase* object);

        //! Activate/Deactivate event
        void onObjectActivated(PhysicBase* object);
        void onObjectDeactivated(PhysicBase* object);

    protected:
        //! Physic world
        std::unique_ptr<btDiscreteDynamicsWorld> m_world = nullptr;
        std::unique_ptr<btBroadphaseInterface> m_broadphase = nullptr;
        std::unique_ptr<btDispatcher> m_dispatcher = nullptr;
        std::unique_ptr<btConstraintSolver> m_solver = nullptr;
        std::unique_ptr<btCollisionConfiguration> m_collisionConfiguration = nullptr;
        std::unique_ptr<btGhostPairCallback> m_ghostPairCallback = nullptr;
        std::vector<std::unique_ptr<btRaycastVehicle>> m_vehicles;

        //! Last frame time
        uint64_t m_lastFrameTime = 0;

        //! Deformable (enable SoftBody simulation)
        bool m_bDeformable = false;

        //! Numer of iteration per frame
        int m_numIteration = 10;

        //! Frame update ratio (speedup/slower effects)
        float m_frameUpdateRatio = 1.f;

        //! Frame max simulation sub step
        int m_frameMaxSubStep = 4;

        //! Fixed time steps
        float m_fixedTimeStep = 1 / 240.f;

        //! Gravity
        btVector3 m_gravity = {0.f, -9.81f, 0.f};

        //! Collision event map
        static std::map< std::pair<PhysicBase*, PhysicBase*>, bool> m_collisionEvents;

        //! Physic objects list
        std::vector<PhysicBase*> m_physicObjects;
    };
} // namespace ige::scene