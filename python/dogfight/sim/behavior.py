"""Behavior descriptors for diversity-preserving selection."""

from __future__ import annotations

import math
from dataclasses import dataclass, field
from typing import List


@dataclass
class BehaviorDescriptor:
    mean_altitude: float = 0.0
    mean_speed: float = 0.0
    path_curvature: float = 0.0
    time_on_target: float = 0.0
    aggression: float = 0.0
    frames: int = 0
    _prev_heading: tuple = field(default=(0.0, 0.0, 0.0), repr=False)

    def record_frame(
        self,
        *,
        pos_y: float,
        speed: float,
        heading: tuple,
        on_target: bool,
        shots_fired: int,
        damage_dealt: float,
    ) -> None:
        self.frames += 1
        self.mean_altitude += pos_y
        self.mean_speed += speed
        if on_target:
            self.time_on_target += 1.0
        self.aggression += shots_fired + damage_dealt

        hx, hy, hz = heading
        phx, phy, phz = self._prev_heading
        dot = max(-1.0, min(1.0, hx * phx + hy * phy + hz * phz))
        self.path_curvature += math.acos(dot)
        self._prev_heading = heading

    def finalize(self) -> List[float]:
        if self.frames <= 0:
            return [0.0, 0.0, 0.0, 0.0, 0.0]
        f = float(self.frames)
        return [
            self.mean_altitude / f,
            self.mean_speed / f,
            self.path_curvature / f,
            self.time_on_target / f,
            self.aggression / f,
        ]

    def reset(self) -> None:
        self.mean_altitude = 0.0
        self.mean_speed = 0.0
        self.path_curvature = 0.0
        self.time_on_target = 0.0
        self.aggression = 0.0
        self.frames = 0
        self._prev_heading = (0.0, 0.0, 0.0)


def descriptor_distance(a: List[float], b: List[float]) -> float:
    return math.sqrt(sum((x - y) ** 2 for x, y in zip(a, b)))
