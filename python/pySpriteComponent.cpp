#include "python/pySpriteComponent.h"
#include "python/pySpriteComponent_doc_en.h"

#include "components/SpriteComponent.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void SpriteComponent_dealloc(PyObject_SpriteComponent *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *SpriteComponent_str(PyObject_SpriteComponent *self)
    {
        return PyUnicode_FromString("C++ SpriteComponent object");
    }

    // Get path
    PyObject *SpriteComponent_getPath(PyObject_SpriteComponent *self)
    {
        return PyUnicode_FromString(self->component->getPath().c_str());
    }

    // Set path
    int SpriteComponent_setPath(PyObject_SpriteComponent *self, PyObject *value)
    {
        if (PyUnicode_Check(value))
        {
            const char* name = PyUnicode_AsUTF8(value);
            self->component->setPath(std::string(name));
            return 0;
        }
        return -1;
    }

    // Get size
    PyObject *SpriteComponent_getSize(PyObject_SpriteComponent *self)
    {
        auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(self->component->getSize().P(), 2, vec2Obj->v);
        vec2Obj->d = 2;
        return (PyObject *)vec2Obj;
    }

    // Set size
    int SpriteComponent_setSize(PyObject_SpriteComponent *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setSize(*((Vec2 *)v));
        return 0;
    }

    PyObject* SpriteComponent_isBillboard(PyObject_SpriteComponent* self)
    {
        return PyBool_FromLong(self->component->isBillboard());
    }

    int SpriteComponent_setBillboard(PyObject_SpriteComponent* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
            auto isActive = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setBillboard(isActive);
            return 0;
        }
        return -1;
    }

    // Get color
    PyObject* SpriteComponent_getColor(PyObject_SpriteComponent* self)
    {
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(self->component->getColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set color
    int SpriteComponent_setColor(PyObject_SpriteComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        self->component->setColor(*((Vec4*)v));
        return 0;
    }

    // Get Fill Method
    PyObject* SpriteComponent_getFillMethod(PyObject_SpriteComponent* self)
    {
        return PyLong_FromLong((int)self->component->getFillMethod());
    }

    // Set Fill Method
    int SpriteComponent_setFillMethod(PyObject_SpriteComponent* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setFillMethod(val);
            return 0;
        }
        return -1;
    }

    // Get Fill Origin
    PyObject* SpriteComponent_getFillOrigin(PyObject_SpriteComponent* self)
    {
        return PyLong_FromLong((int)self->component->getFillOrigin());
    }

    // Set Fill Origin
    int SpriteComponent_setFillOrigin(PyObject_SpriteComponent* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setFillOrigin(val);
            return 0;
        }
        return -1;
    }

    // Get Fill Amount
    PyObject* SpriteComponent_getFillAmount(PyObject_SpriteComponent* self)
    {
        return PyFloat_FromDouble(self->component->getFillAmount());
    }

    // Set Fill Amount
    int SpriteComponent_setFillAmount(PyObject_SpriteComponent* self, PyObject* value)
    {
        if (PyFloat_Check(value))
        {
            auto val = (float)PyFloat_AsDouble(value);
            self->component->setFillAmount(val);
            return 0;
        }
        return -1;
    }

    // Get Fill Clockwise
    PyObject* SpriteComponent_getClockwise(PyObject_SpriteComponent* self)
    {
        return PyBool_FromLong(self->component->getClockwise());
    }

    // Set Fill Clockwise
    int SpriteComponent_setClockwise(PyObject_SpriteComponent* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setClockwise(val);
            return 0;
        }
        return -1;
    }

    // Texture
    PyObject* SpriteComponent_getTexture(PyObject_SpriteComponent* self)
    {
        auto texture = self->component->getTexture();
        if (texture) {
            auto texObj = (texture_obj*)(&TextureType)->tp_alloc(&TextureType, 0);
            texObj->colortexture = self->component->getTexture();
            if (texObj->colortexture) texObj->colortexture->IncReference();
            return (PyObject*)texObj;
        }
        Py_RETURN_NONE;
    }

    int SpriteComponent_setTexture(PyObject_SpriteComponent* self, PyObject* value)
    {
        if (value) {
            if (value->ob_type == &TextureType) {
                auto texObj = (texture_obj*)value;
                self->component->setTexture(texObj->colortexture);
                return 0;
            }
            else if (value->ob_type == &_PyNone_Type) {
                self->component->setTexture(nullptr);
                return 0;
            }
        }
        return -1;
    }

    // figure
    PyObject* SpriteComponent_getFigure(PyObject_SpriteComponent* self)
    {
        auto figure = self->component->getFigure();
        if (figure) {
            auto figObj = (editablefigure_obj*)(&EditableFigureType)->tp_alloc(&EditableFigureType, 0);
            figObj->editablefigure = figure;
            figObj->editablefigure->IncReference();
            return (PyObject*)figObj;
        }
        Py_RETURN_NONE;
    }

    PyGetSetDef SpriteComponent_getsets[] = {
        {"path", (getter)SpriteComponent_getPath, (setter)SpriteComponent_setPath, SpriteComponent_path_doc, NULL},
        {"size", (getter)SpriteComponent_getSize, (setter)SpriteComponent_setSize, SpriteComponent_size_doc, NULL},
        {"isBillboard", (getter)SpriteComponent_isBillboard, (setter)SpriteComponent_setBillboard, SpriteComponent_isBillboard_doc, NULL},
        {"color", (getter)SpriteComponent_getColor, (setter)SpriteComponent_setColor, SpriteComponent_color_doc, NULL},
        {"fillMethod", (getter)SpriteComponent_getFillMethod, (setter)SpriteComponent_setFillMethod, SpriteComponent_fillMethod_doc, NULL},
        {"fillOrigin", (getter)SpriteComponent_getFillOrigin, (setter)SpriteComponent_setFillOrigin, SpriteComponent_fillOrigin_doc, NULL},
        {"fillAmount", (getter)SpriteComponent_getFillAmount, (setter)SpriteComponent_setFillAmount, SpriteComponent_fillAmount_doc, NULL},
        {"clockwise", (getter)SpriteComponent_getClockwise, (setter)SpriteComponent_setClockwise, SpriteComponent_clockwise_doc, NULL},
        {"texture", (getter)SpriteComponent_getTexture, (setter)SpriteComponent_setTexture, SpriteComponent_texture_doc, NULL},
        {"figure", (getter)SpriteComponent_getFigure, NULL, SpriteComponent_figure_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_SpriteComponent = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.Sprite",          /* tp_name */
        sizeof(PyObject_SpriteComponent),                          /* tp_basicsize */
        0,                                                         /* tp_itemsize */
        (destructor)SpriteComponent_dealloc,                       /* tp_dealloc */
        0,                                                         /* tp_print */
        0,                                                         /* tp_getattr */
        0,                                                         /* tp_setattr */
        0,                                                         /* tp_reserved */
        0,                                                         /* tp_repr */
        0,                                                         /* tp_as_number */
        0,                                                         /* tp_as_sequence */
        0,                                                         /* tp_as_mapping */
        0,                                                         /* tp_hash */
        0,                                                         /* tp_call */
        (reprfunc)SpriteComponent_str,                             /* tp_str */
        0,                                                         /* tp_getattro */
        0,                                                         /* tp_setattro */
        0,                                                         /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                  /* tp_flags */
        0,                                                         /* tp_doc */
        0,                                                         /* tp_traverse */
        0,                                                         /* tp_clear */
        0,                                                         /* tp_richcompare */
        0,                                                         /* tp_weaklistoffset */
        0,                                                         /* tp_iter */
        0,                                                         /* tp_iternext */
        0,                                                         /* tp_methods */
        0,                                                         /* tp_members */
        SpriteComponent_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                                   /* tp_base */
        0,                                                         /* tp_dict */
        0,                                                         /* tp_descr_get */
        0,                                                         /* tp_descr_set */
        0,                                                         /* tp_dictoffset */
        0,                                                         /* tp_init */
        0,                                                         /* tp_alloc */
        0,                                                         /* tp_new */
        0,                                                         /* tp_free */
    };
} // namespace ige::scene
