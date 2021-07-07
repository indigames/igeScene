#include "python/pyOffMeshLink.h"
#include "python/pyOffMeshLink_doc_en.h"
#include "python/pySceneObject.h"

#include "components/navigation/OffMeshLink.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void OffMeshLink_dealloc(PyObject_OffMeshLink *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *OffMeshLink_str(PyObject_OffMeshLink *self)
    {
        return PyUnicode_FromString("C++ OffMeshLink object");
    }

    // Radius
    PyObject *OffMeshLink_getRadius(PyObject_OffMeshLink *self)
    {
        return PyFloat_FromDouble(self->component->getRadius());
    }

    int OffMeshLink_setRadius(PyObject_OffMeshLink *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setRadius(val);
            return 0;
        }
        return -1;
    }

    //! Mask
    PyObject *OffMeshLink_getMask(PyObject_OffMeshLink *self)
    {
        return PyLong_FromLong(self->component->getMask());
    }

    int OffMeshLink_setMask(PyObject_OffMeshLink *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setMask(val);
            return 0;
        }
        return -1;
    }

    //! AreaId
    PyObject *OffMeshLink_getAreaId(PyObject_OffMeshLink *self)
    {
        return PyLong_FromLong(self->component->getAreaId());
    }

    int OffMeshLink_setAreaId(PyObject_OffMeshLink *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setAreaId(val);
            return 0;
        }
        return -1;
    }

    //! Endpoint
    PyObject *OffMeshLink_getEndPoint(PyObject_OffMeshLink *self)
    {
        auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
        obj->sceneObject = self->component->getEndPoint().get();
        return (PyObject*)obj;
    }

    int OffMeshLink_setEndPoint(PyObject_OffMeshLink *self, PyObject *value)
    {
        if (PyUnicode_Check(value))
        {
            const char* uuid = PyUnicode_AsUTF8(value);
            auto sceneObject = self->component->getOwner()->getScene()->findObjectByUUID(std::string(uuid));
            if (sceneObject)
            {
                self->component->setEndPoint(sceneObject);
                return 0;
            }
        }
        else if (value->ob_type == &PyTypeObject_SceneObject)
        {
            auto sceneObj = (PyObject_SceneObject*)value;
            auto sceneObject = self->component->getOwner()->getScene()->findObjectByUUID(std::string(sceneObj->sceneObject->getUUID()));
            self->component->setEndPoint(sceneObject);
            return 0;
        }
        return -1;
    }

    //! Bidirectional
    PyObject *OffMeshLink_isBidirectional(PyObject_OffMeshLink *self)
    {
        return PyBool_FromLong(self->component->isBidirectional());
    }

    int OffMeshLink_setBidirectional(PyObject_OffMeshLink *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setBidirectional(val);
            return 0;
        }
        return -1;
    }

    PyGetSetDef OffMeshLink_getsets[] = {
        {"radius", (getter)OffMeshLink_getRadius, (setter)OffMeshLink_setRadius, OffMeshLink_radius_doc, NULL},
        {"mask", (getter)OffMeshLink_getMask, (setter)OffMeshLink_setMask, OffMeshLink_mask_doc, NULL},
        {"areaId", (getter)OffMeshLink_getAreaId, (setter)OffMeshLink_setAreaId, OffMeshLink_areaId_doc, NULL},
        {"endpoint", (getter)OffMeshLink_getEndPoint, (setter)OffMeshLink_setEndPoint, OffMeshLink_endpoint_doc, NULL},
        {"bidirectional", (getter)OffMeshLink_isBidirectional, (setter)OffMeshLink_setBidirectional, OffMeshLink_bidirectional_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_OffMeshLink = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.OffMeshLink", /* tp_name */
        sizeof(PyObject_OffMeshLink),                          /* tp_basicsize */
        0,                                                     /* tp_itemsize */
        (destructor)OffMeshLink_dealloc,                       /* tp_dealloc */
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
        (reprfunc)OffMeshLink_str,                             /* tp_str */
        0,                                                     /* tp_getattro */
        0,                                                     /* tp_setattro */
        0,                                                     /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                    /* tp_flags */
        0,                                                     /* tp_doc */
        0,                                                     /* tp_traverse */
        0,                                                     /* tp_clear */
        0,                                                     /* tp_richcompare */
        0,                                                     /* tp_weaklistoffset */
        0,                                                     /* tp_iter */
        0,                                                     /* tp_iternext */
        0,                                                     /* tp_methods */
        0,                                                     /* tp_members */
        OffMeshLink_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                               /* tp_base */
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
