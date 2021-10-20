#pragma once

#include <Python.h>

// name
PyDoc_STRVAR(Component_name_doc,
    "The name of the component.\n"\
    "Type: string\n"
);

// owner
PyDoc_STRVAR(Component_owner_doc,
    "The scene object that own this component.\n"\
    "Type: SceneObject\n"
);

// onUpdate
PyDoc_STRVAR(Component_onUpdate_doc,
	"Utility to force update component, eg transform...\n"
	"\n"
	"Component().onUpdate(dt: float) -> bool\n"
	"\n"
	"Parameters\n"
	"----------\n"
	"    dt : float\n"
	"        Delta time, should be 0.f in case of force update.\n"
	"Return:\n"
	"----------\n"
	"    True: if updated\n"
	"    False: if failed\n");
