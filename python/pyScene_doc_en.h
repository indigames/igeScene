#pragma once

#include <Python.h>

// root
PyDoc_STRVAR(Scene_root_doc,
	"[Readonly] The root object in the scene.\n");

// createObject
PyDoc_STRVAR(Scene_createObject_doc,
			 "Create new scene object.\n"
			 "\n"
			 "Scene().createObject(name: string, parent: SceneObject = None)\n"
			 "\n"
			 "Parameters\n"
			 "----------\n"
			 "    name : string\n"
			 "        Name of the new scene object\n"
			 "    parent : SceneObject\n"
			 "        Parent object\n");

// loadPrefab
PyDoc_STRVAR(Scene_loadPrefab_doc,
			 "Create new scene object from prefab.\n"
			 "\n"
			 "Scene().loadPrefab(path: string, name: string, parent: SceneObject = None)\n"
			 "\n"
			 "Parameters\n"
			 "----------\n"
			 "    path : string\n"
			 "        Path to the prefab\n"
			 "    name : string\n"
			 "        Name of the new scene object\n"
			 "    parent : SceneObject\n"
			 "        Parent object\n"
			 "Return:\n"
			 "----------\n"
			 "    obj: SceneObject\n");

// removeObject
PyDoc_STRVAR(Scene_removeObject_doc,
			 "Remove object out of scene.\n"
			 "\n"
			 "Scene().removeObject(param: string|number|SceneObject)\n"
			 "\n"
			 "Parameters\n"
			 "----------\n"
			 "    param : string\n"
			 "        Name of the scene object to remove\n"
			 "    param : number (unsigned long long)\n"
			 "        ID of the scene object to remove\n"
			 "    param : SceneObject\n"
			 "        Scene object instance to remove\n"
			 "Return:\n"
			 "----------\n"
			 "    True: if removed success\n"
			 "    False: if removed fail\n");

// findObject
PyDoc_STRVAR(Scene_findObject_doc,
			 "Find a object in the scene.\n"
			 "\n"
			 "Scene().findObject(param: string|number)\n"
			 "\n"
			 "Parameters\n"
			 "----------\n"
			 "    param : string\n"
			 "        UUID of the scene object to find\n"
			 "    param : number (unsigned long long)\n"
			 "        ID of the scene object to find\n"
			 "Return:\n"
			 "----------\n"
			 "    SceneObject: if found\n"
			 "    None: if not found\n");

// getObjects
PyDoc_STRVAR(Scene_getObjects_doc,
			 "Get the list of all objects in the scene.\n"
			 "\n"
			 "Scene().getObjects()\n"
			 "\n"
			 "Return:\n"
			 "----------\n"
			 "    Type: List<SceneObject>\n");

// getRoot
PyDoc_STRVAR(Scene_getRoot_doc,
			 "Get the root object in the scene.\n"
			 "\n"
			 "Scene().getRoot()\n"
			 "\n"
			 "Return:\n"
			 "----------\n"
			 "    Type: SceneObject\n");

// getPath
PyDoc_STRVAR(Scene_getPath_doc,
			 "Get the relative path to the scene file.\n"
			 "\n"
			 "Scene().getPath()\n"
			 "\n"
			 "Return:\n"
			 "----------\n"
			 "    Type: string\n");

// getShowcase
PyDoc_STRVAR(Scene_getShowcase_doc,
			 "Get the associated showcase instance.\n"
			 "\n"
			 "Scene().getShowcase()\n"
			 "\n"
			 "Return:\n"
			 "----------\n"
			 "    Type: igeCore.showcase\n");

// getEnvironment
PyDoc_STRVAR(Scene_getEnvironment_doc,
			 "Get the associated environment instance.\n"
			 "\n"
			 "Scene().getEnvironment()\n"
			 "\n"
			 "Return:\n"
			 "----------\n"
			 "    Type: igeCore.environment\n");

// Raycast
PyDoc_STRVAR(Scene_raycast_doc,	
	"Perform raycast to get the hit object and hit position.\n"
	"\n"
	"Scene().raycast(origin: Vec3, direction: Vec3, distance: float)\n"
	"\n"
	"Parameters:\n"
	"----------\n"
	"    origin: Vec3\n"
	"      the ray's start point \n"
	"    direction: Vec3\n"
	"      the ray's direction\n"
	"    distance: float [Optional]\n"
	"      Max distance. Default: 10000.f\n"
	"Return:\n"
	"----------\n"
	"    Tuple: [object, position]\n");

PyDoc_STRVAR(Scene_raycastFromCamera_doc,	
	"Perform raycast to get the hit object and hit position.\n"
	"\n"
	"Parameters:\n"
	"----------\n"
	"    screenPos: Vec2\n"
	"      Screen position (2d screen space)\n"
	"    camera: pyxieCamera, CameraComponent, Camera SceneObject\n"
	"      Camera to perform raycast\n"
	"    distance: float [Optional]\n"
	"      Max distance. Default: 10000.f\n"
	"Return:\n"
	"----------\n"
	"    Tuple: [object, position]\n");

PyDoc_STRVAR(Scene_raycastUI_doc,	
	"Perform raycast to get the hit object and hit position on UI layout.\n"
	"\n"
	"Scene().raycastUI(screenPos: Vec2, camera: Camera, distance: float)\n"
	"\n"
	"Parameters:\n"
	"----------\n"
	"    screenPos: Vec2\n"
	"      Screen position (2d screen space)\n"
	"    camera: pyxieCamera, CameraComponent, Camera SceneObject\n"
	"      Camera to perform raycast\n"
	"    distance: float [Optional]\n"
	"      Max distance. Default: 10000.f\n"
	"Return:\n"
	"----------\n"
	"    Tuple: [object, position]\n");

PyDoc_STRVAR(Scene_capture_doc,
	"Capture the current rendered scene to device storage as a PNG file.\n"
	"To capture whole scene, call Scene.capture() in onRenderUI().\n"
	"To capture just 3D scene, call it in onRender().\n"
	"Call the function else where like onUpdate, onClick, ... will return false.\n"
	"\n"
	"Scene().capture(file_name: string)\n"
	"\n"
	"Parameters:\n"
	"----------\n"
	"    file_name: string\n"
	"      Name of the file, without extension.\n"
	"Return:\n"
	"----------\n"
	"    result: bool\n");

// name
PyDoc_STRVAR(Scene_name_doc,
			 "Name of scene.\n"
			 "Type: string\n");

// activeCamera
PyDoc_STRVAR(Scene_activeCamera_doc,
			 "The current active camera.\n"
			 "Type: CameraComponent\n");
