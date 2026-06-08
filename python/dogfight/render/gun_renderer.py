"""Bullet tracer rendering, ported from ``Gun::draw`` in Gun.cpp.

Bullets are stored in world space, so they are drawn outside the plane's model
matrix (the original drew them inside it, which mislocated the tracers; drawing
in world space is the corrected, visually accurate behavior)."""

from __future__ import annotations

from OpenGL.GL import (
    GL_BLEND,
    GL_LIGHTING,
    GL_LINE_SMOOTH,
    GL_LINES,
    GL_ONE,
    GL_SRC_ALPHA,
    GL_TRUE,
    GL_FALSE,
    glBegin,
    glBlendFunc,
    glColor4f,
    glDepthMask,
    glDisable,
    glEnable,
    glEnd,
    glVertex3d,
)


def draw_bullets(planes) -> None:
    glDisable(GL_LIGHTING)
    glEnable(GL_LINE_SMOOTH)
    glDepthMask(GL_FALSE)
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE)
    glBegin(GL_LINES)
    for plane in planes:
        for shot in plane.shots:
            if not shot.alive:
                continue
            ddx = shot.dx * 2.5
            ddy = shot.dy * 2.5
            ddz = shot.dz * 2.5
            glColor4f(1, 1, 0, 0)
            glVertex3d(shot.pos_x, shot.pos_y, shot.pos_z)
            glColor4f(1, 1, 0.5, 1)
            glVertex3d(shot.pos_x + ddx, shot.pos_y + ddy, shot.pos_z + ddz)
    glEnd()
    glEnable(GL_LIGHTING)
    glDisable(GL_LINE_SMOOTH)
    glDepthMask(GL_TRUE)
    glDisable(GL_BLEND)
