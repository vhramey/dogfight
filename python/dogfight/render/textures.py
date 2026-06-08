"""Minimal uncompressed-TGA loader, ported from the C++ ``LoadTGA``.

The bundled textures are uncompressed (type 2) 24- or 32-bit TGAs. This reads
them, swaps the red/blue byte order, and builds a mipmapped 2D texture.
"""

from __future__ import annotations

import struct
from pathlib import Path
from typing import List

from OpenGL.GL import (
    GL_LINEAR,
    GL_LINEAR_MIPMAP_LINEAR,
    GL_RGB,
    GL_RGBA,
    GL_TEXTURE_2D,
    GL_TEXTURE_MAG_FILTER,
    GL_TEXTURE_MIN_FILTER,
    GL_UNSIGNED_BYTE,
    glBindTexture,
    glGenTextures,
    glTexParameterf,
)
from OpenGL.GLU import gluBuild2DMipmaps

# Texture order matches the C++ program's texture[0..5] indices.
TEXTURE_FILES = [
    "cloud.tga",
    "land.tga",
    "landtexture.tga",
    "sky.tga",
    "prop.tga",
    "gunflare.tga",
]


def _load_tga_bytes(path: Path):
    data = path.read_bytes()
    # 12-byte header; bytes 12..17 hold width/height/bpp.
    width = data[13] * 256 + data[12]
    height = data[15] * 256 + data[14]
    bpp = data[16]
    bytes_per_pixel = bpp // 8
    image_size = width * height * bytes_per_pixel
    pixels = bytearray(data[18 : 18 + image_size])

    # Swap red and blue (TGA stores BGR(A)).
    for i in range(0, image_size, bytes_per_pixel):
        pixels[i], pixels[i + 2] = pixels[i + 2], pixels[i]

    gl_type = GL_RGB if bpp == 24 else GL_RGBA
    return width, height, gl_type, bytes(pixels)


def load_textures(textures_dir: Path) -> List[int]:
    """Load all six textures and return their OpenGL texture ids."""
    texture_ids: List[int] = []
    for name in TEXTURE_FILES:
        width, height, gl_type, pixels = _load_tga_bytes(textures_dir / name)
        tex_id = int(glGenTextures(1))
        glBindTexture(GL_TEXTURE_2D, tex_id)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
        gluBuild2DMipmaps(GL_TEXTURE_2D, gl_type, width, height, gl_type, GL_UNSIGNED_BYTE, pixels)
        texture_ids.append(tex_id)
    return texture_ids
