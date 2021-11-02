#include "python/pyEditableFigureComponent.h"
#include "python/pyEditableFigureComponent_doc_en.h"

#include "components/EditableFigureComponent.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void EditableFigureComponent_dealloc(PyObject_EditableFigureComponent *self)
    {
        if(self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject* EditableFigureComponent_str(PyObject_EditableFigureComponent *self)
    {
        return PyUnicode_FromString("C++ EditableFigureComponent object");
    }

    // Get path
    PyObject* EditableFigureComponent_getPath(PyObject_EditableFigureComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyUnicode_FromString(self->component->getPath().c_str());
    }

    // Set path
    int EditableFigureComponent_setPath(PyObject_EditableFigureComponent* self, PyObject* value)
    {
        if (PyUnicode_Check(value) && self->component)
        {
            const char* val = PyUnicode_AsUTF8(value);
            self->component->setPath(std::string(val));
            return 0;
        }
        return -1;
    }

    // Set figure
    int EditableFigureComponent_setFigure(PyObject_EditableFigureComponent* self, PyObject* value)
    {
        if(value && self->component) {
            if (value->ob_type == &EditableFigureType) {
                auto figObj = (editablefigure_obj*)value;
                self->component->setFigure(figObj->editablefigure);
                return 0;
            }
            else if (value->ob_type == &_PyNone_Type) {
                self->component->setFigure(nullptr);
                return 0;
            }
        }
        return -1;
    }

    // Get figure
    PyObject* EditableFigureComponent_getFigure(PyObject_EditableFigureComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        auto figure = self->component->getFigure();
        if (figure) {
            auto figObj = (editablefigure_obj*)(&EditableFigureType)->tp_alloc(&EditableFigureType, 0);
            figObj->editablefigure = self->component->getFigure();
            figObj->editablefigure->IncReference();
            return (PyObject*)figObj;
        }
        Py_RETURN_NONE;
    }

    // Fog
    PyObject* EditableFigureComponent_isFogEnabled(PyObject_EditableFigureComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyBool_FromLong(self->component->isFogEnabled());
    }

    int EditableFigureComponent_setFogEnabled(PyObject_EditableFigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value) && self->component)
        {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setFogEnabled(enable);
            return 0;
        }
        return -1;
    }

    // Front face culling
    PyObject* EditableFigureComponent_isCullFaceEnable(PyObject_EditableFigureComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyBool_FromLong(self->component->isCullFaceEnable());
    }

    int EditableFigureComponent_setCullFaceEnable(PyObject_EditableFigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value) && self->component)
        {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setCullFaceEnable(enable);
            return 0;
        }
        return -1;
    }

    // Double Side
    PyObject* EditableFigureComponent_isDoubleSideEnable(PyObject_EditableFigureComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyBool_FromLong(self->component->isDoubleSideEnable());
    }

    int EditableFigureComponent_setDoubleSideEnable(PyObject_EditableFigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value) && self->component)
        {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setDoubleSideEnable(enable);
            return 0;
        }
        return -1;
    }

    // Depth test
    PyObject* EditableFigureComponent_isDepthTestEnable(PyObject_EditableFigureComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyBool_FromLong(self->component->isDepthTestEnable());
    }

    int EditableFigureComponent_setDepthTestEnable(PyObject_EditableFigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value) && self->component)
        {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setDepthTestEnable(enable);
            return 0;
        }
        return -1;
    }

    // Depth write
    PyObject* EditableFigureComponent_isDepthWriteEnable(PyObject_EditableFigureComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyBool_FromLong(self->component->isDepthWriteEnable());
    }

    int EditableFigureComponent_setDepthWriteEnable(PyObject_EditableFigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value) && self->component)
        {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setDepthWriteEnable(enable);
            return 0;
        }
        return -1;
    }

    // Scissor Test
    PyObject* EditableFigureComponent_isScissorTestEnable(PyObject_EditableFigureComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyBool_FromLong(self->component->isScissorTestEnable());
    }

    int EditableFigureComponent_setScissorTestEnable(PyObject_EditableFigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value) && self->component)
        {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setScissorTestEnable(enable);
            return 0;
        }
        return -1;
    }

    // Frame update ratio (speedup/slower effects)
    PyObject* EditableFigureComponent_getFrameUpdateRatio(PyObject_EditableFigureComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyFloat_FromDouble(self->component->getFrameUpdateRatio());
    }

    int EditableFigureComponent_setFrameUpdateRatio(PyObject_EditableFigureComponent* self, PyObject* value)
    {
        if (PyFloat_Check(value) && self->component)
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setFrameUpdateRatio(val);
            return 0;
        }
        return -1;
    }

    PyGetSetDef EditableFigureComponent_getsets[] = {
        { "path", (getter)EditableFigureComponent_getPath, (setter)EditableFigureComponent_setPath, EditableFigureComponent_path_doc, NULL },
        { "fogEnabled", (getter)EditableFigureComponent_isFogEnabled, (setter)EditableFigureComponent_setFogEnabled, EditableFigureComponent_fogEnabled_doc, NULL },
        { "frontFaceCullingEnabled", (getter)EditableFigureComponent_isCullFaceEnable, (setter)EditableFigureComponent_setCullFaceEnable, EditableFigureComponent_frontFaceCullingEnabled_doc, NULL },
        { "doubleSideEnabled", (getter)EditableFigureComponent_isDoubleSideEnable, (setter)EditableFigureComponent_setDoubleSideEnable, EditableFigureComponent_doubleSideEnabled_doc, NULL },
        { "depthTestEnabled", (getter)EditableFigureComponent_isDepthTestEnable, (setter)EditableFigureComponent_setDepthTestEnable, EditableFigureComponent_depthTestEnabled_doc, NULL },
        { "depthWriteEnabled", (getter)EditableFigureComponent_isDepthWriteEnable, (setter)EditableFigureComponent_setDepthWriteEnable, EditableFigureComponent_depthWriteEnabled_doc, NULL },
        { "scissorTestEnabled", (getter)EditableFigureComponent_isScissorTestEnable , (setter)EditableFigureComponent_setScissorTestEnable, EditableFigureComponent_scissorTestEnabled_doc, NULL },
        { "updateRatio", (getter)EditableFigureComponent_getFrameUpdateRatio, (setter)EditableFigureComponent_setFrameUpdateRatio, EditableFigureComponent_updateRatio_doc, NULL},
        { "figure", (getter)EditableFigureComponent_getFigure, (setter)EditableFigureComponent_setFigure, EditableFigureComponent_figure_doc, NULL },
        { NULL, NULL }
    };

    PyTypeObject PyTypeObject_EditableFigureComponent = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.EditableFigure",                           /* tp_name */
        sizeof(PyObject_EditableFigureComponent),            /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)EditableFigureComponent_dealloc,         /* tp_dealloc */
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
        (reprfunc)EditableFigureComponent_str,               /* tp_str */
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
        EditableFigureComponent_getsets,                     /* tp_getset */
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
