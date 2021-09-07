#include "components/physic/PhysicManager.h"

#include <algorithm>

#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletDynamics/Featherstone/btMultiBodyConstraintSolver.h>

#include <BulletSoftBody/btDeformableMultiBodyDynamicsWorld.h>
#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#include <BulletSoftBody/btDeformableBodySolver.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>

#include "components/physic/PhysicObject.h"
#include "components/physic/PhysicSoftBody.h"
#include "components/physic/BulletDebugRender.h"
#include "scene/SceneManager.h"
#include "utils/PhysicHelper.h"

namespace ige::scene
{
    //! Static member initialization
    std::map< std::pair<PhysicObject*, PhysicObject*>, bool> PhysicManager::m_collisionEvents;

    //! Constructor
    PhysicManager::PhysicManager(SceneObject& owner, int numIteration, bool deformable)
        : Component(owner), m_numIteration(numIteration), m_bDeformable(deformable)
    {
    }

    bool PhysicManager::initialize()
    {
        clear();

        if (m_bDeformable)
        {
            m_collisionConfiguration = std::make_unique<btSoftBodyRigidBodyCollisionConfiguration>();
            m_dispatcher = std::make_unique<btCollisionDispatcher>(m_collisionConfiguration.get());
            m_broadphase = std::make_unique<btDbvtBroadphase>();
            m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
            m_world = std::make_unique<btSoftRigidDynamicsWorld>(m_dispatcher.get(), m_broadphase.get(), m_solver.get(), m_collisionConfiguration.get());

            auto& worldInfo = getDeformableWorld()->getWorldInfo();
            worldInfo.m_dispatcher = m_dispatcher.get();
            worldInfo.m_broadphase = m_broadphase.get();
            worldInfo.m_gravity = m_gravity;
            worldInfo.m_sparsesdf.Initialize();
        }
        else
        {
            m_collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
            m_dispatcher = std::make_unique<btCollisionDispatcher>(m_collisionConfiguration.get());
            m_broadphase = std::make_unique<btDbvtBroadphase>();
            m_ghostPairCallback = std::make_unique<btGhostPairCallback>();
            m_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback.get());
            m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
            m_world = std::make_unique<btDiscreteDynamicsWorld>(m_dispatcher.get(), m_broadphase.get(), m_solver.get(), m_collisionConfiguration.get());
        }

        m_world->setGravity(m_gravity);
        m_world->getSolverInfo().m_numIterations = m_numIteration;
        m_world->getDispatchInfo().m_useContinuous = true;
        m_world->getSolverInfo().m_splitImpulse = false;
        m_world->setSynchronizeAllMotionStates(true);

        // Set collision callback
        setCollisionCallback();

        // Init debug renderer
        m_debugRenderer = std::make_unique<BulletDebugRender>();
        m_world->setDebugDrawer(m_debugRenderer.get());

        // Register event listeners
        PhysicObject::getOnCreatedEvent().addListener(std::bind(static_cast<void(PhysicManager::*)(PhysicObject*)>(&PhysicManager::onCreated), this, std::placeholders::_1));
        PhysicObject::getOnDestroyedEvent().addListener(std::bind(static_cast<void(PhysicManager::*)(PhysicObject*)>(&PhysicManager::onDestroyed), this, std::placeholders::_1));
        PhysicObject::getOnActivatedEvent().addListener(std::bind(static_cast<void(PhysicManager::*)(PhysicObject*)>(&PhysicManager::onActivated), this, std::placeholders::_1));
        PhysicObject::getOnDeactivatedEvent().addListener(std::bind(static_cast<void(PhysicManager::*)(PhysicObject*)>(&PhysicManager::onDeactivated), this, std::placeholders::_1));

        // Constraint event listeners
        PhysicConstraint::getOnActivatedEvent().addListener(std::bind(static_cast<void(PhysicManager::*)(PhysicConstraint*)>(&PhysicManager::onActivated), this, std::placeholders::_1));
        PhysicConstraint::getOnDeactivatedEvent().addListener(std::bind(static_cast<void(PhysicManager::*)(PhysicConstraint*)>(&PhysicManager::onDeactivated), this, std::placeholders::_1));

        return true;
    }

    //! Destructor
    PhysicManager::~PhysicManager()
    {
        clear();
    }

    //! Clear world
    void PhysicManager::clear()
    {
        // Unregister event listeners
        PhysicObject::getOnCreatedEvent().removeAllListeners();
        PhysicObject::getOnDestroyedEvent().removeAllListeners();
        PhysicObject::getOnActivatedEvent().removeAllListeners();
        PhysicObject::getOnDeactivatedEvent().removeAllListeners();

        // Unregister constraint listeners
        PhysicConstraint::getOnActivatedEvent().removeAllListeners();
        PhysicConstraint::getOnDeactivatedEvent().removeAllListeners();

        // Destroy world
        if (m_world)
        {
            auto world = ((btDiscreteDynamicsWorld*)m_world.get());
            for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
            {
                auto obj = world->getCollisionObjectArray()[i];
                world->removeCollisionObject(obj);
            }

            for (int i = world->getNumConstraints() - 1; i >= 0; i--)
            {
                auto obj = world->getConstraint(i);
                world->removeConstraint(obj);
            }

            for (auto it = m_vehicles.begin(); it != m_vehicles.end(); it++)
            {
                world->removeVehicle((btRaycastVehicle*)((*it).get()));
            }
        }

        m_vehicles.clear();
        m_collisionEvents.clear();

        m_collisionConfiguration = nullptr;
        m_dispatcher = nullptr;
        m_broadphase = nullptr;
        m_solver = nullptr;
        m_ghostPairCallback = nullptr;
        m_world = nullptr;
        m_debugRenderer = nullptr;
    }


    // Set deformable
    void PhysicManager::setDeformable(bool deformable)
    {
        if (m_bDeformable != deformable)
        {
            m_bDeformable = deformable;
        }
    }

    // Set gravity
    void PhysicManager::setGravity(const btVector3& gravity)
    {
        m_gravity = gravity;
        if (m_world)
            m_world->setGravity(m_gravity);
    }

    //! Update
    void PhysicManager::onUpdate(float dt)
    {
        if (m_world == nullptr)
            initialize();

        preUpdate();

        // Run simulation if not in edit mode
        if (!SceneManager::getInstance()->isEditor())
            if (m_world->stepSimulation(dt * m_frameUpdateRatio, m_frameMaxSubStep, m_fixedTimeStep))
                postUpdate();

        // Show debug
        if(isShowDebug())
            getWorld()->debugDrawWorld();

        // Do GC
        if(isDeformable() && getDeformableWorld())
            getDeformableWorld()->getWorldInfo().m_sparsesdf.GarbageCollect();
    }

    void PhysicManager::preUpdate()
    {
        // Reset collision events
        for (auto &element : m_collisionEvents)
            element.second = false;

        // Update bullet transform
        std::for_each(m_physicObjects.begin(), m_physicObjects.end(), std::mem_fn(&PhysicObject::updateBtTransform));
    }

    void PhysicManager::postUpdate()
    {
        // Check remove collision events
        for (auto it = m_collisionEvents.begin(); it != m_collisionEvents.end();)
        {
            auto objects = it->first;
            if (!it->second)
            {
                if (!objects.first->isTrigger() && !objects.second->isTrigger())
                {
                    objects.first->getCollisionStopEvent().invoke(objects.second);
                    objects.second->getCollisionStopEvent().invoke(objects.first);
                }
                else
                {
                    if (objects.first->isTrigger())
                        objects.first->getTriggerStopEvent().invoke(objects.second);
                    else
                        objects.second->getTriggerStopEvent().invoke(objects.first);
                }
                it = m_collisionEvents.erase(it);
            }
            else
                ++it;
        }

        // Update object transform
        std::for_each(m_physicObjects.begin(), m_physicObjects.end(), std::mem_fn(&PhysicObject::updateIgeTransform));
    }

    //! Create/Destroy event
    void PhysicManager::onCreated(PhysicObject *object)
    {
        m_physicObjects.push_back(object);
    }

    void PhysicManager::onDestroyed(PhysicObject *object)
    {
        // Find and remove object from the objects list
        auto found = std::find_if(m_physicObjects.begin(), m_physicObjects.end(), [&object](PhysicObject* element) {
            return object == element;
        });

        if (found != m_physicObjects.end())
            m_physicObjects.erase(found);

        // Find and remove collision events
        auto evFound = std::find_if(m_collisionEvents.begin(), m_collisionEvents.end(), [&object](auto pair) {
            return object == pair.first.first || object == pair.first.second;
        });

        // Find and remove all collision events
        while (evFound != m_collisionEvents.end())
        {
            m_collisionEvents.erase(evFound);

            evFound = std::find_if(m_collisionEvents.begin(), m_collisionEvents.end(), [&object](auto pair) {
                return object == pair.first.first || object == pair.first.second;
            });
        }
    }

    //! Activate/Deactivate event
    void PhysicManager::onActivated(PhysicObject *object)
    {
        if (isDeformable())
        {
            if (object->getBody())
                getDeformableWorld()->addRigidBody(object->getBody());
            else if (object->getSoftBody())
                getDeformableWorld()->addSoftBody(object->getSoftBody(), object->getCollisionFilterGroup(), object->getCollisionFilterMask());
        }
        else
        {
            if (object->getBody())
                getWorld()->addRigidBody(object->getBody());
        }
    }

    void PhysicManager::onDeactivated(PhysicObject *object)
    {
        if (isDeformable())
        {
            if (object->getBody())
                getDeformableWorld()->removeRigidBody(object->getBody());
            else if (object->getSoftBody())
                getDeformableWorld()->removeSoftBody(object->getSoftBody());
        }
        else
        {
            if (object->getBody())
                getWorld()->removeRigidBody(object->getBody());
        }
    }

    //! Constraint activated event
    void PhysicManager::onActivated(PhysicConstraint *constraint)
    {
        m_world->addConstraint(constraint->getConstraint(), !constraint->isEnableCollisionBetweenBodies());
    }

    //! Constraint deactivated event
    void PhysicManager::onDeactivated(PhysicConstraint *constraint)
    {
        m_world->removeConstraint(constraint->getConstraint());
    }

    //! Ray test closest
    RaycastHit PhysicManager::rayTestClosest(const btVector3 &rayFromWorld, const btVector3 &rayToWorld, int group, int mask)
    {
        RaycastHit result;
        result.object = nullptr;
        btCollisionWorld::ClosestRayResultCallback closestRayCallback(rayFromWorld, rayToWorld);
        closestRayCallback.m_collisionFilterMask = mask;
        closestRayCallback.m_collisionFilterGroup = group;

        m_world->rayTest(rayFromWorld, rayToWorld, closestRayCallback);
        if (closestRayCallback.hasHit())
        {
            // Get closest hit
            result.object = closestRayCallback.m_collisionObject;
            result.position = closestRayCallback.m_hitPointWorld;
            result.normal = closestRayCallback.m_hitNormalWorld;
        }
        return result;
    }

    //! Ray test all
    std::vector<RaycastHit> PhysicManager::rayTestAll(const btVector3 &rayFromWorld, const btVector3 &rayToWorld, int group, int mask)
    {
        std::vector<RaycastHit> result;
        auto closeRayHit = rayTestClosest(rayFromWorld, rayToWorld, group, mask);
        if (closeRayHit.object == nullptr)
            return result;

        // Get all hits
        btCollisionWorld::AllHitsRayResultCallback allHitsRayCallback(rayFromWorld, rayToWorld);
        allHitsRayCallback.m_collisionFilterMask = mask;
        allHitsRayCallback.m_collisionFilterGroup = group;

        m_world->rayTest(rayFromWorld, rayToWorld, allHitsRayCallback);

        // Get all Hit
        for (int i = 0; i < allHitsRayCallback.m_collisionObjects.size(); i++)
        {
            RaycastHit hit;
            hit.object = allHitsRayCallback.m_collisionObjects[i];
            hit.position = allHitsRayCallback.m_hitPointWorld[i];
            hit.normal = allHitsRayCallback.m_hitNormalWorld[i];
            result.push_back(hit);
        }
        return result;
    }

    //! Contact result callback
    btScalar ContactResultCB::addSingleResult(btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
        const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
    {
        ContactTestResult result;
        result.objectA = colObj0Wrap->m_collisionObject;
        result.objectB = colObj1Wrap->m_collisionObject;
        result.worldPosA = cp.m_positionWorldOnA;
        result.worldPosB = cp.m_positionWorldOnB;
        result.localPosA = cp.m_localPointA;
        result.localPosB = cp.m_localPointB;
        result.normalB = cp.m_normalWorldOnB;
        outResults.push_back(result);
        count++;
        return 1.f;
    }

    //! Contact test
    std::vector<ContactTestResult> PhysicManager::contactTest(btCollisionObject* object, int group, int mask)
    {
        std::vector<ContactTestResult> outResults;
        ContactResultCB cb(outResults, group, mask);
        m_world->contactTest(object, cb);
        return outResults;
    }

    //! Contact pair test
    std::vector<ContactTestResult> PhysicManager::contactPairTest(btCollisionObject* objectA, btCollisionObject* objectB, int group, int mask)
    {
        std::vector<ContactTestResult> outResults;
        ContactResultCB cb(outResults, group, mask);
        m_world->contactPairTest(objectA, objectB, cb);
        return outResults;
    }

    bool PhysicManager::collisionCallback(btManifoldPoint &cp, const btCollisionObjectWrapper *obj1, int id1, int index1, const btCollisionObjectWrapper *obj2, int id2, int index2)
    {
        auto object1 = reinterpret_cast<PhysicObject *>(obj1->getCollisionObject()->getUserPointer());
        auto object2 = reinterpret_cast<PhysicObject *>(obj2->getCollisionObject()->getUserPointer());

        if (object1 && object2)
        {
            /* If the objects are not all trigger, enter */
            if (!object1->isTrigger() || !object2->isTrigger())
            {
                if (m_collisionEvents.find({object1, object2}) == m_collisionEvents.end())
                {
                    /*
                    *  If object is trigger, invoke Trigger event,
                    *  else : is the other object trigger?
                    *    yes -> do nothing
                    *    no -> invoke Collision event
                    */

                    // Object 1 (Start event)
                    if (object1->isTrigger())
                        object1->getTriggerStartEvent().invoke(object2);
                    else if (!object2->isTrigger())
                        object1->getCollisionStartEvent().invoke(object2);

                    // Object 2 (Start event)
                    if (object2->isTrigger())
                        object2->getTriggerStartEvent().invoke(object1);
                    else if (!object1->isTrigger())
                        object2->getCollisionStartEvent().invoke(object1);

                    // Object 1 (Stay event)
                    if (object1->isTrigger())
                        object1->getTriggerStayEvent().invoke(object2);
                    else if (!object2->isTrigger())
                        object1->getCollisionStayEvent().invoke(object2);

                    // Object 2 (Stay event)
                    if (object2->isTrigger())
                        object2->getTriggerStayEvent().invoke(object1);
                    else if (!object1->isTrigger())
                        object2->getCollisionStayEvent().invoke(object1);

                    m_collisionEvents[{object1, object2}] = true;
                    return true;
                }
                else
                {
                    if (!m_collisionEvents[{object1, object2}])
                    {
                        // Object 1 (Stay event)
                        if (object1->isTrigger())
                            object1->getTriggerStayEvent().invoke(object2);
                        else if (!object2->isTrigger())
                            object1->getCollisionStayEvent().invoke(object2);

                        // Object 2 (Stay event)
                        if (object2->isTrigger())
                            object2->getTriggerStayEvent().invoke(object1);
                        else if (!object1->isTrigger())
                            object2->getCollisionStayEvent().invoke(object1);

                        m_collisionEvents[{object1, object2}] = true;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void PhysicManager::setCollisionCallback()
    {
        gContactAddedCallback = &PhysicManager::collisionCallback;
    }

    //! Serialize
    void PhysicManager::to_json(json &j) const
    {
        Component::to_json(j);
        j["deform"] = isDeformable();
        j["numIter"] = getNumIteration();
        j["timeStep"] = getFixedTimeStep();
        j["maxSupStep"] = getFrameMaxSubStep();
        j["timeRatio"] = getFrameUpdateRatio();
        j["gravity"] = PhysicHelper::from_btVector3(getGravity());
        j["debug"] = isShowDebug();
    }

    //! Deserialize
    void PhysicManager::from_json(const json &j)
    {
        setDeformable(j.value("deform", false));
        setNumIteration(j.value("numIter", 10.f));
        setFixedTimeStep(j.value("timeStep", 1 / 240.f));
        setFrameMaxSubStep(j.value("maxSupStep", 4));
        setFrameUpdateRatio(j.value("timeRatio", 1.f));
        setGravity(PhysicHelper::to_btVector3(j.value("gravity", Vec3(0.f, -9.81f, 0.f))));
        setShowDebug(j.value("debug", false));
        Component::from_json(j);
        initialize();
    }
} // namespace ige::scene
