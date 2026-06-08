"""Configuration loading.

Replaces the C++ ``par.ini`` file and the interactive ``stdin`` prompts that
the original program used to gather simulation and genetic-algorithm
parameters.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path
from typing import List, Optional

import yaml

_DEFAULT_CONFIG_PATH = Path(__file__).resolve().parent.parent / "config" / "default.yaml"


@dataclass
class SimulationConfig:
    num_planes: int = 100
    num_targets: int = 5
    generation_time: int = 2000
    reset_time: int = 1_000_000
    target_inc_time: int = 100
    ground_level: float = -50.0
    use_batch_update: bool = True
    headless_workers: int = 0  # 0 = single process; >0 enables multiprocessing


@dataclass
class GeneticConfig:
    mutation_rate: int = 5
    max_mutation: float = 1.0
    crossover_rate: int = 70
    tournament_size: int = 3
    elitism: int = 2
    seed: Optional[int] = None
    selection: str = "tournament"  # tournament | novelty | map_elites | speciation
    novelty_k: int = 10
    novelty_weight: float = 0.3
    map_elites_bins: List[int] = field(default_factory=lambda: [8, 8, 8])
    speciation_threshold: float = 0.15
    fitness_sharing_sigma: float = 0.1


@dataclass
class SensorConfig:
    own_speed: bool = False
    body_velocity: bool = False
    angular_rates: bool = False
    altitude: bool = False
    distance_log: bool = False
    targeted_vector: bool = False
    opponent_relative: bool = False
    ammo_status: bool = False

    def extra_input_count(self) -> int:
        count = 0
        if self.own_speed:
            count += 1
        if self.body_velocity:
            count += 3
        if self.angular_rates:
            count += 3
        if self.altitude:
            count += 1
        if self.distance_log:
            count += 1
        if self.targeted_vector:
            count += 3
        if self.opponent_relative:
            count += 5  # dir (3) + inv-distance (1) + normalized distance (1)
        if self.ammo_status:
            count += 2  # ammo fraction + cooldown ready
        return count


@dataclass
class NetworkConfig:
    kind: str = "feedforward"  # feedforward | elman | gru
    num_inputs: int = 7
    num_outputs: int = 4
    num_hidden_layers: int = 3
    neurons_per_hidden_layer: int = 20
    sensors: SensorConfig = field(default_factory=SensorConfig)
    output_mapping: str = "legacy"  # legacy | tanh
    use_throttle_output: bool = False
    use_learned_shooting: bool = False

    BASE_INPUTS: int = field(default=7, init=False, repr=False)

    def effective_num_inputs(self) -> int:
        extra = self.sensors.extra_input_count()
        if extra == 0:
            return self.num_inputs
        return self.BASE_INPUTS + extra

    def effective_num_outputs(self) -> int:
        outputs = self.num_outputs
        if self.use_learned_shooting:
            outputs += 1
        return outputs

    def num_weights(self) -> int:
        """Total weight count for this topology (bias weight included)."""
        i = self.effective_num_inputs()
        o = self.effective_num_outputs()
        h = self.num_hidden_layers
        n = self.neurons_per_hidden_layer
        if self.kind == "feedforward":
            if h <= 0:
                return o * (i + 1)
            return n * (i + 1) + (h - 1) * n * (n + 1) + o * (n + 1)
        if self.kind == "elman":
            # Recurrent feedback on the first hidden layer (+n inputs per neuron).
            if h <= 0:
                return o * (i + 1)
            return n * (i + n + 1) + (h - 1) * n * (n + 1) + o * (n + 1)
        if self.kind == "gru":
            # GRU-lite: one hidden layer with 3 gates (update, reset, candidate).
            if h <= 0:
                return o * (i + 1)
            gate_weights = 3 * n * (i + n + 1)
            rest = (h - 1) * n * (n + 1) if h > 1 else 0
            return gate_weights + rest + o * (n + 1)
        raise ValueError(f"unknown network kind: {self.kind}")

    def topology_signature(self) -> List[int]:
        """Serializable topology key for population I/O."""
        s = self.sensors
        return [
            self.effective_num_inputs(),
            self.effective_num_outputs(),
            self.num_hidden_layers,
            self.neurons_per_hidden_layer,
            _kind_id(self.kind),
            int(s.own_speed),
            int(s.body_velocity),
            int(s.angular_rates),
            int(s.altitude),
            int(s.distance_log),
            int(s.targeted_vector),
            int(s.opponent_relative),
            int(s.ammo_status),
            int(self.use_learned_shooting),
        ]


def _kind_id(kind: str) -> int:
    return {"feedforward": 0, "elman": 1, "gru": 2}.get(kind, 0)


@dataclass
class FitnessConfig:
    coeff_upright: float = 50.0
    coeff_heading: float = 0.5
    coeff_velocity: float = 0.01
    lock_threshold: float = 0.999
    crash_penalty: float = 1000.0
    hit_reward: float = 10.0
    kill_reward: float = 100.0
    evasion_reward: float = 0.1
    damage_penalty: float = 50.0
    survival_reward: float = 0.01
    shot_cost: float = 0.05
    miss_penalty: float = 0.5


@dataclass
class PhysicsConfig:
    max_flap_angle: float = 60.0
    max_yaw_flap_angle: float = 10.0
    max_thrust: float = 0.3
    default_thrust: float = 0.25
    gravity: float = 0.15
    lift_constant: float = 0.8
    kite_constant: float = 0.8
    drag_constant: float = 0.8
    flap_constant: float = 3.0


@dataclass
class CombatConfig:
    enabled: bool = False
    max_health: float = 100.0
    bullet_damage: float = 25.0
    hit_radius: float = 2.0
    magazine_size: int = 60
    rounds_per_burst: int = 2
    burst_cooldown_frames: int = 45
    burst_frames_between_shots: int = 3
    miss_check_frames: int = 50
    hall_of_fame_size: int = 5
    hall_of_fame_path: str = "hall_of_fame.npz"


@dataclass
class CurriculumStage:
    name: str = "chase"
    generations: int = 100


@dataclass
class CurriculumConfig:
    enabled: bool = False
    stages: List[CurriculumStage] = field(
        default_factory=lambda: [
            CurriculumStage("chase", 100),
            CurriculumStage("evasion", 50),
            CurriculumStage("combat", 50),
        ]
    )


@dataclass
class PopulationConfig:
    initial_weights: Optional[str] = None
    auto_save_every: int = 10
    save_path: str = "population.npz"


@dataclass
class MetricsConfig:
    enabled: bool = True
    log_path: str = "metrics.csv"
    best_genome_path: str = "best_genome.npz"
    log_json_path: Optional[str] = "metrics.jsonl"


@dataclass
class RenderConfig:
    width: int = 640
    height: int = 480
    fov: float = 45.0
    textures_dir: Optional[str] = None


@dataclass
class Config:
    simulation: SimulationConfig
    genetic: GeneticConfig
    network: NetworkConfig
    fitness: FitnessConfig
    physics: PhysicsConfig
    combat: CombatConfig
    curriculum: CurriculumConfig
    population: PopulationConfig
    metrics: MetricsConfig
    render: RenderConfig


def _load_sensors(raw: dict) -> SensorConfig:
    return SensorConfig(**{k: v for k, v in raw.items() if k in SensorConfig.__dataclass_fields__})


def _load_curriculum(raw: dict) -> CurriculumConfig:
    stages_raw = raw.pop("stages", None)
    cfg = CurriculumConfig(**{k: v for k, v in raw.items() if k in CurriculumConfig.__dataclass_fields__})
    if stages_raw:
        cfg.stages = [CurriculumStage(**s) for s in stages_raw]
    return cfg


def load_config(path: Optional[str | Path] = None) -> Config:
    """Load configuration from a YAML file, falling back to bundled defaults."""
    config_path = Path(path) if path is not None else _DEFAULT_CONFIG_PATH
    with open(config_path, "r") as handle:
        raw = yaml.safe_load(handle) or {}

    network_raw = dict(raw.get("network", {}))
    sensors_raw = network_raw.pop("sensors", {})
    sensors = _load_sensors(sensors_raw)
    network = NetworkConfig(sensors=sensors, **network_raw)

    curriculum_raw = dict(raw.get("curriculum", {}))
    curriculum = _load_curriculum(curriculum_raw)

    return Config(
        simulation=SimulationConfig(**raw.get("simulation", {})),
        genetic=GeneticConfig(**raw.get("genetic", {})),
        network=network,
        fitness=FitnessConfig(**raw.get("fitness", {})),
        physics=PhysicsConfig(**raw.get("physics", {})),
        combat=CombatConfig(**raw.get("combat", {})),
        curriculum=curriculum,
        population=PopulationConfig(**raw.get("population", {})),
        metrics=MetricsConfig(**raw.get("metrics", {})),
        render=RenderConfig(**raw.get("render", {})),
    )
