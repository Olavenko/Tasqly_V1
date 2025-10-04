# ADR: LogManager Rotation Strategy

## Status
Accepted

## Context
In the Tasqly project, the `LogManager` handles file-based logging using rotation (renaming and creating new log files once a size/time threshold is reached). Benchmarks showed high variance in `BM_LogManager_Rotation` due to synchronous file I/O (`rename()`, `flush()`, `open()`), especially on Windows with MinGW where worker-threaded approaches caused instability and crashes.

Requirements for production-grade logging are:
- Non-blocking logging (no UI stutters).
- Safe persistence (no lost logs during rotation).
- Deterministic behavior (predictable rotation timing).
- Recoverability in case of crashes.

## Decision
We will redesign `LogManager::rotate()` to use **debounced + periodic rotation** instead of direct synchronous rotation or worker threads.

### Key Changes
1. **Flag-based Rotation**  
   - Any trigger (file size exceeded, time-based policy) sets a `rotationPending = true` flag instead of rotating immediately.

2. **Debounce Timer (250ms)**  
   - A `QTimer` (singleShot) ensures that multiple triggers in quick succession result in only one rotation.  
   - Example: 1000 logs in 1 second → one rotation event.

3. **Periodic Timer (5s)**  
   - A secondary `QTimer` checks every 5 seconds. If `rotationPending` is still true (rotation not yet executed), it forces a rotation.  
   - This guarantees eventual consistency and crash safety.

4. **Final Flush on Shutdown**  
   - In the destructor or `QCoreApplication::aboutToQuit`, if `rotationPending == true`, `doRotate()` is called once more.

5. **doRotate() Implementation**  
   - Contains the actual file rename + reopen logic.  
   - Called only by timers or final flush.

## Consequences
- ✅ **Production Performance**: Logging is non-blocking (critical path = set flag only).  
- ✅ **Safety**: Logs are eventually rotated, even if the app crashes.  
- ✅ **Compatibility**: No worker threads, so stable with MinGW.  
- ✅ **Benchmarks**: Variance reduced, since rotation no longer occurs inline with every log write.  
- ⚠️ **Slight Delay**: Rotation occurs within 250ms (debounce) or up to 5s (periodic). This is acceptable for production logging.

## Alternatives Considered
- **Worker Thread Rotation**: Rejected due to instability and crashes on MinGW.  
- **Immediate Rotation**: Causes high variance and blocking latency.  
- **Truncate Instead of Rename**: Fast, but discards log history (unacceptable for debugging).

## Decision Makers
- Mohamed Ali (Author)
- Tasqly AI Assistant (Reviewed)

## Date
2025-09-25
