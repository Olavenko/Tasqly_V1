# 🚀 Tasqly V1

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++20](https://img.shields.io/badge/C++-20-blue.svg)](https://en.cppreference.com/)
[![Qt](https://img.shields.io/badge/Qt-6.9.2-green.svg)](https://www.qt.io/)

Tasqly is a cross-platform C++/Qt application for managing tasks and goals with a modular, layered architecture. Built with clean separation of concerns, it demonstrates professional software engineering practices including domain-driven design, comprehensive testing, and production-grade infrastructure.

**Current Status:** Phase 1 - Slice 2 (PostgreSQL persistence with InMemory fallback)

## 📋 Table of Contents

- [🏗️ Architecture Overview](#-architecture-overview)
- [✨ Implemented Subsystems](#-implemented-subsystems)
- [🚀 Quick Start](#-quick-start)
- [🔧 Build Requirements](#-build-requirements)
- [🧪 Testing](#-testing)
- [⚡ Performance](#-performance-targets)
- [🗄️ Database Support](#%EF%B8%8F-database-support)
- [📚 Documentation](#-documentation)
- [🛠️ Development Tools](#%EF%B8%8F-development-tools)
- [📦 Project Structure](#-project-structure)
- [🤝 Contributing](#-contributing)
- [📄 License](#-license)

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

## 🚀 Quick Start

### Prerequisites
- **CMake** ≥ 3.25
- **Ninja** build system
- **C++20** compatible compiler
- **Qt 6.9.2** development libraries
- **Git** (for version control)

### Build & Run
```bash
# Clone the repository
git clone https://github.com/yourusername/Tasqly_V1.git
cd Tasqly_V1

# Configure and build (Linux example)
cmake --preset=gcc-debug
cmake --build --preset=build-gcc-debug

# Run the application
./build/gcc-debug/Tasqly
```

## 🔧 Build Requirements

### Platform-Specific Requirements

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

## 🛠️ Development Tools

### Version Control
- **Git** for version control
- **.gitignore** - Comprehensive ignore rules for build artifacts, IDE files, and system files
- **.gitattributes** - Ensures consistent line endings and file handling across platforms

### Code Quality
- **Cppcheck** - Static analysis
- **Clang-Format** - Code formatting
- **Clang-Tidy** - Static analysis and style checking
- **Doxygen** - Documentation generation

### Scripts
- `scripts/run_benchmarks.py` - Run performance benchmarks
- `scripts/coverage_report_updated.py` - Generate coverage reports
- `scripts/fix_includes.py` - Fix include paths
- `scripts/gen_adr_index.py` - Generate ADR index
- `scripts/update_uml_tree.py` - Update UML documentation

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

The project maintains comprehensive documentation following industry best practices:

### Core Documentation
- [Building Guide](docs/building.md) – Detailed build instructions for all platforms
- [Development Standards](Development%20Standards/) – Comprehensive coding standards and guidelines
  - [Policies](Development%20Standards/Policies/) – Development and maintenance policies
  - [Standards](Development%20Standards/Standards/) – Coding and documentation standards
  - [Templates](Development%20Standards/Templates/) – File and documentation templates
- [Subsystems Overview](Subsystems_Unified.md) – Complete subsystem documentation

### Architecture & Design
- [Architecture Decision Records](docs/ADR/) – ADRs for all major technical decisions
  - [Phase 0](docs/ADR/phase0/) – Foundational architecture decisions
  - [Phase 1](docs/ADR/phase1/) – Persistence layer decisions
- [Knowledge Base](docs/KnowledgeBase/) – Technical documentation and guides
- [UML Diagrams](docs/uml/) – System architecture and design documentation
- [Checklists](docs/checklists/) – Development and review checklists

### Testing
- [Testing Guide](Development%20Standards/TESTING.md) – Testing standards and practices

---

## 🤝 Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

Please make sure to update tests as appropriate and follow the [Code of Conduct](CODE_OF_CONDUCT.md).

##  Project Structure

```
Tasqly_V1/
├── .github/                  # GitHub configurations and workflows
├── build/                    # Build directory (ignored in git)
├── cmake/                    # CMake modules and toolchains
│   └── toolchains/           # Cross-compilation toolchains
├── Development Standards/    # Development guidelines
│   ├── Policies/             # Development policies
│   ├── Standards/            # Coding standards
│   └── Templates/            # File templates
├── docs/                     # Comprehensive documentation
│   ├── ADR/                  # Architecture Decision Records
│   │   ├── phase0/           # Foundational decisions
│   │   └── phase1/           # Persistence layer decisions
│   ├── KnowledgeBase/        # Technical documentation
│   ├── checklists/           # Development checklists
│   └── uml/                  # System architecture diagrams
├── externals/                # External dependencies
│   └── _deps/                # Fetched dependencies
├── performance/              # Performance testing
│   └── logs/                 # Performance logs
├── reports/                  # Generated reports
│   ├── benchmarks/           # Benchmark results
│   ├── coverage/             # Test coverage reports
│   └── tests/                # Test execution reports
├── scripts/                  # Development scripts
│   └── treegen/              # Directory tree generator
├── src/                      # Source code
│   ├── app/                  # Application layer
│   │   ├── core/             # Core application services
│   │   ├── errors/           # Error handling
│   │   ├── loading/          # Page loading system
│   │   ├── logging/          # Logging system
│   │   ├── navigation/       # Navigation service
│   │   ├── notifier/         # Toast notifications
│   │   ├── qml/              # QML integration
│   │   ├── settings/         # Application settings
│   │   ├── theme/            # Theme management
│   │   └── usecases/         # Business logic use cases
│   ├── domain/               # Domain layer
│   │   └── core/             # Core domain types and entities
│   │       ├── contracts/    # Interfaces and contracts
│   │       ├── entities/     # Core business objects (Task, Goal)
│   │       ├── errors/       # Domain errors
│   │       └── mappers/      # Domain mappers
│   ├── infra/                # Infrastructure layer
│   │   ├── db/               # Database configuration
│   │   ├── factories/        # Repository factory pattern
│   │   ├── logging/          # Logging backends
│   │   ├── migrations/       # Database migrations
│   │   ├── persistence/      # Repository implementations
│   │   ├── runtime/          # Runtime adapters
│   │   └── utils/            # Utility functions
│   └── ui/                   # User interface
│       ├── components/       # Reusable QML components
│       └── pages/            # Application screens
├── tests/                    # Test suite
│   ├── common/               # Test utilities
│   ├── fakes/                # Test doubles
│   ├── integration/          # Integration tests
│   ├── qml/                  # QML component tests
│   └── unit/                 # Unit tests
├── benchmarks/               # Performance benchmarks
├── .clang-format             # Code formatting rules
├── .clang-tidy               # Static analysis config
├── .gitattributes            # Git attributes
├── .gitignore                # Git ignore rules
├── CMakeLists.txt            # Root CMake configuration
├── CMakePresets.json         # Build presets
├── Main.qml                  # Main QML entry point
├── main.cpp                  # C++ entry point
├── resources.qrc             # Qt resource file
└── README.md                 # This file
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

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

Built with Qt 6.9.2, CMake, GoogleTest, and Google Benchmark.
