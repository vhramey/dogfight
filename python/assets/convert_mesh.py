"""Convert the C++ ``Displaylists.h`` plane mesh into a NumPy ``.npz`` asset.

The original mesh lives in a C header as three static arrays: ``face_indicies``
(410 triangles, each storing vertex/normal/texture indices), ``vertices``, and
``normals``. This script extracts them and bakes the triangles into flat
position/normal arrays the renderer can hand straight to OpenGL.

Run once from the project root (or anywhere):

    python python/assets/convert_mesh.py [path/to/Displaylists.h]

Writes ``plane_mesh.npz`` next to this script.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

import numpy as np

_NUMBER = re.compile(r"[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?")


def _extract_block(text: str, name: str) -> str:
    """Return the text inside the ``{ ... }`` of a named array declaration."""
    start = text.index(name)
    brace = text.index("{", start)
    depth = 0
    for i in range(brace, len(text)):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return text[brace : i + 1]
    raise ValueError(f"unbalanced braces for array {name}")


def _strip_comments(block: str) -> str:
    block = re.sub(r"//[^\n]*", "", block)
    block = re.sub(r"/\*.*?\*/", "", block, flags=re.DOTALL)
    return block


def _numbers(block: str):
    return [float(m.group(0)) for m in _NUMBER.finditer(_strip_comments(block))]


def convert(header_path: Path, out_path: Path) -> None:
    text = header_path.read_text()

    face_nums = _numbers(_extract_block(text, "face_indicies"))
    vert_nums = _numbers(_extract_block(text, "vertices"))
    norm_nums = _numbers(_extract_block(text, "normals"))

    faces = np.array(face_nums, dtype=np.int64).reshape(-1, 9)
    vertices = np.array(vert_nums, dtype=np.float64).reshape(-1, 3)
    normals = np.array(norm_nums, dtype=np.float64).reshape(-1, 3)

    num_faces = faces.shape[0]
    tri_positions = np.empty((num_faces * 3, 3), dtype=np.float32)
    tri_normals = np.empty((num_faces * 3, 3), dtype=np.float32)

    for f in range(num_faces):
        for j in range(3):
            vi = faces[f, j]
            ni = faces[f, j + 3]
            tri_positions[f * 3 + j] = vertices[vi]
            tri_normals[f * 3 + j] = normals[ni]

    np.savez(
        out_path,
        tri_positions=tri_positions,
        tri_normals=tri_normals,
    )
    print(
        f"wrote {out_path} ({num_faces} triangles, "
        f"{vertices.shape[0]} vertices, {normals.shape[0]} normals)"
    )


def main() -> None:
    here = Path(__file__).resolve().parent
    default_header = here.parent.parent / "Displaylists.h"
    header_path = Path(sys.argv[1]) if len(sys.argv) > 1 else default_header
    out_path = here / "plane_mesh.npz"
    convert(header_path, out_path)


if __name__ == "__main__":
    main()
