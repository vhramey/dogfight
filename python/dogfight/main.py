"""Entry point for the Python port of dogfight."""

from __future__ import annotations

import argparse
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

import numpy as np

from .config import load_config
from .io.population import maybe_load_initial_weights, save_population
from .sim.gene_pool import GenePool

_DEFAULT_TEXTURES_DIR = Path(__file__).resolve().parent.parent / "assets" / "textures"


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Dogfight: neuro-evolution flight simulator")
    parser.add_argument("--config", type=str, default=None, help="Path to a YAML config file")
    parser.add_argument("--planes", type=int, default=None, help="Override number of planes")
    parser.add_argument("--targets", type=int, default=None, help="Override number of targets")
    parser.add_argument("--generation-time", type=int, default=None, help="Frames per generation")
    parser.add_argument("--seed", type=int, default=None, help="Random seed for reproducible runs")
    parser.add_argument("--weights", type=str, default=None, help="Initial population .npz to load")
    parser.add_argument("--save", type=str, default=None, help="Override the population save path")
    parser.add_argument(
        "--headless-smoke",
        type=int,
        default=None,
        metavar="N",
        help="Run N generations with no window (for development/testing)",
    )
    return parser.parse_args()


def _apply_overrides(config, args) -> None:
    if args.planes is not None:
        config.simulation.num_planes = args.planes
    if args.targets is not None:
        config.simulation.num_targets = args.targets
    if args.generation_time is not None:
        config.simulation.generation_time = args.generation_time
    if args.seed is not None:
        config.genetic.seed = args.seed
    if args.weights is not None:
        config.population.initial_weights = args.weights
    if args.save is not None:
        config.population.save_path = args.save


def _run_headless(pool: GenePool, generations: int, generation_time: int) -> None:
    pool.init_planes()
    pool.reset_targets()
    for _ in range(generations):
        for _ in range(generation_time):
            pool.update_population()
        pool.next_generation()
        pool.init_planes()


def _run_headless_parallel(config_path: str | None, generations: int, workers: int) -> GenePool:
    """Evaluate independent sub-populations in parallel, then merge best weights."""
    config = load_config(config_path)
    sub_planes = max(4, config.simulation.num_planes // workers)
    config.simulation.num_planes = sub_planes
    config.simulation.headless_workers = 0

    def _worker(seed_offset: int) -> np.ndarray:
        cfg = load_config(config_path)
        cfg.simulation.num_planes = sub_planes
        cfg.genetic.seed = (cfg.genetic.seed or 0) + seed_offset
        pool = GenePool(cfg)
        _run_headless(pool, generations, cfg.simulation.generation_time)
        ranked = sorted(pool.planes, key=lambda p: p.fitness, reverse=True)
        return np.asarray(ranked[0].get_weights(), dtype=np.float64)

    with ProcessPoolExecutor(max_workers=workers) as executor:
        bests = list(executor.map(_worker, range(workers)))

    main_config = load_config(config_path)
    initial = np.tile(bests[0], (main_config.simulation.num_planes, 1))
    for i, weights in enumerate(bests[1:]):
        if i + 1 < len(initial):
            initial[i + 1] = weights
    pool = GenePool(main_config, initial)
    return pool


def main() -> None:
    args = _parse_args()
    config = load_config(args.config)
    _apply_overrides(config, args)

    if args.headless_smoke is not None and config.simulation.headless_workers > 0:
        pool = _run_headless_parallel(args.config, args.headless_smoke, config.simulation.headless_workers)
        save_population(
            config.population.save_path,
            pool.export_weights(),
            config.network,
            args.headless_smoke,
        )
        print(f"saved population to {config.population.save_path}")
        return

    initial_weights = maybe_load_initial_weights(config.population.initial_weights, config.network)
    pool = GenePool(config, initial_weights)

    if args.headless_smoke is not None:
        _run_headless(pool, args.headless_smoke, config.simulation.generation_time)
        save_population(
            config.population.save_path,
            pool.export_weights(),
            config.network,
            args.headless_smoke,
        )
        print(f"saved population to {config.population.save_path}")
        return

    from .render.app import App

    textures_dir = (
        Path(config.render.textures_dir) if config.render.textures_dir else _DEFAULT_TEXTURES_DIR
    )
    App(config, pool, textures_dir).run()


if __name__ == "__main__":
    main()
