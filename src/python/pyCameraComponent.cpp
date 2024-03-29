#include "python/pyCameraComponent.h"
#include "python/pyCameraComponent_doc_en.h"

#include "components/CameraComponent.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void CameraComponent_dealloc(PyObject_CameraComponent *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject* CameraComponent_str(PyObject_CameraComponent *self)
    {
        return PyUnicode_FromString("C++ CameraComponent object");
    }

    // Get position
    PyObject* CameraComponent_getPosition(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;        
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getPosition().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;        
    }

    // Set position
    int CameraComponent_setPosition(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;        
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setPosition(*((Vec3*)v));
        return 0;
    }

    // Get rotation
    PyObject* CameraComponent_getRotation(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto quatObj = PyObject_New(vec_obj, _QuatType);
        vmath_cpy(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getRotation().P(), 4, quatObj->v);
        quatObj->d = 4;
        return (PyObject*)quatObj;
    }

    // Set rotation
    int CameraComponent_setRotation(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d1;
        float buff[4];
        float* v1 = pyObjToFloat((PyObject*)value, buff, d1);
        if (!v1) return -1;
        std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setRotation(*((Quat*)v1));
        return 0;
    }

    // Get aspect ratio
    PyObject* CameraComponent_getAspectRatio(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getAspectRatio());
    }

    // Set aspect ratio
    int CameraComponent_setAspectRatio(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setAspectRatio(val);
            return 0;
        }
        return -1;
    }

    // Get width based
    PyObject* CameraComponent_getWidthBased(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->isWidthBase());
    }

    // Set width based
    int CameraComponent_setWidthBased(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setWidthBase(val);
            return 0;
        }
        return -1;
    }

    // Get FOV
    PyObject* CameraComponent_getFieldOfView(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getFieldOfView());
    }

    // Set FOV
    int CameraComponent_setFieldOfView(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setFieldOfView(val);
            return 0;
        }
        return -1;
    }

    // Get Near Plane
    PyObject* CameraComponent_getNearPlane(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getNearPlane());
    }

    // Set Near Plane
    int CameraComponent_setNearPlane(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setNearPlane(val);
            return 0;
        }
        return -1;
    }

    // Get Far Plane
    PyObject* CameraComponent_getFarPlane(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getFarPlane());
    }

    // Set Far Plane
    int CameraComponent_setFarPlane(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setFarPlane(val);
            return 0;
        }
        return -1;
    }

    // Get Pan
    PyObject* CameraComponent_getPan(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getPan());
    }

    // Set Pan
    int CameraComponent_setPan(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setPan(val);
            return 0;
        }
        return -1;
    }

    // Get Tilt
    PyObject* CameraComponent_getTilt(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getTilt());
    }

    // Set Tilt
    int CameraComponent_setTilt(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setTilt(val);
            return 0;
        }
        return -1;
    }

    // Get Roll
    PyObject* CameraComponent_getRoll(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getRoll());
    }

    // Set Roll
    int CameraComponent_setRoll(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setRoll(val);
            return 0;
        }
        return -1;
    }

    // Get Lock On Target
    PyObject* CameraComponent_getLockOnTarget(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getLockOn());
    }

    // Set Lock On Target
    int CameraComponent_setLockOnTarget(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->lockOnTarget(val);
            return 0;
        }
        return -1;
    }

    // Get Target
    PyObject* CameraComponent_getTarget(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getTarget().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Get World Target
    PyObject* CameraComponent_getWorldTarget(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getWorldTarget().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Target
    int CameraComponent_setTarget(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setTarget(*((Vec3*)v));
        return 0;
    }

    // Get Ortho Projection
    PyObject* CameraComponent_getOrthoProjection(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->isOrthoProjection());
    }

    // Set Ortho Projection
    int CameraComponent_setOrthoProjection(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setOrthoProjection(val);
            return 0;
        }
        return -1;
    }

    // Get Ortho Width
    PyObject* CameraComponent_getOrthoWidth(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getOrthoWidth());
    }

    // Set Ortho Width
    int CameraComponent_setOrthoWidth(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setOrthoWidth(val);
            return 0;
        }
        return -1;
    }

    // Get Ortho Height
    PyObject* CameraComponent_getOrthoHeight(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getOrthoHeight());
    }

    // Set Ortho Height
    int CameraComponent_setOrthoHeight(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setOrthoHeight(val);
            return 0;
        }
        return -1;
    }

    // Get Screen Scale
    PyObject* CameraComponent_getScreenScale(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getScreenScale().P(), 2, vec2Obj->v);
        vec2Obj->d = 2;
        return (PyObject*)vec2Obj;
    }

    // Set Screen Scale
    int CameraComponent_setScreenScale(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setScreenScale(*((Vec2*)v));
        return 0;
    }

    // Get Screen Offset
    PyObject* CameraComponent_getScreenOffset(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getScreenOffset().P(), 2, vec2Obj->v);
        vec2Obj->d = 2;
        return (PyObject*)vec2Obj;
    }

    // Set Screen Offset
    int CameraComponent_setScreenOffset(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setScreenOffset(*((Vec2*)v));
        return 0;
    }

    // Get Screen Radian
    PyObject* CameraComponent_getScreenRadian(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getScreenRadian());
    }

    // Set Screen Radian
    int CameraComponent_setScreenRadian(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setScreenRadian(val);
            return 0;
        }
        return -1;
    }

    // Get Up Axis
    PyObject* CameraComponent_getUpAxis(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getUpAxis());
    }

    // Set Up Axis
    int CameraComponent_setUpAxis(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setUpAxis(val);
            return 0;
        }
        return -1;
    }

    // Get clear color
    PyObject* CameraComponent_getClearColor(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getClearColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set Up Axis
    int CameraComponent_setClearColor(PyObject_CameraComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->setClearColor(*((Vec4*)v));
        return 0;
    }

    // Get Projection Matrix
    PyObject* CameraComponent_getProjectionMatrix(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto m4obj = PyObject_New(mat_obj, _Mat44Type);
        if (!m4obj) return NULL;
        Mat4 mat;
        std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getProjectionMatrix(mat);
        vmath_cpy(mat.P(), 16, m4obj->m);
        m4obj->d = 4;
        return (PyObject*)m4obj;
    }

    // Get View Inverse Matrix
    PyObject* CameraComponent_getViewInverseMatrix(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto m4obj = PyObject_New(mat_obj, _Mat44Type);
        if (!m4obj) return NULL;
        Mat4 mat;
        std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getViewInverseMatrix(mat);
        vmath_cpy(mat.P(), 16, m4obj->m);
        m4obj->d = 4;
        return (PyObject*)m4obj;
    }

    // Get Screen Matrix
    PyObject* CameraComponent_getScreenMatrix(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto m4obj = PyObject_New(mat_obj, _Mat44Type);
        if (!m4obj) return NULL;
        Mat4 mat;
        std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getScreenMatrix(mat);
        vmath_cpy(mat.P(), 16, m4obj->m);
        m4obj->d = 4;
        return (PyObject*)m4obj;
    }

    // Get camera
    PyObject* CameraComponent_getCamera(PyObject_CameraComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto camera = std::dynamic_pointer_cast<CameraComponent>(self->component.lock())->getCamera();
        if (camera) {
            auto camObj = (camera_obj*)(&CameraType)->tp_alloc(&CameraType, 0);
            camObj->camera = camera;
            camObj->camera->IncReference();
            return (PyObject*)camObj;
        }
        Py_RETURN_NONE;
    }

    PyGetSetDef CameraComponent_getsets[] = {
        { "position", (getter)CameraComponent_getPosition, (setter)CameraComponent_setPosition, CameraComponent_position_doc, NULL },
        { "rotation", (getter)CameraComponent_getRotation, (setter)CameraComponent_setRotation, CameraComponent_rotation_doc, NULL },
        { "aspectRatio", (getter)CameraComponent_getAspectRatio, (setter)CameraComponent_setAspectRatio, CameraComponent_aspectRatio_doc, NULL },
        { "widthBased", (getter)CameraComponent_getWidthBased, (setter)CameraComponent_setWidthBased, CameraComponent_widthBased_doc, NULL },
        { "fieldOfView", (getter)CameraComponent_getFieldOfView, (setter)CameraComponent_setFieldOfView, CameraComponent_fieldOfView_doc, NULL },
        { "near", (getter)CameraComponent_getNearPlane, (setter)CameraComponent_setNearPlane, CameraComponent_near_doc, NULL },
        { "far", (getter)CameraComponent_getFarPlane, (setter)CameraComponent_setFarPlane, CameraComponent_far_doc, NULL },
        { "pan", (getter)CameraComponent_getPan, (setter)CameraComponent_setPan, CameraComponent_pan_doc, NULL },
        { "tilt", (getter)CameraComponent_getTilt, (setter)CameraComponent_setTilt, CameraComponent_tilt_doc, NULL },
        { "roll", (getter)CameraComponent_getRoll, (setter)CameraComponent_setRoll, CameraComponent_roll_doc, NULL },
        { "lockOnTarget", (getter)CameraComponent_getLockOnTarget, (setter)CameraComponent_setLockOnTarget, CameraComponent_lockOnTarget_doc, NULL },
        { "target", (getter)CameraComponent_getTarget, (setter)CameraComponent_setTarget, CameraComponent_target_doc, NULL },
        { "worldTarget", (getter)CameraComponent_getWorldTarget, NULL, CameraComponent_worldTarget_doc, NULL },
        { "orthoProjection", (getter)CameraComponent_getOrthoProjection, (setter)CameraComponent_setOrthoProjection, CameraComponent_orthoProjection_doc, NULL },
        { "orthoWidth", (getter)CameraComponent_getOrthoWidth, (setter)CameraComponent_setOrthoWidth, CameraComponent_orthoWidth_doc, NULL },
        { "orthoHeight", (getter)CameraComponent_getOrthoHeight, (setter)CameraComponent_setOrthoHeight, CameraComponent_orthoHeight_doc, NULL },
        { "screenScale", (getter)CameraComponent_getScreenScale, (setter)CameraComponent_setScreenScale, CameraComponent_screenScale_doc, NULL },
        { "screenOffset", (getter)CameraComponent_getScreenOffset, (setter)CameraComponent_setScreenOffset, CameraComponent_screenOffset_doc, NULL },
        { "screenRadian", (getter)CameraComponent_getScreenRadian, (setter)CameraComponent_setScreenRadian, CameraComponent_screenRadian_doc, NULL },
        { "upAxis", (getter)CameraComponent_getUpAxis, (setter)CameraComponent_setUpAxis, CameraComponent_upAxis_doc, NULL },
        { "projectionMatrix", (getter)CameraComponent_getProjectionMatrix, NULL, CameraComponent_projectionMatrix_doc, NULL },
        { "viewInverseMatrix", (getter)CameraComponent_getViewInverseMatrix, NULL, CameraComponent_viewInverseMatrix_doc, NULL },
        { "screenMatrix", (getter)CameraComponent_getScreenMatrix, NULL, CameraComponent_screenMatrix_doc, NULL },
        { "camera", (getter)CameraComponent_getCamera, NULL, CameraComponent_getCamera_doc, NULL },
        { "clearColor", (getter)CameraComponent_getClearColor, (setter)CameraComponent_setClearColor, CameraComponent_clearColor_doc, NULL },
        { NULL, NULL }
    };

    PyTypeObject PyTypeObject_CameraComponent = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.Camera",                           /* tp_name */
        sizeof(PyObject_CameraComponent),            /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)CameraComponent_dealloc,         /* tp_dealloc */
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
        (reprfunc)CameraComponent_str,               /* tp_str */
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
        CameraComponent_getsets,                     /* tp_getset */
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
