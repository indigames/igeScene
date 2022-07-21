#pragma once
#include <Python.h>

// anchor
PyDoc_STRVAR(HingeConstraint_anchor_doc,
    "Anchor point. Default (0.0, 0.0, 0.0).\n"\
    "Type: Vec3\n"
);

// axis1
PyDoc_STRVAR(HingeConstraint_axis1_doc,
    "The first axis. Default (0.0, 1.0, 0.0).\n"\
    "Type: Vec3\n"
);

// axis2
PyDoc_STRVAR(HingeConstraint_axis2_doc,
    "The second axis. Default (1.0, 0.0, 0.0).\n"\
    "Type: Vec3\n"
);

// lowerLimit
PyDoc_STRVAR(HingeConstraint_lowerLimit_doc,
    "Lower angle limit in radian. Default -3.14.\n"\
    "Type: float\n"
);

// upperLimit
PyDoc_STRVAR(HingeConstraint_upperLimit_doc,
    "Upper angle limit in radian. Default 3.14.\n"\
    "Type: float\n"
);
