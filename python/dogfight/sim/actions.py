"""Map neural-network outputs to plane control surfaces."""

from __future__ import annotations

from typing import List, Tuple

from ..config import NetworkConfig, PhysicsConfig


def map_outputs(
    outputs: List[float],
    net_config: NetworkConfig,
    physics: PhysicsConfig,
) -> Tuple[float, float, float, float, bool]:
    """Return (fl_flap, rp_flap, ry_flap, thrust_accel, shooting)."""
    mapping = net_config.output_mapping
    max_flap = physics.max_flap_angle
    max_yaw = physics.max_yaw_flap_angle

    if mapping == "tanh":
        def to_angle(value: float, limit: float) -> float:
            import math
            return math.tanh(value) * (limit / 2.0)

        fl_flap = to_angle(outputs[0], max_flap)
        rp_flap = to_angle(outputs[1], max_flap)
        ry_flap = to_angle(outputs[2], max_yaw)
    else:
        # Legacy sigmoid outputs in [0, 1], centered at 0.5.
        fl_flap = (outputs[0] - 0.5) * max_flap
        rp_flap = (outputs[1] - 0.5) * max_flap
        ry_flap = (outputs[2] - 0.5) * max_yaw

    if net_config.use_throttle_output and len(outputs) > 3:
        thrust = max(0.0, min(physics.max_thrust, outputs[3] * physics.max_thrust))
    else:
        thrust = physics.default_thrust

    shooting = False
    if net_config.use_learned_shooting:
        shoot_idx = 4 if net_config.use_throttle_output else 3
        if len(outputs) > shoot_idx:
            shooting = outputs[shoot_idx] > 0.5

    return fl_flap, rp_flap, ry_flap, thrust, shooting
