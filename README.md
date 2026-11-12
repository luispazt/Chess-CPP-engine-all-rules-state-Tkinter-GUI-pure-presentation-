# Chess Engine

C++ chess rules engine with a Tkinter GUI, bridged via pybind11. All game logic lives in C++; the Python side only renders state and forwards input.

## Project structure

```
chess/
├── cpp/
│   ├── include/chess/   # headers: Piece, Board, Game, Player, ...
│   ├── src/             # implementations + binding.cpp (pybind11)
│   └── tests/           # Catch2 tests (perft, game rules, pieces)
├── python/
│   ├── main.py          # entry point
│   ├── engine_client.py # wraps chess_core, JSON in/out
│   └── ui/              # Tkinter board, app, dialogs, theme
├── CMakeLists.txt
└── build/               # generated
```

## Prerequisites

- CMake >= 3.20
- C++20 compiler
- Python 3 with `pybind11` and `tkinter`

## Build

Linux

```bash
cmake -B build -Dpybind11_DIR=$(python -m pybind11 --cmakedir)
cmake --build build -j
```

Windows

```bash
cmake -S . -B build -G Ninja -Dpybind11_DIR="$(python -m pybind11 --cmakedir)"
cmake --build build -j
```

The `chess_core` extension module is placed directly in `python/`, so no install step is needed.

## Run

```bash
python python/main.py
```

## Tests

```bash
cmake --build build --target test
```

C++ tests use Catch2 (perft depth 1..4 = 20 / 400 / 8902 / 197281). Python tests use pytest.
