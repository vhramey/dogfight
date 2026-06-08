"""Combat: bullet hit detection, health, and Hall of Fame."""

from __future__ import annotations

from pathlib import Path
from typing import List, Optional, TYPE_CHECKING

import numpy as np

from ..config import CombatConfig, NetworkConfig

if TYPE_CHECKING:
    from .plane import Plane


def _segment_point_distance(
    ax: float, ay: float, az: float,
    bx: float, by: float, bz: float,
    px: float, py: float, pz: float,
) -> float:
    abx, aby, abz = bx - ax, by - ay, bz - az
    apx, apy, apz = px - ax, py - ay, pz - az
    ab_len_sq = abx * abx + aby * aby + abz * abz
    if ab_len_sq < 1e-12:
        return (apx * apx + apy * apy + apz * apz) ** 0.5
    t = max(0.0, min(1.0, (apx * abx + apy * aby + apz * abz) / ab_len_sq))
    cx = ax + t * abx
    cy = ay + t * aby
    cz = az + t * abz
    dx, dy, dz = px - cx, py - cy, pz - cz
    return (dx * dx + dy * dy + dz * dz) ** 0.5


class CombatSystem:
    def __init__(self, config: CombatConfig):
        self.config = config
        self.hall_of_fame: List[np.ndarray] = []

    def reset_plane_combat(self, plane: Plane) -> None:
        plane.health = self.config.max_health
        plane.hits_scored = 0
        plane.kills = 0
        plane.damage_taken = 0.0
        plane.evasion_frames = 0
        plane.alive = True
        plane._reset_ammo()

    def resolve_hits(self, planes: List[Plane]) -> None:
        if not self.config.enabled:
            return

        for shooter in planes:
            if not shooter.alive:
                continue
            for shot in shooter.shots:
                if not shot.alive:
                    continue
                prev_x = shot.pos_x - shot.dx
                prev_y = shot.pos_y - shot.dy
                prev_z = shot.pos_z - shot.dz
                for target in planes:
                    if target is shooter or not target.alive:
                        continue
                    dist = _segment_point_distance(
                        prev_x, prev_y, prev_z,
                        shot.pos_x, shot.pos_y, shot.pos_z,
                        target.pos_x, target.pos_y, target.pos_z,
                    )
                    if dist <= self.config.hit_radius:
                        target.health -= self.config.bullet_damage
                        target.damage_taken += self.config.bullet_damage
                        shooter.hits_scored += 1
                        shooter.note_burst_hit()
                        shot.alive = False
                        if target.health <= 0:
                            target.alive = False
                            shooter.kills += 1
                        break

    def apply_hit_fitness(self, planes: List[Plane], fitness_cfg) -> None:
        """Award combat fitness for hits/kills scored this frame."""
        if not self.config.enabled:
            return
        for plane in planes:
            if plane.hits_scored:
                plane.fitness += plane.hits_scored * fitness_cfg.hit_reward
                plane.hits_scored = 0
            if plane.kills:
                plane.fitness += plane.kills * fitness_cfg.kill_reward
                plane.kills = 0

    def update_targeting(self, planes: List[Plane]) -> None:
        """Track how often a plane is being aimed at (for evasion fitness)."""
        if not self.config.enabled:
            return
        for plane in planes:
            if not plane.alive:
                continue
            aimed = False
            for other in planes:
                if other is plane or not other.alive:
                    continue
                dot = (
                    other.heading.x * (plane.pos_x - other.pos_x)
                    + other.heading.y * (plane.pos_y - other.pos_y)
                    + other.heading.z * (plane.pos_z - other.pos_z)
                )
                if dot > 0.999:
                    aimed = True
                    break
            if aimed:
                plane.evasion_frames += 1
                plane.targetedcount += 1

    def maybe_add_to_hall_of_fame(self, weights: np.ndarray) -> None:
        if not self.config.enabled:
            return
        self.hall_of_fame.append(weights.copy())
        if len(self.hall_of_fame) > self.config.hall_of_fame_size:
            self.hall_of_fame.pop(0)

    def save_hall_of_fame(self, net: NetworkConfig) -> None:
        if not self.hall_of_fame:
            return
        path = Path(self.config.hall_of_fame_path)
        np.savez(
            path,
            weights=np.array(self.hall_of_fame, dtype=np.float64),
            topology=np.array(net.topology_signature(), dtype=np.int64),
        )

    def load_hall_of_fame(self, net: NetworkConfig) -> None:
        path = Path(self.config.hall_of_fame_path)
        if not path.exists():
            return
        data = np.load(path)
        stored = data["topology"]
        expected = np.array(net.topology_signature(), dtype=np.int64)
        if not np.array_equal(stored, expected):
            return
        self.hall_of_fame = [row.copy() for row in data["weights"]]

    def pick_opponent_weights(self, rng) -> Optional[np.ndarray]:
        if not self.hall_of_fame:
            return None
        return self.hall_of_fame[rng.randrange(len(self.hall_of_fame))]
