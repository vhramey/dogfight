"""A moving target the planes chase.

In the C++ project targets were full ``Plane`` objects, but only their
position, orientation, and ``localize`` were ever used. This lightweight class
provides exactly that (so targets do not each carry a neural network).
"""

from __future__ import annotations

from ..mathlib.quaternion import (
    PI,
    PIUNDER180,
    Quaternion,
    identity_matrix,
    mat_mult,
    rotation_matrix,
)
from ..mathlib.vector3 import MyVector


class Target:
    def __init__(self):
        self.pos_x = 0.0
        self.pos_y = 0.0
        self.pos_z = 0.0
        self.pitch = 0.0
        self.roll = 0.0
        self.yaw = 0.0
        self.rotation = Quaternion()
        self.m16 = identity_matrix()

    def init(self) -> None:
        self.rotation.reset()
        self.m16 = identity_matrix()
        self.pitch = 0.0
        self.roll = 0.0
        self.yaw = 0.0
        self.pos_x = 0.0
        self.pos_y = 0.0
        self.pos_z = 0.0
        self.yaw = -PI
        self.update_qm()
        self.roll = PI / 2.0
        self.update_qm()

    def update_qm(self) -> None:
        rroll = self.roll
        ppitch = self.pitch
        yyaw = self.yaw
        if -0.0005 < rroll < 0.0005:
            rroll = 0.0
        if -0.0005 < ppitch < 0.0005:
            ppitch = 0.0
        if -0.0005 < yyaw < 0.0005:
            yyaw = 0.0

        increment = Quaternion(-rroll, -ppitch, -yyaw)
        self.rotation.post_mult(increment)

        ax, ay, az, arot = increment.get_axis_angle()
        rot = -arot * PIUNDER180
        r = rotation_matrix(rot, ax, ay, az)
        self.m16 = mat_mult(self.m16, r)

        self.pitch = 0.0
        self.roll = 0.0
        self.yaw = 0.0

    def localize(self, t: MyVector) -> MyVector:
        m = self.m16
        out = MyVector(0.0, 0.0, 0.0)
        out.x = m[0] * t.x + m[1] * t.y + m[2] * t.z + m[3]
        out.y = m[4] * t.x + m[5] * t.y + m[6] * t.z + m[7]
        out.z = m[8] * t.x + m[9] * t.y + m[10] * t.z + m[11]
        out.calc_mag()
        out.unitize()
        return out
