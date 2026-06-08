"""Port of the C++ ``Gun`` class.

Bullets are purely visual in the original (no hit detection). The drawing is
handled separately by the renderer; this class only tracks position and life.
"""

from __future__ import annotations

from ..mathlib.vector3 import MyVector


class Gun:
    __slots__ = ("life", "alive", "pos_x", "pos_y", "pos_z", "dx", "dy", "dz", "speed")

    def __init__(self):
        self.life = 0.0
        self.alive = False
        self.pos_x = 0.0
        self.pos_y = 0.0
        self.pos_z = 0.0
        self.speed = 2.0
        self.dx = 0.0
        self.dy = 0.0
        self.dz = 0.0

    def fire(self, x: float, y: float, z: float, aim: MyVector) -> None:
        self.pos_x = x
        self.pos_y = y
        self.pos_z = z
        self.life = 1.0
        self.alive = True
        self.dx = aim.x * self.speed
        self.dy = aim.y * self.speed
        self.dz = aim.z * self.speed

    def update(self) -> None:
        if not self.alive:
            return
        self.life -= 0.02
        if self.life < 0:
            self.alive = False
        self.pos_x += self.dx
        self.pos_y += self.dy
        self.pos_z += self.dz
