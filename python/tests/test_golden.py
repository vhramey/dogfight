"""Regression and parity tests for the flight physics.

The C++ original cannot be run in this environment for a live trace comparison
(it is OpenGL-coupled, prompts on stdin, and ships ``Plane.cpp`` only in git),
so correctness is anchored two ways:

1. ``test_golden_trajectory`` pins exact values from a deterministic scenario,
   guarding against any silent change in the ported numerics.
2. ``test_acceleration_matches_force_model`` checks one integration step
   against the force model derived directly from the C++ equations, using an
   analytic case (zero velocity) that does not depend on implementation detail.
"""

import math
import random

from dogfight.config import NetworkConfig
from dogfight.mathlib.vector3 import MyVector
from dogfight.sim.plane import GRAVITY, Plane


def _make_plane(seed):
    return Plane(NetworkConfig(), random.Random(seed))


def test_golden_trajectory():
    plane = _make_plane(12345)
    plane.scatterbrain()
    plane.init()
    plane.set_target(MyVector(0.3, 0.7, -0.2))
    plane.set_distance(42.0)
    for _ in range(25):
        plane.update()

    assert math.isclose(plane.pos_x, 5.901382866609573, rel_tol=0, abs_tol=1e-9)
    assert math.isclose(plane.pos_y, -0.3862909056495517, rel_tol=0, abs_tol=1e-9)
    assert math.isclose(plane.pos_z, -4.5078231160702975, rel_tol=0, abs_tol=1e-9)
    assert math.isclose(plane.fitness, -18.154825610557666, rel_tol=0, abs_tol=1e-9)


def test_acceleration_matches_force_model():
    plane = _make_plane(1)
    plane.init()
    plane.update_vectors()

    # With zero velocity, drag and lift vanish, leaving thrust (along heading)
    # and gravity (downward). This mirrors the C++ calcAccel equations.
    plane.vel.init(0.0, 0.0, 0.0)
    plane.thrust_accel = 0.25
    plane.dt = 1.0
    plane.fl_flap = plane.rp_flap = plane.ry_flap = 0.0

    h = plane.heading
    expected_ax = 0.25 * h.x
    expected_ay = 0.25 * h.y - GRAVITY
    expected_az = 0.25 * h.z
    expected_mag = math.sqrt(expected_ax**2 + expected_ay**2 + expected_az**2)

    plane.calc_accel()

    assert math.isclose(plane.net_accel.mag, expected_mag, abs_tol=1e-12)
    assert math.isclose(plane.net_accel.x * plane.net_accel.mag, expected_ax, abs_tol=1e-12)
    assert math.isclose(plane.net_accel.y * plane.net_accel.mag, expected_ay, abs_tol=1e-12)
    assert math.isclose(plane.net_accel.z * plane.net_accel.mag, expected_az, abs_tol=1e-12)
