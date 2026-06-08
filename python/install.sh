#!/usr/bin/env bash
# Install dogfight without requiring a standalone `pip` command on PATH.
set -euo pipefail

cd "$(dirname "$0")"

PYTHON="${PYTHON:-python3}"

if ! command -v "$PYTHON" >/dev/null 2>&1; then
  echo "error: $PYTHON not found. Install Python 3.9+ first." >&2
  exit 1
fi

echo "Using: $($PYTHON --version)"

# Bootstrap pip into this Python if the module is missing.
if ! "$PYTHON" -m pip --version >/dev/null 2>&1; then
  echo "pip module not found; bootstrapping with ensurepip..."
  "$PYTHON" -m ensurepip --upgrade
fi

if [ ! -d .venv ]; then
  echo "Creating virtual environment..."
  "$PYTHON" -m venv .venv
fi

# shellcheck disable=SC1091
source .venv/bin/activate

echo "Installing dependencies..."
python -m pip install --upgrade pip
python -m pip install -e .

echo
echo "Done. Activate the environment, then run:"
echo "  source .venv/bin/activate"
echo "  python -m dogfight"
