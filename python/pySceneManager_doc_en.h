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

// createScene
PyDoc_STRVAR(SceneManager_createScene_doc,
    "Create new scene.\n"\
    "\n"\
    "SceneManager.createScene(scene)\n"\
    "\n"\
    "Parameters:\n"\
    "----------\n"\
    "    scene: string\n"\
    "        Name of the scene.\n"\
    "Return:\n"\
    "----------\n"\
    "    scene: Scene\n"
);

// loadScene
PyDoc_STRVAR(SceneManager_loadScene_doc,
    "Load scene from file system.\n"\
    "\n"\
    "SceneManager.loadScene(path)\n"\
    "\n"\
    "Parameters:\n"\
    "----------\n"\
    "    path: string\n"\
    "        Path to the scene file in file system.\n"\
    "Return:\n"\
    "----------\n"\
    "    scene: Scene\n"
);

// unloadScene
PyDoc_STRVAR(SceneManager_unloadScene_doc,
    "Unload scene from memory.\n"\
    "\n"\
    "SceneManager.unloadScene(scene)\n"\
    "\n"\
    "Parameters:\n"\
    "----------\n"\
    "    scene: string\n"\
    "        scene name.\n"\
	"    scene: Scene\n"\
    "        scene instance.\n"\
    "Return:\n"\
    "----------\n"\
    "    True:\n"\
    "        Success\n"\
    "    False:\n"\
    "        Fail\n"
);

// saveScene
PyDoc_STRVAR(SceneManager_saveScene_doc,
    "Save scene to file system.\n"\
    "\n"\
    "SceneManager.saveScene(path)\n"\
    "\n"\
    "Parameters:\n"\
    "----------\n"\
    "    path: string\n"\
    "        Path to save.\n"\
    "Return:\n"\
    "----------\n"\
    "    True:\n"\
    "        Success\n"\
    "    False:\n"\
    "        Fail\n"
);

// currentScene
PyDoc_STRVAR(SceneManager_currentScene_doc,
    "Current activated scene.\n"\
    "Type: Scene\n"
);

// scenes
PyDoc_STRVAR(SceneManager_scenes_doc,
    "List of all loaded scenes.\n"\
    "Type: List<Scene>\n"
);
