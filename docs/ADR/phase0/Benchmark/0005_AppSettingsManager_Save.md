# ADR 0005: AppSettingsManager Save Strategy

## Status
Accepted

## Context
The `AppSettingsManager` persists application settings using `QSettings` (INI backend). Initial benchmarks showed high variance and latency in benchmarks such as `BM_AppSettingsManager_Save`, `BM_AppSettingsManager_DebouncedSave`, and `BM_AppSettingsManager_CorruptFileRecovery`.

The root cause was synchronous calls to `QSettings::sync()` on every save, which performs file I/O and introduces unpredictable delays due to disk caching, antivirus hooks, and file system latency. Worker-threaded approaches were attempted but caused instability and crashes on MinGW.

Production requirements are:
- Non-blocking save operations (no UI freeze).
- Reliable persistence of settings.
- Crash safety (data not lost).
- Predictable performance.

## Decision
We redesigned the save mechanism to use **buffered writes + periodic sync + final flush**, instead of synchronous or worker-threaded persistence.

### Key Changes
1. **Buffered Writes**  
   - `setValue()` stores settings in memory only.  
   - `sync()` is not called immediately.

2. **Debounce Timer (250ms)**  
   - Ensures frequent changes (e.g., theme switching) do not spam the disk with writes.  
   - Restarts every time a change is made.

3. **Periodic Sync (5s)**  
   - A `QTimer` runs every 5 seconds to call `sync()`.  
   - Guarantees eventual persistence without blocking critical paths.

4. **Final Flush on Shutdown**  
   - In the destructor, or during `QCoreApplication::aboutToQuit`, `sync()` is called to persist any pending changes.

5. **Error Handling**  
   - If `QSettings` reports errors, a toast notification is emitted once per cooldown (3s).  
   - Prevents spamming the user with duplicate errors.

## Consequences
- ✅ **Production Performance**: Save operations are non-blocking and near-zero latency.  
- ✅ **Safety**: Changes are persisted periodically and on shutdown, minimizing data loss.  
- ✅ **Compatibility**: No worker threads, stable with MinGW.  
- ✅ **Benchmarks**: Significant improvement in variance; only unavoidable file I/O remains medium variance.  
- ⚠️ **Slight Delay**: Settings may take up to 5 seconds to fully flush to disk. This is acceptable in production use cases.

## Alternatives Considered
- **Worker Thread Persistence**: Rejected due to instability with MinGW.  
- **Immediate Sync on Every Save**: Caused latency and variance.  
- **No Periodic Sync**: Risk of data loss on crash.

## Decision Makers
- Mohamed Ali (Author)
- Tasqly AI Assistant (Reviewed)

## Date
2025-09-25
