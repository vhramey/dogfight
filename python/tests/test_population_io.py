import numpy as np
import pytest

from dogfight.config import NetworkConfig
from dogfight.io.population import load_population, save_population


def test_save_and_load_roundtrip(tmp_path):
    net = NetworkConfig(num_inputs=7, num_outputs=4, num_hidden_layers=1, neurons_per_hidden_layer=6)
    weights = np.random.RandomState(0).randn(5, net.num_weights())
    path = tmp_path / "pop.npz"
    save_population(path, weights, net, generation=7)
    loaded, generation = load_population(path, net)
    assert generation == 7
    assert np.allclose(loaded, weights)


def test_load_rejects_topology_mismatch(tmp_path):
    net = NetworkConfig(num_inputs=7, num_outputs=4, num_hidden_layers=1, neurons_per_hidden_layer=6)
    weights = np.zeros((3, net.num_weights()))
    path = tmp_path / "pop.npz"
    save_population(path, weights, net, generation=0)

    other = NetworkConfig(num_inputs=7, num_outputs=4, num_hidden_layers=3, neurons_per_hidden_layer=20)
    with pytest.raises(ValueError):
        load_population(path, other)
