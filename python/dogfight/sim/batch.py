"""Batched neural-network inference across the population."""

from __future__ import annotations

from typing import List, TYPE_CHECKING

import numpy as np

if TYPE_CHECKING:
    from .plane import Plane


def batch_think(planes: List[Plane], inputs_batch: np.ndarray) -> None:
    """Run feed-forward inference in batch for planes sharing the same topology.

    Recurrent networks fall back to per-plane ``think()`` because they carry state.
    """
    if not planes:
        return

    kind = planes[0].brain_kind
    if kind != "feedforward":
        for plane, row in zip(planes, inputs_batch):
            plane.think_with_inputs(row.tolist())
        return

    net = _feedforward_net(planes[0])
    outputs = _batch_feedforward(net, inputs_batch)
    for plane, out_row in zip(planes, outputs):
        plane.apply_brain_outputs(out_row.tolist())


def _feedforward_net(plane: Plane):
    brain = plane.brain
    if hasattr(brain, "_net"):
        return brain._net
    return brain


def _batch_feedforward(net, inputs: np.ndarray) -> np.ndarray:
    x = inputs.astype(np.float64)
    for w, b in zip(net.weights, net.biases):
        x = 1.0 / (1.0 + np.exp(-(x @ w.T - b)))
    return x


def collect_observations(planes: List[Plane]) -> np.ndarray:
    rows = [plane.build_observations() for plane in planes]
    return np.asarray(rows, dtype=np.float64)
