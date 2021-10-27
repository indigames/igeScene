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
        if(self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject* FigureComponent_str(PyObject_FigureComponent *self)
    {
        return PyUnicode_FromString("C++ FigureComponent object");
    }
    
    // Get path
    PyObject* FigureComponent_getPath(PyObject_FigureComponent* self)
    {
        return PyUnicode_FromString(self->component->getPath().c_str());
    }

    // Set path
    int FigureComponent_setPath(PyObject_FigureComponent* self, PyObject* value)
    {
        if (PyUnicode_Check(value))
        {
            const char* val = PyUnicode_AsUTF8(value);
            self->component->setPath(std::string(val));
            return 0;
        }
        return -1;
    }

    // Get figure
    PyObject* FigureComponent_getFigure(PyObject_FigureComponent* self)
    {
        auto figureObj = (figure_obj*)(&FigureType)->tp_alloc(&FigureType, 0);
        figureObj->figure = self->component->getFigure();
        if(figureObj->figure) figureObj->figure->IncReference();
        return (PyObject*)figureObj;
        
    }

    // Fog
    PyObject* FigureComponent_isFogEnabled(PyObject_FigureComponent* self)
    {
        return PyBool_FromLong(self->component->isFogEnabled());
    }

    int FigureComponent_setFogEnabled(PyObject_FigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setFogEnabled(enable);
            return 0;
        }
        return -1;
    }

    // Front face culling
    PyObject* FigureComponent_isCullFaceEnable(PyObject_FigureComponent* self)
    {
        return PyBool_FromLong(self->component->isCullFaceEnable());
    }

    int FigureComponent_setCullFaceEnable(PyObject_FigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setCullFaceEnable(enable);
            return 0;
        }
        return -1;
    }

    // Double Side
    PyObject* FigureComponent_isDoubleSideEnable(PyObject_FigureComponent* self)
    {
        return PyBool_FromLong(self->component->isDoubleSideEnable());
    }

    int FigureComponent_setDoubleSideEnable(PyObject_FigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setDoubleSideEnable(enable);
            return 0;
        }
        return -1;
    }

    // Depth test
    PyObject* FigureComponent_isDepthTestEnable(PyObject_FigureComponent* self)
    {
        return PyBool_FromLong(self->component->isDepthTestEnable());
    }

    int FigureComponent_setDepthTestEnable(PyObject_FigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setDepthTestEnable(enable);
            return 0;
        }
        return -1;
    }

    // Depth write
    PyObject* FigureComponent_isDepthWriteEnable(PyObject_FigureComponent* self)
    {
        return PyBool_FromLong(self->component->isDepthWriteEnable());
    }

    int FigureComponent_setDepthWriteEnable(PyObject_FigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setDepthWriteEnable(enable);
            return 0;
        }
        return -1;
    }

    // Alpha blend
    PyObject* FigureComponent_isAlphaBlendingEnable(PyObject_FigureComponent* self)
    {
        return PyBool_FromLong(self->component->isAlphaBlendingEnable());
    }

    int FigureComponent_setAlphaBlendingEnable(PyObject_FigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setAlphaBlendingEnable(enable);
            return 0;
        }
        return -1;
    }

    // Alpha blend Operation
    PyObject* FigureComponent_getAlphaBlendingOp(PyObject_FigureComponent* self)
    {
        return PyLong_FromLong(self->component->getAlphaBlendingOp());
    }

    int FigureComponent_setAlphaBlendingOp(PyObject_FigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setAlphaBlendingOp(val);
            return 0;
        }
        return -1;
    }

    // Scissor Test
    PyObject* FigureComponent_isScissorTestEnable(PyObject_FigureComponent* self)
    {
        return PyBool_FromLong(self->component->isScissorTestEnable());
    }

    int FigureComponent_setScissorTestEnable(PyObject_FigureComponent* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
            auto enable = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setScissorTestEnable(enable);
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
        { "alphaBlendEnabled", (getter)FigureComponent_isAlphaBlendingEnable , (setter)FigureComponent_setAlphaBlendingEnable, FigureComponent_alphaBlendEnabled_doc, NULL },
        { "alphaBlendOp", (getter)FigureComponent_getAlphaBlendingOp , (setter)FigureComponent_setAlphaBlendingOp, FigureComponent_alphaBlendOp_doc, NULL },
        { "scissorTestEnabled", (getter)FigureComponent_isScissorTestEnable , (setter)FigureComponent_setScissorTestEnable, FigureComponent_scissorTestEnabled_doc, NULL },
        { "figure", (getter)FigureComponent_getFigure, NULL, FigureComponent_figure_doc, NULL },
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
