"""Port of the C++ ``MyVector`` class.

The original stores a direction and a magnitude separately. After
construction (and after most mutating operations) ``x, y, z`` hold a unit
direction while ``mag`` holds the length. Faithfully reproducing this split is
important because the flight physics relies on it.
"""

from __future__ import annotations

import math

_EPS = 0.000001
_DOT_EPS = 0.00001


class MyVector:
    __slots__ = ("x", "y", "z", "mag")

    def __init__(self, px: float = 0.0, py: float = 0.0, pz: float = 0.0):
        self.x = float(px)
        self.y = float(py)
        self.z = float(pz)
        self.mag = 0.0
        self.calc_mag()
        self.unitize_pres_mag()

    def init(self, px: float, py: float, pz: float) -> None:
        self.x = float(px)
        self.y = float(py)
        self.z = float(pz)
        self.calc_mag()
        self.unitize_pres_mag()

    def set_vector(self, other: "MyVector") -> None:
        self.x = other.x
        self.y = other.y
        self.z = other.z
        self.mag = other.mag

    def copy(self) -> "MyVector":
        v = MyVector.__new__(MyVector)
        v.x = self.x
        v.y = self.y
        v.z = self.z
        v.mag = self.mag
        return v

    def calc_mag(self) -> None:
        self.mag = math.sqrt(self.x * self.x + self.y * self.y + self.z * self.z)

    def unitize(self) -> None:
        """Normalize and set the magnitude to 1.0."""
        if self.mag != 0.0:
            self.x /= self.mag
            self.y /= self.mag
            self.z /= self.mag
        self.mag = 1.0

    def unitize_pres_mag(self) -> None:
        """Normalize the direction while preserving the stored magnitude."""
        if -_EPS < self.mag < _EPS:
            return
        if self.mag != 0.0:
            self.x /= self.mag
            self.y /= self.mag
            self.z /= self.mag

    def scalar_mult(self, scalar: float) -> None:
        self.mag *= scalar
        if self.mag < 0:
            self.mag *= -1
            self.x *= -1
            self.y *= -1
            self.z *= -1

    def __repr__(self) -> str:  # pragma: no cover - debugging aid
        return f"MyVector(x={self.x}, y={self.y}, z={self.z}, mag={self.mag})"


def dot_product(a: MyVector, b: MyVector) -> float:
    """Dot product of the (unit) direction components, with the C++ guards."""
    if (-_DOT_EPS < a.mag < _DOT_EPS) or (-_DOT_EPS < b.mag < _DOT_EPS):
        return 0.0
    scalar = a.x * b.x + a.y * b.y + a.z * b.z
    if -_DOT_EPS < scalar < _DOT_EPS:
        scalar = 0.0
    return scalar


def add(a: MyVector, b: MyVector) -> MyVector:
    """Add two vectors after scaling each direction by its magnitude."""
    ax = a.x * a.mag
    ay = a.y * a.mag
    az = a.z * a.mag
    bx = b.x * b.mag
    by = b.y * b.mag
    bz = b.z * b.mag
    return MyVector(ax + bx, ay + by, az + bz)
