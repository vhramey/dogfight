"""Port of the C++ ``GenePool`` class.

Manages the population of planes, the moving targets, and the genetic-algorithm
step (tournament selection, single-segment crossover, mutation, and elitism).
"""

from __future__ import annotations

import random
from typing import List, Optional, Tuple

import numpy as np

from ..config import Config
from ..io.metrics import MetricsLogger
from ..mathlib.vector3 import MyVector
from .batch import batch_think, collect_observations
from .behavior import descriptor_distance
from .combat import CombatSystem
from .curriculum import CurriculumManager
from .plane import Plane
from .selection import (
    MapElitesArchive,
    combined_score,
    fitness_sharing,
    novelty_scores,
    tournament_select,
)
from .target import Target


class GenePool:
    def __init__(self, config: Config, initial_weights: Optional[np.ndarray] = None):
        self.config = config
        sim = config.simulation
        gen = config.genetic

        self.rng = random.Random(gen.seed)
        self.num_targets = sim.num_targets
        self.inc_time = sim.target_inc_time
        self.ground_level = sim.ground_level
        self.mutation_rate = gen.mutation_rate
        self.max_mutation = gen.max_mutation
        self.crossover_rate = gen.crossover_rate
        self.tournament_size = max(1, gen.tournament_size)
        self.elitism = gen.elitism
        self.selection_mode = gen.selection
        self.use_batch_update = sim.use_batch_update

        self.combat = CombatSystem(config.combat)
        self.combat.load_hall_of_fame(config.network)
        self.curriculum = CurriculumManager(config.curriculum)
        self.metrics = MetricsLogger(config.metrics)
        self._novelty_archive: List[List[float]] = []
        self._map_elites = MapElitesArchive(gen.map_elites_bins)
        self._generation_count = 0

        self._active_fitness = config.fitness
        self._active_combat = config.combat

        self.targets: List[Target] = [Target() for _ in range(self.num_targets)]
        self.waypoints: List[List[float]] = []
        self.increments: List[List[float]] = []
        for i in range(self.num_targets):
            tx, ty, tz = self._rand_target()
            self.targets[i].pos_x = tx
            self.targets[i].pos_y = ty
            self.targets[i].pos_z = tz
            self.targets[i].yaw = self.rng.random() * 2 * 3.141592653589793
            self.targets[i].update_qm()
            self.waypoints.append([float(tx), float(ty), float(tz)])
            self.increments.append([0.0, 0.0, 0.0])
            self.set_inc(i)

        self.planes: List[Plane] = []
        num_planes = sim.num_planes
        if initial_weights is not None:
            for i in range(num_planes):
                plane = self._new_plane()
                plane.setbrain(list(initial_weights[i]))
                self.planes.append(plane)
        else:
            for _ in range(num_planes):
                plane = self._new_plane()
                plane.scatterbrain()
                self.planes.append(plane)

        self.num_bases = len(self.planes[0].getbrain())

    # ------------------------------------------------------------- helpers
    def _new_plane(self) -> Plane:
        plane = Plane(
            self.config.network,
            self.rng,
            self.ground_level,
            fitness_config=self._active_fitness,
            physics_config=self.config.physics,
        )
        plane.configure_combat(self._active_combat)
        if self._active_combat.enabled:
            self.combat.reset_plane_combat(plane)
        return plane

    def _rand_target(self) -> Tuple[int, int, int]:
        return (
            self.rng.randrange(200) - 100,
            self.rng.randrange(40) - 20,
            self.rng.randrange(200) - 100,
        )

    def get_plane(self, i: int) -> Plane:
        return self.planes[i]

    def _apply_curriculum(self) -> None:
        self.curriculum.on_generation_start(self._generation_count)
        self._active_fitness, self._active_combat = self.curriculum.apply_stage(
            self.config.fitness, self.config.combat
        )
        for plane in self.planes:
            plane.fitness_config = self._active_fitness
            plane.configure_combat(self._active_combat)
            if self._active_combat.enabled:
                self.combat.reset_plane_combat(plane)

    # ------------------------------------------------------------- targets
    def set_inc(self, i: int) -> None:
        target = self.targets[i]
        waypoint = self.waypoints[i]
        if abs(target.pos_x - waypoint[0]) < 1:
            tx, ty, tz = self._rand_target()
            waypoint[0] = float(tx)
            waypoint[1] = float(ty)
            waypoint[2] = float(tz)
        self.increments[i][0] = (waypoint[0] - target.pos_x) / self.inc_time
        self.increments[i][1] = (waypoint[1] - target.pos_y) / self.inc_time
        self.increments[i][2] = (waypoint[2] - target.pos_z) / self.inc_time

    def set_targets(self) -> None:
        for i in range(self.num_targets):
            self.set_inc(i)
            self.targets[i].pos_x += self.increments[i][0]
            self.targets[i].pos_y += self.increments[i][1]
            self.targets[i].pos_z += self.increments[i][2]

        for plane in self.planes:
            distance = 10000.0
            target_index = 0
            for j in range(self.num_targets):
                tgt = self.targets[j]
                dx = plane.pos_x - tgt.pos_x
                dy = plane.pos_y - tgt.pos_y
                dz = plane.pos_z - tgt.pos_z
                temp = dx * dx + dy * dy + dz * dz
                if temp < distance:
                    target_index = j
                    distance = temp

            tgt = self.targets[target_index]
            new_target = MyVector(
                tgt.pos_x - plane.pos_x,
                tgt.pos_y - plane.pos_y,
                tgt.pos_z - plane.pos_z,
            )
            new_target.calc_mag()
            new_target.unitize()
            plane.set_target(new_target)
            plane.set_distance(distance ** 0.5)

            new_targeted = MyVector(
                -tgt.pos_x + plane.pos_x,
                -tgt.pos_y + plane.pos_y,
                -tgt.pos_z + plane.pos_z,
            )
            new_targeted.calc_mag()
            new_targeted.unitize()
            plane.set_targeted(tgt.localize(new_targeted))

        if self._active_combat.enabled and self.config.network.sensors.opponent_relative:
            self._assign_nearest_opponents()

    def _assign_nearest_opponents(self) -> None:
        for i, plane in enumerate(self.planes):
            if not plane.alive:
                continue
            best_dist = 10000.0
            best_dir = MyVector(0.0, 0.0, 0.0)
            for j, other in enumerate(self.planes):
                if i == j or not other.alive:
                    continue
                dx = other.pos_x - plane.pos_x
                dy = other.pos_y - plane.pos_y
                dz = other.pos_z - plane.pos_z
                dist = (dx * dx + dy * dy + dz * dz) ** 0.5
                if dist < best_dist:
                    best_dist = dist
                    best_dir.init(dx, dy, dz)
                    best_dir.calc_mag()
                    best_dir.unitize()
            plane.set_nearest_opponent(best_dir, best_dist)

    # ------------------------------------------------------------- stepping
    def update_population(self) -> None:
        self.set_targets()

        for plane in self.planes:
            if plane.pos_y < plane.ground_level:
                plane.init()

        if self.use_batch_update and self.config.network.kind == "feedforward":
            inputs = collect_observations(self.planes)
            batch_think(self.planes, inputs)
            for plane in self.planes:
                plane.dt = 1.0
                prev_roll, prev_pitch, prev_yaw = plane.roll, plane.pitch, plane.yaw
                plane.update_vectors()
                plane.calc_accel()
                plane.calc_vel()
                plane.calc_pos()
                plane.calc_angles()
                plane._roll_rate = plane.roll - prev_roll
                plane._pitch_rate = plane.pitch - prev_pitch
                plane._yaw_rate = plane.yaw - prev_yaw
                plane.update_qm()
                plane.set_fitness()
        else:
            for plane in self.planes:
                plane.update()

        if self._active_combat.enabled:
            self.combat.update_targeting(self.planes)
            for plane in self.planes:
                plane.update_guns()
            self.combat.resolve_hits(self.planes)
            self.combat.apply_hit_fitness(self.planes, self._active_fitness)

    def init_planes(self) -> None:
        for plane in self.planes:
            plane.init()
            plane._reset_ammo()
            if self._active_combat.enabled:
                self.combat.reset_plane_combat(plane)

    # ------------------------------------------------------------- evolution
    def tournament(self) -> Plane:
        return tournament_select(self.planes, self.rng, self.tournament_size)

    def reproduce(self, father: Plane, mother: Plane) -> Tuple[Plane, Plane]:
        child1 = self._new_plane()
        child2 = self._new_plane()

        father_wt = list(father.getbrain())
        mother_wt = list(mother.getbrain())

        if self.rng.randrange(100) < self.crossover_rate:
            crossover = self.rng.randrange(self.num_bases)
            length = self.rng.randrange(self.num_bases - crossover)
            for i in range(crossover, crossover + length):
                father_wt[i], mother_wt[i] = mother_wt[i], father_wt[i]

        child1.setbrain(father_wt)
        child1.mutatebrain(self.mutation_rate, self.max_mutation)
        child2.setbrain(mother_wt)
        child2.mutatebrain(self.mutation_rate, self.max_mutation)
        return child1, child2

    def _break_ties(self, ranked: List[Plane]) -> None:
        """Scatterbrain tied non-elite individuals only (preserves elite genomes)."""
        for i in range(len(ranked) - 1):
            if ranked[i].fitness == ranked[i + 1].fitness and i >= self.elitism:
                ranked[i].scatterbrain()
                ranked[i].add_fitness(-1)

    def _rank_planes(self) -> List[Plane]:
        """Rank planes using the configured selection strategy (non-destructive)."""
        planes = list(self.planes)
        gen = self.config.genetic

        if self.selection_mode == "novelty":
            descs = [p.behavior.finalize() for p in planes]
            nov = novelty_scores(descs, self._novelty_archive, gen.novelty_k)
            self._novelty_archive.extend(descs)
            if len(self._novelty_archive) > 500:
                self._novelty_archive = self._novelty_archive[-500:]
            score_map = {
                id(p): combined_score(p.fitness, n, gen.novelty_weight)
                for p, n in zip(planes, nov)
            }
            return sorted(planes, key=lambda p: score_map[id(p)], reverse=True)

        if self.selection_mode == "map_elites":
            for plane in planes:
                self._map_elites.insert(plane, plane.behavior.finalize())
            elites = self._map_elites.elites()
            if elites:
                return sorted(elites, key=lambda p: p.fitness, reverse=True)

        if self.selection_mode == "speciation":
            shared = list(planes)
            fitness_sharing(shared, gen.fitness_sharing_sigma)
            return sorted(shared, key=lambda p: p.fitness, reverse=True)

        return sorted(planes, key=lambda p: p.fitness, reverse=True)

    def next_generation(self) -> None:
        fitness_snapshot = [p.fitness for p in self.planes]
        behavior_planes = list(self.planes)

        ranked = self._rank_planes()
        if len(ranked) < self.elitism:
            ranked = sorted(self.planes, key=lambda p: p.fitness, reverse=True)
        self._break_ties(ranked)
        ranked.sort(key=lambda p: p.fitness, reverse=True)

        best = ranked[0].fitness
        worst = ranked[-1].fitness
        print(f"fitness[best] = {best:.6f}  fitness[worst] = {worst:.6f}")

        next_generation: List[Plane] = list(self.planes)

        for i in range(self.elitism):
            elite = self._new_plane()
            elite.setbrain(ranked[i].getbrain())
            next_generation[i] = elite

        for i in range(self.elitism // 2, len(self.planes) // 2):
            father = self.tournament()
            mother = self.tournament()
            child1, child2 = self.reproduce(father, mother)
            next_generation[i * 2] = child1
            next_generation[i * 2 + 1] = child2

        for plane in next_generation:
            plane.reset_fitness()

        self.planes = next_generation
        self.reset_targets()

        self._generation_count += 1
        self._apply_curriculum()

        behavior_stats = self._behavior_summary(behavior_planes)
        self.metrics.log_generation(
            self._generation_count,
            fitness_snapshot,
            behavior_stats,
            stage=self.curriculum.current_stage,
            selection=self.selection_mode,
        )
        best_weights = ranked[0].get_weights()
        self.metrics.save_best_genome(
            np.asarray(best_weights, dtype=np.float64),
            self.config.network,
            self._generation_count,
            best,
        )
        if self._active_combat.enabled:
            self.combat.maybe_add_to_hall_of_fame(np.asarray(best_weights, dtype=np.float64))
            self.combat.save_hall_of_fame(self.config.network)

    def _behavior_summary(self, planes: List[Plane]) -> dict:
        if not planes:
            return {}
        descs = [p.behavior.finalize() for p in planes]
        arr = np.asarray(descs, dtype=np.float64)
        return {
            "behavior_altitude_mean": float(np.mean(arr[:, 0])),
            "behavior_speed_mean": float(np.mean(arr[:, 1])),
            "behavior_curvature_mean": float(np.mean(arr[:, 2])),
            "behavior_on_target_mean": float(np.mean(arr[:, 3])),
            "behavior_aggression_mean": float(np.mean(arr[:, 4])),
            "behavior_diversity": float(
                np.mean([descriptor_distance(descs[0], d) for d in descs[1:]])
            ) if len(descs) > 1 else 0.0,
        }

    def reset_targets(self) -> None:
        for i in range(self.num_targets):
            self.targets[i].init()
            tx, ty, tz = self._rand_target()
            self.targets[i].pos_x = tx
            self.targets[i].pos_y = ty
            self.targets[i].pos_z = tz
            self.targets[i].yaw = self.rng.random() * 2 * 3.141592653589793
            self.targets[i].update_qm()

    def export_weights(self) -> np.ndarray:
        return np.array([plane.get_weights() for plane in self.planes], dtype=np.float64)
