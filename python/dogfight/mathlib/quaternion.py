"""Port of the C++ ``QUATERNION`` class plus OpenGL-style matrix helpers.

The original ``Plane::updateQM`` advanced the orientation matrix using live
OpenGL calls (``glLoadMatrixf`` / ``glRotatef`` / ``glGetFloatv``). To decouple
the simulation from the renderer, those calls are reproduced here as pure
numeric operations on a 16-element, column-major matrix that matches OpenGL's
storage layout exactly.
"""

from __future__ import annotations

import math
from typing import List, Tuple

EPSILON = 1e-12
PI = 3.1415926535897932384626433832795
PIOVER180 = 0.0174532925199432957692369076848861
PIUNDER180 = 57.2957795130823208767981548141052


class Quaternion:
    """Unit-ish quaternion stored as ``(w, x, y, z)``."""

    __slots__ = ("w", "x", "y", "z")

    def __init__(self, *args: float):
        if len(args) == 0:
            self.reset()
        elif len(args) == 3:
            self.set_euler(args[0], args[1], args[2])
        elif len(args) == 4:
            self.set_axis_angle(args[0], args[1], args[2], args[3])
        else:  # pragma: no cover - defensive
            raise TypeError("Quaternion expects 0, 3, or 4 arguments")

    def reset(self) -> "Quaternion":
        self.w = 1.0
        self.x = 0.0
        self.y = 0.0
        self.z = 0.0
        return self

    def set_from(self, other: "Quaternion") -> "Quaternion":
        self.w = other.w
        self.x = other.x
        self.y = other.y
        self.z = other.z
        return self

    def copy(self) -> "Quaternion":
        q = Quaternion.__new__(Quaternion)
        q.w, q.x, q.y, q.z = self.w, self.x, self.y, self.z
        return q

    def set_euler(self, xt: float, yt: float, zt: float) -> "Quaternion":
        xq = Quaternion(xt, 1.0, 0.0, 0.0)
        yq = Quaternion(yt, 0.0, 1.0, 0.0)
        zq = Quaternion(zt, 0.0, 0.0, 1.0)
        self.set_from(xq)
        self.post_mult(yq)
        self.post_mult(zq)
        return self

    def set_axis_angle(self, angle: float, xt: float, yt: float, zt: float) -> "Quaternion":
        factor = xt * xt + yt * yt + zt * zt
        if not factor:
            factor = EPSILON
        scale_by = 1.0 / math.sqrt(factor)
        self.w = math.cos(angle / 2.0)
        sin_half = math.sin(angle / 2.0)
        self.x = xt * scale_by * sin_half
        self.y = yt * scale_by * sin_half
        self.z = zt * scale_by * sin_half
        return self

    def post_mult(self, quat: "Quaternion") -> "Quaternion":
        temp = self.copy()
        self._mult_and_set(temp, quat)
        return self

    def _mult_and_set(self, q1: "Quaternion", q2: "Quaternion") -> None:
        self.w = (q2.w * q1.w - q2.x * q1.x - q2.y * q1.y - q2.z * q1.z)
        self.x = (q2.w * q1.x + q2.x * q1.w + q2.y * q1.z - q2.z * q1.y)
        self.y = (q2.w * q1.y - q2.x * q1.z + q2.y * q1.w + q2.z * q1.x)
        self.z = (q2.w * q1.z + q2.x * q1.y - q2.y * q1.x + q2.z * q1.w)

    def normalize(self) -> "Quaternion":
        factor = self.w * self.w + self.x * self.x + self.y * self.y + self.z * self.z
        scale_by = 1.0 / math.sqrt(factor)
        self.w *= scale_by
        self.x *= scale_by
        self.y *= scale_by
        self.z *= scale_by
        return self

    def get_axis_angle(self) -> Tuple[float, float, float, float]:
        len_of_vector = self.x * self.x + self.y * self.y + self.z * self.z
        if len_of_vector < EPSILON:
            return 1.0, 0.0, 0.0, 0.0
        inv_len = 1.0 / math.sqrt(len_of_vector)
        axis_x = self.x * inv_len
        axis_y = self.y * inv_len
        axis_z = self.z * inv_len
        rot_angle = 2.0 * math.acos(max(-1.0, min(1.0, self.w)))
        return axis_x, axis_y, axis_z, rot_angle

    def get_direction_vector(self) -> Tuple[float, float, float]:
        self.normalize()
        xx = 2.0 * (self.x * self.z - self.w * self.y)
        yy = 2.0 * (self.y * self.z + self.w * self.x)
        zz = 1.0 - 2.0 * (self.x * self.x + self.y * self.y)
        return xx, yy, zz

    def get_matrix(self) -> List[float]:
        """Return a 16-element column-major matrix (OpenGL layout)."""
        self.normalize()
        xx = self.x * self.x
        yy = self.y * self.y
        zz = self.z * self.z
        x, y, z, w = self.x, self.y, self.z, self.w
        m = [0.0] * 16
        m[0] = 1.0 - 2.0 * (yy + zz)
        m[1] = 2.0 * (x * y + w * z)
        m[2] = 2.0 * (x * z - w * y)
        m[3] = 0.0
        m[4] = 2.0 * (x * y - w * z)
        m[5] = 1.0 - 2.0 * (xx + zz)
        m[6] = 2.0 * (y * z + w * x)
        m[7] = 0.0
        m[8] = 2.0 * (x * z + w * y)
        m[9] = 2.0 * (y * z - w * x)
        m[10] = 1.0 - 2.0 * (xx + yy)
        m[11] = 0.0
        m[12] = 0.0
        m[13] = 0.0
        m[14] = 0.0
        m[15] = 1.0
        return m


def identity_matrix() -> List[float]:
    m = [0.0] * 16
    m[0] = m[5] = m[10] = m[15] = 1.0
    return m


def rotation_matrix(angle_degrees: float, ax: float, ay: float, az: float) -> List[float]:
    """Reproduce the matrix built by ``glRotatef`` (column-major, 16 floats)."""
    length = math.sqrt(ax * ax + ay * ay + az * az)
    if length == 0.0:
        return identity_matrix()
    x = ax / length
    y = ay / length
    z = az / length

    angle = angle_degrees * PIOVER180
    c = math.cos(angle)
    s = math.sin(angle)
    one_c = 1.0 - c

    m = [0.0] * 16
    # Column 0
    m[0] = x * x * one_c + c
    m[1] = y * x * one_c + z * s
    m[2] = x * z * one_c - y * s
    m[3] = 0.0
    # Column 1
    m[4] = x * y * one_c - z * s
    m[5] = y * y * one_c + c
    m[6] = y * z * one_c + x * s
    m[7] = 0.0
    # Column 2
    m[8] = x * z * one_c + y * s
    m[9] = y * z * one_c - x * s
    m[10] = z * z * one_c + c
    m[11] = 0.0
    m[12] = 0.0
    m[13] = 0.0
    m[14] = 0.0
    m[15] = 1.0
    return m


def mat_mult(a: List[float], b: List[float]) -> List[float]:
    """Column-major 4x4 multiply matching ``glMultMatrixf`` (result = a * b)."""
    result = [0.0] * 16
    for col in range(4):
        for row in range(4):
            total = 0.0
            for k in range(4):
                total += a[row + k * 4] * b[k + col * 4]
            result[row + col * 4] = total
    return result
