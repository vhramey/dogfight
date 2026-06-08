"""Curriculum stages that progressively change fitness emphasis."""

from __future__ import annotations

from ..config import CombatConfig, CurriculumConfig, FitnessConfig


class CurriculumManager:
    def __init__(self, config: CurriculumConfig):
        self.config = config
        self._generation = 0
        self._stage_index = 0

    @property
    def current_stage(self) -> str:
        if not self.config.enabled or not self.config.stages:
            return "chase"
        return self.config.stages[min(self._stage_index, len(self.config.stages) - 1)].name

    def on_generation_start(self, generation: int) -> None:
        self._generation = generation
        if not self.config.enabled:
            return
        total = 0
        for i, stage in enumerate(self.config.stages):
            total += stage.generations
            if generation <= total:
                self._stage_index = i
                return
        self._stage_index = len(self.config.stages) - 1

    def apply_stage(self, fitness: FitnessConfig, combat: CombatConfig) -> tuple[FitnessConfig, CombatConfig]:
        """Return stage-adjusted copies of fitness/combat config."""
        if not self.config.enabled:
            return fitness, combat

        stage = self.current_stage
        f = FitnessConfig(**fitness.__dict__)
        c = CombatConfig(**combat.__dict__)

        if stage == "chase":
            c.enabled = False
            f.hit_reward = 0.0
            f.kill_reward = 0.0
            f.evasion_reward = 0.0
        elif stage == "evasion":
            c.enabled = False
            f.coeff_heading *= 0.5
            f.evasion_reward *= 3.0
        elif stage == "combat":
            c.enabled = True
            f.hit_reward *= 2.0
            f.kill_reward *= 2.0

        return f, c
