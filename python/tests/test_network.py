import random

from dogfight.brain.network import NeuralNet
from dogfight.config import NetworkConfig


def test_weight_count_matches_topology():
    cfg = NetworkConfig(num_inputs=7, num_outputs=4, num_hidden_layers=3, neurons_per_hidden_layer=20)
    net = NeuralNet(cfg, random.Random(0))
    assert cfg.num_weights() == 1084
    assert net.num_weights() == 1084
    assert len(net.get_weights()) == 1084


def test_weights_roundtrip():
    cfg = NetworkConfig(num_inputs=7, num_outputs=4, num_hidden_layers=3, neurons_per_hidden_layer=20)
    net = NeuralNet(cfg, random.Random(1))
    weights = net.get_weights()
    net.put_weights(weights)
    assert net.get_weights() == weights


def test_update_outputs_in_unit_range():
    cfg = NetworkConfig(num_inputs=7, num_outputs=4, num_hidden_layers=3, neurons_per_hidden_layer=20)
    net = NeuralNet(cfg, random.Random(2))
    outputs = net.update([0.1, -0.2, 0.3, 0.0, 1.0, -1.0, 0.5])
    assert len(outputs) == 4
    assert all(0.0 <= o <= 1.0 for o in outputs)


def test_single_layer_topology():
    cfg = NetworkConfig(num_inputs=3, num_outputs=2, num_hidden_layers=0, neurons_per_hidden_layer=20)
    net = NeuralNet(cfg, random.Random(3))
    # One output layer with 2 neurons, each 3 inputs + bias.
    assert net.num_weights() == 2 * (3 + 1)
    assert len(net.update([1.0, 2.0, 3.0])) == 2
