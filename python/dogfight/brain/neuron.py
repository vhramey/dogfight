"""Port of the C++ ``Neuron`` class.

Each neuron stores ``num_inputs + 1`` weights; the extra weight is the
threshold (bias). Initial weights are uniform in roughly ``[-1, 1]``.
"""

from __future__ import annotations

import random
from typing import List


def small_random(rng: random.Random) -> float:
    """Match the C++ ``(double)(rand() % 4000 - 2000) / 2000.0``."""
    return (rng.randrange(4000) - 2000) / 2000.0


class Neuron:
    __slots__ = ("num_inputs", "weights")

    def __init__(self, num_inputs: int, rng: random.Random):
        # +1 for the bias weight, mirroring the C++ constructor.
        self.num_inputs = num_inputs + 1
        self.weights: List[float] = [small_random(rng) for _ in range(num_inputs + 1)]
