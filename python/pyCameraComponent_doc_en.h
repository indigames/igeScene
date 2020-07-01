#pragma once

#include <Python.h>

// position
PyDoc_STRVAR(CameraComponent_position_doc,
    "Position.\n"\
    "Type: Vec3\n"
);

// rotation
PyDoc_STRVAR(CameraComponent_rotation_doc,
    "Rotation.\n"\
    "Type: Quat\n"
);

// aspectRatio
PyDoc_STRVAR(CameraComponent_aspectRatio_doc,
    "Aspect Ratio.\n"\
    "Type: float\n"
);

// widthBased
PyDoc_STRVAR(CameraComponent_widthBased_doc,
    "Width Based.\n"\
    "Type: bool\n"
);

// fieldOfView
PyDoc_STRVAR(CameraComponent_fieldOfView_doc,
    "Field Of View.\n"\
    "Type: float\n"
);

// near
PyDoc_STRVAR(CameraComponent_near_doc,
    "Near Plane.\n"\
    "Type: float\n"
);

// far
PyDoc_STRVAR(CameraComponent_far_doc,
    "Far Plane.\n"\
    "Type: float\n"
);

// pan
PyDoc_STRVAR(CameraComponent_pan_doc,
    "Pan (Y-axis).\n"\
    "Type: float\n"
);

// tilt
PyDoc_STRVAR(CameraComponent_tilt_doc,
    "Tilt (X-axis).\n"\
    "Type: float\n"
);

// roll
PyDoc_STRVAR(CameraComponent_roll_doc,
    "Roll (Z-axis).\n"\
    "Type: float\n"
);

// lockOnTarget
PyDoc_STRVAR(CameraComponent_lockOnTarget_doc,
    "Lock On Target.\n"\
    "Type: bool\n"
);

// target
PyDoc_STRVAR(CameraComponent_target_doc,
    "Locked Target.\n"\
    "Type: Vec3\n"
);

// worldTarget
PyDoc_STRVAR(CameraComponent_worldTarget_doc,
    "Locked Target in world coordination.\n"\
    "Type: Vec3\n"
);

// orthoProjection
PyDoc_STRVAR(CameraComponent_orthoProjection_doc,
    "Orthographics Projection.\n"\
    "Type: bool\n"
);

// orthoWidth
PyDoc_STRVAR(CameraComponent_orthoWidth_doc,
    "Ortho Width.\n"\
    "Type: float\n"
);

// orthoHeight
PyDoc_STRVAR(CameraComponent_orthoHeight_doc,
    "Ortho Height.\n"\
    "Type: float\n"
);

// screenScale
PyDoc_STRVAR(CameraComponent_screenScale_doc,
    "Screen Scale.\n"\
    "Type: Vec2\n"
);

// screenOffset
PyDoc_STRVAR(CameraComponent_screenOffset_doc,
    "Screen Offset.\n"\
    "Type: Vec2\n"
);

// screenRadian
PyDoc_STRVAR(CameraComponent_screenRadian_doc,
    "Screen Radian.\n"\
    "Type: float\n"
);

// upAxis
PyDoc_STRVAR(CameraComponent_upAxis_doc,
    "Up Axis, default 1.\n"\
    "Type: int\n"
);

// projectionMatrix
PyDoc_STRVAR(CameraComponent_projectionMatrix_doc,
    "ProjectionMatrix.\n"\
    "Type: Mat4\n"
);

// viewInverseMatrix
PyDoc_STRVAR(CameraComponent_viewInverseMatrix_doc,
    "View Inverse Matrix.\n"\
    "Type: Mat4\n"
);

// screenMatrix
PyDoc_STRVAR(CameraComponent_screenMatrix_doc,
    "Screen Matrix.\n"\
    "Type: Mat4\n"
);
