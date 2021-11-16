#include "python/pyScene.h"
#include "python/pyScene_doc_en.h"

#include "python/pySceneObject.h"
#include "python/pyCameraComponent.h"

#include "scene/SceneManager.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    PyObject* Scene_new(PyTypeObject* type, PyObject* args, PyObject* kw)
    {
        char* name;
        PyObject_Scene* self = nullptr;
        if (PyArg_ParseTuple(args, "s", &name)) {
            auto scene = SceneManager::getInstance()->createScene(std::string(name));
            self = (PyObject_Scene*)type->tp_alloc(type, 0);
            self->scene = scene;
        }
        return (PyObject*)self;
    }

    void  Scene_dealloc(PyObject_Scene *self)
    {
        if(self && self->scene.expired()) {
            self->scene.reset();
        }
        Py_TYPE(self)->tp_free(self);
    }

    PyObject* Scene_str(PyObject_Scene *self)
    {
        return PyUnicode_FromString("C++ Scene object");
    }

    // Get name
    PyObject* Scene_getName(PyObject_Scene* self)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        return PyUnicode_FromString(self->scene.lock()->getName().c_str());
    }

    // Set name
    int Scene_setName(PyObject_Scene* self, PyObject* value)
    {
        if (self->scene.expired()) return -1;
        if (PyUnicode_Check(value)) {
            const char* val = PyUnicode_AsUTF8(value);
            if(val != NULL)
                self->scene.lock()->setName(std::string(val));
        }
        return 0;
    }

    // Get active camera
    PyObject* Scene_getActiveCamera(PyObject_Scene* self)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        auto camera = self->scene.lock()->getActiveCamera();
        if (camera) {
           auto* obj = (PyObject_CameraComponent*)(&PyTypeObject_CameraComponent)->tp_alloc(&PyTypeObject_CameraComponent, 0);
           obj->component = camera;
           return (PyObject*)obj;
        }
        Py_RETURN_NONE;
    }

    // Set active camera
    int Scene_setActiveCamera(PyObject_Scene* self, PyObject* value)
    {
        if (self->scene.expired()) return -1;
        PyObject* camera;
        if (PyArg_ParseTuple(value, "O", &camera) && camera->ob_type == &PyTypeObject_CameraComponent) {
            auto cameraObj = (PyObject_CameraComponent*)camera;
            if(!cameraObj->component.expired())
                self->scene.lock()->setActiveCamera(std::dynamic_pointer_cast<CameraComponent>(cameraObj->component.lock()));
        }
        return 0;
    }

    // Create object
    PyObject* Scene_createObject(PyObject_Scene *self, PyObject* args)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        char* name = "";
        PyObject* parentObj = nullptr;
        if (PyArg_ParseTuple(args, "sO", &name, &parentObj)) {
            if (parentObj && parentObj->ob_type == &PyTypeObject_SceneObject) {
                auto* obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
                auto parent = (PyObject_SceneObject*)parentObj;
                obj->sceneObject = self->scene.lock()->createObject(name, parent->sceneObject.expired() ? nullptr : parent->sceneObject.lock());
                return (PyObject*)obj;
            }
        }
        Py_RETURN_NONE;
    }

    // Create object from Prefab
    PyObject* Scene_loadPrefab(PyObject_Scene* self, PyObject* args) {
        if (self->scene.expired()) Py_RETURN_NONE;
        char* path = "";
        char* name = "";
        PyObject* parentObj = nullptr;
        PyObject* position = nullptr;
        PyObject* rotation = nullptr;

        if (PyArg_ParseTuple(args, "ssO|OO", &path, &name, &parentObj, &position, &rotation)) {
            if (parentObj && parentObj->ob_type == &PyTypeObject_SceneObject) {
                auto parent = (PyObject_SceneObject*)parentObj;
                int type = 0;
                Vec3 pPosition = Vec3(0, 0, 0);
                if (position)
                {
                    int d;
                    float buff[4];
                    auto v1 = pyObjToFloat(position, buff, d);
                    if (v1)
                        pPosition = Vec3(v1[0], v1[1], v1[2]);
                    type++;
                }

                Quat pRotation = Quat(0, 0, 0, 1);
                if (rotation != nullptr)
                {
                    int d;
                    float buff[4];
                    auto v2 = pyObjToFloat(rotation, buff, d);
                    if (v2)
                        pRotation = Quat(v2[0], v2[1], v2[2], v2[3]);
                    type++;
                }
                std::shared_ptr<SceneObject> preObj = nullptr;

                switch (type)
                {
                case 0:
                default:
                    preObj = self->scene.lock()->createObjectFromPrefab(path, name, parent->sceneObject.expired() ? nullptr : parent->sceneObject.lock());
                    break;
                case 1:
                    preObj = self->scene.lock()->createObjectFromPrefab(path, name, parent->sceneObject.expired() ? nullptr : parent->sceneObject.lock(), pPosition);
                    break;
                case 2:
                    preObj = self->scene.lock()->createObjectFromPrefab(path, name, parent->sceneObject.expired() ? nullptr : parent->sceneObject.lock(), pPosition, pRotation);
                    break;
                }
                
                if (preObj) {
                    auto* obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
                    obj->sceneObject = preObj;
                    return (PyObject*)obj;
                }
            }
        }
        Py_RETURN_NONE;
    }

    // Remove object
    PyObject* Scene_removeObject(PyObject_Scene *self, PyObject* args)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        PyObject* obj = nullptr;
		if (PyArg_ParseTuple(args, "O", &obj))
        {
            if(obj)
            {
                if(PyUnicode_Check(obj))
                {
                    const char* name = PyUnicode_AsUTF8(obj);
                    auto sceneObject = self->scene.lock()->findObjectByName(std::string(name));
                    if(sceneObject)
                    {
                        if(self->scene.lock()->removeObject(sceneObject))
                        {
                            Py_RETURN_TRUE;
                        }
                    }
                }
                else if(PyNumber_Check(obj))
                {
                    uint64_t id = PyLong_AsUnsignedLongLong(obj);
                    if(self->scene.lock()->removeObjectById(id))
                    {
                        Py_RETURN_TRUE;
                    }
                }
                else if (obj->ob_type == &PyTypeObject_SceneObject)
                {
                    auto pySceneObject = (PyObject_SceneObject*)(obj);
                    if (!pySceneObject->sceneObject.expired()) {
                        if (self->scene.lock()->removeObjectById(pySceneObject->sceneObject.lock()->getId())) {
                            Py_RETURN_TRUE;
                        }
                    }
                }
            }
        }
        Py_RETURN_FALSE;
    }

    // Find object
    PyObject* Scene_findObject(PyObject_Scene *self, PyObject* args)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        PyObject* obj = nullptr;
		if (PyArg_ParseTuple(args, "O", &obj))
        {
            if (obj)
            {
                if (PyUnicode_Check(obj))
                {
                    const char* uuid = PyUnicode_AsUTF8(obj);
                    auto sceneObject = self->scene.lock()->findObjectByUUID(std::string(uuid));
                    if (sceneObject)
                    {
                        auto* obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
                        obj->sceneObject = sceneObject;
                        return (PyObject*)obj;
                    }
                }
                else if (PyNumber_Check(obj))
                {
                    uint64_t id = PyLong_AsUnsignedLongLong(obj);
                    auto sceneObject = self->scene.lock()->findObjectById(id);
                    if (sceneObject)
                    {
                        auto* obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
                        obj->sceneObject = sceneObject;
                        return (PyObject*)obj;
                    }
                }
            }
        }
        Py_RETURN_NONE;
    }

    // Find object by name
    PyObject* Scene_findObjectByName(PyObject_Scene* self, PyObject* args)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        PyObject* obj = nullptr;
        if (PyArg_ParseTuple(args, "O", &obj))
        {
            if (obj)
            {
                if (PyUnicode_Check(obj))
                {
                    const char* name = PyUnicode_AsUTF8(obj);
                    auto sceneObject = self->scene.lock()->findObjectByName(std::string(name));
                    if (sceneObject)
                    {
                        auto* obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
                        obj->sceneObject = sceneObject;
                        return (PyObject*)obj;
                    }
                }
            }
        }
        Py_RETURN_NONE;
    }

    // Get roots
    PyObject* Scene_getObjects(PyObject_Scene *self)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        auto roots = self->scene.lock()->getObjects();
        PyObject* pyList = PyList_New(0);
        for(int i = 0; i < roots.size(); ++i)
        {
            auto obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            obj->sceneObject = roots[i];
            PyList_Append(pyList, (PyObject*)obj);
            Py_XDECREF(obj);
        }
        return (PyObject*)pyList;
    }

    // Get root object
    PyObject* Scene_getRoot(PyObject_Scene *self)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        auto obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
        obj->sceneObject = self->scene.lock()->getRoot();
        return (PyObject*)obj;
    }

    // Get path
    PyObject* Scene_getPath(PyObject_Scene *self)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        return PyUnicode_FromString(self->scene.lock()->getPath().c_str());
    }

    // Get showcase
    PyObject* Scene_getShowcase(PyObject_Scene *self)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        auto showcase = self->scene.lock()->getShowcase();
        if (showcase) {
            auto obj = (showcase_obj*)(&ShowcaseType)->tp_alloc(&ShowcaseType, 0);
            obj->showcase = showcase;
            return (PyObject*)obj;
        }
        Py_RETURN_NONE;
    }

    // Get environment
    PyObject* Scene_getEnvironment(PyObject_Scene *self)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        auto obj = (environment_obj*)(&EnvironmentType)->tp_alloc(&EnvironmentType, 0);
        obj->envSet = self->scene.lock()->getEnvironment();
        return (PyObject *)obj;
    }

    // Raycast
    PyObject* Scene_raycast(PyObject_Scene* self, PyObject* args)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        PyObject* position;
        PyObject* direction;
        float distance = 10000.f;

        if (!PyArg_ParseTuple(args, "OO|i", &position, &direction, &distance))
            return NULL;

        int d;
        float buff[4];
        auto v1 = pyObjToFloat(position, buff, d);
        if (!v1)
            return NULL;

        auto v2 = pyObjToFloat(direction, buff, d);
        if (!v2)
            return NULL;

        Vec3 pPosition = Vec3(v1[0], v1[1], v1[2]);
        Vec3 pDirection = Vec3(v2[0], v2[1], v2[2]);
        
        auto hit = self->scene.lock()->raycast(pPosition, pDirection, distance);
        if (hit.first == nullptr)
            Py_RETURN_NONE;

        auto hitObj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
        hitObj->sceneObject = hit.first;

        auto hitPos = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(hit.second.P(), 3, hitPos->v);
        hitPos->d = 3;

        PyObject* res = Py_BuildValue("{s:O,s:O}",
            "hitObject", hitObj,
            "hitPosition", hitPos);
        Py_XDECREF(hitObj);
        Py_XDECREF(hitPos);
        return res;
    }

    PyObject* Scene_raycastAll(PyObject_Scene* self, PyObject* args)
    {
        PyObject* position;
        PyObject* direction;
        float distance = 10000.f;

        if (!PyArg_ParseTuple(args, "OO|i", &position, &direction, &distance))
            return NULL;

        int d;
        float buff[4];
        auto v1 = pyObjToFloat(position, buff, d);
        if (!v1)
            return NULL;

        auto v2 = pyObjToFloat(direction, buff, d);
        if (!v2)
            return NULL;

        Vec3 pPosition = Vec3(v1[0], v1[1], v1[2]);
        Vec3 pDirection = Vec3(v2[0], v2[1], v2[2]);

        auto hits = self->scene.lock()->raycastAll(pPosition, pDirection, distance);
        int len = hits.size();
        if (len == 0)
            Py_RETURN_NONE;

        auto hitTuple = PyTuple_New(len);
        for (int i = 0; i < len; ++i)
        {
            auto hitObj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            hitObj->sceneObject = hits[i].first;

            auto hitPos = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(hits[i].second.P(), 3, hitPos->v);
            hitPos->d = 3;

            PyObject* res = Py_BuildValue("{s:O,s:O}",
                "hitObject", hitObj,
                "hitPosition", hitPos);

            Py_XDECREF(hitObj);
            Py_XDECREF(hitPos);

            PyTuple_SetItem(hitTuple, i, res);
        }
        return (PyObject*)hitTuple;
    }

    PyObject* Scene_raycastFromCamera(PyObject_Scene *self, PyObject* args)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        PyObject *screenPosObj;
        PyObject *cameraObj;
        float distance = 10000.f;

        if (!PyArg_ParseTuple(args, "OO|f", &screenPosObj, &cameraObj, &distance))
            return NULL;

        int d;
        float buff[4];
        auto v = pyObjToFloat(screenPosObj, buff, d);
        if (!v)
            return NULL;

        Vec2 screenPos = Vec2(v[0], v[1]);
        Camera* camera = nullptr;
        if(cameraObj->ob_type == &CameraType)
        {
            auto camObj = (camera_obj*)cameraObj;
            camera = camObj->camera;
        }
        else if(cameraObj->ob_type == &PyTypeObject_CameraComponent)
        {
            auto cameraCompObj = (PyObject_CameraComponent*)cameraObj;
            if (!cameraCompObj->component.expired()) {
                camera = std::dynamic_pointer_cast<CameraComponent>(cameraCompObj->component.lock())->getCamera();
            }
        }
        else if(cameraObj->ob_type == &PyTypeObject_SceneObject)
        {
            auto sceneObj = (PyObject_SceneObject*)cameraObj;
            if (!sceneObj->sceneObject.expired()) {
                auto cameraComp = sceneObj->sceneObject.lock()->getComponent<CameraComponent>();
                if (cameraComp) {
                    camera = cameraComp->getCamera();
                }
            }
        }

        if (!camera)
            return NULL;

        auto hit = self->scene.lock()->raycast(screenPos, camera, distance);
        if(hit.first == nullptr)
            Py_RETURN_NONE;

        auto hitObj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
        hitObj->sceneObject = hit.first;

        auto hitPos = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(hit.second.P(), 3, hitPos->v);
        hitPos->d = 3;

        PyObject *res = Py_BuildValue("{s:O,s:O}",
                                      "hitObject", hitObj,
                                      "hitPosition", hitPos);
        Py_XDECREF(hitObj);
        Py_XDECREF(hitPos);
        return res;
    }

    PyObject* Scene_raycastUI(PyObject_Scene* self, PyObject* args)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        PyObject* screenPosObj;

        if (!PyArg_ParseTuple(args, "O", &screenPosObj))
            return NULL;

        int d;
        float buff[4];
        auto v = pyObjToFloat(screenPosObj, buff, d);
        if (!v)
            return NULL;

        Vec2 screenPos = Vec2(v[0], v[1]);

        auto hit = self->scene.lock()->raycastUI(screenPos);
        if (hit.first == nullptr)
            Py_RETURN_NONE;

        auto hitObj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
        hitObj->sceneObject = hit.first;

        auto hitPos = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(hit.second.P(), 3, hitPos->v);
        hitPos->d = 3;

        PyObject* res = Py_BuildValue("{s:O,s:O}",
            "hitObject", hitObj,
            "hitPosition", hitPos);
        Py_XDECREF(hitObj);
        Py_XDECREF(hitPos);
        return res;
    }

    PyObject* Scene_screenToWorldPoint(PyObject_Scene* self, PyObject* args)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        PyObject* screenPosObj;
        PyObject* cameraObj;
        float distance = 10000.f;

        if (!PyArg_ParseTuple(args, "OO|f", &screenPosObj, &cameraObj, &distance))
            return NULL;

        int d;
        float buff[4];
        auto v = pyObjToFloat(screenPosObj, buff, d);
        if (!v)
            return NULL;

        Vec2 screenPos = Vec2(v[0], v[1]);
        Camera* camera = nullptr;
        if (cameraObj->ob_type == &CameraType)
        {
            auto camObj = (camera_obj*)cameraObj;
            camera = camObj->camera;
        }
        else if (cameraObj->ob_type == &PyTypeObject_CameraComponent)
        {
            auto cameraCompObj = (PyObject_CameraComponent*)cameraObj;
            if (!cameraCompObj->component.expired()) {
                camera = std::dynamic_pointer_cast<CameraComponent>(cameraCompObj->component.lock())->getCamera();
            }
        }
        else if (cameraObj->ob_type == &PyTypeObject_SceneObject)
        {
            auto sceneObj = (PyObject_SceneObject*)cameraObj;
            if (!sceneObj->sceneObject.expired()) {
                auto cameraComp = sceneObj->sceneObject.lock()->getComponent<CameraComponent>();
                if (cameraComp) {
                    camera = cameraComp->getCamera();
                }
            }
        }

        if (!camera)
            return NULL;

        auto screenPoint = self->scene.lock()->screenToWorldPoint(screenPos, camera, distance);
       
        auto pos = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(screenPoint.P(), 3, pos->v);
        pos->d = 3;
        return (PyObject*)pos;
    }

    PyObject* Scene_worldToScreenPoint(PyObject_Scene* self, PyObject* args)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        PyObject* worldPosObj;
        PyObject* cameraObj;
        float distance = 10000.f;

        if (!PyArg_ParseTuple(args, "OO", &worldPosObj, &cameraObj))
            return NULL;

        int d;
        float buff[4];
        auto v = pyObjToFloat(worldPosObj, buff, d);
        if (!v)
            return NULL;

        Vec3 worldPos = Vec3(v[0], v[1], v[2]);
        Camera* camera = nullptr;
        if (cameraObj->ob_type == &CameraType)
        {
            auto camObj = (camera_obj*)cameraObj;
            camera = camObj->camera;
        }
        else if (cameraObj->ob_type == &PyTypeObject_CameraComponent)
        {
            auto cameraCompObj = (PyObject_CameraComponent*)cameraObj;
            if (!cameraCompObj->component.expired()) {
                camera = std::dynamic_pointer_cast<CameraComponent>(cameraCompObj->component.lock())->getCamera();
            }
        }
        else if (cameraObj->ob_type == &PyTypeObject_SceneObject)
        {
            auto sceneObj = (PyObject_SceneObject*)cameraObj;
            if (!sceneObj->sceneObject.expired()) {
                auto cameraComp = sceneObj->sceneObject.lock()->getComponent<CameraComponent>();
                if (cameraComp) {
                    camera = cameraComp->getCamera();
                }
            }
        }

        if (!camera)
            return NULL;

        auto screenPoint = self->scene.lock()->worldToScreenPoint(worldPos, camera);

        auto pos = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(screenPoint.P(), 2, pos->v);
        pos->d = 2;
        return (PyObject*)pos;
    }

    PyObject* Scene_convertScreenPoint(PyObject_Scene* self, PyObject* args)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        PyObject* screenPosObj;

        if (!PyArg_ParseTuple(args, "O", &screenPosObj))
            return NULL;

        int d;
        float buff[4];
        auto v = pyObjToFloat(screenPosObj, buff, d);
        if (!v)
            return NULL;

        Vec2 screenPos = Vec2(v[0], v[1]);
        auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(self->scene.lock()->getScreenPos(screenPos).P(), 2, vec2Obj->v);
        vec2Obj->d = 2;
        return (PyObject*)vec2Obj;
    }

    PyObject* Scene_getScreenSize(PyObject_Scene* self)
    {
        if (self->scene.expired()) Py_RETURN_NONE;
        auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(self->scene.lock()->getWindowSize().P(), 2, vec2Obj->v);
        vec2Obj->d = 2;
        return (PyObject*)vec2Obj;
    }

    // Compare function
    static PyObject* Scene_richcompare(PyObject* self, PyObject* other, int op)
    {
        if (op == Py_LT || op == Py_LE || op == Py_GT || op == Py_GE)
        {
            PyErr_Format(PyExc_TypeError, "Only '==' and '!=' are allowed as comparison operators");
            Py_RETURN_NOTIMPLEMENTED;
        }

        PyObject* result = Py_False;
        if (self != Py_None && other != Py_None)
        {
            if (other->ob_type == &PyTypeObject_Scene)
            {
                auto selfCmp = (PyObject_Scene*)(self);
                auto otherCmp = (PyObject_Scene*)(other);

                if (selfCmp->scene.expired() || otherCmp->scene.expired()) {
                    result = Py_False;
                }
                else {
                    bool eq = (selfCmp->scene.lock() == otherCmp->scene.lock());
                    if (op == Py_NE)
                        eq = !eq;
                    result = eq ? Py_True : Py_False;
                }
            }
            else
            {
                result = (op == Py_EQ) ? Py_False : Py_True;
            }
        }
        else if (self == Py_None && other == Py_None)
        {
            result = (op == Py_EQ) ? Py_True : Py_False;
        }
        else
        {
            result = (op == Py_EQ) ? Py_False : Py_True;
        }
        Py_INCREF(result);
        return result;
    }

    // Methods definition
    PyMethodDef Scene_methods[] = {
        { "createObject", (PyCFunction)Scene_createObject, METH_VARARGS, Scene_createObject_doc },
        { "loadPrefab", (PyCFunction)Scene_loadPrefab, METH_VARARGS, Scene_loadPrefab_doc },
        { "removeObject", (PyCFunction)Scene_removeObject, METH_VARARGS, Scene_removeObject_doc },
        { "findObject", (PyCFunction)Scene_findObject, METH_VARARGS, Scene_findObject_doc },
        { "findObjectByName", (PyCFunction)Scene_findObjectByName, METH_VARARGS, Scene_findObject_doc },
        { "getObjects", (PyCFunction)Scene_getObjects, METH_NOARGS, Scene_getObjects_doc },
        { "getRoot", (PyCFunction)Scene_getRoot, METH_NOARGS, Scene_getRoot_doc },
        { "getPath", (PyCFunction)Scene_getPath, METH_NOARGS, Scene_getPath_doc },
        { "getShowcase", (PyCFunction)Scene_getShowcase, METH_NOARGS, Scene_getShowcase_doc },
        { "getEnvironment", (PyCFunction)Scene_getEnvironment, METH_NOARGS, Scene_getEnvironment_doc },
        { "raycast", (PyCFunction)Scene_raycast, METH_VARARGS, Scene_raycast_doc },
        { "raycastAll", (PyCFunction)Scene_raycastAll, METH_VARARGS, NULL },
        { "raycastFromCamera", (PyCFunction)Scene_raycastFromCamera, METH_VARARGS, Scene_raycastFromCamera_doc },
        { "raycastUI", (PyCFunction)Scene_raycastUI, METH_VARARGS, Scene_raycastUI_doc },
        { "screenToWorldPoint", (PyCFunction)Scene_screenToWorldPoint, METH_VARARGS, NULL },
        { "worldToScreenPoint", (PyCFunction)Scene_worldToScreenPoint, METH_VARARGS, NULL },
        { "convertScreenPoint", (PyCFunction)Scene_convertScreenPoint, METH_VARARGS, NULL },
        { "getScreenSize", (PyCFunction)Scene_getScreenSize, METH_NOARGS, NULL },
        { NULL, NULL }
    };

    // Variable definition
    PyGetSetDef Scene_getsets[] = {
        { "name", (getter)Scene_getName, (setter)Scene_setName, Scene_name_doc, NULL },
        { "activeCamera", (getter)Scene_getActiveCamera, (setter)Scene_setActiveCamera, Scene_activeCamera_doc, NULL },
        { NULL, NULL }
    };

    // Type definition
    PyTypeObject PyTypeObject_Scene = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.Scene",                   /* tp_name */
        sizeof(PyObject_Scene),             /* tp_basicsize */
        0,                                  /* tp_itemsize */
        (destructor)Scene_dealloc,          /* tp_dealloc */
        0,                                  /* tp_print */
        0,                                  /* tp_getattr */
        0,                                  /* tp_setattr */
        0,                                  /* tp_reserved */
        0,                                  /* tp_repr */
        0,                                  /* tp_as_number */
        0,                                  /* tp_as_sequence */
        0,                                  /* tp_as_mapping */
        0,                                  /* tp_hash */
        0,                                  /* tp_call */
        (reprfunc)Scene_str,                /* tp_str */
        0,                                  /* tp_getattro */
        0,                                  /* tp_setattro */
        0,                                  /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                 /* tp_flags */
        0,                                  /* tp_doc */
        0,                                  /* tp_traverse */
        0,                                  /* tp_clear */
        Scene_richcompare,                  /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,                                  /* tp_iter */
        0,                                  /* tp_iternext */
        Scene_methods,                      /* tp_methods */
        0,                                  /* tp_members */
        Scene_getsets,                      /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        0,                                  /* tp_init */
        0,                                  /* tp_alloc */
        Scene_new,                          /* tp_new */
        0,                                  /* tp_free */
    };
}
