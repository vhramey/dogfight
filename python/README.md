# Dogfight (Python port)

Genetic evolution of neural networks that fly airplanes, ported from the
original C++/OpenGL project to Python. A population of planes, each controlled
by its own feed-forward neural network, chases moving targets in a 3D world.
The networks are evolved generation after generation with a genetic algorithm
(tournament selection, crossover, mutation, and elitism). Evolution happens
live in the window, exactly as in the original.

Everything here is self-contained under this `python/` directory.

## What was ported

| Original C++ | Python module |
|---|---|
| `MyVector`, `Quaternion` | `dogfight/mathlib/` |
| `Neuron`, `NeuronLayer`, `NeuralNet` | `dogfight/brain/` |
| `Plane`, `Gun`, target/waypoint logic, `GenePool` | `dogfight/sim/` |
| `par.ini` + interactive prompts | `config/default.yaml` + `dogfight/config.py` |
| `population.dat` (flat weights) | `population.npz` via `dogfight/io/` |
| `main.cpp`, `Displaylists.h` (GLUT/OpenGL) | `dogfight/render/` |

The physics, neural network, and genetic algorithm reproduce the original
numerics, including its quirks: a fixed timestep of one per frame, thrust
hard-coded at 0.25 (only the three flap angles are evolved), automatic firing
when the heading locks onto a target, and per-frame fitness accumulation.

The orientation update originally relied on live OpenGL matrix calls; here it is
reproduced with pure numeric helpers in `dogfight/mathlib/quaternion.py`, so the
simulation runs without any graphics context.

## Install

Requires Python 3.9+ and a system OpenGL/GLUT (present by default on macOS).

On macOS, the `pip` command is often missing from your PATH even though Python
includes it. Use `python3 -m pip` instead of bare `pip`.

**Quick install** (creates `.venv`, bootstraps pip if needed):

```bash
cd python
./install.sh
source .venv/bin/activate
python -m dogfight
```

**Manual install:**

```bash
cd python
python3 -m venv .venv
source .venv/bin/activate
python -m pip install -e .
```

If `python3 -m pip` fails with "No module named pip", bootstrap it first:

```bash
python3 -m ensurepip --upgrade
python3 -m pip install -e .
```

The mesh asset (`assets/plane_mesh.npz`) and textures (`assets/textures/`) are
already bundled. To regenerate the mesh from the C++ header:

```bash
python assets/convert_mesh.py ../Displaylists.h
```

## Run

Open the 3D window and evolve in real time:

```bash
dogfight
# or
python -m dogfight
```

Useful options (these override `config/default.yaml`):

```bash
python -m dogfight --planes 100 --targets 5 --generation-time 2000 --seed 1
python -m dogfight --weights population.npz        # resume from a saved run
python -m dogfight --config myconfig.yaml
```

Run without a window (fast, for development or batch evolution):

```bash
python -m dogfight --headless-smoke 50 --planes 100 --save population.npz
```

## Controls

| Key | Action |
|---|---|
| `l` | Cycle camera mode (0 free, 1 above, 2 floppy, 3 fixed) |
| `y` / `h` | Next / previous chase plane |
| `r` / `f` | Zoom in / out |
| `p` | Force a new generation |
| `w` | Save population to the configured path |
| `a` / `s` | Fill / wireframe rendering |

The population also auto-saves every `auto_save_every` generations.

## Configuration

`config/default.yaml` holds all simulation, genetic-algorithm, network,
population, and render settings. The default network topology (7 inputs, 4
outputs, 3 hidden layers of 20 neurons) matches the original `par.ini`, giving
1084 weights per plane.

## Tests

```bash
source .venv/bin/activate   # if not already active
python -m pip install -e ".[dev]"
python -m pytest
```

The suite covers the vector/quaternion math, the neural network, plane physics
(determinism, force model, crash reset), the genetic algorithm, and population
save/load. `tests/test_golden.py` pins a deterministic trajectory as a
regression guard. A live trace comparison against the C++ binary is not
included because that program is OpenGL-coupled, prompts on stdin, and keeps
`Plane.cpp` only in git history, which makes it impractical to run headlessly
here; correctness is instead anchored by faithful formula transcription plus the
analytic and determinism tests.
