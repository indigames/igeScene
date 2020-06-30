#pragma once

#include <Python.h>

// createObject
PyDoc_STRVAR(Scene_createObject_doc,
	"Create new scene object.\n"\
	"\n"\
	"Scene().createObject(name: string, parent: SceneObject = None)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    name : string\n"\
	"        Name of the new scene object\n"\
	"    parent : SceneObject\n"\
	"        Parent object\n"
);

// removeObject
PyDoc_STRVAR(Scene_removeObject_doc,
	"Remove object out of scene.\n"\
	"\n"\
	"Scene().removeObject(param: string|number|SceneObject)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    param : string\n"\
	"        Name of the scene object to remove\n"\
	"    param : number (unsigned long long)\n"\
	"        ID of the scene object to remove\n"\
	"    param : SceneObject\n"\
	"        Scene object instance to remove\n"\
	"Return:\n"\
	"----------\n"\
	"    True: if removed success\n"\
	"    False: if removed fail\n"
);

// findObject
PyDoc_STRVAR(Scene_findObject_doc,
	"Find a object in the scene.\n"\
	"\n"\
	"Scene().findObject(param: string|number)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    param : string\n"\
	"        Name of the scene object to find\n"\
	"    param : number (unsigned long long)\n"\
	"        ID of the scene object to find\n"\
	"Return:\n"\
	"----------\n"\
	"    SceneObject: if found\n"\
	"    None: if not found\n"
);

// getRoot
PyDoc_STRVAR(Scene_getRoot_doc,
	"Get the root object of the scene.\n"\
	"\n"\
	"Scene().findObject(param: string|number)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    param : string\n"\
	"        Name of the scene object to find\n"\
	"    param : number (unsigned long long)\n"\
	"        ID of the scene object to find\n"\
	"Return:\n"\
	"----------\n"\
	"    SceneObject: if found\n"\
	"    None: if not found\n"
);

// name
PyDoc_STRVAR(Scene_name_doc,
	"Name of scene.\n");

