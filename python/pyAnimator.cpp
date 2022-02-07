#include "python/pyAnimator.h"
#include "python/pyAnimator_doc_en.h"

#include "components/animation/AnimatorController.h"
#include "components/animation/AnimatorStateMachine.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void Animator_dealloc(PyObject_Animator *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *Animator_str(PyObject_Animator *self)
    {
        return PyUnicode_FromString("C++ Animator object");
    }

	// Controller path
    PyObject *Animator_getControllerPath(PyObject_Animator *self)
	{
		if (self->component.expired()) Py_RETURN_NONE;
		return PyUnicode_FromString(std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock())->getControllerPath().c_str());
	}
	
    int Animator_setControllerPath(PyObject_Animator *self, PyObject *value)
	{
		if (self->component.expired()) return -1;
		if (PyUnicode_Check(value)) {
            const char* val = PyUnicode_AsUTF8(value);
            std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock())->setControllerPath(std::string(val));
            return 0;
        }
        return -1;
	}

    //! isInitialized
    PyObject* Animator_isInitialized(PyObject_Animator* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock())->getController() != nullptr);
    }

    //! getParameterCount
    PyObject* Animator_getParameterCount(PyObject_Animator* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto controller = std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock())->getController();
        if(controller == nullptr) Py_RETURN_NONE;
        return PyLong_FromLong(controller->getParameters().size());
    }

    //! speed
    PyObject* Animator_getSpeed(PyObject_Animator* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto controller = std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock())->getController();
        if (controller == nullptr) Py_RETURN_NONE;
        return PyFloat_FromDouble(controller->getTimeScale());
    }
    int Animator_setSpeed(PyObject_Animator* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            auto controller = std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock())->getController();
            if (controller == nullptr) return -1;
            controller->setTimeScale(val);
            return 0;
        }
        return -1;
    }

    //! updateMode
    PyObject* Animator_getUpdateMode(PyObject_Animator* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto animator = std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock());
        if (animator == nullptr) Py_RETURN_NONE;
        return PyLong_FromLong((int)animator->getUpdateMode());
    }
    int Animator_setUpdateMode(PyObject_Animator* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (int)PyLong_AsLong(value);
            auto animator = std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock());
            if (animator == nullptr) return -1;
            animator->setUpdateMode((AnimatorComponent::UpdateMode)val);
            return 0;
        }
        return -1;
    }

    //! Get value
    PyObject* Animator_getValue(PyObject_Animator* self, PyObject* value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        char* param = {};
        if (PyArg_ParseTuple(value, "s", &param)) {
            if (param != nullptr && strlen(param) > 0) {
                auto controller = std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock())->getController();
                if (controller) {
                    auto [type, val] = controller->getParameter(std::string((const char*)param));
                    if (type == AnimatorParameterType::Bool || type == AnimatorParameterType::Trigger)
                        return PyBool_FromLong(val != 0);
                    if (type == AnimatorParameterType::Float)
                        return PyFloat_FromDouble(val);
                    if (type == AnimatorParameterType::Int)
                        return PyLong_FromLong((int)val);
                }
            }
        }
        Py_RETURN_NONE;
    }

    //! Set value
    PyObject* Animator_setValue(PyObject_Animator* self, PyObject* value)
    {
        if (self->component.expired()) Py_RETURN_FALSE;
        char* param = {};
        PyObject* pyVal = nullptr;
        if (PyArg_ParseTuple(value, "sO", &param, &pyVal)) {
            if (param != nullptr && strlen(param) > 0) {
                auto controller = std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock())->getController();
                if (controller) {
                    if (PyBool_Check(pyVal)) {
                        auto val = (bool)PyLong_AsLong(pyVal);
                        controller->setParameter(std::string((const char*)param), (int)AnimatorParameterType::Bool, val ? 1.f : 0.f);
                        Py_RETURN_TRUE;
                    }
                    else if (PyLong_Check(pyVal)) {
                        auto val = PyLong_AsLong(pyVal);
                        controller->setParameter(std::string((const char*)param), (int)AnimatorParameterType::Int, val);
                        Py_RETURN_TRUE;
                    }
                    else if (PyFloat_Check(pyVal)) {
                        auto val = PyFloat_AsDouble(pyVal);
                        controller->setParameter(std::string((const char*)param), (int)AnimatorParameterType::Float, val);
                        Py_RETURN_TRUE;
                    }                    
                }
            }
        }
        Py_RETURN_FALSE;
    }

    //! Set trigger
    PyObject* Animator_setTrigger(PyObject_Animator* self, PyObject* value)
    {
        if (self->component.expired()) Py_RETURN_FALSE;
        char* param = {};
        if (PyArg_ParseTuple(value, "s", &param)) {
            if (param != nullptr && strlen(param) > 0) {
                auto controller = std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock())->getController();
                if (controller) {
                    controller->setParameter(std::string((const char*)param), (int)AnimatorParameterType::Trigger, 1.f);
                    Py_RETURN_TRUE;
                }
            }
        }
        Py_RETURN_FALSE;
    }

    //! Reset trigger
    PyObject* Animator_resetTrigger(PyObject_Animator* self, PyObject* value)
    {
        if (self->component.expired()) Py_RETURN_FALSE;
        char* param = {};
        if (PyArg_ParseTuple(value, "s", &param)) {
            if (param != nullptr && strlen(param) > 0) {
                auto controller = std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock())->getController();
                if (controller) {
                    controller->setParameter(std::string((const char*)param), (int)AnimatorParameterType::Trigger, 0.f);
                    Py_RETURN_TRUE;
                }
            }
        }
        Py_RETURN_FALSE;
    }

    //! Rebind
    PyObject* Animator_rebind(PyObject_Animator* self)
    {
        if (self->component.expired()) Py_RETURN_FALSE;
        auto animator = std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock());
        if (animator) {
            animator->initialize();
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }

    //! hasState
    PyObject* Animator_hasState(PyObject_Animator* self, PyObject* value)
    {
        if (self->component.expired()) Py_RETURN_FALSE;
        char* state = {};
        if (PyArg_ParseTuple(value, "s", &state)) {
            auto animator = std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock());
            if (animator) {
                auto controller = animator->getController();
                if (controller) {
                    return PyBool_FromLong(controller->getStateMachine()->hasState(std::string((const char*)state)));
                }
            }
        }
        Py_RETURN_FALSE;
    }

    //! play
    PyObject* Animator_play(PyObject_Animator* self, PyObject* value)
    {
        if (self->component.expired()) Py_RETURN_FALSE;
        char* state = {};
        if (PyArg_ParseTuple(value, "s", &state)) {
            auto animator = std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock());
            if (animator) {
                auto controller = animator->getController();
                if (controller) {
                    auto statePtr = controller->getStateMachine()->findState(std::string((const char*)state));
                    if (statePtr) {
                        controller->getStateMachine()->setCurrentState(statePtr);
                    }
                }
            }
        }
        Py_RETURN_FALSE;
    }

    // Methods definition
    PyMethodDef Animator_methods[] = {
        {"getValue", (PyCFunction)Animator_getValue, METH_VARARGS, Animator_getValue_doc},
        {"setValue", (PyCFunction)Animator_setValue, METH_VARARGS, Animator_setValue_doc},
        {"setTrigger", (PyCFunction)Animator_setTrigger, METH_VARARGS, Animator_setTrigger_doc},
        {"resetTrigger", (PyCFunction)Animator_resetTrigger, METH_VARARGS, Animator_resetTrigger_doc},
        {"rebind", (PyCFunction)Animator_rebind, METH_VARARGS, Animator_rebind_doc},
        {"hasState", (PyCFunction)Animator_hasState, METH_VARARGS, Animator_hasState_doc},
        {"play", (PyCFunction)Animator_play, METH_VARARGS, Animator_play_doc},
        {NULL, NULL},
    };

    // Get/set
    PyGetSetDef Animator_getsets[] = {
        {"controllerPath", (getter)Animator_getControllerPath, (setter)Animator_setControllerPath, Animator_controllerPath_doc, NULL},
        {"isInitialized", (getter)Animator_isInitialized, NULL, Animator_isInitialized_doc, NULL},
        {"parameterCount", (getter)Animator_getParameterCount, NULL, Animator_parameterCount_doc, NULL},
        {"speed", (getter)Animator_getSpeed, (setter)Animator_setSpeed, Animator_speed_doc, NULL},
        {"updateMode", (getter)Animator_getUpdateMode, (setter)Animator_setUpdateMode, Animator_updateMode_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_Animator = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.Animator",  /* tp_name */
        sizeof(PyObject_Animator),                           /* tp_basicsize */
        0,                                                   /* tp_itemsize */
        (destructor)Animator_dealloc,                        /* tp_dealloc */
        0,                                                   /* tp_print */
        0,                                                   /* tp_getattr */
        0,                                                   /* tp_setattr */
        0,                                                   /* tp_reserved */
        0,                                                   /* tp_repr */
        0,                                                   /* tp_as_number */
        0,                                                   /* tp_as_sequence */
        0,                                                   /* tp_as_mapping */
        0,                                                   /* tp_hash */
        0,                                                   /* tp_call */
        (reprfunc)Animator_str,                              /* tp_str */
        0,                                                   /* tp_getattro */
        0,                                                   /* tp_setattro */
        0,                                                   /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                  /* tp_flags */
        0,                                                   /* tp_doc */
        0,                                                   /* tp_traverse */
        0,                                                   /* tp_clear */
        0,                                                   /* tp_richcompare */
        0,                                                   /* tp_weaklistoffset */
        0,                                                   /* tp_iter */
        0,                                                   /* tp_iternext */
        Animator_methods,                                    /* tp_methods */
        0,                                                   /* tp_members */
        Animator_getsets,                                    /* tp_getset */
        &PyTypeObject_Component,                             /* tp_base */
        0,                                                   /* tp_dict */
        0,                                                   /* tp_descr_get */
        0,                                                   /* tp_descr_set */
        0,                                                   /* tp_dictoffset */
        0,                                                   /* tp_init */
        0,                                                   /* tp_alloc */
        0,                                                   /* tp_new */
        0,                                                   /* tp_free */
    };
} // namespace ige::scene
