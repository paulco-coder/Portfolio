# Orbital Parameters Calculation

This small project contains a Python script to compute orbital parameters (semi-major axis, eccentricity, inclination, etc.) from a position vector and a velocity vector.

Important files
- `OrbitalParameterDetermination.ipynb`: Jupyter notebook, it explain the code and test it.
- `ArgparseOrbitalParameters.py`: File to run the program from a command line.

Requirements
- Python 3.x

Installation

(Optional) Create and activate a virtual environment, (no dependencies needed):

```bash
# Unix / macOS
python -m venv venv
source venv/bin/activate

# Windows (PowerShell)
python -m venv venv
venv\Scripts\Activate.ps1

# Windows (cmd)
venv\Scripts\activate.bat
```

Usage: *(More informations about the parameters and outputs are provided in the notebook.)*

The script `ArgparseOrbitalParameters.py` is a command-line tool and accepts the following arguments:

- `--position x y z`: components of the position vector (three floats) — required.
- `--velocity vx vy vz`: components of the velocity vector (three floats) — required.
- `--canonical`: optional flag; if present, the script uses canonical units (GM = 1). Otherwise GM = 398600.442 (default, in km^3 / s^2).

Examples

1) Standard run (km units):

```bash
python ArgparseOrbitalParameters.py --position 7000 0 0 --velocity 0 7.5 1
```

2) Canonical units run:

```bash
python ArgparseOrbitalParameters.py --canonical --position 1.023 1.076 1.011 --velocity 0.620 0.700 -0.250
```

Output

The script prints the following orbital elements:
- a: semi-major axis (in km or canonical units)
- e: eccentricity
- i: inclination (radians and degrees)
- Ω: right ascension of the ascending node (radians and degrees)
- ω: argument of periapsis (radians and degrees)
- θ: true anomaly (radians and degrees)

Notebook / Tests

`OrbitalParameterDetermination.ipynb` contains cells to test and visualize the calculations and explanations about the code.
