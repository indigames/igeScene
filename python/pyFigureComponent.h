#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/FigureComponent.h"

#include "python/pyComponent.h"


namespace ige::scene
{
    struct PyObject_FigureComponent
    {
        PyObject_Component super;
        FigureComponent* component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_FigureComponent;

    // Dealloc
    void  FigureComponent_dealloc(PyObject_FigureComponent *self);

    // String represent
    PyObject* FigureComponent_str(PyObject_FigureComponent *self);

    // Get figure
    PyObject* FigureComponent_getFigure(PyObject_FigureComponent* self);

    // Get path
    PyObject* FigureComponent_getPath(PyObject_FigureComponent* self);

    // Set path
    int FigureComponent_setPath(PyObject_FigureComponent* self, PyObject* value);

    // Fog
    PyObject* FigureComponent_isFogEnabled(PyObject_FigureComponent* self);    
    int FigureComponent_setFogEnabled(PyObject_FigureComponent* self, PyObject* value);

    // Front face culling
    PyObject* FigureComponent_isCullFaceEnable(PyObject_FigureComponent* self);
    int FigureComponent_setCullFaceEnable(PyObject_FigureComponent* self, PyObject* value);

    // Double Side
    PyObject* FigureComponent_isDoubleSideEnable(PyObject_FigureComponent* self);
    int FigureComponent_setDoubleSideEnable(PyObject_FigureComponent* self, PyObject* value);

    // Depth test
    PyObject* FigureComponent_isDepthTestEnable(PyObject_FigureComponent* self);
    int FigureComponent_setDepthTestEnable(PyObject_FigureComponent* self, PyObject* value);

    // Depth write
    PyObject* FigureComponent_isDepthWriteEnable(PyObject_FigureComponent* self);
    int FigureComponent_setDepthWriteEnable(PyObject_FigureComponent* self, PyObject* value);

    // Alpha blend
    PyObject* FigureComponent_isAlphaBlendingEnable(PyObject_FigureComponent* self);
    int FigureComponent_setAlphaBlendingEnable(PyObject_FigureComponent* self, PyObject* value);

    // Alpha blend Operation
    PyObject* FigureComponent_getAlphaBlendingOp(PyObject_FigureComponent* self);
    int FigureComponent_setAlphaBlendingOp(PyObject_FigureComponent* self, PyObject* value);

    // Scissor Test
    PyObject* FigureComponent_isScissorTestEnable(PyObject_FigureComponent* self);
    int FigureComponent_setScissorTestEnable(PyObject_FigureComponent* self, PyObject* value);

    // Frame update ratio (speedup/slower effects)
    PyObject* FigureComponent_getFrameUpdateRatio(PyObject_FigureComponent* self);
    int FigureComponent_setFrameUpdateRatio(PyObject_FigureComponent* self, PyObject* value);
}
