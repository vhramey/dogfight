"""Camera modes ported from main.cpp (chasecamabove/floppy/fixed + free)."""

from __future__ import annotations

from OpenGL.GL import glTranslatef
from OpenGL.GLU import gluLookAt


def apply_camera(mode: int, plane, zoom: float) -> None:
    """Set up the modelview camera for the given mode (0-3)."""
    if mode == 0:
        glTranslatef(0.0, 0.0, zoom)
    elif mode == 1:
        _chase_above(plane)
    elif mode == 2:
        _chase_floppy(plane)
    else:
        _chase_fixed(plane, zoom)


def _chase_above(plane) -> None:
    h = plane.heading
    p = plane.perp_y
    x, y, z = plane.pos_x, plane.pos_y, plane.pos_z
    gluLookAt(
        x - h.x * 2 + p.x * 0.5,
        y - h.y * 2 + p.y * 0.5,
        z - h.z * 2 + p.z * 0.5,
        x + p.x * 0.5,
        y + p.y * 0.5,
        z + p.z * 0.5,
        p.x,
        p.y,
        p.z,
    )


def _chase_floppy(plane) -> None:
    g = plane.global_target
    p = plane.perp_y
    x, y, z = plane.pos_x, plane.pos_y, plane.pos_z
    gluLookAt(
        x - g.x * 2 + p.x * 0.5,
        y - g.y * 2 + p.y * 0.5,
        z - g.z * 2 + p.z * 0.5,
        x + p.x * 0.5,
        y + p.y * 0.5,
        z + p.z * 0.5,
        p.x,
        p.y,
        p.z,
    )


def _chase_fixed(plane, zoom: float) -> None:
    x, y, z = plane.pos_x, plane.pos_y, plane.pos_z
    gluLookAt(x, y, z - zoom, x, y, z, 0.0, 1.0, 0.0)
