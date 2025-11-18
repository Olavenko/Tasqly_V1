# Setup Toolchain

Composite action that normalizes compiler setup across Linux and Windows runners.

## Inputs
| name | description |
| --- | --- |
| `compiler` | Compiler family (`gcc`, `clang`, `msvc`, `mingw`). |
| `needs-ninja` | Install Ninja on the host (default `true`). |
| `enable-sanitizers` | Adds sanitizer runtimes when supported. |
| `toolchain-file` | Optional CMake toolchain file to export. |

## Outputs
| name | description |
| --- | --- |
| `cc` / `cxx` | Exported compiler executables. |
| `generator` | Currently always `Ninja`. |
| `toolchain-file` | Echo of the supplied toolchain. |
| `cache-key-fragment` | `${runner.os}-${compiler}` for cache reuse. |

The action also writes `CC`, `CXX`, `CMAKE_GENERATOR`, and `CMAKE_TOOLCHAIN_FILE` to `$GITHUB_ENV`, and exposes `/mingw64/bin` for MinGW builds.
