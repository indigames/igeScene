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

// color
PyDoc_STRVAR(SpriteComponent_color_doc,
    "Color.\n"\
    "Type: Vec4\n"
);

// fillMethod
PyDoc_STRVAR(SpriteComponent_fillMethod_doc,
    "Fill method: None = 0, Horizontal = 1, Vertical = 2, Radial90 = 3, Radial180 = 4, Radial360 = 5.\n"\
    "Type: int\n"
);

// fillOrigin
PyDoc_STRVAR(SpriteComponent_fillOrigin_doc,
    "Fill origin: Bottom = 0, Top = 1, Left = 2, Right = 3, BottomLeft = 4, TopLeft = 5, TopRight = 6, BottomRight = 7.\n"\
    "Type: int\n"
);

// fillAmount
PyDoc_STRVAR(SpriteComponent_fillAmount_doc,
    "Fill amount.\n"\
    "Type: float\n"
);

// clockwise
PyDoc_STRVAR(SpriteComponent_clockwise_doc,
    "Render clock wise.\n"\
    "Type: bool\n"
);

// texture
PyDoc_STRVAR(SpriteComponent_texture_doc,
    "Texture instance.\n"\
    "Type: Texture\n"
);

// SpriteComponent_figure_doc
PyDoc_STRVAR(SpriteComponent_figure_doc,
    "Editable Figure instance (Read only).\n"\
    "Type: EditableFigure\n"
);
