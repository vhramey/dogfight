"""Plane and target rendering, ported from Plane::transform / Plane::shoot and
GenePool::DrawPopulation / DrawTargets."""

from __future__ import annotations

from typing import List

from OpenGL.GL import (
    GL_BLEND,
    GL_CULL_FACE,
    GL_FALSE,
    GL_LIGHTING,
    GL_LINES,
    GL_ONE,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_QUADS,
    GL_SRC_ALPHA,
    GL_TEXTURE_2D,
    GL_TRUE,
    glBegin,
    glBindTexture,
    glBlendFunc,
    glCallList,
    glColor3f,
    glColor4d,
    glDepthMask,
    glDisable,
    glEnable,
    glEnd,
    glMultMatrixf,
    glPopMatrix,
    glPushMatrix,
    glRotated,
    glScalef,
    glTexCoord2f,
    glTranslated,
    glTranslatef,
    glVertex3d,
    glVertex3f,
)

from .gun_renderer import draw_bullets


def _transform(plane) -> None:
    glTranslatef(plane.pos_x, plane.pos_y, plane.pos_z)

    glDisable(GL_LIGHTING)
    glBegin(GL_LINES)
    glColor3f(1, 1, 0)
    glVertex3f(0, 0, 0)
    g = plane.global_target
    glVertex3f(g.x, g.y, g.z)
    glEnd()
    glEnable(GL_LIGHTING)

    glMultMatrixf(plane.m16)

    glColor3f(0.4, 0.4, 0.4)
    if plane.lockcount:
        glColor3f(1, 0, 0)
    if plane.targetedcount > 0:
        glColor3f(0, 0, 1)
    if plane.targetedcount < 0:
        glColor3f(0, 1, 0)


def _draw_prop_and_flares(plane, textures: List[int]) -> None:
    radius = 0.35
    forward = 0.25
    down = 0.05
    fsize = 0.1

    if plane.fired_this_frame:
        glDisable(GL_LIGHTING)
        glDisable(GL_CULL_FACE)
        glEnable(GL_BLEND)
        glEnable(GL_TEXTURE_2D)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE)
        glDepthMask(GL_FALSE)
        glColor3f(1, 1, 1)
        glBindTexture(GL_TEXTURE_2D, textures[5])

        for sign in (-1.0, 1.0):
            glPushMatrix()
            glTranslated(-forward, sign * radius, -down)
            glBegin(GL_QUADS)
            glTexCoord2f(0, 0)
            glVertex3d(0, -fsize, -fsize)
            glTexCoord2f(1, 0)
            glVertex3d(0, fsize, -fsize)
            glTexCoord2f(1, 1)
            glVertex3d(0, fsize, fsize)
            glTexCoord2f(0, 1)
            glVertex3d(0, -fsize, fsize)
            glEnd()
            glPopMatrix()

        glDisable(GL_BLEND)
        glEnable(GL_CULL_FACE)
        glEnable(GL_LIGHTING)
        glDisable(GL_TEXTURE_2D)
        glDepthMask(GL_TRUE)

    # Spinning prop disc.
    f = 0.46
    d = 0.05
    psize = 0.2

    glDisable(GL_LIGHTING)
    glDisable(GL_CULL_FACE)
    glEnable(GL_BLEND)
    glEnable(GL_TEXTURE_2D)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    glDepthMask(GL_FALSE)
    glColor4d(1, 1, 1, plane.propvision)

    glPushMatrix()
    glTranslated(-f, 0, -d)
    glRotated(plane.proprotation, 1, 0, 0)
    glBindTexture(GL_TEXTURE_2D, textures[4])
    glBegin(GL_QUADS)
    glTexCoord2f(0, 0)
    glVertex3d(0, -psize, -psize)
    glTexCoord2f(1, 0)
    glVertex3d(0, psize, -psize)
    glTexCoord2f(1, 1)
    glVertex3d(0, psize, psize)
    glTexCoord2f(0, 1)
    glVertex3d(0, -psize, psize)
    glEnd()
    glPopMatrix()

    glDisable(GL_BLEND)
    glEnable(GL_CULL_FACE)
    glEnable(GL_LIGHTING)
    glDisable(GL_TEXTURE_2D)
    glDepthMask(GL_TRUE)


def draw_population(planes, plane_list: int, textures: List[int]) -> None:
    for plane in planes:
        plane.update_guns()
        glPushMatrix()
        _transform(plane)
        glCallList(plane_list)
        _draw_prop_and_flares(plane, textures)
        glPopMatrix()
    draw_bullets(planes)


def draw_targets(targets, plane_list: int) -> None:
    for target in targets:
        glPushMatrix()
        glTranslatef(target.pos_x, target.pos_y, target.pos_z)
        glMultMatrixf(target.m16)
        glScalef(3, 3, 3)
        glColor3f(0.8, 1, 0.8)
        glCallList(plane_list)
        glPopMatrix()
