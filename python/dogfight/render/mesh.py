"""Plane mesh display list, ported from ``Gen3DObjectList`` in Displaylists.h."""

from __future__ import annotations

from pathlib import Path

import numpy as np
from OpenGL.GL import (
    GL_COMPILE,
    GL_FRONT_AND_BACK,
    GL_TRIANGLES,
    glBegin,
    glEnd,
    glEndList,
    glGenLists,
    glNewList,
    glNormal3f,
    glVertex3f,
)

_MESH_PATH = Path(__file__).resolve().parent.parent.parent / "assets" / "plane_mesh.npz"


def build_plane_list() -> int:
    """Compile the plane triangle mesh into an OpenGL display list."""
    data = np.load(_MESH_PATH)
    positions = data["tri_positions"]
    normals = data["tri_normals"]

    list_id = int(glGenLists(1))
    glNewList(list_id, GL_COMPILE)
    glBegin(GL_TRIANGLES)
    for i in range(positions.shape[0]):
        n = normals[i]
        v = positions[i]
        glNormal3f(float(n[0]), float(n[1]), float(n[2]))
        glVertex3f(float(v[0]), float(v[1]), float(v[2]))
    glEnd()
    glEndList()
    return list_id
