#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/EnvironmentComponent.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_EnvironmentComponent
    {
        PyObject_Component super;
        std::shared_ptr<EnvironmentComponent> component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_EnvironmentComponent;

    // Dealloc
    void  EnvironmentComponent_dealloc(PyObject_EnvironmentComponent *self);

    // String represent
    PyObject* EnvironmentComponent_str(PyObject_EnvironmentComponent *self);

    // Get Ambient Sky Color
    PyObject* EnvironmentComponent_getAmbientSkyColor(PyObject_EnvironmentComponent* self);

    // Set Ambient Sky Color
    int EnvironmentComponent_setAmbientSkyColor(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Ambient Ground Color
    PyObject* EnvironmentComponent_getAmbientGroundColor(PyObject_EnvironmentComponent* self);

    // Set Ambient Ground Color
    int EnvironmentComponent_setAmbientGroundColor(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Ambient Direction
    PyObject* EnvironmentComponent_getAmbientDirection(PyObject_EnvironmentComponent* self);

    // Set Ambient Direction
    int EnvironmentComponent_setAmbientDirection(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Directional Light 0 Color
    PyObject* EnvironmentComponent_getDirectionalLight0_Color(PyObject_EnvironmentComponent* self);

    // Set Directional Light 0 Color
    int EnvironmentComponent_setDirectionalLight0_Color(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Directional Light 0 Direction
    PyObject* EnvironmentComponent_getDirectionalLight0_Direction(PyObject_EnvironmentComponent* self);

    // Set Directional Light 0 Direction
    int EnvironmentComponent_setDirectionalLight0_Direction(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Directional Light 0 Intensity
    PyObject* EnvironmentComponent_getDirectionalLight0_Intensity(PyObject_EnvironmentComponent* self);

    // Set Directional Light 0 Intensity
    int EnvironmentComponent_setDirectionalLight0_Intensity(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Directional Light 1 Color
    PyObject* EnvironmentComponent_getDirectionalLight1_Color(PyObject_EnvironmentComponent* self);

    // Set Directional Light 1 Color
    int EnvironmentComponent_setDirectionalLight1_Color(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Directional Light 1 Direction
    PyObject* EnvironmentComponent_getDirectionalLight1_Direction(PyObject_EnvironmentComponent* self);

    // Set Directional Light 1 Direction
    int EnvironmentComponent_setDirectionalLight1_Direction(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Directional Light 1 Intensity
    PyObject* EnvironmentComponent_getDirectionalLight1_Intensity(PyObject_EnvironmentComponent* self);

    // Set Directional Light 1 Intensity
    int EnvironmentComponent_setDirectionalLight1_Intensity(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Directional Light 2 Color
    PyObject* EnvironmentComponent_getDirectionalLight2_Color(PyObject_EnvironmentComponent* self);

    // Set Directional Light 2 Color
    int EnvironmentComponent_setDirectionalLight2_Color(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Directional Light 2 Direction
    PyObject* EnvironmentComponent_getDirectionalLight2_Direction(PyObject_EnvironmentComponent* self);

    // Set Directional Light 2 Direction
    int EnvironmentComponent_setDirectionalLight2_Direction(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Directional Light 2 Intensity
    PyObject* EnvironmentComponent_getDirectionalLight2_Intensity(PyObject_EnvironmentComponent* self);

    // Set Directional Light 2 Intensity
    int EnvironmentComponent_setDirectionalLight2_Intensity(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 0 Position
    PyObject* EnvironmentComponent_getPointLight0_Position(PyObject_EnvironmentComponent* self);

    // Set Point Light 0 Position
    int EnvironmentComponent_setPointLight0_Position(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 0 Color
    PyObject* EnvironmentComponent_getPointLight0_Color(PyObject_EnvironmentComponent* self);

    // Set Point Light 0 Color
    int EnvironmentComponent_setPointLight0_Color(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 0 Intensity
    PyObject* EnvironmentComponent_getPointLight0_Intensity(PyObject_EnvironmentComponent* self);

    // Set Point Light 0 Intensity
    int EnvironmentComponent_setPointLight0_Intensity(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 0 Range
    PyObject* EnvironmentComponent_getPointLight0_Range(PyObject_EnvironmentComponent* self);

    // Set Point Light 0 Range
    int EnvironmentComponent_setPointLight0_Range(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 1 Position
    PyObject* EnvironmentComponent_getPointLight1_Position(PyObject_EnvironmentComponent* self);

    // Set Point Light 1 Position
    int EnvironmentComponent_setPointLight1_Position(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 1 Color
    PyObject* EnvironmentComponent_getPointLight1_Color(PyObject_EnvironmentComponent* self);

    // Set Point Light 1 Color
    int EnvironmentComponent_setPointLight1_Color(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 1 Intensity
    PyObject* EnvironmentComponent_getPointLight1_Intensity(PyObject_EnvironmentComponent* self);

    // Set Point Light 1 Intensity
    int EnvironmentComponent_setPointLight1_Intensity(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 1 Range
    PyObject* EnvironmentComponent_getPointLight1_Range(PyObject_EnvironmentComponent* self);

    // Set Point Light 1 Range
    int EnvironmentComponent_setPointLight1_Range(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 2 Position
    PyObject* EnvironmentComponent_getPointLight2_Position(PyObject_EnvironmentComponent* self);

    // Set Point Light 2 Position
    int EnvironmentComponent_setPointLight2_Position(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 2 Color
    PyObject* EnvironmentComponent_getPointLight2_Color(PyObject_EnvironmentComponent* self);

    // Set Point Light 2 Color
    int EnvironmentComponent_setPointLight2_Color(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 2 Intensity
    PyObject* EnvironmentComponent_getPointLight2_Intensity(PyObject_EnvironmentComponent* self);

    // Set Point Light 2 Intensity
    int EnvironmentComponent_setPointLight2_Intensity(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 2 Range
    PyObject* EnvironmentComponent_getPointLight2_Range(PyObject_EnvironmentComponent* self);

    // Set Point Light 2 Range
    int EnvironmentComponent_setPointLight2_Range(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 3 Position
    PyObject* EnvironmentComponent_getPointLight3_Position(PyObject_EnvironmentComponent* self);

    // Set Point Light 3 Position
    int EnvironmentComponent_setPointLight3_Position(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 3 Color
    PyObject* EnvironmentComponent_getPointLight3_Color(PyObject_EnvironmentComponent* self);

    // Set Point Light 3 Color
    int EnvironmentComponent_setPointLight3_Color(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 3 Intensity
    PyObject* EnvironmentComponent_getPointLight3_Intensity(PyObject_EnvironmentComponent* self);

    // Set Point Light 3 Intensity
    int EnvironmentComponent_setPointLight3_Intensity(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 3 Range
    PyObject* EnvironmentComponent_getPointLight3_Range(PyObject_EnvironmentComponent* self);

    // Set Point Light 3 Range
    int EnvironmentComponent_setPointLight3_Range(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 4 Position
    PyObject* EnvironmentComponent_getPointLight4_Position(PyObject_EnvironmentComponent* self);

    // Set Point Light 4 Position
    int EnvironmentComponent_setPointLight4_Position(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 4 Color
    PyObject* EnvironmentComponent_getPointLight4_Color(PyObject_EnvironmentComponent* self);

    // Set Point Light 4 Color
    int EnvironmentComponent_setPointLight4_Color(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 4 Intensity
    PyObject* EnvironmentComponent_getPointLight4_Intensity(PyObject_EnvironmentComponent* self);

    // Set Point Light 4 Intensity
    int EnvironmentComponent_setPointLight4_Intensity(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 4 Range
    PyObject* EnvironmentComponent_getPointLight4_Range(PyObject_EnvironmentComponent* self);

    // Set Point Light 4 Range
    int EnvironmentComponent_setPointLight4_Range(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 5 Position
    PyObject* EnvironmentComponent_getPointLight5_Position(PyObject_EnvironmentComponent* self);

    // Set Point Light 5 Position
    int EnvironmentComponent_setPointLight5_Position(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 5 Color
    PyObject* EnvironmentComponent_getPointLight5_Color(PyObject_EnvironmentComponent* self);

    // Set Point Light 5 Color
    int EnvironmentComponent_setPointLight5_Color(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 5 Intensity
    PyObject* EnvironmentComponent_getPointLight5_Intensity(PyObject_EnvironmentComponent* self);

    // Set Point Light 5 Intensity
    int EnvironmentComponent_setPointLight5_Intensity(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 5 Range
    PyObject* EnvironmentComponent_getPointLight5_Range(PyObject_EnvironmentComponent* self);

    // Set Point Light 5 Range
    int EnvironmentComponent_setPointLight5_Range(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 6 Position
    PyObject* EnvironmentComponent_getPointLight6_Position(PyObject_EnvironmentComponent* self);

    // Set Point Light 6 Position
    int EnvironmentComponent_setPointLight6_Position(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 6 Color
    PyObject* EnvironmentComponent_getPointLight6_Color(PyObject_EnvironmentComponent* self);

    // Set Point Light 6 Color
    int EnvironmentComponent_setPointLight6_Color(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 6 Intensity
    PyObject* EnvironmentComponent_getPointLight6_Intensity(PyObject_EnvironmentComponent* self);

    // Set Point Light 6 Intensity
    int EnvironmentComponent_setPointLight6_Intensity(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Point Light 6 Range
    PyObject* EnvironmentComponent_getPointLight6_Range(PyObject_EnvironmentComponent* self);

    // Set Point Light 6 Range
    int EnvironmentComponent_setPointLight6_Range(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Distance Fog Color
    PyObject* EnvironmentComponent_getDistanceFogColor(PyObject_EnvironmentComponent* self);

    // Set Distance Fog Color
    int EnvironmentComponent_setDistanceFogColor(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Distance Fog Alpha
    PyObject* EnvironmentComponent_getDistanceFogAlpha(PyObject_EnvironmentComponent* self);

    // Set Distance Fog Alpha
    int EnvironmentComponent_setDistanceFogAlpha(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Distance Fog Near
    PyObject* EnvironmentComponent_getDistanceFogNear(PyObject_EnvironmentComponent* self);

    // Set Distance Fog Near
    int EnvironmentComponent_setDistanceFogNear(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Distance Fog Far
    PyObject* EnvironmentComponent_getDistanceFogFar(PyObject_EnvironmentComponent* self);

    // Set Distance Fog Far
    int EnvironmentComponent_setDistanceFogFar(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Shadow Color
    PyObject* EnvironmentComponent_getShadowColor(PyObject_EnvironmentComponent* self);

    // Set Shadow Color
    int EnvironmentComponent_setShadowColor(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Shadow Density
    PyObject* EnvironmentComponent_getShadowDensity(PyObject_EnvironmentComponent* self);

    // Set Shadow Density
    int EnvironmentComponent_setShadowDensity(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Shadow Wideness
    PyObject* EnvironmentComponent_getShadowWideness(PyObject_EnvironmentComponent* self);

    // Set Shadow Wideness
    int EnvironmentComponent_setShadowWideness(PyObject_EnvironmentComponent* self, PyObject* value);

}
