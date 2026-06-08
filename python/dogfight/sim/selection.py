"""Alternative selection strategies for genetic search."""

from __future__ import annotations

import random
from typing import Dict, List, Tuple

import numpy as np

from ..config import GeneticConfig
from .behavior import BehaviorDescriptor, descriptor_distance


def tournament_select(planes: List, rng: random.Random, tournament_size: int):
    group = [planes[rng.randrange(len(planes))] for _ in range(tournament_size)]
    return max(group, key=lambda p: p.fitness)


def novelty_scores(
    descriptors: List[List[float]],
    archive: List[List[float]],
    k: int,
) -> List[float]:
    pool = descriptors + archive
    scores = []
    for desc in descriptors:
        distances = sorted(descriptor_distance(desc, other) for other in pool if other != desc)
        scores.append(sum(distances[:k]) / max(1, min(k, len(distances))))
    return scores


def fitness_sharing(planes: List, sigma: float) -> None:
    """Adjust fitness in-place using behavior descriptor sharing."""
    descs = [p.behavior.finalize() for p in planes]
    for i, plane in enumerate(planes):
        niche = 0.0
        for j, other in enumerate(descs):
            dist = descriptor_distance(descs[i], other)
            niche += np.exp(-(dist * dist) / (2.0 * sigma * sigma))
        if niche > 0:
            plane.fitness /= niche


class MapElitesArchive:
    def __init__(self, bins: List[int]):
        self.bins = bins
        self.cells: Dict[Tuple[int, ...], object] = {}

    def _bin(self, descriptor: List[float]) -> Tuple[int, ...]:
        coords = []
        for value, num_bins in zip(descriptor[: len(self.bins)], self.bins):
            coords.append(min(num_bins - 1, max(0, int(value * num_bins))))
        while len(coords) < len(self.bins):
            coords.append(0)
        return tuple(coords)

    def insert(self, plane, descriptor: List[float]) -> bool:
        key = self._bin(descriptor)
        current = self.cells.get(key)
        if current is None or plane.fitness > current.fitness:
            self.cells[key] = plane
            return True
        return False

    def elites(self) -> List:
        return list(self.cells.values())


def combined_score(fitness: float, novelty: float, weight: float) -> float:
    return fitness * (1.0 - weight) + novelty * weight
