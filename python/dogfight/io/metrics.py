"""Per-generation metrics logging and best-genome checkpointing."""

from __future__ import annotations

import csv
import json
from dataclasses import asdict
from pathlib import Path
from typing import List, Optional

import numpy as np

from ..config import MetricsConfig, NetworkConfig
from .population import save_population


class MetricsLogger:
    def __init__(self, config: MetricsConfig):
        self.config = config
        self._header_written = Path(config.log_path).exists() and Path(config.log_path).stat().st_size > 0

    def log_generation(
        self,
        generation: int,
        fitness_values: List[float],
        behavior_stats: Optional[dict] = None,
        stage: str = "chase",
        selection: str = "tournament",
    ) -> dict:
        if not self.config.enabled:
            return {}

        arr = np.asarray(fitness_values, dtype=np.float64)
        record = {
            "generation": generation,
            "fitness_best": float(np.max(arr)),
            "fitness_worst": float(np.min(arr)),
            "fitness_mean": float(np.mean(arr)),
            "fitness_median": float(np.median(arr)),
            "fitness_std": float(np.std(arr)),
            "stage": stage,
            "selection": selection,
        }
        if behavior_stats:
            record.update(behavior_stats)

        self._write_csv(record)
        self._write_json(record)
        return record

    def _write_csv(self, record: dict) -> None:
        path = Path(self.config.log_path)
        path.parent.mkdir(parents=True, exist_ok=True)
        with open(path, "a", newline="") as handle:
            writer = csv.DictWriter(handle, fieldnames=list(record.keys()))
            if not self._header_written:
                writer.writeheader()
                self._header_written = True
            writer.writerow(record)

    def _write_json(self, record: dict) -> None:
        if not self.config.log_json_path:
            return
        path = Path(self.config.log_json_path)
        path.parent.mkdir(parents=True, exist_ok=True)
        with open(path, "a") as handle:
            handle.write(json.dumps(record) + "\n")

    def save_best_genome(
        self,
        weights: np.ndarray,
        net: NetworkConfig,
        generation: int,
        fitness: float,
    ) -> None:
        if not self.config.enabled:
            return
        path = Path(self.config.best_genome_path)
        save_population(path, weights.reshape(1, -1), net, generation)
        meta_path = path.with_suffix(".meta.json")
        meta_path.write_text(
            json.dumps({"generation": generation, "fitness": fitness}, indent=2)
        )
