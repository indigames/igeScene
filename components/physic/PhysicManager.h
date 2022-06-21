#pragma once

#include <memory>
#include <vector>
#include <string>
#include <tuple>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include "scene/SceneObject.h"
#include "components/Component.h"
#include "components/physic/Rigidbody.h"
#include "components/physic/Softbody.h"
#include "components/physic/PhysicConstraint.h"
#include "components/physic/BulletDebugRender.h"

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
     * Class PhysicManager: manage physic simulation. Should be added to the root node.
     */
    class PhysicManager : public Component
    {
    public:
        //! Constructor
        PhysicManager(SceneObject& owner, bool deformable = true);

        //! Destructor
        virtual ~PhysicManager();

        //! Get name
        std::string getName() const override { return "PhysicManager"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::PhysicManager; }

        //! Initialize
        bool initialize();

        //! Clear world
        void clear();

        //! Update
        void onUpdate(float dt) override;
        void onPhysicUpdate(float dt) override;
        void preUpdate();
        void postUpdate();

        //! Draw debug
        void onRender() override;

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

        //! Deformable
        bool isDeformable() const { return m_bDeformable; }
        void setDeformable(bool deformable = true);

        //! Number of iteration
        int getNumIteration() const { return m_numIteration; }
        void setNumIteration(int numIteration) { m_numIteration = numIteration; }

        //! Fixed time steps
        float getFixedTimeStep() const { return m_fixedTimeStep; }
        void setFixedTimeStep(float timeStep) { m_fixedTimeStep = timeStep; }

        //! Frame max simulation sub step
        int getFrameMaxSubStep() const { return m_frameMaxSubStep; }
        void setFrameMaxSubStep(int nSteps) { m_frameMaxSubStep = nSteps; }

        //! Frame update ratio (speedup/slower effects)
        float getFrameUpdateRatio() const { return m_frameUpdateRatio; };
        void setFrameUpdateRatio(float ratio) { m_frameUpdateRatio = ratio; }

        //! Gravity
        const btVector3 &getGravity() const { return m_gravity; }
        void setGravity(const btVector3& gravity);

        //! Render debug
        bool isShowDebug() const { return m_bShowDebug; }
        void setShowDebug(bool debug = true) { m_bShowDebug = debug; }

        //! Check if multiple edit allowed
        virtual bool canMultiEdit() override { return false; }


    protected:
        //! Collision callback
        void setCollisionCallback();
        static bool collisionCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2);

        //! Create/Destroy event
        void onCreated(const std::shared_ptr<Rigidbody>& object);
        void onDestroyed(const std::shared_ptr<Rigidbody>& object);

        //! Activate/Deactivate event
        void onActivated(const std::shared_ptr<Rigidbody>& object);
        void onDeactivated(const std::shared_ptr<Rigidbody>& object);

        //! Constraint activated/deactivated event
        void onActivated(PhysicConstraint* constraint);
        void onDeactivated(PhysicConstraint* constraint);

        //! Serialize
        virtual void to_json(json &j) const override;

        //! Deserialize
        virtual void from_json(const json &j) override;
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
        int m_numIteration = 1;

        //! Frame update ratio (speedup/slower effects)
        float m_frameUpdateRatio = 1.f;

        //! Frame max simulation sub step
        int m_frameMaxSubStep = 1;

        //! Fixed time steps
        float m_fixedTimeStep = 1 / 60.f;

        //! Gravity
        btVector3 m_gravity = {0.f, -9.81f, 0.f};

        //! Collision event map
        static std::map< std::pair<Rigidbody*, Rigidbody*>, bool> m_collisionEvents;

        //! Physic objects list
        std::vector<std::weak_ptr<Rigidbody>> m_rigidbodys;

        //! Debug renderer
        std::unique_ptr<BulletDebugRender> m_debugRenderer = nullptr;

        //! Render debug
        bool m_bShowDebug = false;
    };
} // namespace ige::scene