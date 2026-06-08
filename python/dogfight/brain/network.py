"""Port of the C++ ``NeuralNet`` class.

The forward pass is vectorized with NumPy but preserves the original
arithmetic: for each neuron the net input is the sum of weight times input,
with the bias (the final weight) subtracted, fed through a logistic sigmoid.
"""

from __future__ import annotations

import random
from typing import List, Tuple

import numpy as np

from .layer import NeuronLayer


def _sigmoid(activation: np.ndarray, response: float = 1.0) -> np.ndarray:
    return 1.0 / (1.0 + np.exp(-activation / response))


class NeuralNet:
    def __init__(self, net_config, rng: random.Random):
        self.num_inputs = net_config.effective_num_inputs()
        self.num_outputs = net_config.effective_num_outputs()
        self.num_hidden_layers = net_config.num_hidden_layers
        self.neurons_per_hidden_layer = net_config.neurons_per_hidden_layer
        self._dims = self._topology()

        # Build via NeuronLayer so random initialization order matches the C++.
        self.weights: List[np.ndarray] = []
        self.biases: List[np.ndarray] = []
        for num_neurons, num_inputs in self._dims:
            layer = NeuronLayer(num_neurons, num_inputs, rng)
            w = np.empty((num_neurons, num_inputs), dtype=np.float64)
            b = np.empty(num_neurons, dtype=np.float64)
            for j, neuron in enumerate(layer.neurons):
                w[j, :] = neuron.weights[:-1]
                b[j] = neuron.weights[-1]
            self.weights.append(w)
            self.biases.append(b)

    def _topology(self) -> List[Tuple[int, int]]:
        i = self.num_inputs
        o = self.num_outputs
        h = self.num_hidden_layers
        n = self.neurons_per_hidden_layer
        if h > 0:
            dims = [(n, i)]
            dims += [(n, n) for _ in range(h - 1)]
            dims.append((o, n))
        else:
            dims = [(o, i)]
        return dims

    def num_weights(self) -> int:
        return sum(num_neurons * (num_inputs + 1) for num_neurons, num_inputs in self._dims)

    def update(self, inputs: List[float]) -> List[float]:
        x = np.asarray(inputs, dtype=np.float64)
        if x.shape[0] != self.num_inputs:
            raise ValueError(
                f"NN error: got {x.shape[0]} inputs, expected {self.num_inputs}"
            )
        for w, b in zip(self.weights, self.biases):
            x = _sigmoid(w @ x - b)
        return x.tolist()

    def get_weights(self) -> List[float]:
        out: List[float] = []
        for w, b in zip(self.weights, self.biases):
            for j in range(w.shape[0]):
                out.extend(w[j, :].tolist())
                out.append(float(b[j]))
        return out

    def put_weights(self, weights: List[float]) -> None:
        index = 0
        for w, b in zip(self.weights, self.biases):
            num_neurons, num_inputs = w.shape
            for j in range(num_neurons):
                for k in range(num_inputs):
                    w[j, k] = weights[index]
                    index += 1
                b[j] = weights[index]
                index += 1
