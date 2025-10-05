# ---------------------------------------------------------------
# 📦 Core Static Library — tasqly_core
# ---------------------------------------------------------------
# 📌 Purpose : Provide core domain logic, app services, and infra utils
# 🧱 Layer   : Core (Domain + Application + Infrastructure)
# 👤 Author  : Mohamed Ali
# 🗓️ Updated : 2025-09-18
#
# 🧠 Description:
# - Encapsulates all core business logic, domain entities, and infra services
# - Shared by Application, Tests, and Benchmarks
# - Provides stable API surface for QML integration
# ---------------------------------------------------------------

# ---------------------------------------------------------------
# 🏗️ Static Library Definition
# ---------------------------------------------------------------
add_library(tasqly_core STATIC
    # -------------------------
    # 🏛️ Application Layer — Core
    # -------------------------
    src/app/core/AppContext.cpp
    src/app/core/AppContext.h

    # 🏛️ Application Layer — Errors
    src/app/errors/AppErrors.cpp
    src/app/errors/AppErrors.h

    # 🏛️ Application Layer — Loading
    src/app/loading/AppLoadingManager.cpp
    src/app/loading/AppLoadingManager.h

    # 🏛️ Application Layer — Logging
    src/app/logging/ErrorReporter.cpp
    src/app/logging/ErrorReporter.h
    src/app/logging/ILogManager.h
    src/app/logging/LogFacade.cpp
    src/app/logging/LogFacade.h

    # 🏛️ Application Layer — Navigation
    src/app/navigation/NavigationService.cpp
    src/app/navigation/NavigationService.h

    # 🏛️ Application Layer — Notifier
    src/app/notifier/Notifier.cpp
    src/app/notifier/Notifier.h

    # 🏛️ Application Layer — Settings
    src/app/settings/AppSettingsManager.cpp
    src/app/settings/AppSettingsManager.h
    src/app/settings/FeatureFlagsManager.cpp
    src/app/settings/FeatureFlagsManager.h

    # 🏛️ Application Layer — Theme
    src/app/theme/ThemeManager.cpp
    src/app/theme/ThemeManager.h

    # 🏛️ Application Layer — QML Facade
    src/app/qml/QmlSingletonProvider.cpp
    src/app/qml/QmlSingletonProvider.h

    # 🏛️ Application Layer — UseCases
    src/app/usecases/InitRepositories.cpp
    src/app/usecases/InitRepositories.h
    src/app/usecases/ListTasks.cpp
    src/app/usecases/ListTasks.h
    src/app/usecases/LoadUserSettings.cpp
    src/app/usecases/LoadUserSettings.h
    src/app/usecases/NavigateTo.cpp
    src/app/usecases/NavigateTo.h
    src/app/usecases/SaveUserSettings.cpp
    src/app/usecases/SaveUserSettings.h
    src/app/usecases/SwitchTheme.cpp
    src/app/usecases/SwitchTheme.h

    # -------------------------
    # 🧩 Domain Layer — Core Entities & Contracts
    # -------------------------
    src/domain/core/Error.h
    src/domain/core/Goal.h
    src/domain/core/IClock.h
    src/domain/core/IGoalRepository.h
    src/domain/core/ITaskRepository.h
    src/domain/core/IUuidGen.h
    src/domain/core/PageState.h
    src/domain/core/Result.h
    src/domain/core/Route.h
    src/domain/core/Task.h
    src/domain/core/ThemeMode.h

    # -------------------------
    # 🏗️ Infrastructure Layer — Logging
    # -------------------------
    src/infra/logging/LogManager.cpp
    src/infra/logging/LogManager.h

    # 🏗️ Infrastructure Layer — Persistence
    src/infra/persistence/InMemoryGoalRepository.cpp
    src/infra/persistence/InMemoryGoalRepository.h
    src/infra/persistence/InMemoryTaskRepository.cpp
    src/infra/persistence/InMemoryTaskRepository.h

    # 🏗️ Infrastructure Layer — Utils
    src/infra/utils/SystemClock.h
    src/infra/utils/SystemUuidGen.h
)

# ---------------------------------------------------------------
# 📂 Include Directories
# ---------------------------------------------------------------
# Expose the "src" folder to consumers of tasqly_core.
# Using PUBLIC ensures tests and app can include headers directly.
target_include_directories(tasqly_core PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

# ---------------------------------------------------------------
# 🔗 Dependencies
# ---------------------------------------------------------------
# Core library links against essential Qt modules.
# QuickControls2 is included here since AppSettings/Theme depend on it.
target_link_libraries(tasqly_core
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
)

# ---------------------------------------------------------------
# 🛡️ Quality Gates
# ---------------------------------------------------------------
tasqly_apply_warnings(tasqly_core)
tasqly_apply_debug_sanitizers(tasqly_core)

# ---------------------------------------------------------------
# 🧪 Test Utilities Library — tasqly_fakes
# ---------------------------------------------------------------
# 📌 Purpose : Provide fake implementations for testing and benchmarking
# 🧱 Layer   : Test Utilities (Fakes / Stubs / Mocks)
# 👤 Author  : Mohamed Ali
# 🗓️ Added   : 2025-10-05
#
# 🧠 Description:
# - Includes fake managers and utility classes used in Benchmarks and Tests.
# - Depends on tasqly_core to access interfaces and domain entities.
# - Prevents duplicate fake definitions across multiple test targets.
# ---------------------------------------------------------------

# ---------------------------------------------------------------
# 🏗️ Static Library Definition
# ---------------------------------------------------------------
add_library(tasqly_fakes STATIC
    tests/fakes/FakeAppSettingsManager.h
    tests/fakes/FakeSettingsStore.h
    tests/fakes/FakeClock.h
    tests/fakes/FakeUuidGen.h
    tests/fakes/LogManagerTestAccessor.h
)

# ---------------------------------------------------------------
# 📂 Include Directories
# ---------------------------------------------------------------
# Expose the "tests/fakes" folder for use by all test and benchmark targets.
target_include_directories(tasqly_fakes PUBLIC
    ${PROJECT_SOURCE_DIR}/tests/fakes
)

# ---------------------------------------------------------------
# 🔗 Dependencies
# ---------------------------------------------------------------
# tasqly_fakes depends on tasqly_core to reuse domain/application interfaces.
target_link_libraries(tasqly_fakes
    PUBLIC
        tasqly_core
        Qt6::Core
)

# ---------------------------------------------------------------
# 🛡️ Quality Gates
# ---------------------------------------------------------------
tasqly_apply_warnings(tasqly_fakes)
tasqly_apply_debug_sanitizers(tasqly_fakes)

# ---------------------------------------------------------------
# ⚙️ Target Properties
# ---------------------------------------------------------------
set_target_properties(tasqly_fakes PROPERTIES
    OUTPUT_NAME "tasqly_fakes"
    FOLDER "Tests/Fakes"
)
