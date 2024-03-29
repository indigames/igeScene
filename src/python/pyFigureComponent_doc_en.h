#pragma once

#include <Python.h>

// path
PyDoc_STRVAR(FigureComponent_path_doc,
    "Path to figure file.\n"\
    "Type: string\n"
);

// figure
PyDoc_STRVAR(FigureComponent_figure_doc,
    "Figure instance.\n"\
    "Type: Figure\n"
);

// fog
PyDoc_STRVAR(FigureComponent_fogEnabled_doc,
    "Whether fog to be enabled or disabled.\n"\
    "Type: bool\n"
    "Default: false\n"
);

// double side
PyDoc_STRVAR(FigureComponent_doubleSideEnabled_doc,
    "Whether double side render to be enabled or disabled. When disable, it use back face culling.\n"\
    "Type: bool\n"
    "Default: true\n"
);

// front face culling
PyDoc_STRVAR(FigureComponent_frontFaceCullingEnabled_doc,
    "Whether front face culling to be enabled or disabled. Only affected when doubleSideEnable set to false.\n"\
    "Type: bool\n"
    "Default: false\n"
);

// depth test
PyDoc_STRVAR(FigureComponent_depthTestEnabled_doc,
    "Whether depth test to be enabled or disabled.\n"\
    "Type: bool\n"
    "Default: true\n"
);

// depth write
PyDoc_STRVAR(FigureComponent_depthWriteEnabled_doc,
    "Whether depth write to be enabled or disabled.\n"\
    "Type: bool\n"
    "Default: true\n"
);

// scissor test
PyDoc_STRVAR(FigureComponent_scissorTestEnabled_doc,
    "Whether scissor test to be enabled or disabled.\n"\
    "Type: bool\n"
    "Default: false\n"
);

// updateRatio
PyDoc_STRVAR(FigureComponent_updateRatio_doc,
    "Frame update ratio (speedup/slower effects).\n"
    "Type: float\n");
