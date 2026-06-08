"""Build neural-network observation vectors from plane state."""

from __future__ import annotations

import math
from typing import List

from ..config import NetworkConfig, SensorConfig
from ..mathlib.vector3 import MyVector


def build_observations(
    *,
    target_local: MyVector,
    local_up: MyVector,
    distance: float,
    sensors: SensorConfig,
    vel: MyVector,
    pos_y: float,
    ground_level: float,
    targeted_local: MyVector,
    roll_rate: float,
    pitch_rate: float,
    yaw_rate: float,
    nearest_opponent_dir: MyVector | None = None,
    nearest_opponent_distance: float = 10000.0,
    ammo_remaining: int = 0,
    magazine_size: int = 1,
    burst_cooldown_remaining: int = 0,
    burst_cooldown_frames: int = 1,
) -> List[float]:
    """Assemble the observation vector (base 7 + optional extras)."""
    dist = max(distance, 0.005)
    d = 1.0 / dist
    if d > 1:
        d = 1.0

    inputs: List[float] = [
        target_local.x,
        target_local.y,
        target_local.z,
        local_up.x,
        local_up.y,
        local_up.z,
        d,
    ]

    if sensors.own_speed:
        inputs.append(min(vel.mag, 10.0) / 10.0)

    if sensors.body_velocity:
        inputs.extend([vel.x, vel.y, vel.z])

    if sensors.angular_rates:
        inputs.extend([roll_rate, pitch_rate, yaw_rate])

    if sensors.altitude:
        span = max(abs(ground_level), 1.0) + 100.0
        inputs.append((pos_y - ground_level) / span)

    if sensors.distance_log:
        inputs.append(math.log1p(dist) / math.log1p(200.0))

    if sensors.targeted_vector:
        inputs.extend([targeted_local.x, targeted_local.y, targeted_local.z])

    if sensors.opponent_relative:
        inputs.extend(
            [
                nearest_opponent_dir.x,
                nearest_opponent_dir.y,
                nearest_opponent_dir.z,
                min(1.0, 1.0 / max(nearest_opponent_distance, 0.005)),
            ]
        )
        inputs.append(nearest_opponent_distance / 200.0)

    if sensors.ammo_status:
        inputs.append(ammo_remaining / max(magazine_size, 1))
        if burst_cooldown_remaining <= 0:
            inputs.append(1.0)
        else:
            inputs.append(1.0 - burst_cooldown_remaining / max(burst_cooldown_frames, 1))

    return inputs


def validate_observation_count(net_config: NetworkConfig, observations: List[float]) -> None:
    expected = net_config.effective_num_inputs()
    if len(observations) != expected:
        raise ValueError(f"expected {expected} observations, got {len(observations)}")
