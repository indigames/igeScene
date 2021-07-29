#pragma once
#include <Python.h>

// getChildren
PyDoc_STRVAR(SceneObject_getChildren_doc,
	"Get the list of all child objects from current scene object.\n"\
	"\n"\
	"SceneObject().getChildren()\n"\
	"\n"\
	"Return:\n"\
	"----------\n"\
	"    List of children objects\n"
);

// removeChildren
PyDoc_STRVAR(SceneObject_removeChildren_doc,
	"Remove all children from current scene object.\n"\
	"\n"\
	"SceneObject().removeChildren()\n"\
	"\n"\
	"Return:\n"\
	"----------\n"\
	"    True: if removed\n"\
	"    False: if not removed\n"
);

// addComponent
PyDoc_STRVAR(SceneObject_addComponent_doc,
	"Add a component to current scene object.\n"\
	"\n"\
	"SceneObject().addComponent(param: string|Component)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    param : string\n"\
	"        Name of component to add\n"\
    "    param : Component\n"\
	"        Instance of component to add\n"\
	"Return:\n"\
	"----------\n"\
	"    Component: if added\n"\
	"    None: if not added\n"
);

// removeComponent
PyDoc_STRVAR(SceneObject_removeComponent_doc,
	"Remove a component from current scene object.\n"\
	"\n"\
	"SceneObject().removeComponent(param: string|Component)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    param : string\n"\
	"        Name of component to remove\n"\
    "    param : Component\n"\
	"        Instance of component to remove\n"\
	"Return:\n"\
	"----------\n"\
	"    True: if remove\n"\
	"    False: if not remove\n"
);

// getComponent
PyDoc_STRVAR(SceneObject_getComponent_doc,
	"Return an instance of component from current scene object.\n"\
	"\n"\
	"SceneObject().getComponent(param: string)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    param : string\n"\
	"        Name of component to get\n"\
	"Return:\n"\
	"----------\n"\
	"    Component: if success\n"\
	"    None: if fail\n"
);

// getComponents
PyDoc_STRVAR(SceneObject_getComponents_doc,
	"Return a list of all components from current scene object.\n"\
	"\n"\
	"SceneObject().getComponents()\n"\
	"\n"\
	"Return:\n"\
	"----------\n"\
	"    List of all attached components\n"
);

// removeComponents
PyDoc_STRVAR(SceneObject_removeComponents_doc,
	"Remove all components from current scene object.\n"\
	"\n"\
	"SceneObject().getComponents()\n"\
	"\n"\
	"Return:\n"\
	"----------\n"\
	"    True: if removed\n"\
    "    False: if not removed\n"\
);

PyDoc_STRVAR(SceneObject_invoke_doc,
	"Invoke scripts\'s method from current scene object.\n"\
	"\n"\
	"SceneObject().Invoke(param: string|ScriptPath, param: string|FunctionName, param: Object|PyObject)\n"\
	"\n"\
	"Return:\n"\
	"----------\n"\
	"    0: if worked\n"\
	"    -1: if not worked\n"\
);

// id
PyDoc_STRVAR(SceneObject_id_doc,
    "The ID of the scene object.\n"\
    "Type: number (unsigned long long).\n"
);

// uuid
PyDoc_STRVAR(SceneObject_uuid_doc,
    "The UUID of the scene object.\n"\
    "Type: string.\n"
);

// name
PyDoc_STRVAR(SceneObject_name_doc,
    "The ID of the scene object.\n"\
    "Type: string.\n"
);

// active
PyDoc_STRVAR(SceneObject_active_doc,
    "The active status of the scene object.\n"\
    "Type: boolean.\n"
);

// selected
PyDoc_STRVAR(SceneObject_selected_doc,
    "The selected status of the scene object.\n"\
    "Type: boolean.\n"
);

// parent
PyDoc_STRVAR(SceneObject_parent_doc,
    "The parent object of the object.\n"\
    "Type: SceneObject.\n"
);

// transform
PyDoc_STRVAR(SceneObject_transform_doc,
	"The transform component of the object.\n"\
	"Type: TransformComponent.\n"
);

// rectTransform
PyDoc_STRVAR(SceneObject_rectTransform_doc,
	"The transform component as RectTransform for UI elements.\n"\
	"Type: RectTransform.\n"
);
