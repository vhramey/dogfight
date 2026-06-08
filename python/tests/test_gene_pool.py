import numpy as np

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
    SimulationConfig,
)
from dogfight.sim.gene_pool import GenePool


def _small_config(seed=0):
    return Config(
        simulation=SimulationConfig(num_planes=8, num_targets=2, generation_time=20, use_batch_update=False),
        genetic=GeneticConfig(tournament_size=3, elitism=2, seed=seed),
        network=NetworkConfig(num_inputs=7, num_outputs=4, num_hidden_layers=1, neurons_per_hidden_layer=6),
        fitness=FitnessConfig(),
        physics=PhysicsConfig(),
        combat=CombatConfig(),
        curriculum=CurriculumConfig(),
        population=PopulationConfig(),
        metrics=MetricsConfig(enabled=False),
        render=RenderConfig(),
    )


def test_population_size_stable_across_generations():
    pool = GenePool(_small_config())
    pool.init_planes()
    for _ in range(3):
        for _ in range(20):
            pool.update_population()
        pool.next_generation()
        pool.init_planes()
    assert len(pool.planes) == 8


def test_num_bases_matches_network():
    pool = GenePool(_small_config())
    expected = pool.config.network.num_weights()
    assert pool.num_bases == expected
    assert len(pool.planes[0].get_weights()) == expected


def test_reproduce_preserves_weight_length():
    pool = GenePool(_small_config())
    pool.init_planes()
    for _ in range(5):
        pool.update_population()
    father = pool.planes[0]
    mother = pool.planes[1]
    c1, c2 = pool.reproduce(father, mother)
    assert len(c1.get_weights()) == pool.num_bases
    assert len(c2.get_weights()) == pool.num_bases


def test_export_weights_shape():
    pool = GenePool(_small_config())
    weights = pool.export_weights()
    assert weights.shape == (8, pool.num_bases)
    assert weights.dtype == np.float64


def test_deterministic_with_seed():
    pool_a = GenePool(_small_config(seed=123))
    pool_b = GenePool(_small_config(seed=123))
    for pool in (pool_a, pool_b):
        pool.init_planes()
        for _ in range(10):
            pool.update_population()
    assert np.allclose(pool_a.export_weights(), pool_b.export_weights())
    assert pool_a.planes[0].pos_x == pool_b.planes[0].pos_x


def test_elite_genome_preserved_on_tie():
    pool = GenePool(_small_config(seed=1))
    pool.init_planes()
    for plane in pool.planes:
        plane.fitness = 10.0
    elite_weights = pool.planes[0].getbrain()
    pool.next_generation()
    assert pool.planes[0].getbrain() == elite_weights
