import math

from dogfight.mathlib.vector3 import MyVector, add, dot_product


def test_constructor_splits_direction_and_magnitude():
    v = MyVector(3.0, 0.0, 4.0)
    assert math.isclose(v.mag, 5.0)
    assert math.isclose(v.x, 0.6)
    assert math.isclose(v.z, 0.8)


def test_zero_vector_stays_zero():
    v = MyVector(0.0, 0.0, 0.0)
    assert v.mag == 0.0
    assert (v.x, v.y, v.z) == (0.0, 0.0, 0.0)


def test_unitize_sets_unit_magnitude():
    # Build a raw (non-normalized) vector, then unitize it to length 1.
    v = MyVector(0.0, 0.0, 0.0)
    v.x, v.y, v.z = 0.0, 2.0, 0.0
    v.calc_mag()
    v.unitize()
    assert math.isclose(v.mag, 1.0)
    assert math.isclose(v.y, 1.0)


def test_dot_product_of_unit_directions():
    a = MyVector(1.0, 0.0, 0.0)
    b = MyVector(1.0, 0.0, 0.0)
    assert math.isclose(dot_product(a, b), 1.0)
    c = MyVector(0.0, 1.0, 0.0)
    assert dot_product(a, c) == 0.0


def test_dot_product_zero_when_magnitude_tiny():
    a = MyVector(0.0, 0.0, 0.0)
    b = MyVector(1.0, 0.0, 0.0)
    assert dot_product(a, b) == 0.0


def test_add_scales_by_magnitude():
    a = MyVector(1.0, 0.0, 0.0)
    a.mag = 2.0
    b = MyVector(0.0, 1.0, 0.0)
    b.mag = 3.0
    result = add(a, b)
    # (2, 3, 0) -> magnitude sqrt(13)
    assert math.isclose(result.mag, math.sqrt(13.0))
