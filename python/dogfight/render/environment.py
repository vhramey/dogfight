"""Sky, ground, and cloud rendering, ported from ``drawplain`` in main.cpp."""

from __future__ import annotations

from typing import List

from OpenGL.GL import (
    GL_BLEND,
    GL_CULL_FACE,
    GL_FALSE,
    GL_LIGHTING,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_QUADS,
    GL_SRC_ALPHA,
    GL_TEXTURE_2D,
    GL_TRUE,
    glBegin,
    glBindTexture,
    glBlendFunc,
    glColor3f,
    glColor4f,
    glDepthMask,
    glDisable,
    glEnable,
    glEnd,
    glNormal3f,
    glPopMatrix,
    glPushMatrix,
    glRotatef,
    glTexCoord2f,
    glTranslatef,
    glVertex3f,
)
from OpenGL.GLU import (
    GLU_INSIDE,
    GLU_TRUE,
    gluNewQuadric,
    gluQuadricOrientation,
    gluQuadricTexture,
    gluSphere,
)


class Environment:
    def __init__(self):
        self.sphere = gluNewQuadric()

    def draw(self, chase_plane, textures: List[int]) -> None:
        size = 50.0
        ssize = 2000.0

        glPushMatrix()
        x = chase_plane.pos_x
        z = chase_plane.pos_z
        glTranslatef(x, 0.0, z)

        speedfactor = 4.0
        scale = 8.0
        s = (x) / ssize * speedfactor
        t = (z) / ssize * speedfactor

        glDisable(GL_LIGHTING)
        glEnable(GL_TEXTURE_2D)
        glDisable(GL_BLEND)
        glEnable(GL_CULL_FACE)
        glDepthMask(GL_TRUE)

        # Sky sphere (inside-out).
        glBindTexture(GL_TEXTURE_2D, textures[3])
        gluQuadricOrientation(self.sphere, GLU_INSIDE)
        gluQuadricTexture(self.sphere, GLU_TRUE)
        glPushMatrix()
        glRotatef(-90, 1, 0, 0)
        glColor3f(1, 1, 1)
        gluSphere(self.sphere, ssize, 10, 10)
        glPopMatrix()

        # Ground.
        glBindTexture(GL_TEXTURE_2D, textures[1])
        glBegin(GL_QUADS)
        glColor3f(1.0, 1.0, 1.0)
        glNormal3f(0, 1, 0)
        glTexCoord2f(0 + s, 0 + t)
        glVertex3f(-ssize, -size, -ssize)
        glTexCoord2f(0 + s, scale + t)
        glVertex3f(-ssize, -size, ssize)
        glTexCoord2f(scale + s, scale + t)
        glVertex3f(ssize, -size, ssize)
        glTexCoord2f(scale + s, 0 + t)
        glVertex3f(ssize, -size, -ssize)
        glEnd()

        glDepthMask(GL_FALSE)
        glEnable(GL_BLEND)
        glDisable(GL_CULL_FACE)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

        # Lower cloud layer.
        glBindTexture(GL_TEXTURE_2D, textures[2])
        glBegin(GL_QUADS)
        glColor4f(1.0, 1.0, 1.0, 0.5)
        glNormal3f(0, -1, 0)
        glTexCoord2f(0 + s, 0 + t)
        glVertex3f(-ssize, size + 2, -ssize)
        glTexCoord2f(1 + s, 0 + t)
        glVertex3f(ssize, size + 2, -ssize)
        glTexCoord2f(1 + s, 1 + t)
        glVertex3f(ssize, size + 2, ssize)
        glTexCoord2f(0 + s, 1 + t)
        glVertex3f(-ssize, size + 2, ssize)
        glEnd()

        # Ceiling.
        glBindTexture(GL_TEXTURE_2D, textures[0])
        glBegin(GL_QUADS)
        glColor3f(1.0, 1.0, 1.0)
        glNormal3f(0, -1, 0)
        glTexCoord2f(0 + s, 0 + t)
        glVertex3f(-ssize, size, -ssize)
        glTexCoord2f(1 + s, 0 + t)
        glVertex3f(ssize, size, -ssize)
        glTexCoord2f(1 + s, 1 + t)
        glVertex3f(ssize, size, ssize)
        glTexCoord2f(0 + s, 1 + t)
        glVertex3f(-ssize, size, ssize)
        glEnd()

        glEnable(GL_LIGHTING)
        glDisable(GL_TEXTURE_2D)
        glDisable(GL_BLEND)
        glEnable(GL_CULL_FACE)
        glDepthMask(GL_TRUE)
        glPopMatrix()
