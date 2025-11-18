# Setup Qt

Installs Qt with [`jurplel/install-qt-action`](https://github.com/jurplel/install-qt-action) and exports `QT_ROOT`, `CMAKE_PREFIX_PATH`, `QT_PLUGIN_PATH`, and `QML2_IMPORT_PATH`.

## Inputs
- `version` — Qt release (default `6.9.2`).
- `host`, `target`, `arch` — forwarded directly to the upstream action.
- `dir` — installation root inside the workspace (default `./Qt`).
- `modules` — optional modules list (`qtshadertools qtimageformats …`).
- `aqtversion` — pin for the underlying `aqtinstall` version (default `==3.1.0` to avoid current upstream regressions).

## Outputs
- `qt-root` — installation directory returned by Aqt.
- `qt-bin` — architecture-specific `bin` directory that is also appended to `PATH`.
- `cmake-prefix` — path you can pass to CMake/Presets.
- `qml-path` — convenience path for QML imports.
