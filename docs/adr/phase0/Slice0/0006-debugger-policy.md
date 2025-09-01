# ADR 0006: Debugger Policy

## Context
Developers need consistent debugging tools across platforms.  
Qt Creator integrates with GDB, LLDB, and MSVC debuggers.  
The policy should define the baseline debugger and supported alternatives.

## Decision
- **Windows (MinGW baseline):** use GDB (from MSYS2 toolchain).
- **Linux:** use GDB (baseline), LLDB (optional).
- **Windows (MSVC optional):** use MSVC debugger when building with MSVC toolchain.
- Document debugger usage in project wiki/README.

## Consequence
- Developers share a consistent debugging baseline (GDB).
- Alternative debuggers are supported but not mandatory.
- CI/CD pipelines remain independent of debugger choice.
