#pragma once
#include <Python.h>

// enable
PyDoc_STRVAR(SpringConstraint_enable_doc,
    "Enable vector. Default (1.0, 0.0, 0.0).\n"\
    "Type: Vec3\n"
);

// stiffness
PyDoc_STRVAR(SpringConstraint_stiffness_doc,
    "Stiffness vector. Default (10.0, 0.0, 0.0).\n"\
    "Type: Vec3\n"
);

// damping
PyDoc_STRVAR(SpringConstraint_damping_doc,
    "Damping vector. Default (0.5, 0.0, 0.0).\n"\
    "Type: Vec3\n"
);

// lowerLimit
PyDoc_STRVAR(SpringConstraint_lowerLimit_doc,
    "Lower limit vector. Default (0.0, 0.0, 0.0).\n"\
    "Type: Vec3\n"
);

// upperLimit
PyDoc_STRVAR(SpringConstraint_upperLimit_doc,
    "Upper limit vector. Default (0.0, 0.0, 0.0)\n"\
    "Type: Vec3\n"
);
