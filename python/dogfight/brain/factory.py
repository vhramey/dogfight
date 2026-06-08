"""Create neural networks by configured kind."""

from __future__ import annotations

import random
from typing import Protocol

from .network import NeuralNet
from .recurrent import ElmanNet, GruLiteNet


class Brain(Protocol):
    def update(self, inputs: list[float]) -> list[float]: ...
    def get_weights(self) -> list[float]: ...
    def put_weights(self, weights: list[float]) -> None: ...
    def num_weights(self) -> int: ...
    def reset_state(self) -> None: ...


def create_brain(net_config, rng: random.Random) -> Brain:
    kind = net_config.kind
    if kind == "feedforward":
        return _FeedForwardAdapter(NeuralNet(net_config, rng))
    if kind == "elman":
        return ElmanNet(net_config, rng)
    if kind == "gru":
        return GruLiteNet(net_config, rng)
    raise ValueError(f"unknown network kind: {kind}")


class _FeedForwardAdapter:
    def __init__(self, net: NeuralNet):
        self._net = net

    def update(self, inputs: list[float]) -> list[float]:
        return self._net.update(inputs)

    def get_weights(self) -> list[float]:
        return self._net.get_weights()

    def put_weights(self, weights: list[float]) -> None:
        self._net.put_weights(weights)

    def num_weights(self) -> int:
        return self._net.num_weights()

    def reset_state(self) -> None:
        pass
