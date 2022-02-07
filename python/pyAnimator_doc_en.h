#pragma once

#include <Python.h>

// controllerPath
PyDoc_STRVAR(Animator_controllerPath_doc,
             "Path to animator controller.\n"
             "Type: string\n");

// isInitialized
PyDoc_STRVAR(Animator_isInitialized_doc,
             "[Readonly] Return true if the animator has been initialized, otherwise return false.\n"
             "Type: bool\n");

// parameterCount
PyDoc_STRVAR(Animator_parameterCount_doc,
             "[Readonly] Return number of parameters the animator has.\n"
             "Type: int\n");

// speed
PyDoc_STRVAR(Animator_speed_doc,
             "Animation playing speed multiplier.\n"
             "Type: float\n");

// updateMode
PyDoc_STRVAR(Animator_updateMode_doc,
             "Animation update mode: Normal = 0, AnimatePhysics = 1, UnscaledTime = 2.\n"
             "Type: int\n");

// getValue
PyDoc_STRVAR(Animator_getValue_doc,
    "Get value of the given parameter.\n"
    "\n"
    "Animator.getValue(param: string)\n"
    "\n"
    "Return:\n"
    "   value: float | int | bool\n");

// setValue
PyDoc_STRVAR(Animator_setValue_doc,
    "Set value of the given parameter.\n"
    "\n"
    "Animator.setValue(param: string, value: float | int | bool)\n"
    "\n"
    "Return:\n"
    "   result: bool\n");

// setTrigger
PyDoc_STRVAR(Animator_setTrigger_doc,
    "Set trigger the given parameter.\n"
    "\n"
    "Animator.setTrigger(param: string)\n"
    "\n"
    "Return:\n"
    "   result: bool\n");

// resetTrigger
PyDoc_STRVAR(Animator_resetTrigger_doc,
    "Reset trigger the given parameter.\n"
    "\n"
    "Animator.resetTrigger(param: string)\n"
    "\n"
    "Return:\n"
    "   result: bool\n");

// rebind
PyDoc_STRVAR(Animator_rebind_doc,
    "Rebinding the animator with the controller, reseting all the states and parameters.\n"
    "\n"
    "Animator.rebind()\n"
    "\n"
    "Return:\n"
    "   result: bool\n");

// hasState
PyDoc_STRVAR(Animator_hasState_doc,
    "Check if the animator controller has the given state.\n"
    "\n"
    "Animator.hasState(stateName: string)\n"
    "\n"
    "Return:\n"
    "   result: bool\n");

// play
PyDoc_STRVAR(Animator_play_doc,
    "Play animation at the given state name.\n"
    "\n"
    "Animator.play(stateName: string)\n"
    "\n"
    "Return:\n"
    "   result: bool\n");

