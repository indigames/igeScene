#pragma once

#include <Python.h>

// owner
PyDoc_STRVAR(Script_owner_doc,
	"Get the object owner.\n"\
	"Type: SceneObject\n"
);

PyDoc_STRVAR(Script_invoke_doc,
	"Invoke scripts\'s method from current scene object.\n"\
	"\n"\
	"SceneObject().invoke(param: string|ScriptPath, param: string|FunctionName, param: Object|PyObject)\n"\
	"\n"\
	"Return: Object\n"\
	"----------\n"\
);
