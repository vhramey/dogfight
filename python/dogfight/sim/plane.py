"""Port of the C++ ``Plane`` class.

This is the heart of the simulation: flight physics, neural-network control,
per-frame fitness accumulation, and the (visual-only) guns. The orientation
update, which originally relied on live OpenGL matrix calls, is reproduced with
pure numeric helpers in :mod:`dogfight.mathlib.quaternion`.
"""

from __future__ import annotations

import math
import random
from typing import List, Optional

from ..brain.factory import create_brain
from ..config import CombatConfig, FitnessConfig, NetworkConfig, PhysicsConfig
from ..mathlib.quaternion import (
    PI,
    PIOVER180,
    PIUNDER180,
    Quaternion,
    identity_matrix,
    mat_mult,
    rotation_matrix,
)
from ..mathlib.vector3 import MyVector, dot_product
from .actions import map_outputs
from .behavior import BehaviorDescriptor
from .fitness import accumulate_fitness
from .gun import Gun
from .sensors import build_observations, validate_observation_count

# Legacy module-level constants (tests import these).
GRAVITY = 0.15
LIFT_CONSTANT = 0.8
KITE_CONSTANT = 0.8
DRAG_CONSTANT = 0.8
FLAP_CONSTANT = 3.0
MAX_FLAP_ANGLE = 60.0
MAX_YAW_FLAP_ANGLE = 10.0
MOMENT_INERTIA_ROLL = 0.05
MOMENT_INERTIA_YAW = 0.02
MOMENT_INERTIA_PITCH = 0.05
PITCH_CORRECT = 0.2
YAW_CORRECT = 0.1
MAX_THRUST = 0.3
CRASH_PENALTY = 1000.0
DEFAULT_GROUND_LEVEL = -50.0

NUM_SHOTS = 100


class Plane:
    def __init__(
        self,
        net_config: NetworkConfig,
        rng: random.Random,
        ground_level: float = DEFAULT_GROUND_LEVEL,
        fitness_config: Optional[FitnessConfig] = None,
        physics_config: Optional[PhysicsConfig] = None,
    ):
        self.rng = rng
        self.ground_level = ground_level
        self.net_config = net_config
        self.fitness_config = fitness_config or FitnessConfig()
        self.physics_config = physics_config or PhysicsConfig()

        self.shooting = False
        self.fitness = 0.0
        self.lockcount = 0
        self.targetedcount = 0
        self.behavior = BehaviorDescriptor()

        self.health = 100.0
        self.alive = True
        self.hits_scored = 0
        self.kills = 0
        self.damage_taken = 0.0
        self.evasion_frames = 0

        self.m16: List[float] = identity_matrix()
        self.rotation = Quaternion()

        self.numshots = NUM_SHOTS
        self.shotindex = 0
        self.shotdelay = 0
        self.shots: List[Gun] = [Gun() for _ in range(self.numshots)]
        self.propvision = 1.0
        self.proprotation = 0.0
        self.fired_this_frame = False

        self.dt = 0.0
        self._roll_rate = 0.0
        self._pitch_rate = 0.0
        self._yaw_rate = 0.0

        self.net_accel = MyVector(0.0, 0.0, 0.0)
        self.vel = MyVector(0.0, 0.0, 0.0)

        self.pos_x = 0.0
        self.pos_y = 0.0
        self.pos_z = 0.0

        self.heading = MyVector(1.0, 0.0, 0.0)
        self.perp_y = MyVector(0.0, 1.0, 0.0)
        self.perp_z = MyVector(0.0, 0.0, 1.0)

        self.pitch = 0.0
        self.roll = 0.0
        self.yaw = 0.0

        self.fl_flap = 0.0
        self.rp_flap = 0.0
        self.ry_flap = 0.0
        self.wflap_dist = 2.0
        self.tflap_dist = 2.0

        self.thrust_accel = 0.0
        self.mass = 170.0

        self.target = MyVector(0.0, 0.0, 0.0)
        self.global_target = MyVector(0.0, 0.0, 0.0)
        self.targeted = MyVector(0.0, 0.0, 0.0)
        self.distance_to_target = 10000.0
        self.nearest_opponent_dir = MyVector(0.0, 0.0, 0.0)
        self.nearest_opponent_distance = 10000.0

        self.brain = create_brain(net_config, rng)
        self.brain_kind = net_config.kind
        self.weights: List[float] = self.brain.get_weights()

        self._combat_config = CombatConfig()
        self._combat_enabled = False
        self._learned_shooting = net_config.use_learned_shooting
        self.ammo_remaining = 0
        self.burst_cooldown_remaining = 0
        self._burst_awaiting_hit = False
        self._burst_got_hit = False
        self._burst_miss_frames_remaining = 0

    # ------------------------------------------------------------------ brain
    def scatterbrain(self) -> None:
        count = self.brain.num_weights()
        self.weights = [(self.rng.randrange(4000) - 2000) / 2000.0 for _ in range(count)]
        self.brain.put_weights(self.weights)

    def mutatebrain(self, mutation_rate: int, max_mutation: float) -> None:
        weights = self.brain.get_weights()
        for i in range(len(weights)):
            if self.rng.randrange(100) < mutation_rate:
                weights[i] += ((self.rng.randrange(4001) - 2000) / 2000.0) * max_mutation
        self.brain.put_weights(weights)
        self.weights = weights

    def setbrain(self, weights: List[float]) -> None:
        self.weights = list(weights)
        self.brain.put_weights(self.weights)

    def getbrain(self) -> List[float]:
        return self.weights

    def get_weights(self) -> List[float]:
        return self.brain.get_weights()

    def num_weights(self) -> int:
        return self.brain.num_weights()

    def reset_brain_state(self) -> None:
        self.brain.reset_state()

    # --------------------------------------------------------------- lifecycle
    def init(self) -> None:
        self.rotation.reset()
        self.m16 = identity_matrix()
        self.dt = 0.0
        self.net_accel.init(0.0, 0.0, 0.0)
        self.thrust_accel = self.physics_config.max_thrust / 2.0
        self.vel.init(0.0, 0.0, 0.0)
        self.pitch = 0.0
        self.roll = 0.0
        self.yaw = 0.0
        self.pos_x = 0.0
        self.pos_y = 0.0
        self.pos_z = 0.0
        self.fl_flap = 0.0
        self.rp_flap = 0.0
        self.ry_flap = 0.0
        self.yaw = -PI
        self.update_qm()
        self.roll = PI / 2.0
        self.update_qm()
        self.lockcount = 0
        self.targetedcount = 0
        self.behavior.reset()
        self.reset_brain_state()
        self._roll_rate = 0.0
        self._pitch_rate = 0.0
        self._yaw_rate = 0.0

    def _reset_ammo(self) -> None:
        self.ammo_remaining = self._combat_config.magazine_size
        self.burst_cooldown_remaining = 0
        self.shotdelay = 0
        self._burst_awaiting_hit = False
        self._burst_got_hit = False
        self._burst_miss_frames_remaining = 0

    def _can_fire(self) -> bool:
        cfg = self._combat_config
        if cfg.magazine_size <= 0:
            return True
        return (
            self.ammo_remaining >= cfg.rounds_per_burst
            and self.burst_cooldown_remaining <= 0
        )

    def build_observations(self) -> List[float]:
        up = MyVector(0.0, 1.0, 0.0)
        localup = self.localize(up)
        local_vel = self.localize(self.vel.copy())
        return build_observations(
            target_local=self.target,
            local_up=localup,
            distance=self.distance_to_target,
            sensors=self.net_config.sensors,
            vel=local_vel,
            pos_y=self.pos_y,
            ground_level=self.ground_level,
            targeted_local=self.targeted,
            roll_rate=self._roll_rate,
            pitch_rate=self._pitch_rate,
            yaw_rate=self._yaw_rate,
            nearest_opponent_dir=self.nearest_opponent_dir,
            nearest_opponent_distance=self.nearest_opponent_distance,
            ammo_remaining=self.ammo_remaining,
            magazine_size=self._combat_config.magazine_size,
            burst_cooldown_remaining=self.burst_cooldown_remaining,
            burst_cooldown_frames=self._combat_config.burst_cooldown_frames,
        )

    def apply_brain_outputs(self, outputs: List[float]) -> None:
        fl, rp, ry, thrust, learned_shoot = map_outputs(
            outputs, self.net_config, self.physics_config
        )
        self.fl_flap = fl
        self.rp_flap = rp
        self.ry_flap = ry
        self.thrust_accel = thrust
        if self._learned_shooting:
            self.shooting = learned_shoot
        else:
            self.shooting = dot_product(self.global_target, self.heading) > self.fitness_config.lock_threshold

    def think_with_inputs(self, inputs: List[float]) -> None:
        validate_observation_count(self.net_config, inputs)
        outputs = self.brain.update(inputs)
        self.apply_brain_outputs(outputs)

    # ------------------------------------------------------------------ think
    def think(self) -> None:
        inputs = self.build_observations()
        self.think_with_inputs(inputs)

    # ------------------------------------------------------------------ update
    def update(self) -> None:
        if self.pos_y < self.ground_level:
            self.init()
        self.dt = 1.0
        prev_roll, prev_pitch, prev_yaw = self.roll, self.pitch, self.yaw
        self.think()
        self.update_vectors()
        self.calc_accel()
        self.calc_vel()
        self.calc_pos()
        self.calc_angles()
        self._roll_rate = self.roll - prev_roll
        self._pitch_rate = self.pitch - prev_pitch
        self._yaw_rate = self.yaw - prev_yaw
        self.update_qm()
        self.set_fitness()

    def calc_accel(self) -> None:
        phys = self.physics_config
        na = self.net_accel
        na.x = 0.0
        na.y = 0.0
        na.z = 0.0
        na.mag = 0.0

        na.x = self.thrust_accel * self.heading.x * self.dt
        na.y = self.thrust_accel * self.heading.y * self.dt
        na.z = self.thrust_accel * self.heading.z * self.dt

        na.y -= phys.gravity * self.dt

        na.x -= phys.drag_constant * self.vel.x * self.vel.mag * self.dt
        na.y -= phys.drag_constant * self.vel.y * self.vel.mag * self.dt
        na.z -= phys.drag_constant * self.vel.z * self.vel.mag * self.dt

        lift_mag = phys.lift_constant * self.vel.mag * abs(dot_product(self.vel, self.heading) * self.dt)
        lift_mag -= phys.kite_constant * self.vel.mag * dot_product(self.vel, self.perp_y)
        na.x += self.perp_y.x * lift_mag
        na.y += self.perp_y.y * lift_mag
        na.z += self.perp_y.z * lift_mag

        na.calc_mag()
        na.unitize_pres_mag()

    def calc_vel(self) -> None:
        velmag = self.vel.mag
        accmag = self.net_accel.mag
        self.vel.x = self.vel.x * velmag + self.net_accel.x * accmag
        self.vel.y = self.vel.y * velmag + self.net_accel.y * accmag
        self.vel.z = self.vel.z * velmag + self.net_accel.z * accmag
        self.vel.calc_mag()
        self.vel.unitize_pres_mag()

    def calc_pos(self) -> None:
        mag = self.vel.mag
        self.pos_x += self.vel.x * mag
        self.pos_y += self.vel.y * mag
        self.pos_z += self.vel.z * mag

    def calc_angles(self) -> None:
        phys = self.physics_config
        proj = dot_product(self.vel, self.heading)
        speed = self.vel.mag

        f_perp_fl = speed * proj * phys.flap_constant * math.sin(-self.fl_flap * PIOVER180)
        fy_fl = f_perp_fl * math.cos(self.fl_flap * PIOVER180)
        torque = 2 * self.wflap_dist * (-fy_fl)
        self.roll += MOMENT_INERTIA_ROLL * torque

        f_perp_ry = speed * proj * phys.flap_constant * math.sin(-self.ry_flap * PIOVER180)
        fz_ry = f_perp_ry * math.cos(self.ry_flap * PIOVER180)
        torque = self.tflap_dist * fz_ry
        self.yaw += MOMENT_INERTIA_YAW * torque

        f_perp_rp = speed * proj * phys.flap_constant * math.sin(-self.rp_flap * PIOVER180)
        fy_rp = f_perp_rp * math.cos(self.rp_flap * PIOVER180)
        torque = self.tflap_dist * fy_rp
        self.pitch -= MOMENT_INERTIA_PITCH * torque

        yc = dot_product(self.vel, self.perp_z) * speed * YAW_CORRECT
        self.yaw -= yc
        pc = dot_product(self.vel, self.perp_y) * speed * PITCH_CORRECT
        self.pitch += pc

    def update_vectors(self) -> None:
        xx, yy, zz = self.rotation.get_direction_vector()
        self.perp_y.init(xx, yy, zz)

        temprot = self.rotation.copy()
        temprot.post_mult(Quaternion(PI / 2.0, 0.0, 0.0))
        xx, yy, zz = temprot.get_direction_vector()
        self.perp_z.init(xx, yy, zz)

        temprot.set_from(self.rotation)
        temprot.post_mult(Quaternion(0.0, PI / 2.0, 0.0))
        xx, yy, zz = temprot.get_direction_vector()
        self.heading.init(xx, yy, zz)

    def update_qm(self) -> None:
        rroll = self.roll
        ppitch = self.pitch
        yyaw = self.yaw
        if -0.0005 < rroll < 0.0005:
            rroll = 0.0
        if -0.0005 < ppitch < 0.0005:
            ppitch = 0.0
        if -0.0005 < yyaw < 0.0005:
            yyaw = 0.0

        increment = Quaternion(-rroll, -ppitch, -yyaw)
        self.rotation.post_mult(increment)

        ax, ay, az, arot = increment.get_axis_angle()
        rot = -arot * PIUNDER180
        r = rotation_matrix(rot, ax, ay, az)
        self.m16 = mat_mult(self.m16, r)

        self.pitch = 0.0
        self.roll = 0.0
        self.yaw = 0.0

    # ----------------------------------------------------------------- targets
    def localize(self, t: MyVector) -> MyVector:
        m = self.m16
        out = MyVector(0.0, 0.0, 0.0)
        out.x = m[0] * t.x + m[1] * t.y + m[2] * t.z + m[3]
        out.y = m[4] * t.x + m[5] * t.y + m[6] * t.z + m[7]
        out.z = m[8] * t.x + m[9] * t.y + m[10] * t.z + m[11]
        out.calc_mag()
        out.unitize()
        return out

    def set_target(self, t: MyVector) -> None:
        self.global_target = t.copy()
        self.global_target.calc_mag()
        self.global_target.unitize()
        self.target = self.localize(t)

    def set_targeted(self, t: MyVector) -> None:
        self.targeted = t.copy()

    def set_distance(self, d: float) -> None:
        self.distance_to_target = d

    def set_nearest_opponent(self, direction: MyVector, distance: float) -> None:
        self.nearest_opponent_dir = direction.copy()
        self.nearest_opponent_distance = distance

    def configure_combat(self, combat_config: CombatConfig) -> None:
        self._combat_config = combat_config
        self._combat_enabled = combat_config.enabled
        self._reset_ammo()

    def note_burst_hit(self) -> None:
        self._burst_got_hit = True

    # ----------------------------------------------------------------- fitness
    def set_fitness(self) -> None:
        from ..config import CombatConfig

        combat_cfg = CombatConfig(enabled=self._combat_enabled)
        delta, self.lockcount = accumulate_fitness(
            fitness_cfg=self.fitness_config,
            combat_cfg=combat_cfg,
            global_target=self.global_target,
            vel=self.vel,
            heading=self.heading,
            perp_y=self.perp_y,
            pos_y=self.pos_y,
            ground_level=self.ground_level,
            lockcount=self.lockcount,
            health=self.health,
            hits_scored=0,
            kills=0,
            damage_taken=self.damage_taken,
            evasion_frames=self.evasion_frames,
        )
        self.fitness += delta

        on_target = dot_product(self.global_target, self.heading) > self.fitness_config.lock_threshold
        self.behavior.record_frame(
            pos_y=self.pos_y,
            speed=self.vel.mag,
            heading=(self.heading.x, self.heading.y, self.heading.z),
            on_target=on_target,
            shots_fired=1 if self.fired_this_frame else 0,
            damage_dealt=0.0,
        )

    def add_fitness(self, d: float) -> None:
        self.fitness += d

    def reset_fitness(self) -> None:
        self.fitness = 0.0
        self.behavior.reset()

    # ------------------------------------------------------------------- guns
    def update_guns(self) -> None:
        """Advance and (re)fire the visual guns. Mirrors the non-GL parts of
        ``Plane::shoot``; the renderer reads the resulting bullet state."""
        cfg = self._combat_config

        if self.burst_cooldown_remaining > 0:
            self.burst_cooldown_remaining -= 1

        if self._burst_miss_frames_remaining > 0:
            self._burst_miss_frames_remaining -= 1
            if self._burst_miss_frames_remaining == 0 and self._burst_awaiting_hit:
                if not self._burst_got_hit and cfg.miss_check_frames > 0:
                    self.fitness -= self.fitness_config.miss_penalty
                self._burst_awaiting_hit = False

        self.fired_this_frame = False
        if self.shooting and self.alive and self._can_fire():
            self.shotdelay += 1
            if self.shotdelay > cfg.burst_frames_between_shots:
                self.shotdelay = 0
                self.fired_this_frame = True
                rounds = cfg.rounds_per_burst
                if cfg.magazine_size > 0:
                    self.ammo_remaining = max(0, self.ammo_remaining - rounds)
                    self.burst_cooldown_remaining = cfg.burst_cooldown_frames
                    if self.fitness_config.shot_cost > 0:
                        self.fitness -= self.fitness_config.shot_cost * rounds
                    self._burst_awaiting_hit = True
                    self._burst_got_hit = False
                    self._burst_miss_frames_remaining = cfg.miss_check_frames

                radius = 0.35
                forward = 0.25
                down = 0.05

                lx = self.pos_x - self.perp_z.x * radius + self.heading.x * forward - self.perp_y.x * down
                ly = self.pos_y - self.perp_z.y * radius + self.heading.y * forward - self.perp_y.y * down
                lz = self.pos_z - self.perp_z.z * radius + self.heading.z * forward - self.perp_y.z * down

                rx = self.pos_x + self.perp_z.x * radius + self.heading.x * forward - self.perp_y.x * down
                ry = self.pos_y + self.perp_z.y * radius + self.heading.y * forward - self.perp_y.y * down
                rz = self.pos_z + self.perp_z.z * radius + self.heading.z * forward - self.perp_y.z * down

                self.shots[self.shotindex].fire(lx, ly, lz, self.heading)
                self.shotindex = (self.shotindex + 1) % self.numshots
                self.shots[self.shotindex].fire(rx, ry, rz, self.heading)
                self.shotindex = (self.shotindex + 1) % self.numshots

        self.proprotation += 2
        self.propvision = 0.5 if self.propvision == 1.0 else 1.0

        for shot in self.shots:
            shot.update()
