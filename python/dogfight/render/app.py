"""GLUT application loop, ported from main.cpp.

Couples the simulation and rendering exactly as the original did: one
simulation step runs per displayed frame (GLUT idle is wired to the display
callback), so generations advance at the frame rate.
"""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Dict

from OpenGL.GL import (
    GL_AMBIENT,
    GL_AMBIENT_AND_DIFFUSE,
    GL_COLOR_BUFFER_BIT,
    GL_COLOR_MATERIAL,
    GL_DEPTH_BUFFER_BIT,
    GL_DEPTH_TEST,
    GL_DIFFUSE,
    GL_EMISSION,
    GL_FILL,
    GL_FRONT_AND_BACK,
    GL_LIGHT0,
    GL_LIGHTING,
    GL_LINE,
    GL_MODELVIEW,
    GL_POSITION,
    GL_PROJECTION,
    GL_SHININESS,
    GL_SMOOTH,
    GL_SPECULAR,
    glClear,
    glClearColor,
    glClearDepth,
    glColor3f,
    glColorMaterial,
    glEnable,
    glFlush,
    glLightfv,
    glLoadIdentity,
    glMaterialf,
    glMaterialfv,
    glMatrixMode,
    glPolygonMode,
    glPopMatrix,
    glPushMatrix,
    glShadeModel,
    glViewport,
)
from OpenGL.GLU import gluPerspective
from OpenGL.GLUT import (
    GLUT_DEPTH,
    GLUT_DOUBLE,
    GLUT_RGB,
    glutCreateWindow,
    glutDisplayFunc,
    glutIdleFunc,
    glutInit,
    glutInitDisplayMode,
    glutInitWindowPosition,
    glutInitWindowSize,
    glutKeyboardFunc,
    glutKeyboardUpFunc,
    glutMainLoop,
    glutReshapeFunc,
    glutSwapBuffers,
)

from ..config import Config
from ..io.population import save_population
from ..sim.gene_pool import GenePool
from .camera import apply_camera
from .environment import Environment
from .mesh import build_plane_list
from .plane_renderer import draw_population, draw_targets
from .textures import load_textures

_APP = None  # module-level handle so GLUT C callbacks can reach the instance.


class App:
    def __init__(self, config: Config, pool: GenePool, textures_dir: Path):
        self.config = config
        self.pool = pool
        self.textures_dir = textures_dir

        self.my_count = 0
        self.generation_count = 0
        self.chaseplane = 0
        self.camera_mode = 3
        self.zoom = -20.0
        self.keys: Dict[str, bool] = {}

        self.textures = []
        self.plane_list = 0
        self.environment = None

    # ----------------------------------------------------------------- setup
    def run(self) -> None:
        global _APP
        _APP = self

        glutInit(sys.argv)
        glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE)
        glutInitWindowPosition(100, 100)
        glutInitWindowSize(self.config.render.width, self.config.render.height)
        glutCreateWindow(b"Dogfight")

        glutDisplayFunc(_display)
        glutKeyboardFunc(_keydown)
        glutKeyboardUpFunc(_keyup)
        glutReshapeFunc(_reshape)
        glutIdleFunc(_display)

        self.environment = Environment()
        self._new_generation()
        self.textures = load_textures(self.textures_dir)
        self.plane_list = build_plane_list()

        glutMainLoop()

    def _apply_gl_state(self) -> None:
        glShadeModel(GL_SMOOTH)
        glEnable(GL_DEPTH_TEST)

        black = [0.0, 0.0, 0.0, 1.0]
        white = [1.0, 1.0, 1.0, 1.0]
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black)
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white)
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0)
        glEnable(GL_COLOR_MATERIAL)
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE)

        glLightfv(GL_LIGHT0, GL_POSITION, [0.0, 1.0, 0.0, 0.0])
        glLightfv(GL_LIGHT0, GL_AMBIENT, [0.3, 0.3, 0.3, 1.0])
        glLightfv(GL_LIGHT0, GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0])
        glLightfv(GL_LIGHT0, GL_SPECULAR, [1.0, 1.0, 1.0, 1.0])
        glEnable(GL_LIGHTING)
        glEnable(GL_LIGHT0)
        glColor3f(1.0, 1.0, 1.0)
        glClearColor(0.0, 0.3, 1.0, 1.0)
        glClearDepth(1.0)
        from OpenGL.GL import GL_CULL_FACE

        glEnable(GL_CULL_FACE)

    def _new_generation(self) -> None:
        self.my_count = 0
        if self.generation_count != 0:
            self.pool.next_generation()
        self.generation_count += 1
        print(f"generation : {self.generation_count}")
        self.chaseplane = 0
        self.pool.init_planes()
        self.pool.reset_targets()
        self.keys.clear()
        self._apply_gl_state()

        auto = self.config.population.auto_save_every
        if auto > 0 and self.generation_count % auto == 0:
            self._save()

    def _save(self) -> None:
        save_population(
            self.config.population.save_path,
            self.pool.export_weights(),
            self.config.network,
            self.generation_count,
        )
        print(f"saved population to {self.config.population.save_path}")

    # ------------------------------------------------------------- callbacks
    def display(self) -> None:
        self.my_count += 1
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

        if self.my_count > self.config.simulation.generation_time:
            self.my_count = 0
            self._new_generation()

        if self.config.simulation.reset_time and self.my_count % self.config.simulation.reset_time == 0:
            self.pool.reset_targets()

        if self.keys.get("l"):
            self.keys["l"] = False
            self.camera_mode += 1
        if self.camera_mode > 3:
            self.camera_mode = 0

        chase = self.pool.get_plane(self.chaseplane)
        apply_camera(self.camera_mode, chase, self.zoom)

        glPushMatrix()
        glLightfv(GL_LIGHT0, GL_POSITION, [0.0, 1.0, 0.0, 0.0])
        glPopMatrix()

        self.environment.draw(chase, self.textures)

        self._handle_held_keys()

        self.pool.update_population()
        draw_population(self.pool.planes, self.plane_list, self.textures)
        draw_targets(self.pool.targets, self.plane_list)

        glFlush()
        glutSwapBuffers()

    def _handle_held_keys(self) -> None:
        k = self.keys
        if k.get("r"):
            self.zoom += 0.1
        if k.get("f"):
            self.zoom -= 0.1

        if k.get("y"):
            k["y"] = False
            self.chaseplane += 1
            if self.chaseplane >= len(self.pool.planes):
                self.chaseplane = 0
            print(f"You are viewing Plane[{self.chaseplane}]")
        if k.get("h"):
            k["h"] = False
            self.chaseplane -= 1
            if self.chaseplane < 0:
                self.chaseplane = len(self.pool.planes) - 1
            print(f"You are viewing Plane[{self.chaseplane}]")

        if k.get("p"):
            k["p"] = False
            self._new_generation()

        if k.get("w"):
            k["w"] = False
            self._save()

        if k.get("a"):
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)
        if k.get("s"):
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)

    def reshape(self, width: int, height: int) -> None:
        if height == 0:
            height = 1
        glViewport(0, 0, width, height)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(self.config.render.fov, width / height, 0.1, 100000.0)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

    def keydown(self, key: int) -> None:
        self.keys[chr(key)] = True

    def keyup(self, key: int) -> None:
        self.keys[chr(key)] = False


def _display() -> None:
    _APP.display()


def _reshape(width, height) -> None:
    _APP.reshape(width, height)


def _keydown(key, x, y) -> None:
    _APP.keydown(ord(key))


def _keyup(key, x, y) -> None:
    _APP.keyup(ord(key))
