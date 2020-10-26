#pragma once

#include <Python.h>

// path
PyDoc_STRVAR(SpriteComponent_path_doc,
    "Path to texture file.\n"\
    "Type: string\n"
);

// size
PyDoc_STRVAR(SpriteComponent_size_doc,
    "Size of sprite.\n"\
    "Type: Vec2\n"
);

// isBillboard
PyDoc_STRVAR(SpriteComponent_isBillboard_doc,
    "Render as billboard.\n"\
    "Type: bool\n"
);
