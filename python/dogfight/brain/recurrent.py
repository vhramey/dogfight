"""Recurrent neural networks (Elman and GRU-lite) with flat weight genomes."""

from __future__ import annotations

import random
from typing import List, Tuple

import numpy as np

from .neuron import small_random
from .network import _sigmoid


class ElmanNet:
    """Elman network with recurrence on the first hidden layer."""

    def __init__(self, net_config, rng: random.Random):
        self.net_config = net_config
        self.num_inputs = net_config.effective_num_inputs()
        self.num_outputs = net_config.effective_num_outputs()
        self.num_hidden_layers = net_config.num_hidden_layers
        self.neurons_per_hidden_layer = net_config.neurons_per_hidden_layer
        self._dims = self._topology()

        self.weights: List[np.ndarray] = []
        self.biases: List[np.ndarray] = []
        for num_neurons, num_inputs in self._dims:
            w = np.array(
                [[small_random(rng) for _ in range(num_inputs)] for _ in range(num_neurons)],
                dtype=np.float64,
            )
            b = np.array([small_random(rng) for _ in range(num_neurons)], dtype=np.float64)
            self.weights.append(w)
            self.biases.append(b)

        n = self.neurons_per_hidden_layer
        self._hidden_state = np.zeros(n, dtype=np.float64)

    def _topology(self) -> List[Tuple[int, int]]:
        i = self.num_inputs
        o = self.num_outputs
        h = self.num_hidden_layers
        n = self.neurons_per_hidden_layer
        if h <= 0:
            return [(o, i)]
        dims = [(n, i + n)]  # recurrent inputs on first hidden layer
        dims += [(n, n) for _ in range(h - 1)]
        dims.append((o, n))
        return dims

    def reset_state(self) -> None:
        self._hidden_state.fill(0.0)

    def num_weights(self) -> int:
        return sum(num_neurons * (num_inputs + 1) for num_neurons, num_inputs in self._dims)

    def update(self, inputs: List[float]) -> List[float]:
        x = np.asarray(inputs, dtype=np.float64)
        for layer_idx, (w, b) in enumerate(zip(self.weights, self.biases)):
            if layer_idx == 0 and self.num_hidden_layers > 0:
                x = np.concatenate([x, self._hidden_state])
            x = _sigmoid(w @ x - b)
            if layer_idx == 0 and self.num_hidden_layers > 0:
                self._hidden_state = x.copy()
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


class GruLiteNet:
    """GRU-lite: one recurrent hidden layer with update/reset/candidate gates."""

    def __init__(self, net_config, rng: random.Random):
        self.net_config = net_config
        self.num_inputs = net_config.effective_num_inputs()
        self.num_outputs = net_config.effective_num_outputs()
        self.num_hidden_layers = net_config.num_hidden_layers
        self.neurons_per_hidden_layer = net_config.neurons_per_hidden_layer
        n = self.neurons_per_hidden_layer
        i = self.num_inputs
        o = self.num_outputs
        h = self.num_hidden_layers

        gate_in = i + n
        self.w_z = self._rand_w(n, gate_in, rng)
        self.w_r = self._rand_w(n, gate_in, rng)
        self.w_h = self._rand_w(n, gate_in, rng)
        self.b_z = self._rand_b(n, rng)
        self.b_r = self._rand_b(n, rng)
        self.b_h = self._rand_b(n, rng)

        self._ff_weights: List[np.ndarray] = []
        self._ff_biases: List[np.ndarray] = []
        if h > 1:
            for _ in range(h - 1):
                self._ff_weights.append(self._rand_w(n, n, rng))
                self._ff_biases.append(self._rand_b(n, rng))
        self.w_out = self._rand_w(o, n, rng)
        self.b_out = self._rand_b(o, rng)

        self._hidden_state = np.zeros(n, dtype=np.float64)

    @staticmethod
    def _rand_w(rows: int, cols: int, rng: random.Random) -> np.ndarray:
        return np.array([[small_random(rng) for _ in range(cols)] for _ in range(rows)], dtype=np.float64)

    @staticmethod
    def _rand_b(size: int, rng: random.Random) -> np.ndarray:
        return np.array([small_random(rng) for _ in range(size)], dtype=np.float64)

    def reset_state(self) -> None:
        self._hidden_state.fill(0.0)

    def num_weights(self) -> int:
        n = self.neurons_per_hidden_layer
        i = self.num_inputs
        o = self.num_outputs
        h = self.num_hidden_layers
        gate_in = i + n
        total = 3 * n * (gate_in + 1)
        if h > 1:
            total += (h - 1) * n * (n + 1)
        total += o * (n + 1)
        return total

    def update(self, inputs: List[float]) -> List[float]:
        x = np.asarray(inputs, dtype=np.float64)
        concat = np.concatenate([x, self._hidden_state])
        z = _sigmoid(self.w_z @ concat - self.b_z)
        r = _sigmoid(self.w_r @ concat - self.b_r)
        h_tilde = np.tanh(self.w_h @ np.concatenate([x, r * self._hidden_state]) - self.b_h)
        self._hidden_state = (1.0 - z) * self._hidden_state + z * h_tilde

        hidden = self._hidden_state
        for w, b in zip(self._ff_weights, self._ff_biases):
            hidden = _sigmoid(w @ hidden - b)
        out = _sigmoid(self.w_out @ hidden - self.b_out)
        return out.tolist()

    def get_weights(self) -> List[float]:
        out: List[float] = []

        def append_layer(w: np.ndarray, b: np.ndarray) -> None:
            for j in range(w.shape[0]):
                out.extend(w[j, :].tolist())
                out.append(float(b[j]))

        append_layer(self.w_z, self.b_z)
        append_layer(self.w_r, self.b_r)
        append_layer(self.w_h, self.b_h)
        for w, b in zip(self._ff_weights, self._ff_biases):
            append_layer(w, b)
        append_layer(self.w_out, self.b_out)
        return out

    def put_weights(self, weights: List[float]) -> None:
        index = 0

        def load_layer(w: np.ndarray, b: np.ndarray) -> None:
            nonlocal index
            for j in range(w.shape[0]):
                for k in range(w.shape[1]):
                    w[j, k] = weights[index]
                    index += 1
                b[j] = weights[index]
                index += 1

        load_layer(self.w_z, self.b_z)
        load_layer(self.w_r, self.b_r)
        load_layer(self.w_h, self.b_h)
        for w, b in zip(self._ff_weights, self._ff_biases):
            load_layer(w, b)
        load_layer(self.w_out, self.b_out)
