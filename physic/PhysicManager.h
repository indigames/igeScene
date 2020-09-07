#pragma once

#include <memory>
#include <vector>
#include <string>
#include <tuple>

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "scene/SceneObject.h"

#include "utils/Singleton.h"
#include "event/Event.h"

namespace ige::scene
{
    struct RaycastHit
    {
        SceneObject* hitObject = nullptr;
        btVector3 hitPosition;
        btVector3 hitNormal;
    };

    struct RaycastResult
    {
        RaycastHit closestHit;
        std::vector<RaycastHit> allHits;
    };

    /**
     * Class PhysicManager: manage physic simulation
     */
    class PhysicManager : public Singleton<PhysicManager>
    {
    public:
        //! Constructor
        PhysicManager(int numIteration = 4, bool deformable = false);

        //! Destructor
        virtual ~PhysicManager();

        //! Update
        void onUpdate(float dt);

        //! Ray test closest
        RaycastHit rayTestClosest(const btVector3 &rayFromWorld, const btVector3 &rayToWorld, int group = btBroadphaseProxy::DefaultFilter, int mask = btBroadphaseProxy::AllFilter);

        //! Ray test all
        std::vector<RaycastHit> rayTestAll(const btVector3 &rayFromWorld, const btVector3 &rayToWorld, int group = btBroadphaseProxy::DefaultFilter, int mask = btBroadphaseProxy::AllFilter);

        //! Ray cast by origin, direction and distance
        RaycastResult raycast(const btVector3& origin, const btVector3& direction, float distance, int group = btBroadphaseProxy::DefaultFilter, int mask = btBroadphaseProxy::AllFilter);

        //! Collision callback
        void setCollisionCallback();
        static bool collisionCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2);

        //! Get world
        btDynamicsWorld *getWorld() { return m_world ? m_world.get() : nullptr; }

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
        float gatFixedTimeStep() { return m_fixedTimeStep; }
        void setFixedTimeStep(float timeStep) { m_fixedTimeStep = timeStep; }

        //! Gravity
        const btVector3 &getGravity() { return m_gravity; }
        void setGravity(const btVector3 &gravity) { m_gravity = gravity; }

        //! Create/Destroy event
        void onObjectCreated(PhysicBase& object);
        void onObjectDestroyed(PhysicBase& object);

        //! Activate/Deactivate event
        void onObjectActivated(PhysicBase& object);
        void onObjectDeactivated(PhysicBase& object);


    protected:
        //! Clear world
        void clear();

    protected:
        //! Physic world
        std::unique_ptr<btDynamicsWorld> m_world = nullptr;
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

        //! Deformable (enable SoftBody simulation)
        int m_numIteration = 4;

        //! Frame update ratio (speedup/slower effects)
        float m_frameUpdateRatio = 1.f;

        //! Frame max simulation sub step
        int m_frameMaxSubStep = 1;

        //! Fixed time steps
        float m_fixedTimeStep = 1 / 60.f;

        //! Gravity
        btVector3 m_gravity = {0.f, -9.81f, 0.f};

        //! Collision event map
        static std::map< std::pair<PhysicBase*, PhysicBase*>, bool> m_collisionEvents;
    };
} // namespace ige::scene