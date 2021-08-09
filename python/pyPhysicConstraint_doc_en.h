#pragma once

#include <Python.h>

// type
PyDoc_STRVAR(PhysicConstraint_type_doc,
    "The type of the physic constraint.\n"\
    "Type: int\n"
);

// owner
PyDoc_STRVAR(PhysicConstraint_owner_doc,
    "The physic object that own this constraint.\n"\
    "Type: PhysicObject\n"
);

// other
PyDoc_STRVAR(PhysicConstraint_other_doc,
    "The physic object that linked to this constraint.\n"\
    "Type: PhysicObject\n"
);

// isEnabled
PyDoc_STRVAR(PhysicConstraint_isEnabled_doc,
    "Enable/disable this constraint. Default: true.\n"\
    "Type: bool\n"
);

// isBodiesCollisionEnabled
PyDoc_STRVAR(PhysicConstraint_isBodiesCollisionEnabled_doc,
    "Enable/disable collision between linked bodies. Default: true.\n"\
    "Type: bool\n"
);

// breakingImpulse
PyDoc_STRVAR(PhysicConstraint_breakingImpulse_doc,
    "Breaking impulse threshold. Default: Very big number.\n"\
    "Type: float\n"
);
