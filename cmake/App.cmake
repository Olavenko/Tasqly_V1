# ---------------------------------------------------------------
# 🏗️ Application Target — appTasqly_V1
# ---------------------------------------------------------------
# 📌 Purpose : Define the main Tasqly application executable
# 🧱 Layer   : Application Entry Point
# 👤 Author  : Mohamed Ali
# 🗓️ Updated : 2025-09-18
#
# 🧠 Description:
# - Defines the main app executable target (appTasqly_V1)
# - Configures QML module and import paths
# - Links against tasqly_core and required Qt modules
# - Applies quality gates (warnings, sanitizers)
# - Sets platform-specific properties (macOS/Windows)
# - Defines installation rules for app deployment
# ---------------------------------------------------------------

# ---------------------------------------------------------------
# 🏗️ Application Executable
# ---------------------------------------------------------------
qt_add_executable(appTasqly_V1
    main.cpp
)

# ---------------------------------------------------------------
# 📦 QML Module Definition
# ---------------------------------------------------------------
list(APPEND QML_IMPORT_PATH ${CMAKE_SOURCE_DIR}/src/ui/qml)
set(QML_IMPORT_PATH ${QML_IMPORT_PATH} CACHE STRING "QML import paths" FORCE)

qt_add_qml_module(appTasqly_V1
    URI Tasqly_V1
    VERSION 1.0

    # 🎨 QML Core
    QML_FILES
        Main.qml

    # 🧩 QML Components — Core
        src/ui/qml/components/SmartFallback.qml
        src/ui/qml/components/SmartPageLoader.qml

    # 🧩 QML Components — Feedback
        src/ui/qml/components/feedback/Toast.qml

    # 🧩 QML Components — Navigation
        src/ui/qml/components/navigation/Sidebar.qml

    # 📄 QML Pages
        src/ui/qml/pages/DashboardPage.qml
        src/ui/qml/pages/SettingsPage.qml
        src/ui/qml/pages/TaskListPage.qml

    # 📦 Resources
    RESOURCES
        resources.qrc
)

# ---------------------------------------------------------------
# 📂 Include Directories
# ---------------------------------------------------------------
target_include_directories(appTasqly_V1 PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

# ---------------------------------------------------------------
# 🔗 Dependencies
# ---------------------------------------------------------------
target_link_libraries(appTasqly_V1
    PRIVATE
        tasqly_core
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
)

# ---------------------------------------------------------------
# 🛡️ Quality Gates
# ---------------------------------------------------------------
tasqly_apply_warnings(appTasqly_V1)
tasqly_apply_debug_sanitizers(appTasqly_V1)

# ---------------------------------------------------------------
# ⚙️ Target Properties
# ---------------------------------------------------------------
set_target_properties(appTasqly_V1 PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# ---------------------------------------------------------------
# 📦 Installation Rules
# ---------------------------------------------------------------
include(GNUInstallDirs)
install(TARGETS appTasqly_V1
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

message(STATUS "[App] appTasqly_V1 linked successfully with tasqly_core + Qt (Core/Gui/Qml/Quick)")
