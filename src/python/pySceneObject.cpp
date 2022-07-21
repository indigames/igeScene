#include "python/pySceneObject.h"
#include "python/pySceneObject_doc_en.h"

#include "python/pyScene.h"
#include "python/pyScript.h"

#include "python/pyComponent.h"
#include "python/pyTransformComponent.h"
#include "python/pyCameraComponent.h"
#include "python/pyFigureComponent.h"
#include "python/pyEditableFigureComponent.h"
#include "python/pyAnimator.h"
#include "python/pyEnvironmentComponent.h"
#include "python/pyAmbientLight.h"
#include "python/pyDirectionalLight.h"
#include "python/pyPointLight.h"
#include "python/pySpotLight.h"
#include "python/pySpriteComponent.h"
#include "python/pyTextComponent.h"
#include "python/pyRectTransform.h"
#include "python/pyCanvas.h"
#include "python/pyUIImage.h"
#include "python/pyUIText.h"
#include "python/pyUITextField.h"
#include "python/pyUIButton.h"
#include "python/pyUISlider.h"
#include "python/pyUIMask.h"
#include "python/pyUIScrollView.h"
#include "python/pyUIScrollBar.h"
#include "python/pyPhysicManager.h"
#include "python/pyBoxCollider.h"
#include "python/pySphereCollider.h"
#include "python/pyCapsuleCollider.h"
#include "python/pyMeshCollider.h"
#include "python/pySoftbody.h"
#include "python/pyPhysicConstraint.h"
#include "python/pyDof6Constraint.h"
#include "python/pyFixedConstraint.h"
#include "python/pyHingeConstraint.h"
#include "python/pySliderConstraint.h"
#include "python/pySpringConstraint.h"
#include "python/pyAudioManager.h"
#include "python/pyAudioListener.h"
#include "python/pyAudioSource.h"
#include "python/pyParticleManager.h"
#include "python/pyParticle.h"
#include "python/pyNavAgentManager.h"
#include "python/pyNavAgent.h"
#include "python/pyNavMesh.h"
#include "python/pyDynamicNavMesh.h"
#include "python/pyNavArea.h"
#include "python/pyNavObstacle.h"
#include "python/pyNavigable.h"
#include "python/pyOffMeshLink.h"

#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"

namespace ige::scene
{
    // Deallocation
    void SceneObject_dealloc(PyObject_SceneObject *self)
    {
        if (self) {
            self->sceneObject.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    // String representation
    PyObject *SceneObject_str(PyObject_SceneObject *self)
    {
        return PyUnicode_FromString("C++ SceneObject object");
    }

    // Get ID
    PyObject *SceneObject_getId(PyObject_SceneObject *self)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        return PyLong_FromUnsignedLongLong(self->sceneObject.lock()->getId());
    }

    // Get UUID
    PyObject *SceneObject_getUUID(PyObject_SceneObject *self)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        return PyUnicode_FromString(self->sceneObject.lock()->getUUID().c_str());
    }

    // Get name
    PyObject *SceneObject_getName(PyObject_SceneObject *self)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        return PyUnicode_FromString(self->sceneObject.lock()->getName().c_str());
    }

    // Set name
    int SceneObject_setName(PyObject_SceneObject *self, PyObject *value)
    {
        if (self->sceneObject.expired()) return -1;
        if (PyUnicode_Check(value))
        {
            const char* val = PyUnicode_AsUTF8(value);
            self->sceneObject.lock()->setName(std::string(val));
            return 0;
        }
        return -1;
    }

    // Get active
    PyObject *SceneObject_getActive(PyObject_SceneObject *self)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(self->sceneObject.lock()->isActive());
    }

    // Set active
    int SceneObject_setActive(PyObject_SceneObject *self, PyObject *value)
    {
        if (self->sceneObject.expired()) return -1;
        if (PyLong_Check(value))
        {
            auto isActive = (uint32_t)PyLong_AsLong(value) != 0;
            self->sceneObject.lock()->setActive(isActive);
            return 0;
        }
        return -1;
    }

    // Get selected
    PyObject *SceneObject_getSelected(PyObject_SceneObject *self)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(self->sceneObject.lock()->isSelected());
    }

    // Set selected
    int SceneObject_setSelected(PyObject_SceneObject *self, PyObject *value)
    {
        if (self->sceneObject.expired()) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            self->sceneObject.lock()->setSelected(val);
            return 0;
        }
        return -1;
    }

    // Get parent
    PyObject *SceneObject_getParent(PyObject_SceneObject *self)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        if (self->sceneObject.lock()->getParent()) {
            auto *obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            obj->sceneObject = self->sceneObject.lock()->getParent();
            return (PyObject *)obj;
        }
        Py_RETURN_NONE;
    }

    // Set parent
    int SceneObject_setParent(PyObject_SceneObject *self, PyObject *value)
    {
        if (self->sceneObject.expired()) return -1;
        PyObject *obj = value;
        if (obj && obj->ob_type == &PyTypeObject_SceneObject) {
            auto parent = ((PyObject_SceneObject*)obj)->sceneObject;
            if (!parent.expired())
            {
                self->sceneObject.lock()->setParent(parent.lock());
                return 0;
            }
        }
        else
        {
            self->sceneObject.lock()->setParent(nullptr);
            return 0;
        }
        return -1;
    }

    // Get transform
    PyObject *SceneObject_getTransform(PyObject_SceneObject *self)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        auto *obj = (PyObject_TransformComponent*)(&PyTypeObject_TransformComponent)->tp_alloc(&PyTypeObject_TransformComponent, 0);
        obj->component = self->sceneObject.lock()->getTransform();
        return (PyObject *)obj;
    }

    // Get rect transform
    PyObject *SceneObject_getRectTransform(PyObject_SceneObject *self)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        auto *obj = (PyObject_RectTransform*)(&PyTypeObject_RectTransform)->tp_alloc(&PyTypeObject_RectTransform, 0);
        obj->component = self->sceneObject.lock()->getRectTransform();
        return (PyObject *)obj;
    }

    // Get SceneObject_getScene
    PyObject * SceneObject_getScene(PyObject_SceneObject *self)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        auto *obj = (PyObject_Scene*)(&PyTypeObject_Scene)->tp_alloc(&PyTypeObject_Scene, 0);
        obj->scene = self->sceneObject.lock()->getScene()->getSharedPtr();
        return (PyObject *)obj;
    }

    PyObject* SceneObject_findChildByName(PyObject_SceneObject* self, PyObject* value)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        PyObject* obj = nullptr;
        if (PyArg_ParseTuple(value, "O", &obj))
        {
            if (obj)
            {
                if (PyUnicode_Check(obj)) {
                    const char* val = PyUnicode_AsUTF8(obj);
                    auto sceneObject = self->sceneObject.lock()->findChildByName(std::string(val));
                    if (sceneObject)
                    {
                        auto* sobj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
                        sobj->sceneObject = sceneObject;
                        return (PyObject*)sobj;
                    }
                }
            }
        }
        Py_RETURN_NONE;
    }

    // Get children
    PyObject *SceneObject_getChildren(PyObject_SceneObject *self)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        auto len = self->sceneObject.lock()->getChildren().size();
        if (len > 0)
        {
            auto children = self->sceneObject.lock()->getChildren();
            auto childrenTuple = PyTuple_New(len);
            for (int i = 0; i < len; ++i)
            {
                if (!children[i].expired()) {
                    auto obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
                    obj->sceneObject = children[i].lock();
                    PyTuple_SetItem(childrenTuple, i, (PyObject*)obj);
                }
            }
            return (PyObject *)childrenTuple;
        }
        Py_RETURN_NONE;
    }

    // Remove children
    PyObject *SceneObject_removeChildren(PyObject_SceneObject *self)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        self->sceneObject.lock()->removeChildren();
        Py_RETURN_TRUE;
    }

    // Add component
    PyObject *SceneObject_addComponent(PyObject_SceneObject *self, PyObject *value)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        char *typeName = nullptr;
        if (PyArg_ParseTuple(value, "s", &typeName) && typeName != nullptr) {
            auto type = std::string((const char*)typeName);
            if (type == "Transform") {                
                auto compObj = (PyObject_TransformComponent*)(&PyTypeObject_TransformComponent)->tp_alloc(&PyTypeObject_TransformComponent, 0);
                compObj->component = self->sceneObject.lock()->addComponent<TransformComponent>();
                return (PyObject*)compObj;
            }
            else if (type == "Camera") {
                auto compObj = (PyObject_CameraComponent*)(&PyTypeObject_CameraComponent)->tp_alloc(&PyTypeObject_CameraComponent, 0);
                compObj->component = self->sceneObject.lock()->addComponent<CameraComponent>(std::string("camera##") + std::to_string(self->sceneObject.lock()->getId()));
                return (PyObject*)compObj;
            }
            else if (type == "Environment") {
                auto compObj = (PyObject_EnvironmentComponent*)(&PyTypeObject_EnvironmentComponent)->tp_alloc(&PyTypeObject_EnvironmentComponent, 0);
                compObj->component = self->sceneObject.lock()->addComponent<EnvironmentComponent>();
                return (PyObject*)compObj;
            }
            else if (type == "AmbientLight") {
                auto compObj = (PyObject_AmbientLight*)(&PyTypeObject_AmbientLight)->tp_alloc(&PyTypeObject_AmbientLight, 0);
                compObj->component = self->sceneObject.lock()->addComponent<AmbientLight>();
                return (PyObject*)compObj;                
            }
            else if (type == "DirectionalLight") {
                auto compObj = (PyObject_DirectionalLight*)(&PyTypeObject_DirectionalLight)->tp_alloc(&PyTypeObject_DirectionalLight, 0);
                compObj->component = self->sceneObject.lock()->addComponent<DirectionalLight>();
                return (PyObject*)compObj;
            }
            else if (type == "PointLight") {
                auto compObj = (PyObject_PointLight*)(&PyTypeObject_PointLight)->tp_alloc(&PyTypeObject_PointLight, 0);
                compObj->component = self->sceneObject.lock()->addComponent<PointLight>();
                return (PyObject*)compObj;
            }
            else if (type == "SpotLight") {
                auto compObj = (PyObject_SpotLight*)(&PyTypeObject_SpotLight)->tp_alloc(&PyTypeObject_SpotLight, 0);
                compObj->component = self->sceneObject.lock()->addComponent<SpotLight>();
                return (PyObject*)compObj;
            }
            else if (type == "Figure") {
                auto compObj = (PyObject_FigureComponent*)(&PyTypeObject_FigureComponent)->tp_alloc(&PyTypeObject_FigureComponent, 0);
                compObj->component = self->sceneObject.lock()->addComponent<FigureComponent>();
                return (PyObject*)compObj;
            }
            else if (type == "EditableFigure") {
                auto compObj = (PyObject_EditableFigureComponent*)(&PyTypeObject_EditableFigureComponent)->tp_alloc(&PyTypeObject_EditableFigureComponent, 0);
                compObj->component = self->sceneObject.lock()->addComponent<EditableFigureComponent>();
                return (PyObject*)compObj;
            }
            else if (type == "Animator") {
                auto compObj = (PyObject_Animator*)(&PyTypeObject_Animator)->tp_alloc(&PyTypeObject_Animator, 0);
                compObj->component = self->sceneObject.lock()->addComponent<AnimatorComponent>();
                return (PyObject*)compObj;
            }
            else if (type == "Sprite") {
                auto compObj = (PyObject_SpriteComponent*)(&PyTypeObject_SpriteComponent)->tp_alloc(&PyTypeObject_SpriteComponent, 0);
                compObj->component = self->sceneObject.lock()->addComponent<SpriteComponent>();
                return (PyObject*)compObj;
            }
            else if (type == "Text") {
                auto compObj = (PyObject_TextComponent*)(&PyTypeObject_TextComponent)->tp_alloc(&PyTypeObject_TextComponent, 0);
                compObj->component = self->sceneObject.lock()->addComponent<TextComponent>();
                return (PyObject*)compObj;
            }
            else if (type == "RectTransform") {
                auto compObj = (PyObject_RectTransform*)(&PyTypeObject_RectTransform)->tp_alloc(&PyTypeObject_RectTransform, 0);
                compObj->component = self->sceneObject.lock()->addComponent<RectTransform>();
                return (PyObject*)compObj;
            }
            else if (type == "Canvas") {
                auto compObj = (PyObject_Canvas*)(&PyTypeObject_Canvas)->tp_alloc(&PyTypeObject_Canvas, 0);
                compObj->component = self->sceneObject.lock()->addComponent<Canvas>();
                return (PyObject*)compObj;
            }
            else if (type == "UIImage") {
                auto compObj = (PyObject_UIImage*)(&PyTypeObject_UIImage)->tp_alloc(&PyTypeObject_UIImage, 0);
                compObj->component = self->sceneObject.lock()->addComponent<UIImage>();
                return (PyObject*)compObj;
            }
            else if (type == "UIText") {
                auto compObj = (PyObject_UIText*)(&PyTypeObject_UIText)->tp_alloc(&PyTypeObject_UIText, 0);
                compObj->component = self->sceneObject.lock()->addComponent<UIText>();
                return (PyObject*)compObj;
            }
            else if (type == "UITextField") {
                auto compObj = (PyObject_UITextField*)(&PyTypeObject_UITextField)->tp_alloc(&PyTypeObject_UITextField, 0);
                compObj->component = self->sceneObject.lock()->addComponent<UITextField>();
                return (PyObject*)compObj;
            }
            else if (type == "UIButton") {
                auto compObj = (PyObject_UIButton*)(&PyTypeObject_UIButton)->tp_alloc(&PyTypeObject_UIButton, 0);
                compObj->component = self->sceneObject.lock()->addComponent<UIButton>();
                return (PyObject*)compObj;
            }
            else if (type == "UISlider") {
                auto compObj = (PyObject_UISlider*)(&PyTypeObject_UISlider)->tp_alloc(&PyTypeObject_UISlider, 0);
                compObj->component = self->sceneObject.lock()->addComponent<UISlider>();
                return (PyObject*)compObj;
            }
            else if (type == "UIMask") {
                auto compObj = (PyObject_UIMask*)(&PyTypeObject_UIMask)->tp_alloc(&PyTypeObject_UIMask, 0);
                compObj->component = self->sceneObject.lock()->addComponent<UIMask>();
                return (PyObject*)compObj;
            }
            else if (type == "UIScrollView") {
                auto compObj = (PyObject_UIScrollView*)(&PyTypeObject_UIScrollView)->tp_alloc(&PyTypeObject_UIScrollView, 0);
                compObj->component = self->sceneObject.lock()->addComponent<UIScrollView>();
                return (PyObject*)compObj;
            }
            else if (type == "UIScrollBar") {
                auto compObj = (PyObject_UIScrollBar*)(&PyTypeObject_UIScrollBar)->tp_alloc(&PyTypeObject_UIScrollBar, 0);;
                compObj->component = self->sceneObject.lock()->addComponent<UIScrollBar>();
                return (PyObject*)compObj;
            }
            else if (type == "PhysicManager") {
                auto compObj = (PyObject_PhysicManager*)(&PyTypeObject_PhysicManager)->tp_alloc(&PyTypeObject_PhysicManager, 0);
                compObj->component = self->sceneObject.lock()->addComponent<PhysicManager>();
                return (PyObject*)compObj;
            }
            else if (type == "BoxCollider") {
                auto compObj = (PyObject_BoxCollider*)(&PyTypeObject_BoxCollider)->tp_alloc(&PyTypeObject_BoxCollider, 0);
                compObj->component = self->sceneObject.lock()->addComponent<BoxCollider>();
                return (PyObject*)compObj;
            }
            else if (type == "SphereCollider") {
                auto compObj = (PyObject_SphereCollider*)(&PyTypeObject_SphereCollider)->tp_alloc(&PyTypeObject_SphereCollider, 0);
                compObj->component = self->sceneObject.lock()->addComponent<SphereCollider>();
                return (PyObject*)compObj;
            }
            else if (type == "CapsuleCollider") {
                auto compObj = (PyObject_CapsuleCollider*)(&PyTypeObject_CapsuleCollider)->tp_alloc(&PyTypeObject_CapsuleCollider, 0);
                compObj->component = self->sceneObject.lock()->addComponent<CapsuleCollider>();
                return (PyObject*)compObj;
            }
            else if (type == "MeshCollider") {
                auto compObj = (PyObject_MeshCollider*)(&PyTypeObject_MeshCollider)->tp_alloc(&PyTypeObject_MeshCollider, 0);
                compObj->component = self->sceneObject.lock()->addComponent<MeshCollider>();
                return (PyObject*)compObj;
            }
            else if (type == "Rigidbody") {
                auto compObj = (PyObject_Rigidbody*)(&PyTypeObject_Rigidbody)->tp_alloc(&PyTypeObject_Rigidbody, 0);
                compObj->component = self->sceneObject.lock()->addComponent<Rigidbody>();
                return (PyObject*)compObj;
            }
            else if (type == "Softbody") {
                auto compObj = (PyObject_Softbody*)(&PyTypeObject_Softbody)->tp_alloc(&PyTypeObject_Softbody, 0);
                compObj->component = self->sceneObject.lock()->addComponent<Softbody>();
                return (PyObject*)compObj;
            }
            else if (type == "AudioManager") {
                auto compObj = (PyObject_AudioManager*)(&PyTypeObject_AudioManager)->tp_alloc(&PyTypeObject_AudioManager, 0);
                compObj->component = self->sceneObject.lock()->addComponent<AudioManager>();
                return (PyObject*)compObj;
            }
            else if (type == "AudioListener") {
                auto compObj = (PyObject_AudioListener*)(&PyTypeObject_AudioListener)->tp_alloc(&PyTypeObject_AudioListener, 0);
                compObj->component = self->sceneObject.lock()->addComponent<AudioListener>();
                return (PyObject*)compObj;
            }
            else if (type == "AudioSource") {
                auto compObj = (PyObject_AudioSource*)(&PyTypeObject_AudioSource)->tp_alloc(&PyTypeObject_AudioSource, 0);
                compObj->component = self->sceneObject.lock()->addComponent<AudioSource>();
                return (PyObject*)compObj;
            }
            else if (type == "Particle") {
                auto compObj = (PyObject_Particle*)(&PyTypeObject_Particle)->tp_alloc(&PyTypeObject_Particle, 0);
                compObj->component = self->sceneObject.lock()->addComponent<Particle>();
                return (PyObject*)compObj;
            }
            else if (type == "ParticleManager") {
                auto compObj = (PyObject_ParticleManager*)(&PyTypeObject_ParticleManager)->tp_alloc(&PyTypeObject_ParticleManager, 0);
                compObj->component = self->sceneObject.lock()->addComponent<ParticleManager>();
                return (PyObject*)compObj;
            }
            else if (type == "NavArea") {
                auto compObj = (PyObject_NavArea*)(&PyTypeObject_NavArea)->tp_alloc(&PyTypeObject_NavArea, 0);
                compObj->component = self->sceneObject.lock()->addComponent<NavArea>();
                return (PyObject*)compObj;
            }
            else if (type == "NavAgent") {
                auto compObj = (PyObject_NavAgent*)(&PyTypeObject_NavAgent)->tp_alloc(&PyTypeObject_NavAgent, 0);
                compObj->component = self->sceneObject.lock()->addComponent<NavAgent>();
                return (PyObject*)compObj;
            }
            else if (type == "NavAgentManager") {
                auto compObj = (PyObject_NavAgentManager*)(&PyTypeObject_NavAgentManager)->tp_alloc(&PyTypeObject_NavAgentManager, 0);
                compObj->component = self->sceneObject.lock()->addComponent<NavAgentManager>();
                return (PyObject*)compObj;
            }
            else if (type == "Navigable") {
                auto compObj = (PyObject_Navigable*)(&PyTypeObject_Navigable)->tp_alloc(&PyTypeObject_Navigable, 0);
                compObj->component = self->sceneObject.lock()->addComponent<Navigable>();
                return (PyObject*)compObj;
            }
            else if (type == "NavArea") {
                auto compObj = (PyObject_Navigable*)(&PyTypeObject_NavArea)->tp_alloc(&PyTypeObject_NavArea, 0);
                compObj->component = self->sceneObject.lock()->addComponent<NavArea>();
                return (PyObject*)compObj;
            }
            else if (type == "NavMesh") {
                auto compObj = (PyObject_NavMesh*)(&PyTypeObject_NavMesh)->tp_alloc(&PyTypeObject_NavMesh, 0);
                compObj->component = self->sceneObject.lock()->addComponent<NavMesh>();
                return (PyObject*)compObj;
            }
            else if (type == "NavObstacle") {
                auto compObj = (PyObject_NavObstacle*)(&PyTypeObject_NavObstacle)->tp_alloc(&PyTypeObject_NavObstacle, 0);
                compObj->component = self->sceneObject.lock()->addComponent<NavObstacle>();
                return (PyObject*)compObj;
            }
            else if (type == "OffMeshLink") {
                auto compObj = (PyObject_OffMeshLink*)(&PyTypeObject_OffMeshLink)->tp_alloc(&PyTypeObject_OffMeshLink, 0);
                compObj->component = self->sceneObject.lock()->addComponent<OffMeshLink>();
                return (PyObject*)compObj;
            }
            else if (type == "DynamicNavMesh") {
                auto compObj = (PyObject_DynamicNavMesh*)(&PyTypeObject_DynamicNavMesh)->tp_alloc(&PyTypeObject_DynamicNavMesh, 0);
                compObj->component = self->sceneObject.lock()->addComponent<DynamicNavMesh>();
                return (PyObject*)compObj;
            }
            else if (type == "Animator") {
                auto compObj = (PyObject_Animator*)(&PyTypeObject_Animator)->tp_alloc(&PyTypeObject_Animator, 0);
                compObj->component = self->sceneObject.lock()->addComponent<DynamicNavMesh>();
                return (PyObject*)compObj;
            }
        }
        Py_RETURN_NONE;
    }

    // Remove component
    PyObject *SceneObject_removeComponent(PyObject_SceneObject *self, PyObject *value)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        PyObject *obj = nullptr;
        if (PyArg_ParseTuple(value, "O", &obj) && obj)
        {
            if (PyUnicode_Check(obj))
            {
                auto type = std::string(PyUnicode_AsUTF8(obj));
                if (self->sceneObject.lock()->removeComponent(type))
                    Py_RETURN_TRUE;
            }
            else
            {
                auto componentObj = (PyObject_Component*)obj;
                if (componentObj && self->sceneObject.lock()->removeComponent(componentObj->component.lock()->getName()))
                    Py_RETURN_TRUE;
            }
        }
        Py_RETURN_FALSE;
    }

    // Get component by type
    PyObject* pySceneObject_getComponent(std::shared_ptr<SceneObject> sceneObject, const std::string& type)
    {        
        if (sceneObject == nullptr) Py_RETURN_NONE;

        if (type == "Transform") {
            auto comp = sceneObject->getTransform();
            if (comp) {
                auto* compObj = (PyObject_TransformComponent*)(&PyTypeObject_TransformComponent)->tp_alloc(&PyTypeObject_TransformComponent, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Camera") {
            auto comp = sceneObject->getComponent<CameraComponent>();
            if (comp) {
                auto* compObj = (PyObject_CameraComponent*)(&PyTypeObject_CameraComponent)->tp_alloc(&PyTypeObject_CameraComponent, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Environment") {
            auto comp = sceneObject->getComponent<EnvironmentComponent>();
            if (comp) {
                auto* compObj = (PyObject_EnvironmentComponent*)(&PyTypeObject_EnvironmentComponent)->tp_alloc(&PyTypeObject_EnvironmentComponent, 0);
                compObj->component = comp;                
                return (PyObject*)compObj;
            }
        }
        else if (type == "AmbientLight") {
            auto comp = sceneObject->getComponent<AmbientLight>();
            if (comp) {
                auto* compObj = (PyObject_AmbientLight*)(&PyTypeObject_AmbientLight)->tp_alloc(&PyTypeObject_AmbientLight, 0);
                compObj->component = comp;                
                return (PyObject*)compObj;
            }
        }
        else if (type == "DirectionalLight") {
            auto comp = sceneObject->getComponent<DirectionalLight>();
            if (comp) {
                auto* compObj = (PyObject_DirectionalLight*)(&PyTypeObject_DirectionalLight)->tp_alloc(&PyTypeObject_DirectionalLight, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "PointLight") {
            auto comp = sceneObject->getComponent<PointLight>();
            if (comp) {
                auto* compObj = (PyObject_PointLight*)(&PyTypeObject_PointLight)->tp_alloc(&PyTypeObject_PointLight, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "SpotLight") {
            auto comp = sceneObject->getComponent<SpotLight>();
            if (comp) {
                auto* compObj = (PyObject_SpotLight*)(&PyTypeObject_SpotLight)->tp_alloc(&PyTypeObject_SpotLight, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Figure") {
            auto comp = sceneObject->getComponent<FigureComponent>();
            if (comp) {
                auto* compObj = (PyObject_FigureComponent*)(&PyTypeObject_FigureComponent)->tp_alloc(&PyTypeObject_FigureComponent, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "EditableFigure") {
            auto comp = sceneObject->getComponent<EditableFigureComponent>();
            if (comp) {
                auto* compObj = (PyObject_EditableFigureComponent*)(&PyTypeObject_EditableFigureComponent)->tp_alloc(&PyTypeObject_EditableFigureComponent, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Animator") {
            auto comp = sceneObject->getComponent<AnimatorComponent>();
            if (comp) {
                auto* compObj = (PyObject_Animator*)(&PyTypeObject_Animator)->tp_alloc(&PyTypeObject_Animator, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Sprite") {
            auto comp = sceneObject->getComponent<SpriteComponent>();
            if (comp) {
                auto* compObj = (PyObject_SpriteComponent*)(&PyTypeObject_SpriteComponent)->tp_alloc(&PyTypeObject_SpriteComponent, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Text") {
            auto comp = sceneObject->getComponent<TextComponent>();
            if (comp) {
                auto* compObj = (PyObject_TextComponent*)(&PyTypeObject_TextComponent)->tp_alloc(&PyTypeObject_TextComponent, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "RectTransform") {
            auto comp = sceneObject->getRectTransform();
            if (comp) {
                auto* compObj = (PyObject_RectTransform*)(&PyTypeObject_RectTransform)->tp_alloc(&PyTypeObject_RectTransform, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Canvas") {
            auto comp = sceneObject->getComponent<Canvas>();
            if (comp) {
                auto* compObj = (PyObject_Canvas*)(&PyTypeObject_Canvas)->tp_alloc(&PyTypeObject_Canvas, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UIImage") {
            auto comp = sceneObject->getComponent<UIImage>();
            if (comp) {
                auto* compObj = (PyObject_UIImage*)(&PyTypeObject_UIImage)->tp_alloc(&PyTypeObject_UIImage, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UIText") {
            auto comp = sceneObject->getComponent<UIText>();
            if (comp) {
                auto* compObj = (PyObject_UIText*)(&PyTypeObject_UIText)->tp_alloc(&PyTypeObject_UIText, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UITextField") {
            auto comp = sceneObject->getComponent<UITextField>();
            if (comp) {
                auto* compObj = (PyObject_UITextField*)(&PyTypeObject_UITextField)->tp_alloc(&PyTypeObject_UITextField, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UIButton") {
            auto comp = sceneObject->getComponent<UIButton>();
            if (comp) {
                auto* compObj = (PyObject_UIButton*)(&PyTypeObject_UIButton)->tp_alloc(&PyTypeObject_UIButton, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UISlider") {
            auto comp = sceneObject->getComponent<UISlider>();
            if (comp) {
                auto* compObj = (PyObject_UISlider*)(&PyTypeObject_UISlider)->tp_alloc(&PyTypeObject_UISlider, 0);
                compObj->component = comp; 
                return (PyObject*)compObj;
            }
        }
        else if (type == "UIMask") {
            auto comp = sceneObject->getComponent<UIMask>();
            if (comp) {
                auto* compObj = (PyObject_UIMask*)(&PyTypeObject_UIMask)->tp_alloc(&PyTypeObject_UIMask, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UIScrollView") {
            auto comp = sceneObject->getComponent<UIScrollView>();
            if (comp) {
                auto* compObj = (PyObject_UIScrollView*)(&PyTypeObject_UIScrollView)->tp_alloc(&PyTypeObject_UIScrollView, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UIScrollBar") {
            auto comp = sceneObject->getComponent<UIScrollBar>();
            if (comp) {
                auto* compObj = (PyObject_UIScrollBar*)(&PyTypeObject_UIScrollBar)->tp_alloc(&PyTypeObject_UIScrollBar, 0);;
                compObj->component = comp; 
                return (PyObject*)compObj;
            }
        }
        else if (type == "PhysicManager") {
            auto comp = sceneObject->getComponent<PhysicManager>();
            if (comp) {
                auto* compObj = (PyObject_PhysicManager*)(&PyTypeObject_PhysicManager)->tp_alloc(&PyTypeObject_PhysicManager, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Rigidbody") {
            auto comp = sceneObject->getComponent<Rigidbody>();
            if (comp) {
                auto* compObj = (PyObject_Rigidbody*)(&PyTypeObject_Rigidbody)->tp_alloc(&PyTypeObject_Rigidbody, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "BoxCollider") {
            auto comp = sceneObject->getComponent<BoxCollider>();
            if (comp) {
                auto* compObj = (PyObject_BoxCollider*)(&PyTypeObject_BoxCollider)->tp_alloc(&PyTypeObject_BoxCollider, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "SphereCollider") {
            auto comp = sceneObject->getComponent<SphereCollider>();
            if (comp) {
                auto* compObj = (PyObject_SphereCollider*)(&PyTypeObject_SphereCollider)->tp_alloc(&PyTypeObject_SphereCollider, 0);
                compObj->component = comp; 
                return (PyObject*)compObj;
            }
        }
        else if (type == "CapsuleCollider") {
            auto comp = sceneObject->getComponent<CapsuleCollider>();
            if (comp) {
                auto* compObj = (PyObject_CapsuleCollider*)(&PyTypeObject_CapsuleCollider)->tp_alloc(&PyTypeObject_CapsuleCollider, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "MeshCollider") {
            auto comp = sceneObject->getComponent<MeshCollider>();
            if (comp) {
                auto* compObj = (PyObject_MeshCollider*)(&PyTypeObject_MeshCollider)->tp_alloc(&PyTypeObject_MeshCollider, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Rigidbody") {
            auto comp = sceneObject->getComponent<Rigidbody>();
            if (comp) {
                auto* compObj = (PyObject_Rigidbody*)(&PyTypeObject_Rigidbody)->tp_alloc(&PyTypeObject_Rigidbody, 0);
                compObj->component = comp; 
                return (PyObject*)compObj;
            }
        }
        else if (type == "Softbody") {
            auto comp = sceneObject->getComponent<Softbody>();
            if (comp) {
                auto* compObj = (PyObject_Softbody*)(&PyTypeObject_Softbody)->tp_alloc(&PyTypeObject_Softbody, 0);
                compObj->component = comp; 
                return (PyObject*)compObj;
            }
        }
        else if (type == "AudioManager") {
            auto comp = sceneObject->getComponent<AudioManager>();
            if (comp) {
                auto* compObj = (PyObject_AudioManager*)(&PyTypeObject_AudioManager)->tp_alloc(&PyTypeObject_AudioManager, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "AudioListener") {
            auto comp = sceneObject->getComponent<AudioListener>();
            if (comp) {
                auto* compObj = (PyObject_AudioListener*)(&PyTypeObject_AudioListener)->tp_alloc(&PyTypeObject_AudioListener, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "AudioSource") {
            auto comp = sceneObject->getComponent<AudioSource>();
            if (comp) {
                auto* compObj = (PyObject_AudioSource*)(&PyTypeObject_AudioSource)->tp_alloc(&PyTypeObject_AudioSource, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "ParticleManager") {
            auto comp = sceneObject->getComponent<ParticleManager>();
            if (comp) {
                auto* compObj = (PyObject_ParticleManager*)(&PyTypeObject_ParticleManager)->tp_alloc(&PyTypeObject_ParticleManager, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Particle") {
            auto comp = sceneObject->getComponent<Particle>();
            if (comp) {
                auto* compObj = (PyObject_Particle*)(&PyTypeObject_Particle)->tp_alloc(&PyTypeObject_Particle, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "NavAgent") {
            auto comp = sceneObject->getComponent<NavAgent>();
            if (comp) {
                auto* compObj = (PyObject_NavAgent*)(&PyTypeObject_NavAgent)->tp_alloc(&PyTypeObject_NavAgent, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "NavAgentManager") {
            auto comp = sceneObject->getComponent<NavAgentManager>();
            if (comp) {
                auto* compObj = (PyObject_NavAgentManager*)(&PyTypeObject_NavAgentManager)->tp_alloc(&PyTypeObject_NavAgentManager, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "NavArea") {
            auto comp = sceneObject->getComponent<NavArea>();
            if (comp) {
                auto* compObj = (PyObject_NavArea*)(&PyTypeObject_NavArea)->tp_alloc(&PyTypeObject_NavArea, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Navigable") {
            auto comp = sceneObject->getComponent<Navigable>();
            if (comp) {
                auto* compObj = (PyObject_Navigable*)(&PyTypeObject_Navigable)->tp_alloc(&PyTypeObject_Navigable, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "NavArea") {
            auto comp = sceneObject->getComponent<NavArea>();
            if (comp) {
                auto* compObj = (PyObject_NavArea*)(&PyTypeObject_NavArea)->tp_alloc(&PyTypeObject_NavArea, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "NavMesh") {
            auto comp = sceneObject->getComponent<NavMesh>();
            if (comp) {
                auto* compObj = (PyObject_NavMesh*)(&PyTypeObject_NavMesh)->tp_alloc(&PyTypeObject_NavMesh, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "DynamicNavMesh") {
            auto comp = sceneObject->getComponent<DynamicNavMesh>();
            if (comp) {
                auto* compObj = (PyObject_DynamicNavMesh*)(&PyTypeObject_DynamicNavMesh)->tp_alloc(&PyTypeObject_DynamicNavMesh, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "NavObstacle") {
            auto comp = sceneObject->getComponent<NavObstacle>();
            if (comp) {
                auto* compObj = (PyObject_NavObstacle*)(&PyTypeObject_NavObstacle)->tp_alloc(&PyTypeObject_NavObstacle, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "OffMeshLink") {
            auto comp = sceneObject->getComponent<OffMeshLink>();
            if (comp) {
                auto* compObj = (PyObject_OffMeshLink*)(&PyTypeObject_OffMeshLink)->tp_alloc(&PyTypeObject_OffMeshLink, 0);
                compObj->component = comp;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Script") {
            auto comp = sceneObject->getComponent<ScriptComponent>();
            if (comp) {
                auto pyObj = comp->getPyInstance();
                if(pyObj) {
                    Py_XINCREF(pyObj);
                    return pyObj;
                }
            }
        }
        else {
            auto components = sceneObject->getComponents();
            auto itr = std::find_if(components.begin(), components.end(), [type](const auto& elem) {
                auto scriptComp = std::dynamic_pointer_cast<ScriptComponent>(elem);
                return (scriptComp && scriptComp->getClassName().compare(type) == 0);
            });
            if (itr != components.end())
            {
                auto pyObj = std::dynamic_pointer_cast<ScriptComponent>(*itr)->getPyInstance();
                if (pyObj) {
                    Py_XINCREF(pyObj);
                    return pyObj;
                }
            }
        }
        Py_RETURN_NONE;
    }

    PyObject *SceneObject_getComponent(PyObject_SceneObject *self, PyObject *value)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        char* typeName = nullptr;
        if (PyArg_ParseTuple(value, "s", &typeName)) {
            auto type = std::string((const char*)typeName);
            return pySceneObject_getComponent(self->sceneObject.lock(), type);
        }
        Py_RETURN_NONE;
    }

    // Get components
    PyObject *SceneObject_getComponents(PyObject_SceneObject *self)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        auto len = self->sceneObject.lock()->getComponentsCount();
        if (len > 0) {
            auto components = self->sceneObject.lock()->getComponents();
            PyObject* pyList = PyList_New(0);
            for(int i = 0; i < len; ++i) {
                auto type = PyUnicode_FromString(components[i]->getName().c_str());
                auto obj = SceneObject_getComponent(self, type);
                PyList_Append(pyList, (PyObject*)obj);
                Py_XDECREF(obj);
            }
            return (PyObject*)pyList;
        }
        Py_RETURN_NONE;
    }

    // Remove components
    PyObject *SceneObject_removeComponents(PyObject_SceneObject *self)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        self->sceneObject.lock()->removeAllComponents();
        Py_RETURN_TRUE;
    }

    // Get/set child index
    PyObject* SceneObject_getChildIndex(PyObject_SceneObject* self, PyObject* value)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        PyObject* obj = nullptr;
        if (PyArg_ParseTuple(value, "O", &obj) && obj) {
            std::shared_ptr<SceneObject> sceneObject = nullptr;
            if (PyUnicode_Check(obj)) {
                auto uuid = std::string(PyUnicode_AsUTF8(obj));
                sceneObject = self->sceneObject.lock()->findChild(uuid);
            }
            else if(obj->ob_type == &PyTypeObject_SceneObject) {
                auto pySceneObject = (PyObject_SceneObject*)obj;
                sceneObject = pySceneObject->sceneObject.expired() ? nullptr : pySceneObject->sceneObject.lock();
            }
            if (sceneObject != nullptr) {
                return PyLong_FromLong(self->sceneObject.lock()->getChildIndex(sceneObject));
            }
        }
        return PyLong_FromLong(-1);
    }

    PyObject* SceneObject_setChildIndex(PyObject_SceneObject* self, PyObject* value)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        PyObject* obj = nullptr;
        int idx = -1;
        if (PyArg_ParseTuple(value, "Oi", &obj, &idx) && obj) {
            std::shared_ptr<SceneObject> sceneObject = nullptr;
            if (PyUnicode_Check(obj)) {
                auto uuid = std::string(PyUnicode_AsUTF8(obj));
                sceneObject = self->sceneObject.lock()->findChild(uuid);
            }
            else if (obj->ob_type == &PyTypeObject_SceneObject) {
                auto pySceneObject = (PyObject_SceneObject*)obj;
                sceneObject = pySceneObject->sceneObject.expired() ? nullptr : pySceneObject->sceneObject.lock();
            }
            if (sceneObject != nullptr) {
                self->sceneObject.lock()->setChildIndex(sceneObject, idx);
                Py_RETURN_TRUE;
            }
        }
        Py_RETURN_FALSE;
    }

    PyObject* SceneObject_getChildByIndex(PyObject_SceneObject* self, PyObject* value)
    {
        if (self->sceneObject.expired()) Py_RETURN_NONE;
        int idx = -1;
        if (PyArg_ParseTuple(value, "i", &idx)) {
            auto sceneObject = self->sceneObject.lock()->getChildByIndex(idx);
            if (sceneObject) {
                auto* sobj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
                sobj->sceneObject = sceneObject;
                return (PyObject*)sobj;
            }
        }
        Py_RETURN_NONE;
    }

    // Compare function
    static PyObject* SceneObject_richcompare(PyObject* self, PyObject* other, int op)
    {
        if (op == Py_LT || op == Py_LE || op == Py_GT || op == Py_GE)
        {
            PyErr_Format(PyExc_TypeError, "Only '==' and '!=' are allowed as comparison operators");
            Py_RETURN_NOTIMPLEMENTED;
        }

        PyObject* result = Py_False;
        if (self != Py_None && other != Py_None)
        {
            if (other->ob_type == &PyTypeObject_SceneObject)
            {
                auto selfCmp = (PyObject_SceneObject*)(self);
                auto otherCmp = (PyObject_SceneObject*)(other);
                if (selfCmp->sceneObject.expired() || otherCmp->sceneObject.expired()) {
                    result = Py_False;
                }
                else {
                    bool eq = (selfCmp->sceneObject.lock()->getId() == otherCmp->sceneObject.lock()->getId());
                    if (op == Py_NE)
                        eq = !eq;
                    result = eq ? Py_True : Py_False;
                }
            }
            else
            {
                result = (op == Py_EQ) ? Py_False : Py_True;
            }
        }
        else if (self == Py_None && other == Py_None)
        {
            result = (op == Py_EQ) ? Py_True : Py_False;
        }
        else
        {
            result = (op == Py_EQ) ? Py_False : Py_True;
        }
        Py_INCREF(result);
        return result;
    }

    // Methods
    PyMethodDef SceneObject_methods[] = {
        {"getChildren", (PyCFunction)SceneObject_getChildren, METH_NOARGS, SceneObject_getChildren_doc},
        {"removeChildren", (PyCFunction)SceneObject_removeChildren, METH_NOARGS, SceneObject_removeChildren_doc},
        {"addComponent", (PyCFunction)SceneObject_addComponent, METH_VARARGS, SceneObject_addComponent_doc},
        {"removeComponent", (PyCFunction)SceneObject_removeComponent, METH_VARARGS, SceneObject_removeComponent_doc},
        {"getComponent", (PyCFunction)SceneObject_getComponent, METH_VARARGS, SceneObject_getComponent_doc},
        {"getComponents", (PyCFunction)SceneObject_getComponents, METH_NOARGS, SceneObject_getComponents_doc},
        {"removeComponents", (PyCFunction)SceneObject_removeComponents, METH_VARARGS, SceneObject_removeComponents_doc},
        {"getScene", (PyCFunction)SceneObject_getScene, METH_NOARGS, SceneObject_getScene_doc},
        {"findChildByName", (PyCFunction)SceneObject_findChildByName, METH_VARARGS, SceneObject_getChildren_doc},
        {"getChildIndex", (PyCFunction)SceneObject_getChildIndex, METH_VARARGS, SceneObject_getChildIndex_doc},
        {"setChildIndex", (PyCFunction)SceneObject_setChildIndex, METH_VARARGS, SceneObject_setChildIndex_doc},
        {"getChildByIndex", (PyCFunction)SceneObject_getChildByIndex, METH_VARARGS, SceneObject_getChildByIndex_doc},
        {NULL, NULL}};

    // Get/Set
    PyGetSetDef SceneObject_getsets[] = {
        {"id", (getter)SceneObject_getId, NULL, SceneObject_id_doc, NULL},
        {"uuid", (getter)SceneObject_getUUID, NULL, SceneObject_uuid_doc, NULL},
        {"name", (getter)SceneObject_getName, (setter)SceneObject_setName, SceneObject_name_doc, NULL},
        {"active", (getter)SceneObject_getActive, (setter)SceneObject_setActive, SceneObject_active_doc, NULL},
        {"selected", (getter)SceneObject_getSelected, (setter)SceneObject_setSelected, SceneObject_selected_doc, NULL},
        {"parent", (getter)SceneObject_getParent, (setter)SceneObject_setParent, SceneObject_parent_doc, NULL},
        {"transform", (getter)SceneObject_getTransform, NULL, SceneObject_transform_doc, NULL},
        {"rectTransform", (getter)SceneObject_getRectTransform, NULL, SceneObject_rectTransform_doc, NULL},
        {"scene", (getter)SceneObject_getScene, NULL, SceneObject_scene_doc, NULL},
        {NULL, NULL}};

    // Type declaration
    PyTypeObject PyTypeObject_SceneObject = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.SceneObject", /* tp_name */
        sizeof(PyObject_SceneObject),                          /* tp_basicsize */
        0,                                                     /* tp_itemsize */
        (destructor)SceneObject_dealloc,                       /* tp_dealloc */
        0,                                                     /* tp_print */
        0,                                                     /* tp_getattr */
        0,                                                     /* tp_setattr */
        0,                                                     /* tp_reserved */
        0,                                                     /* tp_repr */
        0,                                                     /* tp_as_number */
        0,                                                     /* tp_as_sequence */
        0,                                                     /* tp_as_mapping */
        0,                                                     /* tp_hash */
        0,                                                     /* tp_call */
        (reprfunc)SceneObject_str,                             /* tp_str */
        0,                                                     /* tp_getattro */
        0,                                                     /* tp_setattro */
        0,                                                     /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                    /* tp_flags */
        0,                                                     /* tp_doc */
        0,                                                     /* tp_traverse */
        0,                                                     /* tp_clear */
        SceneObject_richcompare,                               /* tp_richcompare */
        0,                                                     /* tp_weaklistoffset */
        0,                                                     /* tp_iter */
        0,                                                     /* tp_iternext */
        SceneObject_methods,                                   /* tp_methods */
        0,                                                     /* tp_members */
        SceneObject_getsets,                                   /* tp_getset */
        0,                                                     /* tp_base */
        0,                                                     /* tp_dict */
        0,                                                     /* tp_descr_get */
        0,                                                     /* tp_descr_set */
        0,                                                     /* tp_dictoffset */
        0,                                                     /* tp_init */
        0,                                                     /* tp_alloc */
        0,                                                     /* tp_new */
        0,                                                     /* tp_free */
    };
} // namespace ige::scene
