"""Fitness accumulation helpers."""

from __future__ import annotations

from ..config import CombatConfig, FitnessConfig
from ..mathlib.vector3 import MyVector, dot_product


def accumulate_fitness(
    *,
    fitness_cfg: FitnessConfig,
    combat_cfg: CombatConfig,
    global_target: MyVector,
    vel: MyVector,
    heading: MyVector,
    perp_y: MyVector,
    pos_y: float,
    ground_level: float,
    lockcount: int,
    health: float,
    hits_scored: int,
    kills: int,
    damage_taken: float,
    evasion_frames: int,
) -> tuple[float, int]:
    """Return (fitness_delta, updated_lockcount)."""
    vec_up = MyVector(0.0, 1.0, 0.0)
    delta = 0.0

    scalar = dot_product(global_target, vel)
    abscalar = scalar
    if scalar < 0:
        abscalar *= -2.0
    delta += fitness_cfg.coeff_velocity * scalar * abscalar

    scalar = dot_product(global_target, heading)
    abscalar = scalar
    if scalar < 0:
        abscalar *= -2.0
    delta += fitness_cfg.coeff_heading * scalar * abscalar

    if scalar > fitness_cfg.lock_threshold:
        lockcount += 1
    else:
        lockcount = 0
    if lockcount:
        delta += fitness_cfg.coeff_heading * scalar * lockcount

    scalar = dot_product(vec_up, perp_y)
    if scalar < 0:
        delta += fitness_cfg.coeff_upright * scalar

    if pos_y < ground_level:
        delta -= fitness_cfg.crash_penalty

    if combat_cfg.enabled:
        if evasion_frames > 0:
            delta += fitness_cfg.evasion_reward
        if health > 0:
            delta += fitness_cfg.survival_reward

    return delta, lockcount
