# =======================================================================
# 🧩 Module: FindTasqlyPostgreSQL.cmake
# -----------------------------------------------------------------------
# 📌 Purpose : Unified PostgreSQL discovery for Tasqly (cross-platform)
# 🧱 Layer   : Build System / Modules
# 👤 Author  : Mohamed Ali
# 🗓️ Updated : 2025-11-05 (smart detection, unified logic)
# =======================================================================

# ---------------------------------------------------------------
# 🧠 Step 1: Try the official CMake module
# ---------------------------------------------------------------
find_package(PostgreSQL QUIET)

if (PostgreSQL_FOUND)
    set(TASQLY_PG_AVAILABLE TRUE)
    set(TASQLY_PG_INCLUDE_DIRS ${PostgreSQL_INCLUDE_DIRS})
    set(TASQLY_PG_LIBRARIES ${PostgreSQL_LIBRARIES})
    message(STATUS "[Tasqly::PostgreSQL] Found official PostgreSQL ${PostgreSQL_VERSION_STRING}")
    message(STATUS "[Tasqly::PostgreSQL] Includes: ${PostgreSQL_INCLUDE_DIRS}")
    message(STATUS "[Tasqly::PostgreSQL] Libraries: ${PostgreSQL_LIBRARIES}")
    return()
endif()

# ---------------------------------------------------------------
# 🧩 Step 2: Manual fallback for Windows local dev environments
# ---------------------------------------------------------------
if (WIN32)
    set(_pg_roots
        "C:/Program Files/PostgreSQL/17"
        "C:/PostgreSQL/17"
    )

    foreach(_pg_root IN LISTS _pg_roots)
        set(_pg_inc "${_pg_root}/include")
        set(_pg_lib "${_pg_root}/lib/libpq.lib")

        if (EXISTS "${_pg_inc}/libpq-fe.h")
            set(TASQLY_PG_AVAILABLE TRUE)
            set(TASQLY_PG_INCLUDE_DIRS ${_pg_inc})
            set(TASQLY_PG_LIBRARIES ${_pg_lib})
            message(STATUS "[Tasqly::PostgreSQL] Fallback applied: ${_pg_root}")
            return()
        endif()
    endforeach()
endif()

# ---------------------------------------------------------------
# 🧱 Step 3: Smart handling for CI (no PostgreSQL)
# ---------------------------------------------------------------
# CI Windows environments usually don't have PostgreSQL installed,
# so we gracefully disable DB integration instead of failing build.
set(TASQLY_PG_AVAILABLE FALSE)
set(TASQLY_PG_INCLUDE_DIRS "")
set(TASQLY_PG_LIBRARIES "")

if (WIN32)
    message(WARNING "[Tasqly::PostgreSQL] PostgreSQL not found — DB tests disabled (likely CI).")
else()
    message(WARNING "[Tasqly::PostgreSQL] PostgreSQL client library not found on this system.")
endif()
