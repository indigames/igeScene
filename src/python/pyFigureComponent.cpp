#include "python/pyFigureComponent.h"
#include "python/pyFigureComponent_doc_en.h"

#include "components/FigureComponent.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void FigureComponent_dealloc(PyObject_FigureComponent *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject* FigureComponent_str(PyObject_FigureComponent *self)
    {
        return PyUnicode_FromString("C++ FigureComponent object");
    }

    // Get path
    PyObject* FigureComponent_getPath(PyObject_FigureComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyUnicode_FromString(std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->getPath().c_str());
    }

    // Set path
    int FigureComponent_setPath(PyObject_FigureComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyUnicode_Check(value)) {
            const char* val = PyUnicode_AsUTF8(value);
            std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->setPath(std::string(val));
            return 0;
        }
        return -1;
    }

    // Set figure
    int FigureComponent_setFigure(PyObject_FigureComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if(value) {
            if (value->ob_type == &FigureType) {
                auto figObj = (figure_obj*)value;
                std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->setFigure(figObj->figure);
                return 0;
            }
            else if (value->ob_type == &_PyNone_Type) {
                std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->setFigure(nullptr);
                return 0;
            }
        }
        return -1;
    }

    // Get figure
    PyObject* FigureComponent_getFigure(PyObject_FigureComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto figure = std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->getFigure();
        if (figure) {
            auto figureObj = (figure_obj*)(&FigureType)->tp_alloc(&FigureType, 0);
            figureObj->figure = std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->getFigure();
            figureObj->figure->IncReference();
            return (PyObject*)figureObj;
        }
        Py_RETURN_NONE;
    }

    // Fog
    PyObject* FigureComponent_isFogEnabled(PyObject_FigureComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->isFogEnabled());
    }

    int FigureComponent_setFogEnabled(PyObject_FigureComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->setFogEnabled(enable);
            return 0;
        }
        return -1;
    }

    // Front face culling
    PyObject* FigureComponent_isCullFaceEnable(PyObject_FigureComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->isCullFaceEnable());
    }

    int FigureComponent_setCullFaceEnable(PyObject_FigureComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->setCullFaceEnable(enable);
            return 0;
        }
        return -1;
    }

    // Double Side
    PyObject* FigureComponent_isDoubleSideEnable(PyObject_FigureComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->isDoubleSideEnable());
    }

    int FigureComponent_setDoubleSideEnable(PyObject_FigureComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->setDoubleSideEnable(enable);
            return 0;
        }
        return -1;
    }

    // Depth test
    PyObject* FigureComponent_isDepthTestEnable(PyObject_FigureComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->isDepthTestEnable());
    }

    int FigureComponent_setDepthTestEnable(PyObject_FigureComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->setDepthTestEnable(enable);
            return 0;
        }
        return -1;
    }

    // Depth write
    PyObject* FigureComponent_isDepthWriteEnable(PyObject_FigureComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->isDepthWriteEnable());
    }

    int FigureComponent_setDepthWriteEnable(PyObject_FigureComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->setDepthWriteEnable(enable);
            return 0;
        }
        return -1;
    }

    // Scissor Test
    PyObject* FigureComponent_isScissorTestEnable(PyObject_FigureComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->isScissorTestEnable());
    }

    int FigureComponent_setScissorTestEnable(PyObject_FigureComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->setScissorTestEnable(enable);
            return 0;
        }
        return -1;
    }

    // Frame update ratio (speedup/slower effects)
    PyObject* FigureComponent_getFrameUpdateRatio(PyObject_FigureComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->getFrameUpdateRatio());
    }

    int FigureComponent_setFrameUpdateRatio(PyObject_FigureComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<FigureComponent>(self->component.lock())->setFrameUpdateRatio(val);
            return 0;
        }
        return -1;
    }

    PyGetSetDef FigureComponent_getsets[] = {
        { "path", (getter)FigureComponent_getPath, (setter)FigureComponent_setPath, FigureComponent_path_doc, NULL },
        { "fogEnabled", (getter)FigureComponent_isFogEnabled, (setter)FigureComponent_setFogEnabled, FigureComponent_fogEnabled_doc, NULL },
        { "frontFaceCullingEnabled", (getter)FigureComponent_isCullFaceEnable, (setter)FigureComponent_setCullFaceEnable, FigureComponent_frontFaceCullingEnabled_doc, NULL },
        { "doubleSideEnabled", (getter)FigureComponent_isDoubleSideEnable, (setter)FigureComponent_setDoubleSideEnable, FigureComponent_doubleSideEnabled_doc, NULL },
        { "depthTestEnabled", (getter)FigureComponent_isDepthTestEnable, (setter)FigureComponent_setDepthTestEnable, FigureComponent_depthTestEnabled_doc, NULL },
        { "depthWriteEnabled", (getter)FigureComponent_isDepthWriteEnable, (setter)FigureComponent_setDepthWriteEnable, FigureComponent_depthWriteEnabled_doc, NULL },
        { "scissorTestEnabled", (getter)FigureComponent_isScissorTestEnable , (setter)FigureComponent_setScissorTestEnable, FigureComponent_scissorTestEnabled_doc, NULL },
        { "updateRatio", (getter)FigureComponent_getFrameUpdateRatio, (setter)FigureComponent_setFrameUpdateRatio, FigureComponent_updateRatio_doc, NULL},
        { "figure", (getter)FigureComponent_getFigure, (setter)FigureComponent_setFigure, FigureComponent_figure_doc, NULL },
        { NULL, NULL }
    };

    PyTypeObject PyTypeObject_FigureComponent = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.Figure",                           /* tp_name */
        sizeof(PyObject_FigureComponent),            /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)FigureComponent_dealloc,         /* tp_dealloc */
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
        (reprfunc)FigureComponent_str,               /* tp_str */
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
        FigureComponent_getsets,                     /* tp_getset */
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
