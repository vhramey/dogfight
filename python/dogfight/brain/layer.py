"""Port of the C++ ``NeuronLayer`` class."""

from __future__ import annotations

import random
from typing import List

from .neuron import Neuron


class NeuronLayer:
    __slots__ = ("num_neurons", "neurons")

    def __init__(self, num_neurons: int, num_inputs_per_neuron: int, rng: random.Random):
        self.num_neurons = num_neurons
        self.neurons: List[Neuron] = [
            Neuron(num_inputs_per_neuron, rng) for _ in range(num_neurons)
        ]
