#pragma once
#include <Python.h>

// addChild
PyDoc_STRVAR(SceneObject_addChild_doc,
	"Add child to current scene object.\n"\
	"\n"\
	"SceneObject().addChild(param: string|number|SceneObject)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    param : string\n"\
	"        Name of the scene object to add\n"\
	"    param : number (unsigned long long)\n"\
	"        ID of the scene object to add\n"\
	"    param : SceneObject\n"\
	"        Instance of the scene object to add\n"\
	"Return:\n"\
	"----------\n"\
	"    True: if added\n"\
	"    False: if not added\n"
);

// removeChild
PyDoc_STRVAR(SceneObject_removeChild_doc,
	"Remove a child from current scene object.\n"\
	"\n"\
	"SceneObject().removeChild(param: string|number|SceneObject)\n"\
	"\n"\
	"Parameters\n"\
	"----------\n"\
	"    param : string\n"\
	"        Name of the scene object to remove\n"\
	"    param : number (unsigned long long)\n"\
	"        ID of the scene object to remove\n"\
	"    param : SceneObject\n"\
	"        Instance of the scene object to remove\n"\
	"Return:\n"\
	"----------\n"\
	"    True: if removed\n"\
	"    False: if not removed\n"
);

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

// findObject
PyDoc_STRVAR(SceneObject_findObject_doc,
	"Find a object from current scene object (as a tree).\n"\
	"\n"\
	"SceneObject().findObject(param: string|number)\n"\
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
	"    True: if added\n"\
	"    False: if not added\n"
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

// id
PyDoc_STRVAR(SceneObject_id_doc,
    "The ID of the scene object.\n"\
    "Type: number (unsigned long long).\n"
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

// parent
PyDoc_STRVAR(SceneObject_parent_doc,
    "The parent object of the object.\n"\
    "Type: SceneObject.\n"
);
