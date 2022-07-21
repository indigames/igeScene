#pragma once

#include <Python.h>

// position
PyDoc_STRVAR(TransformComponent_localPosition_doc,
    "Position.\n"\
    "Type: Vec3\n"
);

// rotation
PyDoc_STRVAR(TransformComponent_localRotation_doc,
    "Rotation.\n"\
    "Type: Quat\n"
);

// scale
PyDoc_STRVAR(TransformComponent_localScale_doc,
    "Scale.\n"\
    "Type: Vec3\n"
);

// localMatrix
PyDoc_STRVAR(TransformComponent_localMatrix_doc,
    "Local transformation matrix.\n"\
    "Type: Mat4\n"
);

// worldPosition
PyDoc_STRVAR(TransformComponent_worldPosition_doc,
    "World Position.\n"\
    "Type: Vec3\n"
);

// worldRotation
PyDoc_STRVAR(TransformComponent_worldRotation_doc,
    "World Rotation.\n"\
    "Type: Quat\n"
);

// worldScale
PyDoc_STRVAR(TransformComponent_worldScale_doc,
    "World Scale.\n"\
    "Type: Vec3\n"
);

// worldMatrix
PyDoc_STRVAR(TransformComponent_worldMatrix_doc,
    "World transformation matrix.\n"\
    "Type: Mat4\n"
);
