import math

from dogfight.mathlib.quaternion import (
    Quaternion,
    identity_matrix,
    mat_mult,
    rotation_matrix,
)


def test_reset_is_identity_rotation():
    q = Quaternion()
    assert (q.w, q.x, q.y, q.z) == (1.0, 0.0, 0.0, 0.0)
    direction = q.get_direction_vector()
    assert math.isclose(direction[2], 1.0, abs_tol=1e-9)


def test_axis_angle_roundtrip():
    q = Quaternion(math.pi / 2.0, 0.0, 1.0, 0.0)
    ax, ay, az, angle = q.get_axis_angle()
    assert math.isclose(ay, 1.0, abs_tol=1e-9)
    assert math.isclose(angle, math.pi / 2.0, abs_tol=1e-6)


def test_get_matrix_is_orthonormal():
    q = Quaternion(0.3, 0.7, -0.4)
    m = q.get_matrix()
    # Column 0 should be unit length.
    col0 = (m[0], m[1], m[2])
    length = math.sqrt(sum(c * c for c in col0))
    assert math.isclose(length, 1.0, abs_tol=1e-6)


def test_identity_matrix_is_multiplicative_identity():
    r = rotation_matrix(37.0, 0.0, 1.0, 0.0)
    ident = identity_matrix()
    product = mat_mult(ident, r)
    for a, b in zip(product, r):
        assert math.isclose(a, b, abs_tol=1e-9)


def test_rotation_matrix_matches_known_y_rotation():
    # 90 degrees about +Y maps +Z to +X in column-major OpenGL form.
    r = rotation_matrix(90.0, 0.0, 1.0, 0.0)
    # Column 2 (index 8,9,10) is where +Z lands.
    assert math.isclose(r[8], 1.0, abs_tol=1e-6)
    assert math.isclose(r[10], 0.0, abs_tol=1e-6)
