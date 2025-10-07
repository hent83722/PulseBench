#!/usr/bin/env bash
set -euo pipefail

# Launcher script for PulseBench GUI
# Usage: ./run-gui.sh [-- args]

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

VENV="$ROOT/.venv"
if [ ! -d "$VENV" ]; then
    echo ".venv not found — creating virtual environment..."
    python -m venv .venv
    source "$VENV/bin/activate"
    echo "Installing requirements..."
    pip install -U pip setuptools wheel
    pip install -r gui/requirements.txt
else
    # shellcheck disable=SC1091
    source "$VENV/bin/activate"
fi

# Run the GUI with any additional args passed through
exec python gui/main.py "$@"
