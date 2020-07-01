#pragma once
#include <Python.h>

// getInstance
PyDoc_STRVAR(SceneManager_getInstance_doc,
	"Get SceneManager singleton instance.\n"\
	"\n"\
	"SceneManager.getInstance()\n"\
	"\n"\
	"Return:\n"\
	"----------\n"\
	"    SceneManager:\n"\
	"        Singleton instance of SceneManager\n"	
);

// loadScene
PyDoc_STRVAR(SceneManager_loadScene_doc,
	"Load scene from file system.\n"\
	"\n"\
	"SceneManager.loadScene(path: string)\n"\
	"\n"\
	"Parameters:\n"\
	"----------\n"\
	"    path: string\n"\
    "        Path to the scene file in file system.\n"\
	"Return:\n"\
	"----------\n"\
	"    True:\n"\
	"        Load success\n"\
    "    False:\n"\
	"        Load fail\n"
);

// unloadScene
PyDoc_STRVAR(SceneManager_unloadScene_doc,
	"Unload a scene.\n"\
	"\n"\
	"SceneManager.unloadScene(param: string|Scene)\n"\
	"\n"\
    "Parameters:\n"\
	"----------\n"\
	"    param: string\n"\
    "        Name of the scene to unload.\n"\
    "    param: Scene\n"\
    "        Instance of the scene to unload.\n"\
	"Return\n"\
	"----------\n"\
	"    True:\n"\
	"        Unload success\n"\
    "    False:\n"\
	"        Unload fail\n"
);

// currentScene
PyDoc_STRVAR(SceneManager_currentScene_doc,
	"Current active scene.\n"\
	"Type: Scene\n"
);
