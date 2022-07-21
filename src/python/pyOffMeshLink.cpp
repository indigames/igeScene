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
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *OffMeshLink_str(PyObject_OffMeshLink *self)
    {
        return PyUnicode_FromString("C++ OffMeshLink object");
    }

    // Radius
    PyObject *OffMeshLink_getRadius(PyObject_OffMeshLink *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<OffMeshLink>(self->component.lock())->getRadius());
    }

    int OffMeshLink_setRadius(PyObject_OffMeshLink *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<OffMeshLink>(self->component.lock())->setRadius(val);
            return 0;
        }
        return -1;
    }

    //! Mask
    PyObject *OffMeshLink_getMask(PyObject_OffMeshLink *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<OffMeshLink>(self->component.lock())->getMask());
    }

    int OffMeshLink_setMask(PyObject_OffMeshLink *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<OffMeshLink>(self->component.lock())->setMask(val);
            return 0;
        }
        return -1;
    }

    //! AreaId
    PyObject *OffMeshLink_getAreaId(PyObject_OffMeshLink *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<OffMeshLink>(self->component.lock())->getAreaId());
    }

    int OffMeshLink_setAreaId(PyObject_OffMeshLink *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<OffMeshLink>(self->component.lock())->setAreaId(val);
            return 0;
        }
        return -1;
    }

    //! Endpoint
    PyObject *OffMeshLink_getEndPoint(PyObject_OffMeshLink *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
        auto endPoint = std::dynamic_pointer_cast<OffMeshLink>(self->component.lock())->getEndPoint();
        obj->sceneObject = endPoint;
        return (PyObject*)obj;
    }

    int OffMeshLink_setEndPoint(PyObject_OffMeshLink *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyUnicode_Check(value))
        {
            const char* uuid = PyUnicode_AsUTF8(value);
            auto sceneObject = std::dynamic_pointer_cast<OffMeshLink>(self->component.lock())->getOwner()->getScene()->findObjectByUUID(std::string(uuid));
            if (sceneObject)
            {
                std::dynamic_pointer_cast<OffMeshLink>(self->component.lock())->setEndPoint(sceneObject);
                return 0;
            }
        }
        else if (value->ob_type == &PyTypeObject_SceneObject)
        {
            auto sceneObj = (PyObject_SceneObject*)value;
            if (!sceneObj->sceneObject.expired()) {
                auto sceneObject = std::dynamic_pointer_cast<OffMeshLink>(self->component.lock())->getOwner()->getScene()->findObjectByUUID(std::string(sceneObj->sceneObject.lock()->getUUID()));
                std::dynamic_pointer_cast<OffMeshLink>(self->component.lock())->setEndPoint(sceneObject);
                return 0;
            }
        }
        return -1;
    }

    //! Bidirectional
    PyObject *OffMeshLink_isBidirectional(PyObject_OffMeshLink *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<OffMeshLink>(self->component.lock())->isBidirectional());
    }

    int OffMeshLink_setBidirectional(PyObject_OffMeshLink *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<OffMeshLink>(self->component.lock())->setBidirectional(val);
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
