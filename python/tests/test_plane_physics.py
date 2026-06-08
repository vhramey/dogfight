import math
import random

from dogfight.config import NetworkConfig
from dogfight.mathlib.vector3 import MyVector
from dogfight.sim.plane import Plane


def _make_plane(seed=0):
    cfg = NetworkConfig(num_inputs=7, num_outputs=4, num_hidden_layers=3, neurons_per_hidden_layer=20)
    return Plane(cfg, random.Random(seed))


def test_init_sets_finite_orientation():
    plane = _make_plane()
    plane.init()
    plane.update_vectors()
    for v in (plane.heading, plane.perp_y, plane.perp_z):
        assert all(math.isfinite(c) for c in (v.x, v.y, v.z))
    # Orientation matrix should be finite.
    assert all(math.isfinite(x) for x in plane.m16)


def test_update_produces_finite_state():
    plane = _make_plane()
    plane.init()
    plane.set_target(MyVector(1.0, 0.0, 0.0))
    plane.set_distance(100.0)
    for _ in range(50):
        plane.update()
    assert math.isfinite(plane.pos_x)
    assert math.isfinite(plane.pos_y)
    assert math.isfinite(plane.pos_z)
    assert math.isfinite(plane.fitness)


def test_simulation_is_deterministic():
    a = _make_plane(seed=42)
    b = _make_plane(seed=42)
    for plane in (a, b):
        plane.init()
        plane.set_target(MyVector(0.0, 1.0, 0.0))
        plane.set_distance(50.0)
    for _ in range(100):
        a.update()
        b.update()
    assert a.pos_x == b.pos_x
    assert a.pos_y == b.pos_y
    assert a.pos_z == b.pos_z
    assert a.fitness == b.fitness


def test_ground_crash_resets_position():
    plane = _make_plane()
    plane.init()
    plane.pos_y = plane.ground_level - 10.0
    plane.set_target(MyVector(1.0, 0.0, 0.0))
    plane.set_distance(10.0)
    plane.update()
    # init() runs at the top of update() when below ground, resetting position.
    assert plane.pos_y >= plane.ground_level


def test_flap_mapping_within_range():
    plane = _make_plane()
    plane.init()
    plane.set_target(MyVector(1.0, 0.0, 0.0))
    plane.set_distance(10.0)
    plane.think()
    assert -30.0 <= plane.fl_flap <= 30.0
    assert -30.0 <= plane.rp_flap <= 30.0
    assert -5.0 <= plane.ry_flap <= 5.0
    assert plane.thrust_accel == 0.25
