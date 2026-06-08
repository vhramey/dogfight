"""Save and load evolved populations in a Python-native ``.npz`` format."""

from __future__ import annotations

from pathlib import Path
from typing import Optional

import numpy as np

from ..config import NetworkConfig


def _topology_signature(net: NetworkConfig) -> np.ndarray:
    return np.array(net.topology_signature(), dtype=np.int64)


def save_population(
    path: str | Path,
    weights: np.ndarray,
    net: NetworkConfig,
    generation: int,
) -> None:
    """Persist the population weights with topology and generation metadata."""
    path = Path(path)
    if path.suffix != ".npz":
        path = path.with_suffix(".npz")
    np.savez(
        path,
        weights=np.asarray(weights, dtype=np.float64),
        topology=_topology_signature(net),
        generation=np.int64(generation),
    )


def load_population(path: str | Path, net: NetworkConfig) -> tuple[np.ndarray, int]:
    """Load population weights, validating the topology against ``net``."""
    data = np.load(Path(path))
    weights = data["weights"]
    stored = data["topology"]
    expected = _topology_signature(net)
    if not np.array_equal(stored, expected):
        raise ValueError(
            f"population topology {stored.tolist()} does not match config {expected.tolist()}"
        )
    generation = int(data["generation"]) if "generation" in data else 0
    return weights, generation


def maybe_load_initial_weights(path: Optional[str], net: NetworkConfig) -> Optional[np.ndarray]:
    """Return weights from ``path`` if it exists, else ``None`` for random init."""
    if not path:
        return None
    if not Path(path).exists():
        return None
    weights, _ = load_population(path, net)
    return weights
