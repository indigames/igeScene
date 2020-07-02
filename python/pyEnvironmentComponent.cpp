#include "python/pyEnvironmentComponent.h"
#include "python/pyEnvironmentComponent_doc_en.h"

#include "components/EnvironmentComponent.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void EnvironmentComponent_dealloc(PyObject_EnvironmentComponent *self)
    {
        if(self && self->component)
        {
            self->super.component = nullptr;
            self->component = nullptr;
            Py_TYPE(self)->tp_free(self);
        }
    }
    
    PyObject* EnvironmentComponent_str(PyObject_EnvironmentComponent *self)
    {
        return PyUnicode_FromString("C++ EnvironmentComponent object");
    }
    
    // Get Ambient Sky Color
    PyObject* EnvironmentComponent_getAmbientSkyColor(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getAmbientSkyColor().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Ambient Sky Color
    int EnvironmentComponent_setAmbientSkyColor(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setAmbientSkyColor(*((Vec3*)v));
        return 0;
    }

    // Get Ambient Ground Color
    PyObject* EnvironmentComponent_getAmbientGroundColor(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getAmbientGroundColor().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Ambient Ground Color
    int EnvironmentComponent_setAmbientGroundColor(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setAmbientGroundColor(*((Vec3*)v));
        return 0;
    }

    // Get Ambient Direction
    PyObject* EnvironmentComponent_getAmbientDirection(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getAmbientDirection().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Ambient Direction
    int EnvironmentComponent_setAmbientDirection(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setAmbientDirection(*((Vec3*)v));
        return 0;
    }

    // Get Directional Light 0 Color
    PyObject* EnvironmentComponent_getDirectionalLight0_Color(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getDirectionalLightColor(0).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Directional Light 0 Color
    int EnvironmentComponent_setDirectionalLight0_Color(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setDirectionalLightColor(0, *((Vec3*)v));
        return 0;
    }

    // Get Directional Light 0 Direction
    PyObject* EnvironmentComponent_getDirectionalLight0_Direction(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getDirectionalLightDirection(0).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Directional Light 0 Direction
    int EnvironmentComponent_setDirectionalLight0_Direction(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setDirectionalLightDirection(0, *((Vec3*)v));
        return 0;
    }

    // Get Directional Light 0 Intensity
    PyObject* EnvironmentComponent_getDirectionalLight0_Intensity(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getDirectionalLightIntensity(0));
    }

    // Set Directional Light 0 Intensity
    int EnvironmentComponent_setDirectionalLight0_Intensity(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setDirectionalLightIntensity(0, val);
            return 0;
        }
        return -1;
    }

    // Get Directional Light 1 Color
    PyObject* EnvironmentComponent_getDirectionalLight1_Color(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getDirectionalLightColor(1).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Directional Light 1 Color
    int EnvironmentComponent_setDirectionalLight1_Color(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setDirectionalLightColor(1, *((Vec3*)v));
        return 0;
    }

    // Get Directional Light 1 Direction
    PyObject* EnvironmentComponent_getDirectionalLight1_Direction(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getDirectionalLightDirection(1).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Directional Light 1 Direction
    int EnvironmentComponent_setDirectionalLight1_Direction(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setDirectionalLightDirection(1, *((Vec3*)v));
        return 0;
    }

    // Get Directional Light 1 Intensity
    PyObject* EnvironmentComponent_getDirectionalLight1_Intensity(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getDirectionalLightIntensity(1));
    }

    // Set Directional Light 1 Intensity
    int EnvironmentComponent_setDirectionalLight1_Intensity(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setDirectionalLightIntensity(1, val);
            return 0;
        }
        return -1;
    }

    // Get Directional Light 2 Color
    PyObject* EnvironmentComponent_getDirectionalLight2_Color(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getDirectionalLightColor(2).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Directional Light 2 Color
    int EnvironmentComponent_setDirectionalLight2_Color(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setDirectionalLightColor(2, *((Vec3*)v));
        return 0;
    }

    // Get Directional Light 2 Direction
    PyObject* EnvironmentComponent_getDirectionalLight2_Direction(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getDirectionalLightDirection(2).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Directional Light 2 Direction
    int EnvironmentComponent_setDirectionalLight2_Direction(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setDirectionalLightDirection(2, *((Vec3*)v));
        return 0;
    }

    // Get Directional Light 2 Intensity
    PyObject* EnvironmentComponent_getDirectionalLight2_Intensity(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getDirectionalLightIntensity(2));
    }

    // Set Directional Light 2 Intensity
    int EnvironmentComponent_setDirectionalLight2_Intensity(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setDirectionalLightIntensity(2, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 0 Position
    PyObject* EnvironmentComponent_getPointLight0_Position(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightPosition(0).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 0 Position
    int EnvironmentComponent_setPointLight0_Position(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightPosition(0, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 0 Color
    PyObject* EnvironmentComponent_getPointLight0_Color(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightColor(0).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 0 Color
    int EnvironmentComponent_setPointLight0_Color(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightColor(0, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 0 Intensity
    PyObject* EnvironmentComponent_getPointLight0_Intensity(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightIntensity(0));
    }

    // Set Point Light 0 Intensity
    int EnvironmentComponent_setPointLight0_Intensity(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightIntensity(0, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 0 Range
    PyObject* EnvironmentComponent_getPointLight0_Range(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightRange(0));
    }

    // Set Point Light 0 Range
    int EnvironmentComponent_setPointLight0_Range(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightRange(0, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 1 Position
    PyObject* EnvironmentComponent_getPointLight1_Position(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightPosition(1).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 1 Position
    int EnvironmentComponent_setPointLight1_Position(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightPosition(1, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 1 Color
    PyObject* EnvironmentComponent_getPointLight1_Color(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightColor(1).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 1 Color
    int EnvironmentComponent_setPointLight1_Color(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightColor(1, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 1 Intensity
    PyObject* EnvironmentComponent_getPointLight1_Intensity(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightIntensity(1));
    }

    // Set Point Light 1 Intensity
    int EnvironmentComponent_setPointLight1_Intensity(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightIntensity(1, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 1 Range
    PyObject* EnvironmentComponent_getPointLight1_Range(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightRange(1));
    }

    // Set Point Light 1 Range
    int EnvironmentComponent_setPointLight1_Range(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightRange(1, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 2 Position
    PyObject* EnvironmentComponent_getPointLight2_Position(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightPosition(2).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 2 Position
    int EnvironmentComponent_setPointLight2_Position(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightPosition(2, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 2 Color
    PyObject* EnvironmentComponent_getPointLight2_Color(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightColor(2).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 2 Color
    int EnvironmentComponent_setPointLight2_Color(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightColor(2, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 2 Intensity
    PyObject* EnvironmentComponent_getPointLight2_Intensity(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightIntensity(2));
    }

    // Set Point Light 2 Intensity
    int EnvironmentComponent_setPointLight2_Intensity(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightIntensity(2, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 2 Range
    PyObject* EnvironmentComponent_getPointLight2_Range(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightRange(2));
    }

    // Set Point Light 2 Range
    int EnvironmentComponent_setPointLight2_Range(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightRange(2, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 3 Position
    PyObject* EnvironmentComponent_getPointLight3_Position(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightPosition(3).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 3 Position
    int EnvironmentComponent_setPointLight3_Position(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightPosition(3, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 3 Color
    PyObject* EnvironmentComponent_getPointLight3_Color(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightColor(3).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 3 Color
    int EnvironmentComponent_setPointLight3_Color(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightColor(3, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 3 Intensity
    PyObject* EnvironmentComponent_getPointLight3_Intensity(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightIntensity(3));
    }

    // Set Point Light 3 Intensity
    int EnvironmentComponent_setPointLight3_Intensity(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightIntensity(3, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 3 Range
    PyObject* EnvironmentComponent_getPointLight3_Range(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightRange(3));
    }

    // Set Point Light 3 Range
    int EnvironmentComponent_setPointLight3_Range(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightRange(3, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 4 Position
    PyObject* EnvironmentComponent_getPointLight4_Position(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightPosition(4).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 4 Position
    int EnvironmentComponent_setPointLight4_Position(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightPosition(4, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 4 Color
    PyObject* EnvironmentComponent_getPointLight4_Color(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightColor(4).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 4 Color
    int EnvironmentComponent_setPointLight4_Color(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightColor(4, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 4 Intensity
    PyObject* EnvironmentComponent_getPointLight4_Intensity(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightIntensity(4));
    }

    // Set Point Light 4 Intensity
    int EnvironmentComponent_setPointLight4_Intensity(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightIntensity(4, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 4 Range
    PyObject* EnvironmentComponent_getPointLight4_Range(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightRange(4));
    }

    // Set Point Light 4 Range
    int EnvironmentComponent_setPointLight4_Range(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightRange(4, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 5 Position
    PyObject* EnvironmentComponent_getPointLight5_Position(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightPosition(5).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 5 Position
    int EnvironmentComponent_setPointLight5_Position(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightPosition(5, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 5 Color
    PyObject* EnvironmentComponent_getPointLight5_Color(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightColor(5).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 5 Color
    int EnvironmentComponent_setPointLight5_Color(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightColor(5, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 5 Intensity
    PyObject* EnvironmentComponent_getPointLight5_Intensity(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightIntensity(5));
    }

    // Set Point Light 5 Intensity
    int EnvironmentComponent_setPointLight5_Intensity(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightIntensity(5, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 5 Range
    PyObject* EnvironmentComponent_getPointLight5_Range(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightRange(5));
    }

    // Set Point Light 5 Range
    int EnvironmentComponent_setPointLight5_Range(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightRange(5, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 6 Position
    PyObject* EnvironmentComponent_getPointLight6_Position(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightPosition(6).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 6 Position
    int EnvironmentComponent_setPointLight6_Position(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightPosition(6, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 6 Color
    PyObject* EnvironmentComponent_getPointLight6_Color(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPointLightColor(6).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Point Light 6 Color
    int EnvironmentComponent_setPointLight6_Color(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPointLightColor(6, *((Vec3*)v));
        return 0;
    }

    // Get Point Light 6 Intensity
    PyObject* EnvironmentComponent_getPointLight6_Intensity(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightIntensity(6));
    }

    // Set Point Light 6 Intensity
    int EnvironmentComponent_setPointLight6_Intensity(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightIntensity(6, val);
            return 0;
        }
        return -1;
    }

    // Get Point Light 6 Range
    PyObject* EnvironmentComponent_getPointLight6_Range(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getPointLightRange(6));
    }

    // Set Point Light 6 Range
    int EnvironmentComponent_setPointLight6_Range(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPointLightRange(6, val);
            return 0;
        }
        return -1;
    }

    // Get Distance Fog Color
    PyObject* EnvironmentComponent_getDistanceFogColor(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getDistanceFogColor().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Distance Fog Color
    int EnvironmentComponent_setDistanceFogColor(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setDistanceFogColor(*((Vec3*)v));
        return 0;
    }

    // Get Distance Fog Alpha
    PyObject* EnvironmentComponent_getDistanceFogAlpha(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getDistanceFogAlpha());
    }

    // Set Distance Fog Alpha
    int EnvironmentComponent_setDistanceFogAlpha(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setDistanceFogAlpha(val);
            return 0;
        }
        return -1;
    }

    // Get Distance Fog Near
    PyObject* EnvironmentComponent_getDistanceFogNear(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getDistanceFogNear());
    }

    // Set Distance Fog Near
    int EnvironmentComponent_setDistanceFogNear(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setDistanceFogNear(val);
            return 0;
        }
        return -1;
    }

    // Get Distance Fog Far
    PyObject* EnvironmentComponent_getDistanceFogFar(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getDistanceFogFar());
    }

    // Set Distance Fog Far
    int EnvironmentComponent_setDistanceFogFar(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setDistanceFogFar(val);
            return 0;
        }
        return -1;
    }

    // Get Shadow Color
    PyObject* EnvironmentComponent_getShadowColor(PyObject_EnvironmentComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getShadowColor().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Shadow Color
    int EnvironmentComponent_setShadowColor(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setShadowColor(*((Vec3*)v));
        return 0;
    }

    // Get Shadow Density
    PyObject* EnvironmentComponent_getShadowDensity(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getShadowDensity());
    }

    // Set Shadow Density
    int EnvironmentComponent_setShadowDensity(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setShadowDensity(val);
            return 0;
        }
        return -1;
    }

    // Get Shadow Wideness
    PyObject* EnvironmentComponent_getShadowWideness(PyObject_EnvironmentComponent* self)
    {
        return PyFloat_FromDouble(self->component->getShadowWideness());
    }

    // Set Shadow Wideness
    int EnvironmentComponent_setShadowWideness(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setShadowWideness(val);
            return 0;
        }
        return -1;
    }

    PyGetSetDef EnvironmentComponent_getsets[] = {
        { "ambientSkyColor", (getter)EnvironmentComponent_getAmbientSkyColor, (setter)EnvironmentComponent_setAmbientSkyColor, EnvironmentComponent_ambientSkyColor_doc, NULL },
        { "ambientGroundColor", (getter)EnvironmentComponent_getAmbientGroundColor, (setter)EnvironmentComponent_setAmbientGroundColor, EnvironmentComponent_ambientGroundColor_doc, NULL },
        { "ambientDirection", (getter)EnvironmentComponent_getAmbientDirection, (setter)EnvironmentComponent_setAmbientDirection, EnvironmentComponent_ambientDirection_doc, NULL },
        { "directionalLight0_Color", (getter)EnvironmentComponent_getDirectionalLight0_Color, (setter)EnvironmentComponent_setDirectionalLight0_Color, EnvironmentComponent_directionalLight0_Color_doc, NULL },
        { "directionalLight0_Direction", (getter)EnvironmentComponent_getDirectionalLight0_Direction, (setter)EnvironmentComponent_setDirectionalLight0_Direction, EnvironmentComponent_directionalLight0_Direction_doc, NULL },
        { "directionalLight0_Intensity", (getter)EnvironmentComponent_getDirectionalLight0_Intensity, (setter)EnvironmentComponent_setDirectionalLight0_Intensity, EnvironmentComponent_directionalLight0_Intensity_doc, NULL },
        { "directionalLight1_Color", (getter)EnvironmentComponent_getDirectionalLight1_Color, (setter)EnvironmentComponent_setDirectionalLight1_Color, EnvironmentComponent_directionalLight1_Color_doc, NULL },
        { "directionalLight1_Direction", (getter)EnvironmentComponent_getDirectionalLight1_Direction, (setter)EnvironmentComponent_setDirectionalLight1_Direction, EnvironmentComponent_directionalLight1_Direction_doc, NULL },
        { "directionalLight1_Intensity", (getter)EnvironmentComponent_getDirectionalLight1_Intensity, (setter)EnvironmentComponent_setDirectionalLight1_Intensity, EnvironmentComponent_directionalLight1_Intensity_doc, NULL },
        { "directionalLight2_Color", (getter)EnvironmentComponent_getDirectionalLight2_Color, (setter)EnvironmentComponent_setDirectionalLight2_Color, EnvironmentComponent_directionalLight2_Color_doc, NULL },
        { "directionalLight2_Direction", (getter)EnvironmentComponent_getDirectionalLight2_Direction, (setter)EnvironmentComponent_setDirectionalLight2_Direction, EnvironmentComponent_directionalLight2_Direction_doc, NULL },
        { "directionalLight2_Intensity", (getter)EnvironmentComponent_getDirectionalLight2_Intensity, (setter)EnvironmentComponent_setDirectionalLight2_Intensity, EnvironmentComponent_directionalLight2_Intensity_doc, NULL },
        { "pointLight0_Color", (getter)EnvironmentComponent_getPointLight0_Color, (setter)EnvironmentComponent_setPointLight0_Color, EnvironmentComponent_pointLight0_Color_doc, NULL },
        { "pointLight0_Position", (getter)EnvironmentComponent_getPointLight0_Position, (setter)EnvironmentComponent_setPointLight0_Position, EnvironmentComponent_pointLight0_Position_doc, NULL },
        { "pointLight0_Range", (getter)EnvironmentComponent_getPointLight0_Range, (setter)EnvironmentComponent_setPointLight0_Range, EnvironmentComponent_pointLight0_Range_doc, NULL },
        { "pointLight0_Intensity", (getter)EnvironmentComponent_getPointLight0_Intensity, (setter)EnvironmentComponent_setPointLight0_Intensity, EnvironmentComponent_pointLight0_Intensity_doc, NULL },
        { "pointLight1_Color", (getter)EnvironmentComponent_getPointLight1_Color, (setter)EnvironmentComponent_setPointLight1_Color, EnvironmentComponent_pointLight1_Color_doc, NULL },
        { "pointLight1_Position", (getter)EnvironmentComponent_getPointLight1_Position, (setter)EnvironmentComponent_setPointLight1_Position, EnvironmentComponent_pointLight1_Position_doc, NULL },
        { "pointLight1_Range", (getter)EnvironmentComponent_getPointLight1_Range, (setter)EnvironmentComponent_setPointLight1_Range, EnvironmentComponent_pointLight1_Range_doc, NULL },
        { "pointLight1_Intensity", (getter)EnvironmentComponent_getPointLight1_Intensity, (setter)EnvironmentComponent_setPointLight1_Intensity, EnvironmentComponent_pointLight1_Intensity_doc, NULL },
        { "pointLight2_Color", (getter)EnvironmentComponent_getPointLight2_Color, (setter)EnvironmentComponent_setPointLight2_Color, EnvironmentComponent_pointLight2_Color_doc, NULL },
        { "pointLight2_Position", (getter)EnvironmentComponent_getPointLight2_Position, (setter)EnvironmentComponent_setPointLight2_Position, EnvironmentComponent_pointLight2_Position_doc, NULL },
        { "pointLight2_Range", (getter)EnvironmentComponent_getPointLight2_Range, (setter)EnvironmentComponent_setPointLight2_Range, EnvironmentComponent_pointLight2_Range_doc, NULL },
        { "pointLight2_Intensity", (getter)EnvironmentComponent_getPointLight2_Intensity, (setter)EnvironmentComponent_setPointLight2_Intensity, EnvironmentComponent_pointLight2_Intensity_doc, NULL },
        { "pointLight3_Color", (getter)EnvironmentComponent_getPointLight3_Color, (setter)EnvironmentComponent_setPointLight3_Color, EnvironmentComponent_pointLight3_Color_doc, NULL },
        { "pointLight3_Position", (getter)EnvironmentComponent_getPointLight3_Position, (setter)EnvironmentComponent_setPointLight3_Position, EnvironmentComponent_pointLight3_Position_doc, NULL },
        { "pointLight3_Range", (getter)EnvironmentComponent_getPointLight3_Range, (setter)EnvironmentComponent_setPointLight3_Range, EnvironmentComponent_pointLight3_Range_doc, NULL },
        { "pointLight3_Intensity", (getter)EnvironmentComponent_getPointLight3_Intensity, (setter)EnvironmentComponent_setPointLight3_Intensity, EnvironmentComponent_pointLight3_Intensity_doc, NULL },
        { "pointLight4_Color", (getter)EnvironmentComponent_getPointLight4_Color, (setter)EnvironmentComponent_setPointLight4_Color, EnvironmentComponent_pointLight4_Color_doc, NULL },
        { "pointLight4_Position", (getter)EnvironmentComponent_getPointLight4_Position, (setter)EnvironmentComponent_setPointLight4_Position, EnvironmentComponent_pointLight4_Position_doc, NULL },
        { "pointLight4_Range", (getter)EnvironmentComponent_getPointLight4_Range, (setter)EnvironmentComponent_setPointLight4_Range, EnvironmentComponent_pointLight4_Range_doc, NULL },
        { "pointLight4_Intensity", (getter)EnvironmentComponent_getPointLight4_Intensity, (setter)EnvironmentComponent_setPointLight4_Intensity, EnvironmentComponent_pointLight4_Intensity_doc, NULL },
        { "pointLight5_Color", (getter)EnvironmentComponent_getPointLight5_Color, (setter)EnvironmentComponent_setPointLight5_Color, EnvironmentComponent_pointLight5_Color_doc, NULL },
        { "pointLight5_Position", (getter)EnvironmentComponent_getPointLight5_Position, (setter)EnvironmentComponent_setPointLight5_Position, EnvironmentComponent_pointLight5_Position_doc, NULL },
        { "pointLight5_Range", (getter)EnvironmentComponent_getPointLight5_Range, (setter)EnvironmentComponent_setPointLight5_Range, EnvironmentComponent_pointLight5_Range_doc, NULL },
        { "pointLight5_Intensity", (getter)EnvironmentComponent_getPointLight5_Intensity, (setter)EnvironmentComponent_setPointLight5_Intensity, EnvironmentComponent_pointLight5_Intensity_doc, NULL },
        { "pointLight6_Color", (getter)EnvironmentComponent_getPointLight6_Color, (setter)EnvironmentComponent_setPointLight6_Color, EnvironmentComponent_pointLight6_Color_doc, NULL },
        { "pointLight6_Position", (getter)EnvironmentComponent_getPointLight6_Position, (setter)EnvironmentComponent_setPointLight6_Position, EnvironmentComponent_pointLight6_Position_doc, NULL },
        { "pointLight6_Range", (getter)EnvironmentComponent_getPointLight6_Range, (setter)EnvironmentComponent_setPointLight6_Range, EnvironmentComponent_pointLight6_Range_doc, NULL },
        { "pointLight6_Intensity", (getter)EnvironmentComponent_getPointLight6_Intensity, (setter)EnvironmentComponent_setPointLight6_Intensity, EnvironmentComponent_pointLight6_Intensity_doc, NULL },
        { "distanceFogColor", (getter)EnvironmentComponent_getDistanceFogColor, (setter)EnvironmentComponent_setDistanceFogColor, EnvironmentComponent_distanceFogColor_doc, NULL },
        { "distanceFogAlpha", (getter)EnvironmentComponent_getDistanceFogAlpha, (setter)EnvironmentComponent_setDistanceFogAlpha, EnvironmentComponent_distanceFogAlpha_doc, NULL },
        { "distanceFogNear", (getter)EnvironmentComponent_getDistanceFogNear, (setter)EnvironmentComponent_setDistanceFogNear, EnvironmentComponent_distanceFogNear_doc, NULL },
        { "distanceFogFar", (getter)EnvironmentComponent_getDistanceFogFar, (setter)EnvironmentComponent_setDistanceFogFar, EnvironmentComponent_distanceFogFar_doc, NULL },
        { "shadowColor", (getter)EnvironmentComponent_getShadowColor, (setter)EnvironmentComponent_setShadowColor, EnvironmentComponent_shadowColor_doc, NULL },
        { "shadowDensity", (getter)EnvironmentComponent_getShadowDensity, (setter)EnvironmentComponent_setShadowDensity, EnvironmentComponent_shadowDensity_doc, NULL },
        { "shadowWideness", (getter)EnvironmentComponent_getShadowWideness, (setter)EnvironmentComponent_setShadowWideness, EnvironmentComponent_shadowWideness_doc, NULL },
        { NULL, NULL }
    };

    PyTypeObject PyTypeObject_EnvironmentComponent = {
        PyVarObject_HEAD_INIT(NULL, 1)
        "igeScene.EnvironmentComponent",             /* tp_name */
        sizeof(PyObject_EnvironmentComponent),       /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)EnvironmentComponent_dealloc,    /* tp_dealloc */
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
        (reprfunc)EnvironmentComponent_str,          /* tp_str */
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
        EnvironmentComponent_getsets,                /* tp_getset */
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
