#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/EditableFigureComponent.h"

#include "python/pyComponent.h"


namespace ige::scene
{
    struct PyObject_EditableFigureComponent
    {
        PyObject_Component super;
        EditableFigureComponent* component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_EditableFigureComponent;

    // Dealloc
    void  EditableFigureComponent_dealloc(PyObject_EditableFigureComponent *self);

    // String represent
    PyObject* EditableFigureComponent_str(PyObject_EditableFigureComponent *self);

    // Get figure
    PyObject* EditableFigureComponent_getFigure(PyObject_EditableFigureComponent* self);

    // Set figure
    int EditableFigureComponent_setFigure(PyObject_EditableFigureComponent* self, PyObject* value);

    // Get path
    PyObject* EditableFigureComponent_getPath(PyObject_EditableFigureComponent* self);

    // Set path
    int EditableFigureComponent_setPath(PyObject_EditableFigureComponent* self, PyObject* value);

    // Fog
    PyObject* EditableFigureComponent_isFogEnabled(PyObject_EditableFigureComponent* self);    
    int EditableFigureComponent_setFogEnabled(PyObject_EditableFigureComponent* self, PyObject* value);

    // Front face culling
    PyObject* EditableFigureComponent_isCullFaceEnable(PyObject_EditableFigureComponent* self);
    int EditableFigureComponent_setCullFaceEnable(PyObject_EditableFigureComponent* self, PyObject* value);

    // Double Side
    PyObject* EditableFigureComponent_isDoubleSideEnable(PyObject_EditableFigureComponent* self);
    int EditableFigureComponent_setDoubleSideEnable(PyObject_EditableFigureComponent* self, PyObject* value);

    // Depth test
    PyObject* EditableFigureComponent_isDepthTestEnable(PyObject_EditableFigureComponent* self);
    int EditableFigureComponent_setDepthTestEnable(PyObject_EditableFigureComponent* self, PyObject* value);

    // Depth write
    PyObject* EditableFigureComponent_isDepthWriteEnable(PyObject_EditableFigureComponent* self);
    int EditableFigureComponent_setDepthWriteEnable(PyObject_EditableFigureComponent* self, PyObject* value);

    // Scissor Test
    PyObject* EditableFigureComponent_isScissorTestEnable(PyObject_EditableFigureComponent* self);
    int EditableFigureComponent_setScissorTestEnable(PyObject_EditableFigureComponent* self, PyObject* value);

    // Frame update ratio (speedup/slower effects)
    PyObject* EditableFigureComponent_getFrameUpdateRatio(PyObject_EditableFigureComponent* self);
    int EditableFigureComponent_setFrameUpdateRatio(PyObject_EditableFigureComponent* self, PyObject* value);
}
