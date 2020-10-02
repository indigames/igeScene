#include "physic/PhysicManager.h"

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

#include "components/physic/PhysicBase.h"

namespace ige::scene
{
    //! Static member initialization
    std::map< std::pair<PhysicBase*, PhysicBase*>, bool> PhysicManager::m_collisionEvents;

    //! Constructor
    PhysicManager::PhysicManager()
    {
    }

    bool PhysicManager::initialize(int numIteration, bool deformable)
    {
        m_numIteration = numIteration;
        m_bDeformable = deformable;

        if (deformable)
        {
            m_collisionConfiguration = std::make_unique<btSoftBodyRigidBodyCollisionConfiguration>();
            m_dispatcher = std::make_unique<btCollisionDispatcher>(m_collisionConfiguration.get());
            m_broadphase = std::make_unique<btDbvtBroadphase>();
            m_solver = std::make_unique<btDeformableMultiBodyConstraintSolver>();
            m_deformableBodySolver = std::make_unique<btDeformableBodySolver>();
            ((btDeformableMultiBodyConstraintSolver *)m_solver.get())->setDeformableSolver(m_deformableBodySolver.get());
            m_world = std::make_unique<btDeformableMultiBodyDynamicsWorld>(m_dispatcher.get(), m_broadphase.get(), (btDeformableMultiBodyConstraintSolver *)m_solver.get(), m_collisionConfiguration.get(), m_deformableBodySolver.get());
            auto worldInfo = ((btDeformableMultiBodyDynamicsWorld *)m_world.get())->getWorldInfo();
            worldInfo.m_gravity = m_gravity;
            worldInfo.m_sparsesdf.setDefaultVoxelsz(0.25f);
            worldInfo.m_sparsesdf.Reset();
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
        m_world->getSolverInfo().m_numIterations = numIteration;

        // Register event listeners
        PhysicBase::getOnCreatedEvent().addListener(std::bind(static_cast<void(PhysicManager::*)(PhysicBase&)>(&PhysicManager::onObjectCreated), this, std::placeholders::_1));
        PhysicBase::getOnDestroyedEvent().addListener(std::bind(static_cast<void(PhysicManager::*)(PhysicBase&)>(&PhysicManager::onObjectDestroyed), this, std::placeholders::_1));
        PhysicBase::getOnActivatedEvent().addListener(std::bind(static_cast<void(PhysicManager::*)(PhysicBase&)>(&PhysicManager::onObjectActivated), this, std::placeholders::_1));
        PhysicBase::getOnDeactivatedEvent().addListener(std::bind(static_cast<void(PhysicManager::*)(PhysicBase&)>(&PhysicManager::onObjectDeactivated), this, std::placeholders::_1));

        // Set collision callback
        setCollisionCallback();

        return true;
    }

    //! Destructor
    PhysicManager::~PhysicManager()
    {
        clear();

        m_collisionConfiguration = nullptr;
        m_dispatcher = nullptr;
        m_broadphase = nullptr;
        m_solver = nullptr;
        m_deformableBodySolver = nullptr;
        m_ghostPairCallback = nullptr;
        m_world = nullptr;
    }

    //! Clear world
    void PhysicManager::clear()
    {
        auto world = ((btDiscreteDynamicsWorld *)m_world.get());
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
            world->removeVehicle((btRaycastVehicle *)((*it).get()));
        }
        m_vehicles.clear();
    }

    //! Update
    void PhysicManager::onUpdate(float dt)
    {
        preUpdate();
        if (m_world->stepSimulation(dt * m_frameUpdateRatio, m_frameMaxSubStep, m_fixedTimeStep))
            postUpdate();
    }

    void PhysicManager::preUpdate()
    {
        // Reset collision events
        for (auto &element : m_collisionEvents)
            element.second = false;

        // Update bullet transform
        std::for_each(m_physicObjects.begin(), m_physicObjects.end(), std::mem_fn(&PhysicBase::updateBtTransform));
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
                    objects.first->getCollisionStopEvent().invoke(*objects.second);
                    objects.second->getCollisionStopEvent().invoke(*objects.first);
                }
                else
                {
                    if (objects.first->isTrigger())
                        objects.first->getTriggerStopEvent().invoke(*objects.second);
                    else
                        objects.second->getTriggerStopEvent().invoke(*objects.first);
                }
                it = m_collisionEvents.erase(it);
            }
            else
                ++it;
        }

        // Update object transform
        std::for_each(m_physicObjects.begin(), m_physicObjects.end(), std::mem_fn(&PhysicBase::updateIgeTransform));
    }

    //! Create/Destroy event
    void PhysicManager::onObjectCreated(PhysicBase &object)
    {
        m_physicObjects.push_back(std::ref(object));
    }

    void PhysicManager::onObjectDestroyed(PhysicBase &object)
    {
        // Find and remove object from the objects list
        auto found = std::find_if(m_physicObjects.begin(), m_physicObjects.end(), [&object](std::reference_wrapper<PhysicBase> element) {
            return std::addressof(object) == std::addressof(element.get());
        });

        if (found != m_physicObjects.end())
            m_physicObjects.erase(found);

        // Find and remove collision events
        for (auto iter = m_collisionEvents.begin(); iter != m_collisionEvents.end();)
        {
            if (iter->first.first == std::addressof(object) || iter->first.second == std::addressof(object))
            {
                m_collisionEvents.erase(iter);
            }
            if(iter != m_collisionEvents.end())
                ++iter;
        }
    }

    //! Activate/Deactivate event
    void PhysicManager::onObjectActivated(PhysicBase &object)
    {
        m_world->addRigidBody(&(object.getBody()));
    }

    void PhysicManager::onObjectDeactivated(PhysicBase &object)
    {
        m_world->removeRigidBody(&(object.getBody()));
    }

    //! Ray test closest
    RaycastHit PhysicManager::rayTestClosest(const btVector3 &rayFromWorld, const btVector3 &rayToWorld, int group, int mask)
    {
        RaycastHit result;
        btCollisionWorld::ClosestRayResultCallback closestRayCallback(rayFromWorld, rayToWorld);
        closestRayCallback.m_collisionFilterMask = mask;
        closestRayCallback.m_collisionFilterGroup = group;

        m_world->rayTest(rayFromWorld, rayToWorld, closestRayCallback);
        if (closestRayCallback.hasHit())
        {
            // Get closest hit
            result.hitObject = reinterpret_cast<PhysicBase *>(closestRayCallback.m_collisionObject->getUserPointer())->getOwner();
            result.hitPosition = closestRayCallback.m_hitPointWorld;
            result.hitNormal = closestRayCallback.m_hitNormalWorld;
        }
        return result;
    }

    //! Ray test all
    std::vector<RaycastHit> PhysicManager::rayTestAll(const btVector3 &rayFromWorld, const btVector3 &rayToWorld, int group, int mask)
    {
        std::vector<RaycastHit> result;
        auto closeRayHit = rayTestClosest(rayFromWorld, rayToWorld, group, mask);
        if (closeRayHit.hitObject == nullptr)
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
            hit.hitObject = reinterpret_cast<PhysicBase *>(allHitsRayCallback.m_collisionObjects[i]->getUserPointer())->getOwner();
            hit.hitPosition = allHitsRayCallback.m_hitPointWorld[i];
            hit.hitNormal = allHitsRayCallback.m_hitNormalWorld[i];
            result.push_back(hit);
        }
        return result;
    }

    //! Ray cast by origin, direction and distance
    RaycastResult PhysicManager::raycast(const btVector3 &origin, const btVector3 &direction, float distance, int group, int mask)
    {
        btVector3 target = origin + direction * distance;
        RaycastResult resultHit;

        btCollisionWorld::ClosestRayResultCallback closestRayCallback(origin, target);
        closestRayCallback.m_collisionFilterMask = mask;
        closestRayCallback.m_collisionFilterGroup = group;

        m_world->rayTest(origin, target, closestRayCallback);

        if (closestRayCallback.hasHit())
        {
            // Get closest hit
            resultHit.closestHit.hitObject = reinterpret_cast<PhysicBase *>(closestRayCallback.m_collisionObject->getUserPointer())->getOwner();
            resultHit.closestHit.hitPosition = closestRayCallback.m_hitPointWorld;
            resultHit.closestHit.hitNormal = closestRayCallback.m_hitNormalWorld;

            // Get all hits
            btCollisionWorld::AllHitsRayResultCallback allHitsRayCallback(origin, target);
            allHitsRayCallback.m_collisionFilterMask = mask;
            allHitsRayCallback.m_collisionFilterGroup = group;

            m_world->rayTest(origin, target, allHitsRayCallback);

            // Get all Hit
            for (int i = 0; i < allHitsRayCallback.m_collisionObjects.size(); i++)
            {
                RaycastHit hit;
                hit.hitObject = reinterpret_cast<PhysicBase *>(allHitsRayCallback.m_collisionObjects[i]->getUserPointer())->getOwner();
                hit.hitPosition = allHitsRayCallback.m_hitPointWorld[i];
                hit.hitNormal = allHitsRayCallback.m_hitNormalWorld[i];
                resultHit.allHits.push_back(hit);
            }
        }
        return resultHit;
    }

    bool PhysicManager::collisionCallback(btManifoldPoint &cp, const btCollisionObjectWrapper *obj1, int id1, int index1, const btCollisionObjectWrapper *obj2, int id2, int index2)
    {
        auto object1 = reinterpret_cast<PhysicBase *>(obj1->getCollisionObject()->getUserPointer());
        auto object2 = reinterpret_cast<PhysicBase *>(obj2->getCollisionObject()->getUserPointer());

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
                        object1->getTriggerStartEvent().invoke(*object2);
                    else if (!object2->isTrigger())
                        object1->getCollisionStartEvent().invoke(*object2);

                    // Object 2 (Start event)
                    if (object2->isTrigger())
                        object2->getTriggerStartEvent().invoke(*object1);
                    else if (!object1->isTrigger())
                        object2->getCollisionStartEvent().invoke(*object1);

                    // Object 1 (Stay event)
                    if (object1->isTrigger())
                        object1->getTriggerStayEvent().invoke(*object2);
                    else if (!object2->isTrigger())
                        object1->getCollisionStayEvent().invoke(*object2);

                    // Object 2 (Stay event)
                    if (object2->isTrigger())
                        object2->getTriggerStayEvent().invoke(*object1);
                    else if (!object1->isTrigger())
                        object2->getCollisionStayEvent().invoke(*object1);

                    m_collisionEvents[{object1, object2}] = true;
                    return true;
                }
                else
                {
                    if (!m_collisionEvents[{object1, object2}])
                    {
                        // Object 1 (Stay event)
                        if (object1->isTrigger())
                            object1->getTriggerStayEvent().invoke(*object2);
                        else if (!object2->isTrigger())
                            object1->getCollisionStayEvent().invoke(*object2);

                        // Object 2 (Stay event)
                        if (object2->isTrigger())
                            object2->getTriggerStayEvent().invoke(*object1);
                        else if (!object1->isTrigger())
                            object2->getCollisionStayEvent().invoke(*object1);

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

} // namespace ige::scene
