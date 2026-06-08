"""Tests for evolution-harness upgrades."""

import math
import random
from pathlib import Path

import numpy as np
import pytest

from dogfight.brain.factory import create_brain
from dogfight.brain.recurrent import ElmanNet, GruLiteNet
from dogfight.config import (
    CombatConfig,
    Config,
    CurriculumConfig,
    FitnessConfig,
    GeneticConfig,
    MetricsConfig,
    NetworkConfig,
    PhysicsConfig,
    PopulationConfig,
    RenderConfig,
    SensorConfig,
    SimulationConfig,
    load_config,
)
from dogfight.io.metrics import MetricsLogger
from dogfight.sim.actions import map_outputs
from dogfight.sim.combat import CombatSystem
from dogfight.sim.curriculum import CurriculumManager
from dogfight.sim.gene_pool import GenePool
from dogfight.sim.plane import Plane
from dogfight.sim.sensors import build_observations


def _base_config(**network_kw):
    network = NetworkConfig(**network_kw)
    return Config(
        simulation=SimulationConfig(num_planes=4, num_targets=2, generation_time=10, use_batch_update=False),
        genetic=GeneticConfig(seed=7, elitism=2),
        network=network,
        fitness=FitnessConfig(),
        physics=PhysicsConfig(),
        combat=CombatConfig(),
        curriculum=CurriculumConfig(),
        population=PopulationConfig(),
        metrics=MetricsConfig(enabled=False),
        render=RenderConfig(),
    )


def test_config_loads_defaults():
    cfg = load_config()
    assert cfg.network.kind == "elman"
    assert cfg.network.effective_num_inputs() == 26
    assert cfg.combat.enabled is True
    assert cfg.curriculum.enabled is True
    assert cfg.fitness.coeff_heading == 0.5
    assert cfg.physics.default_thrust == 0.25


def test_throttle_output_mapping():
    net = NetworkConfig(use_throttle_output=True)
    phys = PhysicsConfig(max_thrust=0.3)
    fl, rp, ry, thrust, _ = map_outputs([0.5, 0.5, 0.5, 0.8], net, phys)
    assert math.isclose(thrust, 0.24)
    assert -30.0 <= fl <= 30.0


def test_tanh_output_mapping():
    net = NetworkConfig(output_mapping="tanh")
    phys = PhysicsConfig()
    fl, rp, ry, thrust, _ = map_outputs([1.0, -1.0, 0.0, 0.5], net, phys)
    assert abs(fl) <= 30.0
    assert thrust == 0.25


def test_extra_sensors_increase_input_count():
    sensors = SensorConfig(own_speed=True, body_velocity=True, altitude=True)
    net = NetworkConfig(sensors=sensors)
    assert net.effective_num_inputs() == 7 + 1 + 3 + 1


def test_build_observations_with_extras():
    sensors = SensorConfig(own_speed=True, targeted_vector=True)
    obs = build_observations(
        target_local=__import__("dogfight.mathlib.vector3", fromlist=["MyVector"]).MyVector(1, 0, 0),
        local_up=__import__("dogfight.mathlib.vector3", fromlist=["MyVector"]).MyVector(0, 1, 0),
        distance=50.0,
        sensors=sensors,
        vel=__import__("dogfight.mathlib.vector3", fromlist=["MyVector"]).MyVector(0.5, 0, 0),
        pos_y=0.0,
        ground_level=-50.0,
        targeted_local=__import__("dogfight.mathlib.vector3", fromlist=["MyVector"]).MyVector(0, 0, -1),
        roll_rate=0.1,
        pitch_rate=0.2,
        yaw_rate=0.3,
    )
    assert len(obs) == 7 + 1 + 3


def test_elman_network_deterministic():
    cfg = NetworkConfig(kind="elman", num_hidden_layers=1, neurons_per_hidden_layer=4)
    net_a = ElmanNet(cfg, random.Random(1))
    net_b = ElmanNet(cfg, random.Random(1))
    inputs = [0.1] * cfg.effective_num_inputs()
    assert net_a.update(inputs) == net_b.update(inputs)


def test_gru_network_weight_roundtrip():
    cfg = NetworkConfig(kind="gru", num_hidden_layers=1, neurons_per_hidden_layer=4)
    net = GruLiteNet(cfg, random.Random(2))
    weights = net.get_weights()
    net.put_weights(weights)
    assert net.get_weights() == weights


def test_recurrent_factory():
    cfg = NetworkConfig(kind="elman", num_hidden_layers=1, neurons_per_hidden_layer=4)
    brain = create_brain(cfg, random.Random(0))
    assert brain.num_weights() == cfg.num_weights()


def test_novelty_selection_runs():
    config = _base_config()
    config.genetic.selection = "novelty"
    pool = GenePool(config)
    pool.init_planes()
    for _ in range(10):
        pool.update_population()
    pool.next_generation()
    assert len(pool.planes) == 4


def test_map_elites_selection_runs():
    config = _base_config()
    config.genetic.selection = "map_elites"
    pool = GenePool(config)
    pool.init_planes()
    for _ in range(10):
        pool.update_population()
    pool.next_generation()
    assert len(pool.planes) == 4


def test_curriculum_stage_progression():
    mgr = CurriculumManager(
        CurriculumConfig(
            enabled=True,
            stages=[
                __import__("dogfight.config", fromlist=["CurriculumStage"]).CurriculumStage("chase", 2),
                __import__("dogfight.config", fromlist=["CurriculumStage"]).CurriculumStage("combat", 2),
            ],
        )
    )
    mgr.on_generation_start(1)
    assert mgr.current_stage == "chase"
    mgr.on_generation_start(3)
    assert mgr.current_stage == "combat"


def test_combat_hit_detection(tmp_path):
    config = _base_config()
    config.combat.enabled = True
    config.combat.hit_radius = 5.0
    pool = GenePool(config)
    pool.init_planes()
    a, b = pool.planes[0], pool.planes[1]
    a.pos_x, a.pos_y, a.pos_z = 0.0, 0.0, 0.0
    b.pos_x, b.pos_y, b.pos_z = 1.0, 0.0, 0.0
    a.shooting = True
    a.shots[0].alive = True
    a.shots[0].pos_x, a.shots[0].pos_y, a.shots[0].pos_z = 0.5, 0.0, 0.0
    a.shots[0].dx, a.shots[0].dy, a.shots[0].dz = 1.0, 0.0, 0.0
    combat = CombatSystem(config.combat)
    combat.resolve_hits([a, b])
    assert b.damage_taken > 0
    assert a.hits_scored == 1


def test_metrics_logger(tmp_path):
    logger = MetricsLogger(MetricsConfig(enabled=True, log_path=str(tmp_path / "m.csv")))
    record = logger.log_generation(1, [1.0, 2.0, 3.0])
    assert record["fitness_best"] == 3.0
    assert (tmp_path / "m.csv").exists()


def test_magazine_limits_firing():
    config = _base_config()
    config.combat.magazine_size = 4
    config.combat.rounds_per_burst = 2
    config.combat.burst_cooldown_frames = 100
    pool = GenePool(config)
    plane = pool.planes[0]
    plane.init()
    plane.shooting = True
    plane.shotdelay = 4
    plane.update_guns()
    assert plane.ammo_remaining == 2
    plane.shooting = True
    plane.shotdelay = 4
    plane.burst_cooldown_remaining = 0
    plane.update_guns()
    assert plane.ammo_remaining == 0
    plane.shooting = True
    plane.shotdelay = 4
    plane.burst_cooldown_remaining = 0
    plane.update_guns()
    assert not plane.fired_this_frame


def test_shot_cost_applied_on_burst():
    config = _base_config()
    config.combat.magazine_size = 10
    config.fitness.shot_cost = 1.0
    pool = GenePool(config)
    plane = pool.planes[0]
    plane.init()
    plane.shooting = True
    plane.shotdelay = 4
    plane.update_guns()
    assert plane.fitness == -config.combat.rounds_per_burst


def test_miss_penalty_after_empty_burst():
    config = _base_config()
    config.combat.miss_check_frames = 2
    config.fitness.miss_penalty = 3.0
    pool = GenePool(config)
    plane = pool.planes[0]
    plane.init()
    plane.shooting = True
    plane.shotdelay = 4
    plane.update_guns()
    plane.update_guns()
    plane.update_guns()
    assert plane.fitness < 0


def test_batch_update_matches_sequential():
    config = _base_config()
    config.simulation.use_batch_update = True
    config_a = _base_config()
    config_a.simulation.use_batch_update = False
    config_a.genetic.seed = config.genetic.seed = 99

    pool_batch = GenePool(config)
    pool_seq = GenePool(config_a)
    for pool in (pool_batch, pool_seq):
        pool.init_planes()
    for _ in range(5):
        pool_batch.update_population()
        pool_seq.update_population()
    assert np.allclose(pool_batch.export_weights(), pool_seq.export_weights())
    assert pool_batch.planes[0].pos_x == pool_seq.planes[0].pos_x
