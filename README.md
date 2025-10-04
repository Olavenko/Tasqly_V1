# Tasqly V1

Tasqly is a cross‑platform C++/Qt application built with a modular architecture for managing tasks, goals, and planning.  
The project demonstrates clean layering and vertical slice design, making it both a learning tool and a production‑grade baseline.

## Architecture Overview
The codebase follows a layered structure:

- **Domain (`src/domain`)** – Core types such as `Error` and `Result`.
- **Application (`src/app`)** – Error handling, logging, feature‑flag management, and use cases.
- **Infrastructure (`src/infra`)** – Services like repositories, utilities, and logging backends.
- **UI (`src/ui`)** – QML components and the main window.
- **Tests (`tests`)** – Unit, integration, and fakes.

## Build Requirements
- CMake ≥ 3.25
- Ninja build system
- GCC or Clang on Linux, or MSYS2/MinGW on Windows
- Qt 6.9.2 development libraries

## Quick Start
```bash
cmake --preset=gcc-debug
cmake --build --preset=build-gcc-debug
ctest --preset=test-gcc-debug --output-on-failure
```

## Development Standards
- [Development Standards](Development%20Standards/) – Templates & Policies for all `.h` / `.cpp` files.

## Testing
See [Testing Guide](docs/TESTING.md) for details on running and structuring unit/integration tests.

## Further Documentation
- [Building Guide](docs/building.md)
- [Architecture Decision Records](docs/adr/Index-ADR.md)
- [UML Diagram Guidelines](docs/uml/Diagrams-Readme.md)
