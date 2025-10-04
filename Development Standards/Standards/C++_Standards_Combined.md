# 🧱 C++ Standards — Tasqly

This document unifies all C++ coding templates used in the Tasqly project.  
It covers headers, implementation files, and benchmarks.  
Always follow these templates to ensure **consistency, maintainability, and quality**.

---

## 🔹 General Rules
- Always use `#pragma once` in header files.  
- Use **relative includes** only (never absolute paths).  
- Order of includes:
  1. Corresponding header file  
  2. Standard library headers  
  3. Third-party libraries  
  4. Project-specific includes  
- Do **not** use `using namespace` in header files (allowed in `.cpp` only if scoped).  
- Declare in `.h`, implement in `.cpp`, benchmark in `benchmarks/`.  
- Inline functions allowed only if **<= 3 lines of code**.  
- Document with Doxygen-style comments (`@brief, @param, @return`).  

---

## 1. Header File Template (`.h`)

```cpp
/*
 * 🧱 File: <FileName>.h
 * ---------------------
 * 📌 Purpose   : [Short sentence about what this file is for]
 * 🧱 Layer     : [Domain (Core) | Application (Use Cases) | Infrastructure | etc.]
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : <YYYY-MM-DD>
 * 🗓️ Updated   : <YYYY-MM-DD>
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable (update explicitly if Experimental/Deprecated)
 *
 * 🧠 Description:
 * - Briefly describe the responsibilities of this file.
 * - Mention how it fits into the overall architecture.
 *
 * 🔗 Depends On:
 *   - List all QML components, pages, or C++ classes that this file requires to function (write `None` if not applicable).
 * 🔗 Related   : [Cross-module related files]
 * ✅ Tested In : [Test file name if available]
 * ⚠️ Errors    : [Exceptions thrown / error handling contract]
 * ⚡ Performance: [Complexity notes / optimization hints]
 * 🔒 Security  : [Sanitization / sensitive handling if applicable, write `N/A` if not applicable]
 * 🔒 Thread Safety: [Notes if class is thread-safe or not]
 * 🧭 Benchmark Measurement Policy:
 *   - Must explicitly specify whether this benchmark uses **automatic timing** (RealTime/CPUTime) 
 *     or **manual timing** (UseManualTime + SetIterationTime).
 *   - Default → automatic timing for fairness and comparability.
 *   - Use manual_time only for I/O-heavy or noisy operations (e.g., logging rotation).
 * 🕒 Last Major Change: <YYYY-MM-DD>
 */

#pragma once

#include "domain/core/Task.h"   // example include

namespace tasqly::<layer>::<subsystem> {

class ExampleClass {
public:
    ExampleClass();
    ~ExampleClass();

    /// @brief Does something useful
    /// @param value input parameter
    /// @return result of computation
    int doSomething(int value);

private:
    int m_internalValue; ///< internal storage
};

} // namespace tasqly::<layer>::<subsystem>

```

---

## 2. Implementation File Template (`.cpp`)

```cpp
/*
 * 🧱 File: <FileName>.cpp
 * -----------------------
 * 📌 Purpose   : Implementation of <FileName>.h
 * 🧱 Layer     : Same as header
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : <YYYY-MM-DD>
 * 🗓️ Updated   : <YYYY-MM-DD>
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable (update explicitly if Experimental/Deprecated)
 *
 * 🧠 Description:
 * - Implements methods declared in the corresponding header file.
 * - Keep business logic clear and aligned with architecture.
 *
 * 🔗 Depends On:
 *   - List all QML components, pages, or C++ classes that this file requires to function (write `None` if not applicable).
 * 🔗 Related   : [Cross-module related files]
 * ✅ Tested In : [Test file name if available]
 * ⚠️ Errors    : [Exceptions thrown / error handling contract]
 * ⚡ Performance: [Complexity notes / optimization hints]
 * 🔒 Security  : [Sanitization / sensitive handling if applicable, write `N/A` if not applicable]
 * 🪵 Logging   : [When/how this file should log using LogManager]
 * 🕒 Last Major Change: <YYYY-MM-DD>
 */

#include "app/usecases/<FileName>.h"   // Always include the corresponding header first

// Standard library includes
#include <string>
#include <stdexcept>

// Optional: using namespace tasqly::<layer>::<subsystem>;

// Example method implementation
int tasqly::<layer>::<subsystem>::ExampleClass::doSomething(int value) {
    if (value < 0) {
        // Example error handling
        throw std::invalid_argument("value must be non-negative");
    }
    // Example logging (pseudo-code)
    // LogManager::info("Processing value:", value);

    return value * 2;
}
```

---

## 3. Benchmark Template (`.cpp`)

```cpp
/*
 * 🧱 File: benchmarks/<subsystem>/<BenchmarkName>.cpp
 * ----------------------------
 * 📌 Purpose   : [Short sentence about what this benchmark measures]
 * 🧱 Layer     : Benchmark (Performance Validation)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : <YYYY-MM-DD>
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable (update explicitly if Experimental/Deprecated)
 *
 * 🧠 Description:
 * - Describe the performance scenario being measured.
 * - Mention expected inputs, outputs, and stress conditions.
 *
 * 🔗 Depends On:
 *   - List all QML components, pages, or C++ classes that this benchmark requires (write `None` if not applicable).
 * 🔗 Related   : [Cross-module related files or ADRs]
 * ✅ Tested In : TasqlyBenchmarksRunner
 * ⚠️ Errors    : [Exceptions thrown / error handling contract]
 * ⚡ Performance: [Target latency/throughput numbers]
 * 🔒 Security  : [Sensitive handling if applicable, write `N/A` if not applicable]
 * 🧪 Test Coverage: [Datasets or scenarios covered: Empty/Medium/Stress, etc.]
 * 🕒 Last Major Change: <YYYY-MM-DD>
 */

#include <benchmark/benchmark.h>
#include <string>

// Example Benchmark — String Creation
static void BM_StringCreation(benchmark::State& state) {
    for (auto _ : state) {
        std::string empty_string;
        benchmark::DoNotOptimize(empty_string);
    }
}
BENCHMARK(BM_StringCreation);

// Example Benchmark — Task Processing
static void BM_TaskProcessing(benchmark::State& state) {
    for (auto _ : state) {
        // Simulate a Task operation
        int result = 42 * 2;
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_TaskProcessing);
```

---

## 🔗 References
- [PROJECT_QUALITY_POLICY.md](../PROJECT_QUALITY_POLICY.md)  
- [TESTING.md](../TESTING.md)  

---

## ✅ Maintenance
- Review this document every **Phase end**.  
- Update when new coding standards or tools are introduced.  
