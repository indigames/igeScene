#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/CameraComponent.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_CameraComponent : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_CameraComponent;

    // Dealloc
    void  CameraComponent_dealloc(PyObject_CameraComponent *self);

    // String represent
    PyObject* CameraComponent_str(PyObject_CameraComponent *self);

    // Get position
    PyObject* CameraComponent_getPosition(PyObject_CameraComponent* self);

    // Set position
    int CameraComponent_setPosition(PyObject_CameraComponent* self, PyObject* value);

    // Get rotation
    PyObject* CameraComponent_getRotation(PyObject_CameraComponent* self);

    // Set rotation
    int CameraComponent_setRotation(PyObject_CameraComponent* self, PyObject* value);

    // Get aspect ratio
    PyObject* CameraComponent_getAspectRatio(PyObject_CameraComponent* self);

    // Set aspect ratio
    int CameraComponent_setAspectRatio(PyObject_CameraComponent* self, PyObject* value);

    // Get width based
    PyObject* CameraComponent_getWidthBased(PyObject_CameraComponent* self);

    // Set width based
    int CameraComponent_setWidthBased(PyObject_CameraComponent* self, PyObject* value);

    // Get FOV
    PyObject* CameraComponent_getFieldOfView(PyObject_CameraComponent* self);

    // Set FOV
    int CameraComponent_setFieldOfView(PyObject_CameraComponent* self, PyObject* value);

    // Get Near Plane
    PyObject* CameraComponent_getNearPlane(PyObject_CameraComponent* self);

    // Set Near Plane
    int CameraComponent_setNearPlane(PyObject_CameraComponent* self, PyObject* value);

    // Get Far Plane
    PyObject* CameraComponent_getFarPlane(PyObject_CameraComponent* self);

    // Set Far Plane
    int CameraComponent_setFarPlane(PyObject_CameraComponent* self, PyObject* value);

    // Get Pan
    PyObject* CameraComponent_getPan(PyObject_CameraComponent* self);

    // Set Pan
    int CameraComponent_setPan(PyObject_CameraComponent* self, PyObject* value);

    // Get Tilt
    PyObject* CameraComponent_getTilt(PyObject_CameraComponent* self);

    // Set Tilt
    int CameraComponent_setTilt(PyObject_CameraComponent* self, PyObject* value);

    // Get Roll
    PyObject* CameraComponent_getRoll(PyObject_CameraComponent* self);

    // Set Roll
    int CameraComponent_setRoll(PyObject_CameraComponent* self, PyObject* value);

    // Get Lock On Target
    PyObject* CameraComponent_getLockOnTarget(PyObject_CameraComponent* self);

    // Set Lock On Target
    int CameraComponent_setLockOnTarget(PyObject_CameraComponent* self, PyObject* value);

    // Get Target
    PyObject* CameraComponent_getTarget(PyObject_CameraComponent* self);

    // Get World Target
    PyObject* CameraComponent_getWorldTarget(PyObject_CameraComponent* self);

    // Set Target
    int CameraComponent_setTarget(PyObject_CameraComponent* self, PyObject* value);

    // Get Ortho Projection
    PyObject* CameraComponent_getOrthoProjection(PyObject_CameraComponent* self);

    // Set Ortho Projection
    int CameraComponent_setOrthoProjection(PyObject_CameraComponent* self, PyObject* value);

    // Get Ortho Width
    PyObject* CameraComponent_getOrthoWidth(PyObject_CameraComponent* self);

    // Set Ortho Width
    int CameraComponent_setOrthoWidth(PyObject_CameraComponent* self, PyObject* value);

    // Get Ortho Height
    PyObject* CameraComponent_getOrthoHeight(PyObject_CameraComponent* self);

    // Set Ortho Height
    int CameraComponent_setOrthoHeight(PyObject_CameraComponent* self, PyObject* value);

    // Get Screen Scale
    PyObject* CameraComponent_getScreenScale(PyObject_CameraComponent* self);

    // Set Screen Scale
    int CameraComponent_setScreenScale(PyObject_CameraComponent* self, PyObject* value);

    // Get Screen Offset
    PyObject* CameraComponent_getScreenOffset(PyObject_CameraComponent* self);

    // Set Screen Offset
    int CameraComponent_setScreenOffset(PyObject_CameraComponent* self, PyObject* value);

    // Get Screen Radian
    PyObject* CameraComponent_getScreenRadian(PyObject_CameraComponent* self);

    // Set Screen Radian
    int CameraComponent_setScreenRadian(PyObject_CameraComponent* self, PyObject* value);

    // Get Up Axis
    PyObject* CameraComponent_getUpAxis(PyObject_CameraComponent* self);

    // Set Up Axis
    int CameraComponent_setUpAxis(PyObject_CameraComponent* self, PyObject* value);

    // Get Clear Color
    PyObject* CameraComponent_getClearColor(PyObject_CameraComponent* self);

    // Set Clear Color
    int CameraComponent_setClearColor(PyObject_CameraComponent* self, PyObject* value);

    // Get Projection Matrix
    PyObject* CameraComponent_getProjectionMatrix(PyObject_CameraComponent* self);

    // Get View Inverse Matrix
    PyObject* CameraComponent_getViewInverseMatrix(PyObject_CameraComponent* self);

    // Get Screen Matrix
    PyObject* CameraComponent_getScreenMatrix(PyObject_CameraComponent* self);

    // Get camera
    PyObject* CameraComponent_getCamera(PyObject_CameraComponent* self);
}
