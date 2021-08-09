#pragma once

#include <Python.h>

// path
PyDoc_STRVAR(Particle_path_doc,
             "Path to the .efk file.\n"
             "Type: string\n");

// enable
PyDoc_STRVAR(Particle_enable_doc,
             "Enable/disable. Default: true.\n"
             "Type: bool\n");

// loop
PyDoc_STRVAR(Particle_loop_doc,
             "Should be looped? Default: false.\n"
             "Type: bool\n");

// autoDrawing
PyDoc_STRVAR(Particle_autoDrawing_doc,
             "Particle autoDrawing? Default: true.\n"
             "Type: bool\n");

// layer
PyDoc_STRVAR(Particle_layer_doc,
             "Particle layer. Default: 0.\n"
             "Type: int\n");

// groupMask
PyDoc_STRVAR(Particle_groupMask_doc,
             "Particle group mask. Default: 0.\n"
             "Type: int\n");

// speed
PyDoc_STRVAR(Particle_speed_doc,
             "Particle playing speed. Default: 1.0f\n"
             "Type: float\n");

// timeScale
PyDoc_STRVAR(Particle_timeScale_doc,
             "Particle playing time scale. Default: 1.0f\n"
             "Type: float\n");

// targetLocation
PyDoc_STRVAR(Particle_targetLocation_doc,
             "Particle target location. Default: (0.0f, 0.0f, 0.0f)\n"
             "Type: Vec3\n");

// dynamicParameter
PyDoc_STRVAR(Particle_dynamicParameter_doc,
             "Particle dynamic input parameter. Default: (0.0f, 0.0f, 0.0f, 0.0f)\n"
             "Type: Vec4\n");

// color
PyDoc_STRVAR(Particle_color_doc,
             "Particle Color. Default: (1.0f, 1.0f, 1.0f, 1.0f)\n"
             "Type: Vec4\n");
