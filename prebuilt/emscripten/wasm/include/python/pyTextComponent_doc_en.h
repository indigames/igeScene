#pragma once

#include <Python.h>

// text
PyDoc_STRVAR(TextComponent_text_doc,
    "Text.\n"\
    "Type: String\n"
);

// fontPath
PyDoc_STRVAR(TextComponent_fontPath_doc,
    "Font path.\n"\
    "Type: String\n"
);

// fontSize
PyDoc_STRVAR(TextComponent_fontSize_doc,
    "Font size.\n"\
    "Type: int\n"
);

// color
PyDoc_STRVAR(TextComponent_color_doc,
    "Color.\n"\
    "Type: Vec4\n"
);


// isBillboard
PyDoc_STRVAR(TextComponent_isBillboard_doc,
    "Render as billboard.\n"\
    "Type: bool\n"
);
