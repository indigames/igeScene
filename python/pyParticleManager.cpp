#include "python/pyParticleManager.h"
#include "python/pyParticleManager_doc_en.h"

#include "components/particle/ParticleManager.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void ParticleManager_dealloc(PyObject_ParticleManager *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *ParticleManager_str(PyObject_ParticleManager *self)
    {
        return PyUnicode_FromString("C++ ParticleManager object");
    }

    // Get singleton instance
    PyObject* ParticleManager_getInstance()
    {
        if (SceneManager::getInstance()->getCurrentScene())
        {
            auto particleManager = SceneManager::getInstance()->getCurrentScene()->getRoot()->getComponent<ParticleManager>();
            if (particleManager)
            {
                auto* self = PyObject_New(PyObject_ParticleManager, &PyTypeObject_ParticleManager);
                self->component = particleManager.get();
                return (PyObject*)self;
            }
        }
        Py_RETURN_NONE;
    }

    //! Culling
    PyObject *ParticleManager_isCullingEnabled(PyObject_ParticleManager *self)
    {
        return PyBool_FromLong(self->component->isCullingEnabled());
    }

    int ParticleManager_setCullingEnabled(PyObject_ParticleManager *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setCullingEnabled(val);
            return 0;
        }
		return -1;
    }

    //! Culling world size
    PyObject *ParticleManager_getCullingWorldSize(PyObject_ParticleManager *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getCullingWorldSize().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int ParticleManager_setCullingWorldSize(PyObject_ParticleManager *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setCullingWorldSize(*((Vec3 *)v));
        return 0;
    }

    //! Culling layers number
    PyObject *ParticleManager_getCullingLayerNumber(PyObject_ParticleManager *self)
    {
        return PyLong_FromLong(self->component->getCullingLayerNumber());
    }

    int ParticleManager_setCullingLayerNumber(PyObject_ParticleManager *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setCullingLayerNumber(val);
            return 0;
        }
        return -1;
    }

    //! Max particles number
    PyObject *ParticleManager_getMaxParticleNumber(PyObject_ParticleManager *self)
    {
        return PyLong_FromLong(self->component->getMaxParticleNumber());
    }

    int ParticleManager_setMaxParticleNumber(PyObject_ParticleManager *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setMaxParticleNumber(val);
            return 0;
        }
        return -1;
    }

    //! Threads number
    PyObject *ParticleManager_getNumberOfThreads(PyObject_ParticleManager *self)
    {
        return PyLong_FromLong(self->component->getNumberOfThreads());
    }

    int ParticleManager_setNumberOfThreads(PyObject_ParticleManager *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setNumberOfThreads(val);
            return 0;
        }
        return -1;
    }

    // Get/Set
    PyGetSetDef ParticleManager_getsets[] = {
        {"enableCulling", (getter)ParticleManager_isCullingEnabled, (setter)ParticleManager_setCullingEnabled, ParticleManager_enableCulling_doc, NULL},
        {"cullingWorldSize", (getter)ParticleManager_getCullingWorldSize, (setter)ParticleManager_setCullingWorldSize, ParticleManager_cullingWorldSize_doc, NULL},
        {"cullingLayerNumber", (getter)ParticleManager_getCullingLayerNumber, (setter)ParticleManager_setCullingLayerNumber, ParticleManager_cullingLayerNumber_doc, NULL},
        {"maxParticleNumber", (getter)ParticleManager_getMaxParticleNumber, (setter)ParticleManager_setMaxParticleNumber, ParticleManager_maxParticleNumber_doc, NULL},
        {"threadNumber", (getter)ParticleManager_getNumberOfThreads, (setter)ParticleManager_setNumberOfThreads, ParticleManager_threadNumber_doc, NULL},
        {NULL, NULL}};

    // Methods
    PyMethodDef ParticleManager_methods[] = {
        {"getInstance", (PyCFunction)ParticleManager_getInstance, METH_NOARGS | METH_STATIC, ParticleManager_getInstance_doc},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_ParticleManager = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.ParticleManager", /* tp_name */
        sizeof(PyObject_ParticleManager),                          /* tp_basicsize */
        0,                                                         /* tp_itemsize */
        (destructor)ParticleManager_dealloc,                       /* tp_dealloc */
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
        (reprfunc)ParticleManager_str,                             /* tp_str */
        0,                                                         /* tp_getattro */
        0,                                                         /* tp_setattro */
        0,                                                         /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                        /* tp_flags */
        0,                                                         /* tp_doc */
        0,                                                         /* tp_traverse */
        0,                                                         /* tp_clear */
        0,                                                         /* tp_richcompare */
        0,                                                         /* tp_weaklistoffset */
        0,                                                         /* tp_iter */
        0,                                                         /* tp_iternext */
        ParticleManager_methods,                                     /* tp_methods */
        0,                                                         /* tp_members */
        ParticleManager_getsets,                                   /* tp_getset */
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
