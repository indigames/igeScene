#include "python/pySceneObject.h"
#include "python/pySceneObject_doc_en.h"

#include "python/pyScene.h"
#include "python/pyScript.h"

#include "python/pyComponent.h"
#include "python/pyTransformComponent.h"
#include "python/pyCameraComponent.h"
#include "python/pyFigureComponent.h"
#include "python/pyEditableFigureComponent.h"
#include "python/pyEnvironmentComponent.h"
#include "python/pyAmbientLight.h"
#include "python/pyDirectionalLight.h"
#include "python/pyPointLight.h"
#include "python/pySpotLight.h"
#include "python/pySpriteComponent.h"
#include "python/pyTextComponent.h"
#include "python/pyTextBitmapComponent.h"
#include "python/pyRectTransform.h"
#include "python/pyCanvas.h"
#include "python/pyUIImage.h"
#include "python/pyUIText.h"
#include "python/pyUITextBitmap.h"
#include "python/pyUITextField.h"
#include "python/pyUIButton.h"
#include "python/pyUISlider.h"
#include "python/pyUIMask.h"
#include "python/pyUIScrollView.h"
#include "python/pyUIScrollBar.h"
#include "python/pyPhysicManager.h"
#include "python/pyPhysicBox.h"
#include "python/pyPhysicSphere.h"
#include "python/pyPhysicCapsule.h"
#include "python/pyPhysicMesh.h"
#include "python/pyPhysicSoftBody.h"
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
        if (self)
        {
            self->sceneObject = nullptr;
        }
        PyObject_Del(self);
    }

    // String representation
    PyObject *SceneObject_str(PyObject_SceneObject *self)
    {
        return PyUnicode_FromString("C++ SceneObject object");
    }

    // Get ID
    PyObject *SceneObject_getId(PyObject_SceneObject *self)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        return PyLong_FromUnsignedLongLong(self->sceneObject->getId());
    }

    // Get UUID
    PyObject *SceneObject_getUUID(PyObject_SceneObject *self)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        return PyUnicode_FromString(self->sceneObject->getUUID().c_str());
    }

    // Get name
    PyObject *SceneObject_getName(PyObject_SceneObject *self)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        return PyUnicode_FromString(self->sceneObject->getName().c_str());
    }

    // Set name
    int SceneObject_setName(PyObject_SceneObject *self, PyObject *value)
    {
        if (!self->sceneObject) return -1;
        if (PyUnicode_Check(value))
        {
            const char* val = PyUnicode_AsUTF8(value);
            self->sceneObject->setName(std::string(val));
        }
        return 0;
    }

    // Get active
    PyObject *SceneObject_getActive(PyObject_SceneObject *self)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        return PyBool_FromLong(self->sceneObject->isActive());
    }

    // Set active
    int SceneObject_setActive(PyObject_SceneObject *self, PyObject *value)
    {
        if (!self->sceneObject) return -1;
        if (PyLong_Check(value))
        {
            auto isActive = (uint32_t)PyLong_AsLong(value) != 0;
            self->sceneObject->setActive(isActive);
            return 0;
        }
        return -1;
    }

    // Get selected
    PyObject *SceneObject_getSelected(PyObject_SceneObject *self)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        return PyBool_FromLong(self->sceneObject->isSelected());
    }

    // Set selected
    int SceneObject_setSelected(PyObject_SceneObject *self, PyObject *value)
    {
        if (!self->sceneObject) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            self->sceneObject->setSelected(val);
            return 0;
        }
        return -1;
    }

    // Get parent
    PyObject *SceneObject_getParent(PyObject_SceneObject *self)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        if (self->sceneObject->getParent())
        {
            auto parent = self->sceneObject->getParent();
            auto *obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            obj->sceneObject = parent.get();
            return (PyObject *)obj;
        }
        Py_RETURN_NONE;
    }

    // Set parent
    int SceneObject_setParent(PyObject_SceneObject *self, PyObject *value)
    {
        if (!self->sceneObject) return -1;
        PyObject *obj = value;
        if (obj && obj->ob_type == &PyTypeObject_SceneObject)
        {
            auto parent = ((PyObject_SceneObject*)obj)->sceneObject;
            if (parent != nullptr)
            {
                self->sceneObject->setParent(parent->getSharedPtr());
                return 0;
            }
        }
        else
        {
            self->sceneObject->setParent(nullptr);
            return 0;
        }
        return -1;
    }

    // Get transform
    PyObject *SceneObject_getTransform(PyObject_SceneObject *self)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        auto *obj = PyObject_New(PyObject_TransformComponent, &PyTypeObject_TransformComponent);
        obj->component = self->sceneObject->getTransform().get();
        obj->super.component = obj->component;
        return (PyObject *)obj;
    }

    // Get rect transform
    PyObject *SceneObject_getRectTransform(PyObject_SceneObject *self)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        auto *obj = PyObject_New(PyObject_RectTransform, &PyTypeObject_RectTransform);
        obj->component = (RectTransform *)(self->sceneObject->getTransform().get());
        obj->super.component = obj->component;
        return (PyObject *)obj;
    }

    // Get SceneObject_getScene
    PyObject * SceneObject_getScene(PyObject_SceneObject *self)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        auto *obj = PyObject_New(PyObject_Scene, &PyTypeObject_Scene);
        obj->scene = (Scene*)(self->sceneObject->getScene());
        return (PyObject *)obj;
    }

    PyObject* SceneObject_findChildByName(PyObject_SceneObject* self, PyObject* value)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        PyObject* obj = nullptr;
        if (PyArg_ParseTuple(value, "O", &obj))
        {
            if (obj)
            {
                if (PyUnicode_Check(obj)) {
                    const char* val = PyUnicode_AsUTF8(obj);
                    auto sceneObject = self->sceneObject->findChildByName(std::string(val));
                    if (sceneObject)
                    {
                        auto* sobj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
                        sobj->sceneObject = sceneObject.get();
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
        if (!self->sceneObject) Py_RETURN_NONE;
        auto len = self->sceneObject->getChildren().size();
        if (len > 0)
        {
            auto children = self->sceneObject->getChildren();
            auto childrenTuple = PyTuple_New(len);
            for (int i = 0; i < len; ++i)
            {
                if (!children[i].expired()) {
                    auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
                    obj->sceneObject = children[i].lock().get();
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
        if (!self->sceneObject) Py_RETURN_NONE;
        if (self->sceneObject)
        {
            self->sceneObject->removeChildren();
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }

    // Add component
    PyObject *SceneObject_addComponent(PyObject_SceneObject *self, PyObject *value)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        char *typeName = nullptr;
        if (PyArg_ParseTuple(value, "s", &typeName))
        {
            auto type = std::string(typeName);
            if (type == "Transform")
            {
                auto comp = self->sceneObject->addComponent<TransformComponent>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_TransformComponent, &PyTypeObject_TransformComponent);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "Camera")
            {
                auto comp = self->sceneObject->addComponent<CameraComponent>(std::string("camera##") + std::to_string(self->sceneObject->getId()));
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_CameraComponent, &PyTypeObject_CameraComponent);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "Environment")
            {
                auto comp = self->sceneObject->addComponent<EnvironmentComponent>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_EnvironmentComponent, &PyTypeObject_EnvironmentComponent);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "AmbientLight")
            {
                auto comp = self->sceneObject->addComponent<AmbientLight>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_AmbientLight, &PyTypeObject_AmbientLight);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "DirectionalLight")
            {
                auto comp = self->sceneObject->addComponent<DirectionalLight>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_DirectionalLight, &PyTypeObject_DirectionalLight);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "PointLight")
            {
                auto comp = self->sceneObject->addComponent<PointLight>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_PointLight, &PyTypeObject_PointLight);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "SpotLight")
            {
                auto comp = self->sceneObject->addComponent<SpotLight>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_SpotLight, &PyTypeObject_SpotLight);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "Figure")
            {
                auto comp = self->sceneObject->addComponent<FigureComponent>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_FigureComponent, &PyTypeObject_FigureComponent);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "EditableFigure")
            {
                auto comp = self->sceneObject->addComponent<EditableFigureComponent>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_EditableFigureComponent, &PyTypeObject_EditableFigureComponent);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "Sprite")
            {
                auto comp = self->sceneObject->addComponent<SpriteComponent>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_SpriteComponent, &PyTypeObject_SpriteComponent);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "Text")
            {
                auto comp = self->sceneObject->addComponent<TextComponent>();
                if (comp)
                {
                    auto* compObj = PyObject_New(PyObject_TextComponent, &PyTypeObject_TextComponent);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject*)compObj;
                }
            }
            else if (type == "TextBitmap")
            {
                auto comp = self->sceneObject->addComponent<TextBitmapComponent>();
                if (comp)
                {
                    auto* compObj = PyObject_New(PyObject_TextBitmapComponent, &PyTypeObject_TextBitmapComponent);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject*)compObj;
                }
            }
            else if (type == "RectTransform")
            {
                auto comp = self->sceneObject->addComponent<RectTransform>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_RectTransform, &PyTypeObject_RectTransform);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "Canvas")
            {
                auto comp = self->sceneObject->addComponent<Canvas>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_Canvas, &PyTypeObject_Canvas);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "UIImage")
            {
                auto comp = self->sceneObject->addComponent<UIImage>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_UIImage, &PyTypeObject_UIImage);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "UIText")
            {
                auto comp = self->sceneObject->addComponent<UIText>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_UIText, &PyTypeObject_UIText);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "UITextBitmap")
            {
            auto comp = self->sceneObject->addComponent<UITextBitmap>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_UITextBitmap, &PyTypeObject_UITextBitmap);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
            }
            else if (type == "UITextField")
            {
                auto comp = self->sceneObject->addComponent<UITextField>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_UITextField, &PyTypeObject_UITextField);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "UIButton")
            {
                auto comp = self->sceneObject->addComponent<UIButton>();
                if (comp)
                {
                    auto* compObj = PyObject_New(PyObject_UIButton, &PyTypeObject_UIButton);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject*)compObj;
                }
            }
            else if (type == "UISlider")
            {
                auto comp = self->sceneObject->addComponent<UISlider>();
                if (comp)
                {
                    auto* compObj = PyObject_New(PyObject_UISlider, &PyTypeObject_UISlider);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject*)compObj;
                }
            }
            else if (type == "UIMask")
            {
                auto comp = self->sceneObject->addComponent<UIMask>();
                if (comp)
                {
                    auto* compObj = PyObject_New(PyObject_UIMask, &PyTypeObject_UIMask);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject*)compObj;
                }
            }
            else if (type == "UIScrollView")
            {
                auto comp = self->sceneObject->addComponent<UIScrollView>();
                if (comp)
                {
                    auto* compObj = PyObject_New(PyObject_UIScrollView, &PyTypeObject_UIScrollView);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject*)compObj;
                }
            }
            else if (type == "UIScrollBar")
            {
                auto comp = self->sceneObject->addComponent<UIScrollBar>();
                if (comp)
                {
                    auto* compObj = PyObject_New(PyObject_UIScrollBar, &PyTypeObject_UIScrollBar);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject*)compObj;
                }
            }
            else if (type == "PhysicManager")
            {
                auto comp = self->sceneObject->addComponent<PhysicManager>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_PhysicManager, &PyTypeObject_PhysicManager);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "PhysicBox")
            {
                auto comp = self->sceneObject->addComponent<PhysicBox>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_PhysicBox, &PyTypeObject_PhysicBox);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "PhysicSphere")
            {
                auto comp = self->sceneObject->addComponent<PhysicSphere>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_PhysicSphere, &PyTypeObject_PhysicSphere);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "PhysicCapsule")
            {
                auto comp = self->sceneObject->addComponent<PhysicCapsule>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_PhysicCapsule, &PyTypeObject_PhysicCapsule);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "PhysicMesh")
            {
                auto comp = self->sceneObject->addComponent<PhysicMesh>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_PhysicMesh, &PyTypeObject_PhysicMesh);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "PhysicSoftBody")
            {
                auto comp = self->sceneObject->addComponent<PhysicSoftBody>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_PhysicSoftBody, &PyTypeObject_PhysicSoftBody);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "AudioManager")
            {
                auto comp = self->sceneObject->addComponent<AudioManager>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_AudioManager, &PyTypeObject_AudioManager);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "AudioListener")
            {
                auto comp = self->sceneObject->addComponent<AudioListener>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_AudioListener, &PyTypeObject_AudioListener);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "AudioSource")
            {
                auto comp = self->sceneObject->addComponent<AudioSource>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_AudioSource, &PyTypeObject_AudioSource);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "Particle")
            {
                auto comp = self->sceneObject->addComponent<Particle>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_Particle, &PyTypeObject_Particle);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "ParticleManager")
            {
                auto comp = self->sceneObject->addComponent<ParticleManager>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_ParticleManager, &PyTypeObject_ParticleManager);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "NavArea")
            {
                auto comp = self->sceneObject->addComponent<NavArea>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_NavArea, &PyTypeObject_NavArea);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "NavAgentr")
            {
                auto comp = self->sceneObject->addComponent<NavAgent>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_NavAgent, &PyTypeObject_NavAgent);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "NavAgentManager")
            {
                auto comp = self->sceneObject->addComponent<NavAgentManager>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_NavAgentManager, &PyTypeObject_NavAgentManager);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "Navigable")
            {
                auto comp = self->sceneObject->addComponent<Navigable>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_Navigable, &PyTypeObject_Navigable);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "NavMesh")
            {
                auto comp = self->sceneObject->addComponent<NavMesh>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_NavMesh, &PyTypeObject_NavMesh);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "NavObstacle")
            {
                auto comp = self->sceneObject->addComponent<NavObstacle>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_NavObstacle, &PyTypeObject_NavObstacle);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "OffMeshLink")
            {
                auto comp = self->sceneObject->addComponent<OffMeshLink>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_OffMeshLink, &PyTypeObject_OffMeshLink);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
            else if (type == "DynamicNavMesh")
            {
                auto comp = self->sceneObject->addComponent<DynamicNavMesh>();
                if (comp)
                {
                    auto *compObj = PyObject_New(PyObject_DynamicNavMesh, &PyTypeObject_DynamicNavMesh);
                    compObj->component = comp.get();
                    compObj->super.component = compObj->component;
                    return (PyObject *)compObj;
                }
            }
        }
        Py_RETURN_NONE;
    }

    // Remove component
    PyObject *SceneObject_removeComponent(PyObject_SceneObject *self, PyObject *value)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        PyObject *obj = nullptr;
        if (PyArg_ParseTuple(value, "O", &obj) && obj)
        {
            if (PyUnicode_Check(obj))
            {
                auto type = std::string(PyUnicode_AsUTF8(obj));
                if (self->sceneObject->removeComponent(type))
                    Py_RETURN_TRUE;
            }
            else
            {
                auto componentObj = (PyObject_Component *)obj;
                if (self->sceneObject && componentObj && self->sceneObject->removeComponent(componentObj->component->getName()))
                    Py_RETURN_TRUE;
            }
        }
        Py_RETURN_FALSE;
    }

    // Get component by type
    PyObject* pySceneObject_getComponent(SceneObject* sceneObject, const std::string& type)
    {        
        if (sceneObject == nullptr) Py_RETURN_NONE;

        if (type == "Transform")
        {
            auto comp = sceneObject->getTransform();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_TransformComponent, &PyTypeObject_TransformComponent);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Camera")
        {
            auto comp = sceneObject->getComponent<CameraComponent>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_CameraComponent, &PyTypeObject_CameraComponent);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Environment")
        {
            auto comp = sceneObject->getComponent<EnvironmentComponent>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_EnvironmentComponent, &PyTypeObject_EnvironmentComponent);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "AmbientLight")
        {
            auto comp = sceneObject->getComponent<AmbientLight>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_AmbientLight, &PyTypeObject_AmbientLight);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "DirectionalLight")
        {
            auto comp = sceneObject->getComponent<DirectionalLight>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_DirectionalLight, &PyTypeObject_DirectionalLight);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "PointLight")
        {
            auto comp = sceneObject->getComponent<PointLight>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_PointLight, &PyTypeObject_PointLight);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "SpotLight")
        {
            auto comp = sceneObject->getComponent<SpotLight>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_SpotLight, &PyTypeObject_SpotLight);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Figure")
        {
            auto comp = sceneObject->getComponent<FigureComponent>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_FigureComponent, &PyTypeObject_FigureComponent);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "EditableFigure")
        {
            auto comp = sceneObject->getComponent<EditableFigureComponent>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_EditableFigureComponent, &PyTypeObject_EditableFigureComponent);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Sprite")
        {
            auto comp = sceneObject->getComponent<SpriteComponent>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_SpriteComponent, &PyTypeObject_SpriteComponent);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Text")
        {
            auto comp = sceneObject->getComponent<TextComponent>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_TextComponent, &PyTypeObject_TextComponent);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "TextBitmap")
        {
            auto comp = sceneObject->getComponent<TextBitmapComponent>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_TextBitmapComponent, &PyTypeObject_TextBitmapComponent);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "RectTransform")
        {
            auto comp = sceneObject->getComponent<RectTransform>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_RectTransform, &PyTypeObject_RectTransform);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Canvas")
        {
            auto comp = sceneObject->getComponent<Canvas>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_Canvas, &PyTypeObject_Canvas);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UIImage")
        {
            auto comp = sceneObject->getComponent<UIImage>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_UIImage, &PyTypeObject_UIImage);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UIText")
        {
            auto comp = sceneObject->getComponent<UIText>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_UIText, &PyTypeObject_UIText);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UITextBitmap")
        {
        auto comp = sceneObject->getComponent<UITextBitmap>();
        if (comp)
        {
            auto* compObj = PyObject_New(PyObject_UITextBitmap, &PyTypeObject_UITextBitmap);
            compObj->component = comp.get();
            compObj->super.component = compObj->component;
            return (PyObject*)compObj;
        }
        }
        else if (type == "UITextField")
        {
            auto comp = sceneObject->getComponent<UITextField>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_UITextField, &PyTypeObject_UITextField);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UIButton")
        {
            auto comp = sceneObject->getComponent<UIButton>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_UIButton, &PyTypeObject_UIButton);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UISlider")
        {
            auto comp = sceneObject->getComponent<UISlider>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_UISlider, &PyTypeObject_UISlider);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UIMask")
        {
            auto comp = sceneObject->getComponent<UIMask>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_UIMask, &PyTypeObject_UIMask);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UIScrollView")
        {
            auto comp = sceneObject->getComponent<UIScrollView>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_UIScrollView, &PyTypeObject_UIScrollView);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "UIScrollBar")
        {
            auto comp = sceneObject->getComponent<UIScrollBar>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_UIScrollBar, &PyTypeObject_UIScrollBar);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "PhysicManager")
        {
            auto comp = sceneObject->getComponent<PhysicManager>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_PhysicManager, &PyTypeObject_PhysicManager);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "PhysicObject")
        {
            auto comp = sceneObject->getComponent<PhysicObject>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_PhysicObject, &PyTypeObject_PhysicObject);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "PhysicBox")
        {
            auto comp = sceneObject->getComponent<PhysicBox>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_PhysicBox, &PyTypeObject_PhysicBox);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "PhysicSphere")
        {
            auto comp = sceneObject->getComponent<PhysicSphere>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_PhysicSphere, &PyTypeObject_PhysicSphere);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "PhysicCapsule")
        {
            auto comp = sceneObject->getComponent<PhysicCapsule>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_PhysicCapsule, &PyTypeObject_PhysicCapsule);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "PhysicMesh")
        {
            auto comp = sceneObject->getComponent<PhysicMesh>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_PhysicMesh, &PyTypeObject_PhysicMesh);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "PhysicSoftBody")
        {
            auto comp = sceneObject->getComponent<PhysicSoftBody>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_PhysicSoftBody, &PyTypeObject_PhysicSoftBody);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "AudioManager")
        {
            auto comp = sceneObject->getComponent<AudioManager>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_AudioManager, &PyTypeObject_AudioManager);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "AudioListener")
        {
            auto comp = sceneObject->getComponent<AudioListener>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_AudioListener, &PyTypeObject_AudioListener);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "AudioSource")
        {
            auto comp = sceneObject->getComponent<AudioSource>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_AudioSource, &PyTypeObject_AudioSource);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "ParticleManager")
        {
            auto comp = sceneObject->getComponent<ParticleManager>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_ParticleManager, &PyTypeObject_ParticleManager);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Particle")
        {
            auto comp = sceneObject->getComponent<Particle>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_Particle, &PyTypeObject_Particle);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "NavAgent")
        {
            auto comp = sceneObject->getComponent<NavAgent>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_NavAgent, &PyTypeObject_NavAgent);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "NavAgentManager")
        {
            auto comp = sceneObject->getComponent<NavAgentManager>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_NavAgentManager, &PyTypeObject_NavAgentManager);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "NavArea")
        {
            auto comp = sceneObject->getComponent<NavArea>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_NavArea, &PyTypeObject_NavArea);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Navigable")
        {
            auto comp = sceneObject->getComponent<Navigable>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_Navigable, &PyTypeObject_Navigable);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "NavMesh")
        {
            auto comp = sceneObject->getComponent<NavMesh>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_NavMesh, &PyTypeObject_NavMesh);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "DynamicNavMesh")
        {
            auto comp = sceneObject->getComponent<DynamicNavMesh>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_DynamicNavMesh, &PyTypeObject_DynamicNavMesh);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "NavObstacle")
        {
            auto comp = sceneObject->getComponent<NavObstacle>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_NavObstacle, &PyTypeObject_NavObstacle);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "OffMeshLink")
        {
            auto comp = sceneObject->getComponent<OffMeshLink>();
            if (comp)
            {
                auto* compObj = PyObject_New(PyObject_OffMeshLink, &PyTypeObject_OffMeshLink);
                compObj->component = comp.get();
                compObj->super.component = compObj->component;
                return (PyObject*)compObj;
            }
        }
        else if (type == "Script")
        {
            auto comp = sceneObject->getComponent<ScriptComponent>();
            if (comp)
            {
                auto pyObj = comp->getPyInstance();
                if(pyObj) 
                {
                    Py_XINCREF(pyObj);
                    return pyObj;
                }
            }
        }
        Py_RETURN_NONE;
    }

    PyObject *SceneObject_getComponent(PyObject_SceneObject *self, PyObject *value)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        char* typeName = nullptr;
        if (PyArg_ParseTuple(value, "s", &typeName))
        {
            auto type = std::string(typeName);
            return pySceneObject_getComponent(self->sceneObject, type);
        }
        Py_RETURN_NONE;
    }

    // Get components
    PyObject *SceneObject_getComponents(PyObject_SceneObject *self)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        auto len = self->sceneObject->getComponentsCount();
        if (len > 0)
        {
            auto components = self->sceneObject->getComponents();
            PyObject* pyList = PyList_New(0);
            for(int i = 0; i < len; ++i)
            {
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
        if (!self->sceneObject) Py_RETURN_NONE;
        self->sceneObject->removeAllComponents();
        Py_RETURN_TRUE;
    }

    PyObject* SceneObject_getScript(PyObject_SceneObject* self, PyObject* args)
    {
        if (!self->sceneObject) Py_RETURN_NONE;
        auto comp = self->sceneObject->getComponent<ScriptComponent>();
        if (comp)
        {
            auto pyObj = comp->getPyInstance();
            if (pyObj)
            {
                Py_XINCREF(pyObj);
                return pyObj;
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
                bool eq = (selfCmp->sceneObject->getId() == otherCmp->sceneObject->getId());
                if (op == Py_NE)
                    eq = !eq;
                result = eq ? Py_True : Py_False;
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
        {"getScript", (PyCFunction)SceneObject_getScript, METH_NOARGS, SceneObject_getScript_doc},
        {"getScene", (PyCFunction)SceneObject_getScene, METH_NOARGS, SceneObject_getScene_doc},
        {"findChildByName", (PyCFunction)SceneObject_findChildByName, METH_VARARGS, SceneObject_getChildren_doc},
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
