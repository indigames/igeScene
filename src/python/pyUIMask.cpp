#include "python/pyUIMask.h"
#include "python/pyUIMask_doc_en.h"

#include "components/gui/UIMask.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void UIMask_dealloc(PyObject_UIMask *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject* UIMask_str(PyObject_UIMask *self)
    {
        return PyUnicode_FromString("C++ UIMask object");
    }
    
    // Set Interactable
    PyObject* UIMask_getInteractable(PyObject_UIMask* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<UIMask>(self->component.lock())->isInteractable());
    }

    // Set Interactable
    int UIMask_setInteractable(PyObject_UIMask* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<UIMask>(self->component.lock())->setInteractable(val);
            return 0;
        }
        return -1;
    }

    PyObject* UIMask_getUseMask(PyObject_UIMask* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<UIMask>(self->component.lock())->isUseMask());
    }

    int UIMask_setUseMask(PyObject_UIMask* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<UIMask>(self->component.lock())->setUseMask(val);
            return 0;
        }
        return -1;
    }

    // Get/Set
    PyGetSetDef UIMask_getsets[] = {
        {"interactable", (getter)UIMask_getInteractable, (setter)UIMask_setInteractable, NULL, NULL},
        {"useMask", (getter)UIMask_getUseMask, (setter)UIMask_setUseMask, NULL, NULL},
        {NULL, NULL} };

    PyTypeObject PyTypeObject_UIMask = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.UIMask",                          /* tp_name */
        sizeof(PyObject_UIMask),                    /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)UIMask_dealloc,                  /* tp_dealloc */
        0,                                           /* tp_print */
        0,                                           /* tp_getattr */
        0,                                           /* tp_setattr */
        0,                                           /* tp_reserved */
        0,                                           /* tp_repr */
        0,                                           /* tp_as_number */
        0,                                           /* tp_as_sequence */
        0,                                           /* tp_as_mapping */
        0,                                           /* tp_hash */
        0,                                           /* tp_call */
        (reprfunc)UIMask_str,                        /* tp_str */
        0,                                           /* tp_getattro */
        0,                                           /* tp_setattro */
        0,                                           /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                          /* tp_flags */
        0,                                           /* tp_doc */
        0,                                           /* tp_traverse */
        0,                                           /* tp_clear */
        0,                                           /* tp_richcompare */
        0,                                           /* tp_weaklistoffset */
        0,                                           /* tp_iter */
        0,                                           /* tp_iternext */
        0,                                           /* tp_methods */  //! [IGE]: no methods
        0,                                           /* tp_members */
        UIMask_getsets,                              /* tp_getset */
        &PyTypeObject_UIImage,                       /* tp_base */  //! [IGE]: inheritance
        0,                                           /* tp_dict */
        0,                                           /* tp_descr_get */
        0,                                           /* tp_descr_set */
        0,                                           /* tp_dictoffset */
        0,                                           /* tp_init */
        0,                                           /* tp_alloc */
        0,                                           /* tp_new */  //! [IGE]: no new
        0,                                           /* tp_free */
    };
}
