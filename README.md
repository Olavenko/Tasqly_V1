# Tasqly V1

Tasqly is a cross‑platform C++/Qt application built with a modular architecture for managing tasks, goals, and planning.  
The project demonstrates clean layering and vertical slice design, making it both a learning tool and a production‑grade baseline.

**Current Status:** Phase 1 - Slice 2 (PostgreSQL persistence with fallback to InMemory)

---

## 🏗️ Architecture Overview

The codebase follows a layered architecture:

- **Domain (`src/domain`)** – Core types such as `Error`, `Result<T,Error>`, entities (`Task`, `Goal`), and domain contracts.
- **Application (`src/app`)** – Error handling, logging, feature‑flag management, use cases, and application services.
- **Infrastructure (`src/infra`)** – Services like repositories (InMemory & PostgreSQL), utilities, logging backends, and runtime adapters.
- **UI (`src/ui`)** – QML components, pages (Dashboard, TaskList, Settings), and navigation components.
- **Tests (`tests`)** – Unit tests, integration tests, QML tests, and fakes for testing.

---

## ✨ Implemented Subsystems

### Phase 0 - Core Infrastructure
- ✅ **Logging System** – LogManager with masking, levels, and performance logging
- ✅ **Error Handling** – `Error` type and `Result<T,Error>` pattern with ErrorReporter
- ✅ **Settings System** – AppSettingsManager with QML facade
- ✅ **Theme System** – ThemeManager with light/dark mode support
- ✅ **Loading System** – SmartPageLoader with fallback states
- ✅ **Navigation System** – NavigationService with route guards
- ✅ **Notifier System** – Toast notifications and fallback alerts
- ✅ **Feature Flags** – Centralized feature flag management

### Phase 1 - Persistence
- ✅ **InMemory Repositories** – Task and Goal repositories (Phase 0 - Slice 4)
- ✅ **PostgreSQL Integration** – Database persistence with automatic fallback to InMemory (Phase 1 - Slice 2)
- ✅ **Repository Factory** – Smart factory pattern for repository selection

---

## 📋 Build Requirements

### Minimum Requirements
- **CMake** ≥ 3.25
- **Ninja** build system
- **C++20** compatible compiler
- **Qt 6.9.2** development libraries

### Platform-Specific
- **Linux:** GCC or Clang
- **Windows:** 
  - MSYS2/MinGW64 (recommended for cross-platform compatibility)
  - Visual Studio 2019/2022 with MSVC (optional)
- **PostgreSQL** (optional, for database persistence)

---

## 🚀 Quick Start

### Linux (GCC)
```bash
cmake --preset=gcc-debug
cmake --build --preset=build-gcc-debug
ctest --preset=test-gcc-debug --output-on-failure
```

### Windows (MinGW)
```bash
# In MSYS2 MinGW64 shell
cmake --preset=mingw-debug
cmake --build --preset=build-mingw-debug
ctest --preset=test-mingw-debug --output-on-failure
```

### Windows (MSVC)
```bash
cmake --preset=msvc-debug
cmake --build --preset=build-msvc-debug
ctest --preset=test-msvc-debug --output-on-failure
```

---

## 🔧 Build Presets

The project supports multiple build configurations:

### Debug Presets
- `gcc-debug` / `clang-debug` – Linux development
- `mingw-debug` – Windows MinGW development
- `msvc-debug` – Windows MSVC development

### Release Presets
- `mingw-release` / `msvc-release` – Optimized builds

### Special Presets
- `*-tests-reports-*` – Build with test coverage reports
- `*-benchmarks-*` – Build with Google Benchmark integration

See `CMakePresets.json` for the complete list of available presets.

---

## 🧪 Testing

The project includes comprehensive testing infrastructure:

- **Unit Tests** – Domain, application, and infrastructure layer tests
- **Integration Tests** – End-to-end flow testing
- **QML Tests** – UI component testing
- **Benchmarks** – Performance validation using Google Benchmark

### Running Tests
```bash
# Run all tests
ctest --preset=test-gcc-debug --output-on-failure

# Run with coverage reports
cmake --preset=mingw-tests-reports-debug
cmake --build --preset=build-mingw-tests-reports-debug
ctest --preset=test-mingw-debug
```

See [Testing Guide](Development%20Standards/TESTING.md) for details on test structure and best practices.

---

## ⚡ Performance Targets

The project maintains strict performance KPIs:

- **Log write:** <2ms
- **Initialization:** <50ms
- **Theme switch:** <100ms
- **Navigation:** <120ms
- **Repository query (≤1k items):** <5ms

Performance is continuously validated through benchmarks and CI/CD pipelines.

---

## 🗄️ Database Support

### PostgreSQL (Optional)
The application supports PostgreSQL for persistent storage. If PostgreSQL is not available, it automatically falls back to InMemory repositories.

**Setup:**
- Install PostgreSQL client libraries
- Set `CMAKE_PREFIX_PATH` or `QT_PREFIX` environment variable
- The build system will automatically detect and configure PostgreSQL support

**Fallback Behavior:**
- If PostgreSQL is unavailable, the application uses InMemory repositories
- No build errors occur if PostgreSQL is missing

---

## 📚 Documentation

### Core Documentation
- [Building Guide](docs/building.md) – Detailed build instructions for all platforms
- [Development Standards](Development%20Standards/) – Templates & Policies for all `.h` / `.cpp` files
- [Subsystems Overview](Subsystems_Unified.md) – Complete subsystem documentation

### Architecture & Design
- [Architecture Decision Records](docs/ADR/Index-ADR.md) – ADRs for all major decisions
- [UML Diagram Guidelines](docs/uml/Diagrams-Readme.md) – UML documentation standards

### Testing
- [Testing Guide](Development%20Standards/TESTING.md) – Testing standards and practices

---

## 🛠️ Development Tools

### Scripts
- `scripts/run_benchmarks.py` – Run performance benchmarks
- `scripts/coverage_report_updated.py` – Generate coverage reports
- `scripts/fix_includes.py` – Fix include paths
- `scripts/gen_adr_index.py` – Generate ADR index
- `scripts/update_uml_tree.py` – Update UML documentation

### Code Quality
- **Cppcheck** – Static analysis (see `cppcheck-suppressions.txt`)
- **Compiler Warnings** – Strict warnings enabled (`-Wall -Wextra -Wpedantic` / `/W4`)
- **Sanitizers** – AddressSanitizer and UndefinedBehaviorSanitizer in Debug builds (Linux/GCC/Clang)

---

## 📦 Project Structure

```
Tasqly_V1/
├── src/              # Source code
│   ├── domain/       # Domain layer (entities, contracts, core types)
│   ├── app/          # Application layer (use cases, services, facades)
│   ├── infra/        # Infrastructure layer (repositories, logging, utilities)
│   └── ui/           # UI layer (QML components and pages)
├── tests/            # Test suite
│   ├── unit/         # Unit tests
│   ├── integration/  # Integration tests
│   ├── qml/          # QML tests
│   └── fakes/        # Fake implementations for testing
├── benchmarks/       # Performance benchmarks
├── cmake/            # CMake modules and toolchains
├── docs/             # Documentation (ADRs, guides, UML)
├── scripts/          # Development and build scripts
└── reports/          # Generated reports (coverage, benchmarks)
```

---

## 🔮 Roadmap

### Phase 2.5+ (Planned)
- Loader UX upgrades
- Performance logging enhancements
- Offline cache support
- Database connection pooling
- Redis caching integration

### Phase 3+ (Future)
- Reminders system
- Scheduling features
- Habits tracking
- Timer functionality
- Mood tracking

---

## 📄 License

[Add license information if applicable]

---

## 👥 Contributing

[Add contributing guidelines if applicable]

---

## 🙏 Acknowledgments

Built with Qt 6.9.2, CMake, GoogleTest, and Google Benchmark.
