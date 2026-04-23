# SolarPILOT CoPylot API — Build Guide for Linux (Ubuntu)

![Platform](https://img.shields.io/badge/platform-Ubuntu%2024.04-orange)
![GCC](https://img.shields.io/badge/GCC-13.3-blue)
![Python](https://img.shields.io/badge/python-3.12-green)
![CMake](https://img.shields.io/badge/CMake-3.28+-lightgrey)

This guide documents how to compile the SolarPILOT CoPylot Python API (`solarpilot.so`) on Linux (tested on **Ubuntu 24.04** with **GCC 13.3**). The official build instructions at [NatLabRockies/SolarPILOT Wiki](https://github.com/NatLabRockies/SolarPILOT/wiki/build-linux) require some patches to work on modern Linux systems. This guide documents the fixes needed at least for Ubuntu 24.04.

The goal is to produce `solarpilot.so` and use it via `copylot.py` to call SolarPILOT programmatically from Python — for example, to dynamically recalculate heliostat flux maps with time-varying aiming points.

---

## System Requirements

- Ubuntu 20.04 or later (tested on 24.04)
- GCC 4.8.5 or later (tested with GCC 13.3)
- CMake 3.12 or later
- Internet connection (to clone repositories and download CPM.cmake)

---

## Step 1 — Install System Dependencies

```bash
sudo apt-get install git g++ build-essential cmake \
    libcurl4-openssl-dev libfontconfig1-dev
```

> `libfontconfig1-dev` is required by the `strace` executable in SolTrace. Without it, the build fails even though `strace` is not needed for the Python API.

---

## Step 2 — Create a Working Directory and Clone Repositories

All repositories must live inside the same parent directory (`spt_dev`).

```bash
mkdir ~/spt_dev && cd ~/spt_dev

# Clone NREL repositories
for repo in lk wex SolTrace SolarPILOT; do
    git clone https://github.com/nrel/$repo
done

# Clone SSC from mjwagner2 and switch to the required branch
git clone https://github.com/mjwagner2/ssc
cd ssc && git checkout solarpilot-develop && cd ..
```

---

## Step 3 — Set Required Environment Variable

SolarPILOT's build system uses `CORETRACEDIR` to find SolTrace headers (particularly `stapi.h`). Without this, the build fails with `fatal error: stapi.h: No such file or directory`.

```bash
export CORETRACEDIR=/home/$USER/spt_dev/SolTrace/coretrace
```

To make this permanent, add it to your `.bashrc`:

```bash
echo 'export CORETRACEDIR=$HOME/spt_dev/SolTrace/coretrace' >> ~/.bashrc
source ~/.bashrc
```

---

## Step 4 — Configure nlopt

```bash
cd ~/spt_dev/ssc/nlopt
./configure
cd ~/spt_dev
```

---

## Step 5 — Download CPM.cmake

SolTrace uses CPM.cmake as a package manager for CMake, but does not bundle it. It must be downloaded manually.

```bash
mkdir -p ~/spt_dev/cmake
wget -O ~/spt_dev/cmake/CPM.cmake \
    https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake
```

---

## Step 6 — Apply Source Code Patches

Four patches are required to fix incompatibilities with modern GCC versions.

### Patch 1 — Inject CPM.cmake into SolTrace's simulation_data

`simulation_data/CMakeLists.txt` calls `CPMAddPackage()` without ever loading CPM.cmake first:

```bash
# Backup original
cp ~/spt_dev/SolTrace/coretrace/simulation_data/CMakeLists.txt \
   ~/spt_dev/SolTrace/coretrace/simulation_data/CMakeLists.txt.bak

# Inject include on line 2 (after the Project() line)
sed -i "1a include(\"$HOME/spt_dev/cmake/CPM.cmake\")" \
    ~/spt_dev/SolTrace/coretrace/simulation_data/CMakeLists.txt

# Verify
head -3 ~/spt_dev/SolTrace/coretrace/simulation_data/CMakeLists.txt
```

Expected output:
```
Project(simdata)
include("/home/<user>/spt_dev/cmake/CPM.cmake")
include_directories(.)
```

### Patch 2 — Remove stray semicolon from preprocessor macro in interop.cpp

GCC 13 rejects a semicolon at the end of a `#define` expression:

```bash
sed -i 's/#define HELIO_INTERCEPT false;/#define HELIO_INTERCEPT false/' \
    ~/spt_dev/ssc/solarpilot/interop.cpp

# Verify
grep "HELIO_INTERCEPT" ~/spt_dev/ssc/solarpilot/interop.cpp
```

Expected output:
```
#define HELIO_INTERCEPT false
```

### Patch 3 — Remove -Werror from ssc/shared build flags

GCC 13 promotes a `malloc` size warning to an error due to `-Werror`, which breaks the `shared` library build:

```bash
sed -i 's/-Werror //' ~/spt_dev/ssc/shared/CMakeLists.txt

# Verify the flag is gone
grep "Werror" ~/spt_dev/ssc/shared/CMakeLists.txt
```

Expected output: no results (the flag has been removed).

---

## Step 7 — Create the Root CMakeLists.txt

Create `~/spt_dev/CMakeLists.txt` with the following content. Note that the project name must be `solarpilot_api` (not `solarpilot_ui`) to build the shared library instead of the GUI:

```cmake
option(SAM_SKIP_TOOLS "Skips the sdktool and tcsconsole builds" ON)
cmake_minimum_required(VERSION 3.12)

Project(solarpilot_api)

if(${CMAKE_PROJECT_NAME} STREQUAL "solarpilot_api")
    option(COPILOT_API "Builds library for Copilot API" ON)
endif()

add_subdirectory(ssc)
add_subdirectory(SolTrace/coretrace)
```

---

## Step 8 — Build

```bash
cd ~/spt_dev
mkdir build && cd build

cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_COPILOT_API=ON \
    -DSOLTRACE_BUILD_CORETRACE=ON

make -j$(nproc)
```

The build takes several minutes. If successful, the last lines should include:

```
[100%] Built target solarpilot
[100%] Built target native_runner
[100%] Built target strace
```

---

## Step 9 — Collect API Files

Create a single directory with all files needed to use the Python API:

```bash
mkdir -p ~/spt_dev/api

# Python API files from SolarPILOT (copied first)
cp ~/spt_dev/SolarPILOT/deploy/api/*.py ~/spt_dev/api/

# The Linux-compiled shared library (must be copied last to overwrite the Windows version)
cp ~/spt_dev/build/ssc/solarpilot/solarpilot.so ~/spt_dev/api/
```

> **Warning:** `SolarPILOT/deploy/api/` also contains a precompiled `solarpilot.so` built for Windows. The commands above copy the `.py` files first and then overwrite the Windows `.so` with the Linux-compiled version. Always run the commands in the order shown.

---

## Step 10 — Python Dependencies

Install the required Python packages:

```bash
pip install pysolar --break-system-packages
```

> Additional packages may be required depending on your script (e.g. `numpy`, `scipy`, `pandas`). Install them as needed with `pip install <package> --break-system-packages`.

---

## Step 11 — Use the API in Python

In your Python script, add the API directory to the path before importing `CoPylot`:

```python
import sys
import os
import platform

# Add the SolarPILOT API directory to the Python path
if platform.system() == "Linux":
    solar_pilot_path = os.path.join(os.path.expanduser("~"), "spt_dev", "api")
elif platform.system() == "Windows":
    solar_pilot_path = os.path.join("C:", os.sep, "Program Files", "SolarPILOT", "1.5.2", "api")

sys.path.append(solar_pilot_path)
os.chdir(solar_pilot_path)

from copylot import CoPylot
cp = CoPylot()
```

> **Important:** avoid hardcoded Windows paths like `C:/Users/...` in the rest of your script. Use `os.path.join()` and relative paths instead for cross-platform compatibility.

---

## Step 12 — Run the Minimal Example

A minimal working example is provided in `minimal_example_Linux.py`. It demonstrates basic field generation, flux map simulation, and dynamic aiming point modification — the core pattern for time-varying heliostat control studies.

```bash
python3 minimal_example_Linux.py
```

The script requires a TMY weather file. By default it looks for the example climate file bundled with SolarPILOT:

```
~/spt_dev/SolarPILOT/deploy/climate_files/USA CA Daggett (TMY2).csv
```

If your weather file is in a different location, update the `WEATHER_FILE` variable at the top of the script. On success, a figure named `solarpilot_example_output.png` is saved to the current directory.

---

## Verify the Build

To check that `solarpilot.so` has no missing dependencies:

```bash
ldd ~/spt_dev/api/solarpilot.so | grep "not found"
```

If no output appears, the library is complete and ready to use.

---

## Troubleshooting

### `Unknown CMake command "CPMAddPackage"`

CPM.cmake was not downloaded or not injected into `simulation_data/CMakeLists.txt`. Make sure Step 5 completed successfully and that Patch 1 from Step 6 was applied before running cmake.

### `fatal error: stapi.h: No such file or directory`

The environment variable `CORETRACEDIR` is not set or points to the wrong path. Verify with:

```bash
echo $CORETRACEDIR
ls $CORETRACEDIR/stapi.h
```

If the variable is empty, re-run the `export` command from Step 3 and make sure it is also saved in `.bashrc`.

### `cannot find -lcoretrace: No such file or directory`

The cmake flag `-DSOLTRACE_BUILD_CORETRACE=ON` was not passed, so `coretrace` was not compiled as a static library. Re-run cmake with both flags as shown in Step 8.

### `cannot find -lfontconfig: No such file or directory`

The `libfontconfig1-dev` package is missing. Install it and re-run `make` — there is no need to re-run cmake:

```bash
sudo apt-get install libfontconfig1-dev
cd ~/spt_dev/build
make -j$(nproc)
```

### `ModuleNotFoundError: No module named 'pysoltrace'`

`pysoltrace.py` was not copied to the API directory. Re-run the copy commands from Step 9, making sure to copy the Linux `.so` last.

### `ModuleNotFoundError: No module named 'pysolar'`

Install the missing Python package:

```bash
pip install pysolar --break-system-packages
```

---

## Summary of Patches Applied

| File | Problem | Fix |
|------|---------|-----|
| `SolTrace/coretrace/simulation_data/CMakeLists.txt` | `CPMAddPackage` called without loading CPM.cmake | Added `include(CPM.cmake)` on line 2 |
| `ssc/solarpilot/interop.cpp` | `#define HELIO_INTERCEPT false;` — stray `;` in macro | Removed the trailing semicolon |
| `ssc/shared/CMakeLists.txt` | `-Werror` causes `malloc` size warning to be treated as error | Removed `-Werror` from CXX flags |
| `~/spt_dev/CMakeLists.txt` (root) | Project name `solarpilot_ui` builds GUI instead of API | Changed to `solarpilot_api` |

---

## CMake Flags Reference

| Flag | Purpose |
|------|---------|
| `-DBUILD_COPILOT_API=ON` | Compiles `solarpilot.so` instead of the static `solarpilot_core.a` |
| `-DSOLTRACE_BUILD_CORETRACE=ON` | Compiles `coretrace` static library required by SolarPILOT |
| `-DCMAKE_BUILD_TYPE=Release` | Optimized build (use `Debug` for development) |

---

## Tested Configuration

| Component | Version |
|-----------|---------|
| OS | Ubuntu 24.04 |
| GCC | 13.3.0 |
| CMake | 3.28+ |
| Python | 3.12 |
| CPM.cmake | 0.42.1 |

---

## References

- [NatLabRockies/SolarPILOT — Build Linux Wiki](https://github.com/NatLabRockies/SolarPILOT/wiki/build-linux)
- [NREL/SolarPILOT](https://github.com/nrel/SolarPILOT)
- [mjwagner2/ssc — solarpilot-develop branch](https://github.com/mjwagner2/ssc/tree/solarpilot-develop)
- [NREL/HALOS — flux_model.py](https://github.com/NREL/HALOS/tree/develop)
- [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake)
