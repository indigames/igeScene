#include "python/pyCanvas.h"
#include "python/pyCanvas_doc_en.h"

#include "components/gui/Canvas.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void Canvas_dealloc(PyObject_Canvas *self)
    {
        if(self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject* Canvas_str(PyObject_Canvas *self)
    {
        return PyUnicode_FromString("C++ Canvas object");
    }

    // Get design canvas size
    PyObject* Canvas_getDesignCanvasSize(PyObject_Canvas* self)
    {
        if (!self->component) Py_RETURN_NONE;
        auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(self->component->getDesignCanvasSize().P(), 2, vec2Obj->v);
        vec2Obj->d = 2;
        return (PyObject*)vec2Obj;
    }

    // Set design canvas size
    int Canvas_setDesignCanvasSize(PyObject_Canvas* self, PyObject* value)
    {
        if (!self->component) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setDesignCanvasSize(*((Vec2*)v));
        return 0;
    }

    // Get target size
    PyObject* Canvas_getTargetCanvasSize(PyObject_Canvas* self)
    {
        if (!self->component) Py_RETURN_NONE;
        auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(self->component->getTargetCanvasSize().P(), 2, vec2Obj->v);
        vec2Obj->d = 2;
        return (PyObject*)vec2Obj;
    }

    // Set target size
    int Canvas_setTargetCanvasSize(PyObject_Canvas* self, PyObject* value)
    {
        if (!self->component) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setTargetCanvasSize(*((Vec2*)v));
        return 0;
    }

    PyGetSetDef Canvas_getsets[] = {
        { "design_size", (getter)Canvas_getDesignCanvasSize, (setter)Canvas_setDesignCanvasSize, Canvas_design_size_doc, NULL },
        { "target_size", (getter)Canvas_getTargetCanvasSize, (setter)Canvas_setTargetCanvasSize, Canvas_target_size_doc, NULL },
        { NULL, NULL }
    };

    PyTypeObject PyTypeObject_Canvas = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.Canvas",                           /* tp_name */
        sizeof(PyObject_Canvas),                     /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)Canvas_dealloc,                  /* tp_dealloc */
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
        (reprfunc)Canvas_str,                        /* tp_str */
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
        Canvas_getsets,                              /* tp_getset */
        &PyTypeObject_Component,                     /* tp_base */  //! [IGE]: inheritance
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
