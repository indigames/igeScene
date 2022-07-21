#pragma once

#include <Python.h>

// path
PyDoc_STRVAR(EditableFigureComponent_path_doc,
    "Path to figure file.\n"\
    "Type: string\n"
);

// figure
PyDoc_STRVAR(EditableFigureComponent_figure_doc,
    "Editable Figure instance.\n"\
    "Type: EditableFigure\n"
);

// fog
PyDoc_STRVAR(EditableFigureComponent_fogEnabled_doc,
    "Whether fog to be enabled or disabled.\n"\
    "Type: bool\n"
    "Default: false\n"
);

// double side
PyDoc_STRVAR(EditableFigureComponent_doubleSideEnabled_doc,
    "Whether double side render to be enabled or disabled. When disable, it use back face culling.\n"\
    "Type: bool\n"
    "Default: true\n"
);

// front face culling
PyDoc_STRVAR(EditableFigureComponent_frontFaceCullingEnabled_doc,
    "Whether front face culling to be enabled or disabled. Only affected when doubleSideEnable set to false.\n"\
    "Type: bool\n"
    "Default: false\n"
);

// depth test
PyDoc_STRVAR(EditableFigureComponent_depthTestEnabled_doc,
    "Whether depth test to be enabled or disabled.\n"\
    "Type: bool\n"
    "Default: true\n"
);

// depth write
PyDoc_STRVAR(EditableFigureComponent_depthWriteEnabled_doc,
    "Whether depth write to be enabled or disabled.\n"\
    "Type: bool\n"
    "Default: true\n"
);

// scissor test
PyDoc_STRVAR(EditableFigureComponent_scissorTestEnabled_doc,
    "Whether scissor test to be enabled or disabled.\n"\
    "Type: bool\n"
    "Default: false\n"
);

// updateRatio
PyDoc_STRVAR(EditableFigureComponent_updateRatio_doc,
    "Frame update ratio (speedup/slower effects).\n"
    "Type: float\n");
