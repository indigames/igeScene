#include <algorithm>

#include "components/BoneTransform.h"
#include "components/TransformComponent.h"
#include "components/FigureComponent.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{
    //! Constructor
    BoneTransform::BoneTransform(SceneObject& owner)
        : Component(owner)
    {
        m_sceneObjectDeleteEventId = SceneObject::getDestroyedEvent().addListener(std::bind(&BoneTransform::onSceneObjectDeleted, this, std::placeholders::_1));
        initialize();
    }

    //! Destructor
    BoneTransform::~BoneTransform()
    {
        if (m_sceneObjectDeleteEventId != (uint64_t)-1)
            SceneObject::getDestroyedEvent().removeListener(m_sceneObjectDeleteEventId);
        m_sceneObjectDeleteEventId = -1;

        clear();
    }

    //! Initialize
    bool BoneTransform::initialize()
    {
        if (!isEnabled())
            return false;

        if (!m_bIsInitialized) {
            auto figureCmp = getOwner()->getComponent<FigureComponent>();
            if (figureCmp && figureCmp->isEnabled())
            {
                m_figure = figureCmp->getFigure();
                if (m_figure)
                {
                    auto numJoints = m_figure->NumJoints();
                    for (int i = 0; i < numJoints; ++i)
                    {
                        auto jointName = m_figure->GetJointName(i);
                        m_jointObjects[jointName] = getOwner()->findChildByName(jointName);
                    }
                    return true;
                }
            }
            m_bIsInitialized = true;
        }
        return m_bIsInitialized;
    }

    void BoneTransform::clear() {
        for (auto& [key, val] : m_jointObjects) {
            getOwner()->getScene()->removeObject(val);
            m_jointObjects[key] = nullptr;
        }
        m_jointObjects.clear();
    }

    void BoneTransform::onJointObjectSelected(const std::string& name, bool selected)
    {
        if (m_figure)
        {
            auto numJoints = m_figure->NumJoints();
            auto idx = m_figure->GetJointIndex(GenerateNameHash(name.c_str()));
            if (idx >= 0 && idx < numJoints)
            {
                auto jointName = m_figure->GetJointName(idx);
                if (selected)
                {
                    if (m_jointObjects[jointName] == nullptr)
                    {                        
                        auto jointObj = getOwner()->findChildByName(jointName);
                        std::shared_ptr<SceneObject> jointObjTmp = nullptr;
                        if (jointObj == nullptr)
                        {
                            auto scene = getOwner()->getScene();
                            if (scene) jointObj = getOwner()->getScene()->createObject(jointName, getOwner()->getSharedPtr());
                        }
                        m_jointObjects[jointName] = jointObj;
                    }
                }
                else
                {
                    if(m_jointObjects[jointName])
                        getOwner()->getScene()->removeObjectById(m_jointObjects[jointName]->getId());
                    m_jointObjects[jointName] = nullptr;
                }
            }
        }
    }

    //! Update
    void BoneTransform::onUpdate(float dt)
    {
        if (m_figure)
        {
            auto numJoints = m_figure->NumJoints();
            for (int i = 0; i < numJoints; ++i)
            {
                auto jointName = m_figure->GetJointName(i);
                auto joint = m_figure->GetJoint(i, Space::WorldSpace);
                auto jointObj = m_jointObjects[jointName];
                if (jointObj)
                {
                    auto transform = jointObj->getTransform();
                    transform->setPosition(joint.translation);
                    transform->setRotation(joint.rotation);
                    transform->setScale({ joint.scale[0], joint.scale[1], joint.scale[2] });
                }
            }
        }
    }

    void BoneTransform::onSceneObjectDeleted(SceneObject& sceneObject)
    {
        if(m_jointObjects.count(sceneObject.getName()))
            m_jointObjects[sceneObject.getName()] = nullptr;
    }

    //! Update
    void BoneTransform::onRender()
    {       
    }

    //! Enable
    void BoneTransform::onEnable()
    {
        initialize();

        // Reactive children
        for (auto& [key, val] : m_jointObjects) {
            if (val) val->setActive(true);
        }
    }

    //! Disable
    void BoneTransform::onDisable()
    {
        // DO NOT CLEAR, just deactive children for references
        for (auto& [key, val] : m_jointObjects) {
            if (val) val->setActive(false);
        }
    }

    //! Serialize
    void BoneTransform::to_json(json &j) const
    {
        Component::to_json(j);
    }

    //! Deserialize
    void BoneTransform::from_json(const json &j)
    {
        Component::from_json(j);
    }

    //! Serialize finished event
    void BoneTransform::onSerializeFinished(Scene* scene)
    {
        Component::onSerializeFinished(scene);
        initialize();
    }
} // namespace ige::scene
