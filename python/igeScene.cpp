#include <Python.h>
#include <pythonResource.h>

#include "python/igeScene.h"
#include "python/igeScene_doc_en.h"

#include "python/pyScene.h"
#include "python/pySceneManager.h"
#include "python/pySceneObject.h"
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
#include "python/pyParticle.h"
#include "python/pyParticleManager.h"
#include "python/pyNavAgent.h"
#include "python/pyNavAgentManager.h"
#include "python/pyNavArea.h"
#include "python/pyNavMesh.h"
#include "python/pyNavObstacle.h"
#include "python/pyNavigable.h"
#include "python/pyDynamicNavMesh.h"
#include "python/pyOffMeshLink.h"

using namespace ige::scene;

static PyObject* igeScene_getVersion(PyObject* self)
{
    return PyUnicode_FromString("V0.0.1");
}

static PyMethodDef igeScene_methods[] =
{
    {"getVersion", (PyCFunction)igeScene_getVersion, METH_NOARGS, getVersion_doc },
    { nullptr, nullptr, 0, nullptr }
};

static PyModuleDef igeScene_module =
{
    PyModuleDef_HEAD_INIT,
    "igeScene",
    "IGE Scene Module",
    0,
    igeScene_methods
};

PyMODINIT_FUNC PyInit_igeScene()
{
    if (!ImportVMath()) {
        PyErr_SetString(PyExc_TypeError, "pyvmath isn't installed. please  pip install pyvmath before.");
        return NULL;
    }

    PyObject *module = PyModule_Create(&igeScene_module);

    if (PyType_Ready(&PyTypeObject_Scene) < 0) return NULL;
    Py_INCREF(&PyTypeObject_Scene);
    PyModule_AddObject(module, "Scene", (PyObject*)&PyTypeObject_Scene);

    if (PyType_Ready(&PyTypeObject_SceneManager) < 0) return NULL;
    Py_INCREF(&PyTypeObject_SceneManager);
    PyModule_AddObject(module, "SceneManager", (PyObject*)&PyTypeObject_SceneManager);

    if (PyType_Ready(&PyTypeObject_SceneObject) < 0) return NULL;
    Py_INCREF(&PyTypeObject_SceneObject);
    PyModule_AddObject(module, "SceneObject", (PyObject*)&PyTypeObject_SceneObject);

    if (PyType_Ready(&PyTypeObject_Component) < 0) return NULL;
    Py_INCREF(&PyTypeObject_Component);
    PyModule_AddObject(module, "Component", (PyObject*)&PyTypeObject_Component);

    if (PyType_Ready(&PyTypeObject_TransformComponent) < 0) return NULL;
    Py_INCREF(&PyTypeObject_TransformComponent);
    PyModule_AddObject(module, "Transform", (PyObject*)&PyTypeObject_TransformComponent);

    if (PyType_Ready(&PyTypeObject_CameraComponent) < 0) return NULL;
    Py_INCREF(&PyTypeObject_CameraComponent);
    PyModule_AddObject(module, "Camera", (PyObject*)&PyTypeObject_CameraComponent);

    if (PyType_Ready(&PyTypeObject_Script) < 0) return NULL;
    Py_INCREF(&PyTypeObject_Script);
    PyModule_AddObject(module, "Script", (PyObject*)&PyTypeObject_Script);

    if (PyType_Ready(&PyTypeObject_FigureComponent) < 0) return NULL;
    Py_INCREF(&PyTypeObject_FigureComponent);
    PyModule_AddObject(module, "Figure", (PyObject*)&PyTypeObject_FigureComponent);

    if (PyType_Ready(&PyTypeObject_EditableFigureComponent) < 0) return NULL;
    Py_INCREF(&PyTypeObject_EditableFigureComponent);
    PyModule_AddObject(module, "EditableFigure", (PyObject*)&PyTypeObject_EditableFigureComponent);

    if (PyType_Ready(&PyTypeObject_Animator) < 0) return NULL;
    Py_INCREF(&PyTypeObject_Animator);
    PyModule_AddObject(module, "Animator", (PyObject*)&PyTypeObject_Animator);
    
    if (PyType_Ready(&PyTypeObject_EnvironmentComponent) < 0) return NULL;
    Py_INCREF(&PyTypeObject_EnvironmentComponent);
    PyModule_AddObject(module, "Environment", (PyObject*)&PyTypeObject_EnvironmentComponent);

    if (PyType_Ready(&PyTypeObject_AmbientLight) < 0) return NULL;
    Py_INCREF(&PyTypeObject_AmbientLight);
    PyModule_AddObject(module, "AmbientLight", (PyObject*)&PyTypeObject_AmbientLight);

    if (PyType_Ready(&PyTypeObject_DirectionalLight) < 0) return NULL;
    Py_INCREF(&PyTypeObject_DirectionalLight);
    PyModule_AddObject(module, "DirectionalLight", (PyObject*)&PyTypeObject_DirectionalLight);

    if (PyType_Ready(&PyTypeObject_PointLight) < 0) return NULL;
    Py_INCREF(&PyTypeObject_PointLight);
    PyModule_AddObject(module, "PointLight", (PyObject*)&PyTypeObject_PointLight);

    if (PyType_Ready(&PyTypeObject_SpotLight) < 0) return NULL;
    Py_INCREF(&PyTypeObject_SpotLight);
    PyModule_AddObject(module, "SpotLight", (PyObject*)&PyTypeObject_SpotLight);

    if (PyType_Ready(&PyTypeObject_SpriteComponent) < 0) return NULL;
    Py_INCREF(&PyTypeObject_SpriteComponent);
    PyModule_AddObject(module, "Sprite", (PyObject*)&PyTypeObject_SpriteComponent);

    if (PyType_Ready(&PyTypeObject_TextComponent) < 0) return NULL;
    Py_INCREF(&PyTypeObject_TextComponent);
    PyModule_AddObject(module, "Text", (PyObject*)&PyTypeObject_TextComponent);

    if (PyType_Ready(&PyTypeObject_RectTransform) < 0) return NULL;
    Py_INCREF(&PyTypeObject_RectTransform);
    PyModule_AddObject(module, "RectTransform", (PyObject*)&PyTypeObject_RectTransform);

    if (PyType_Ready(&PyTypeObject_Canvas) < 0) return NULL;
    Py_INCREF(&PyTypeObject_Canvas);
    PyModule_AddObject(module, "Canvas", (PyObject*)&PyTypeObject_Canvas);

    if (PyType_Ready(&PyTypeObject_UIImage) < 0) return NULL;
    Py_INCREF(&PyTypeObject_UIImage);
    PyModule_AddObject(module, "UIImage", (PyObject*)&PyTypeObject_UIImage);

    if (PyType_Ready(&PyTypeObject_UIText) < 0) return NULL;
    Py_INCREF(&PyTypeObject_UIText);
    PyModule_AddObject(module, "UIText", (PyObject*)&PyTypeObject_UIText);

    if (PyType_Ready(&PyTypeObject_UITextField) < 0) return NULL;
    Py_INCREF(&PyTypeObject_UITextField);
    PyModule_AddObject(module, "UITextField", (PyObject*)&PyTypeObject_UITextField);

    if (PyType_Ready(&PyTypeObject_UIButton) < 0) return NULL;
    Py_INCREF(&PyTypeObject_UIButton);
    PyModule_AddObject(module, "UIButton", (PyObject*)&PyTypeObject_UIButton);

    if (PyType_Ready(&PyTypeObject_UISlider) < 0) return NULL;
    Py_INCREF(&PyTypeObject_UISlider);
    PyModule_AddObject(module, "UISlider", (PyObject*)&PyTypeObject_UISlider);

    if (PyType_Ready(&PyTypeObject_UIMask) < 0) return NULL;
    Py_INCREF(&PyTypeObject_UIMask);
    PyModule_AddObject(module, "UIMask", (PyObject*)&PyTypeObject_UIMask);

    if (PyType_Ready(&PyTypeObject_UIScrollView) < 0) return NULL;
    Py_INCREF(&PyTypeObject_UIScrollView);
    PyModule_AddObject(module, "UIScrollView", (PyObject*)&PyTypeObject_UIScrollView);

    if (PyType_Ready(&PyTypeObject_UIScrollBar) < 0) return NULL;
    Py_INCREF(&PyTypeObject_UIScrollBar);
    PyModule_AddObject(module, "UIScrollBar", (PyObject*)&PyTypeObject_UIScrollBar);

    if (PyType_Ready(&PyTypeObject_PhysicManager) < 0) return NULL;
    Py_INCREF(&PyTypeObject_PhysicManager);
    PyModule_AddObject(module, "PhysicManager", (PyObject*)&PyTypeObject_PhysicManager);

    if (PyType_Ready(&PyTypeObject_PhysicObject) < 0) return NULL;
    Py_INCREF(&PyTypeObject_PhysicObject);
    PyModule_AddObject(module, "PhysicObject", (PyObject*)&PyTypeObject_PhysicObject);

    if (PyType_Ready(&PyTypeObject_PhysicBox) < 0) return NULL;
    Py_INCREF(&PyTypeObject_PhysicBox);
    PyModule_AddObject(module, "PhysicBox", (PyObject*)&PyTypeObject_PhysicBox);

    if (PyType_Ready(&PyTypeObject_PhysicSphere) < 0) return NULL;
    Py_INCREF(&PyTypeObject_PhysicSphere);
    PyModule_AddObject(module, "PhysicSphere", (PyObject*)&PyTypeObject_PhysicSphere);

    if (PyType_Ready(&PyTypeObject_PhysicCapsule) < 0) return NULL;
    Py_INCREF(&PyTypeObject_PhysicCapsule);
    PyModule_AddObject(module, "PhysicCapsule", (PyObject*)&PyTypeObject_PhysicCapsule);

    if (PyType_Ready(&PyTypeObject_PhysicMesh) < 0) return NULL;
    Py_INCREF(&PyTypeObject_PhysicMesh);
    PyModule_AddObject(module, "PhysicMesh", (PyObject*)&PyTypeObject_PhysicMesh);

    if (PyType_Ready(&PyTypeObject_PhysicSoftBody) < 0) return NULL;
    Py_INCREF(&PyTypeObject_PhysicSoftBody);
    PyModule_AddObject(module, "PhysicSoftBody", (PyObject*)&PyTypeObject_PhysicSoftBody);

    if (PyType_Ready(&PyTypeObject_PhysicConstraint) < 0) return NULL;
    Py_INCREF(&PyTypeObject_PhysicConstraint);
    PyModule_AddObject(module, "PhysicConstraint", (PyObject*)&PyTypeObject_PhysicConstraint);

    if (PyType_Ready(&PyTypeObject_Dof6Constraint) < 0) return NULL;
    Py_INCREF(&PyTypeObject_Dof6Constraint);
    PyModule_AddObject(module, "Dof6Constraint", (PyObject*)&PyTypeObject_Dof6Constraint);

    if (PyType_Ready(&PyTypeObject_FixedConstraint) < 0) return NULL;
    Py_INCREF(&PyTypeObject_FixedConstraint);
    PyModule_AddObject(module, "FixedConstraint", (PyObject*)&PyTypeObject_FixedConstraint);

    if (PyType_Ready(&PyTypeObject_HingeConstraint) < 0) return NULL;
    Py_INCREF(&PyTypeObject_HingeConstraint);
    PyModule_AddObject(module, "HingeConstraint", (PyObject*)&PyTypeObject_HingeConstraint);

    if (PyType_Ready(&PyTypeObject_SliderConstraint) < 0) return NULL;
    Py_INCREF(&PyTypeObject_SliderConstraint);
    PyModule_AddObject(module, "SliderConstraint", (PyObject*)&PyTypeObject_SliderConstraint);

    if (PyType_Ready(&PyTypeObject_SpringConstraint) < 0) return NULL;
    Py_INCREF(&PyTypeObject_SpringConstraint);
    PyModule_AddObject(module, "SpringConstraint", (PyObject*)&PyTypeObject_SpringConstraint);

    if (PyType_Ready(&PyTypeObject_AudioManager) < 0) return NULL;
    Py_INCREF(&PyTypeObject_AudioManager);
    PyModule_AddObject(module, "AudioManager", (PyObject*)&PyTypeObject_AudioManager);

    if (PyType_Ready(&PyTypeObject_AudioListener) < 0) return NULL;
    Py_INCREF(&PyTypeObject_AudioListener);
    PyModule_AddObject(module, "AudioListener", (PyObject*)&PyTypeObject_AudioListener);

    if (PyType_Ready(&PyTypeObject_AudioSource) < 0) return NULL;
    Py_INCREF(&PyTypeObject_AudioSource);
    PyModule_AddObject(module, "AudioSource", (PyObject*)&PyTypeObject_AudioSource);

    if (PyType_Ready(&PyTypeObject_ParticleManager) < 0) return NULL;
    Py_INCREF(&PyTypeObject_ParticleManager);
    PyModule_AddObject(module, "ParticleManager", (PyObject*)&PyTypeObject_ParticleManager);

    if (PyType_Ready(&PyTypeObject_Particle) < 0) return NULL;
    Py_INCREF(&PyTypeObject_Particle);
    PyModule_AddObject(module, "Particle", (PyObject*)&PyTypeObject_Particle);

    if (PyType_Ready(&PyTypeObject_NavAgentManager) < 0) return NULL;
    Py_INCREF(&PyTypeObject_NavAgentManager);
    PyModule_AddObject(module, "NavAgentManager", (PyObject*)&PyTypeObject_NavAgentManager);

    if (PyType_Ready(&PyTypeObject_NavMesh) < 0) return NULL;
    Py_INCREF(&PyTypeObject_NavMesh);
    PyModule_AddObject(module, "NavMesh", (PyObject*)&PyTypeObject_NavMesh);

    if (PyType_Ready(&PyTypeObject_DynamicNavMesh) < 0) return NULL;
    Py_INCREF(&PyTypeObject_DynamicNavMesh);
    PyModule_AddObject(module, "DynamicNavMesh", (PyObject*)&PyTypeObject_DynamicNavMesh);

    if (PyType_Ready(&PyTypeObject_NavAgent) < 0) return NULL;
    Py_INCREF(&PyTypeObject_NavAgent);
    PyModule_AddObject(module, "NavAgent", (PyObject*)&PyTypeObject_NavAgent);

    if (PyType_Ready(&PyTypeObject_NavArea) < 0) return NULL;
    Py_INCREF(&PyTypeObject_NavArea);
    PyModule_AddObject(module, "NavArea", (PyObject*)&PyTypeObject_NavArea);

    if (PyType_Ready(&PyTypeObject_NavObstacle) < 0) return NULL;
    Py_INCREF(&PyTypeObject_NavObstacle);
    PyModule_AddObject(module, "NavObstacle", (PyObject*)&PyTypeObject_NavObstacle);

    if (PyType_Ready(&PyTypeObject_Navigable) < 0) return NULL;
    Py_INCREF(&PyTypeObject_Navigable);
    PyModule_AddObject(module, "Navigable", (PyObject*)&PyTypeObject_Navigable);

    if (PyType_Ready(&PyTypeObject_OffMeshLink) < 0) return NULL;
    Py_INCREF(&PyTypeObject_OffMeshLink);
    PyModule_AddObject(module, "OffMeshLink", (PyObject*)&PyTypeObject_OffMeshLink);

    return module;
}
