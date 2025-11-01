# =======================================================================
# 🧩 Module: FindTasqlyPostgreSQL.cmake
# -----------------------------------------------------------------------
# 📌 Purpose : Unified PostgreSQL discovery for Tasqly (cross-platform)
# 🧱 Layer   : Build System / Modules
# 👤 Author  : Mohamed Ali
# 🗓️ Updated : 2025-10-23 (fixed scope warning)
# =======================================================================

# Try the official CMake module first
find_package(PostgreSQL QUIET)

# ✅ Official detection succeeded
if (PostgreSQL_FOUND)
    set(TASQLY_PG_INCLUDE_DIRS ${PostgreSQL_INCLUDE_DIRS})
    set(TASQLY_PG_LIBRARIES ${PostgreSQL_LIBRARIES})
    set(TASQLY_PG_AVAILABLE TRUE)
    message(STATUS "[Tasqly::PostgreSQL] Found official PostgreSQL at ${PostgreSQL_INCLUDE_DIRS}")
    return()
endif()

# ❌ Fallback mode (manual)
if (WIN32)
    set(_pg_root "C:/PostgreSQL/17")
    set(_pg_inc "${_pg_root}/include")
    set(_pg_lib "${_pg_root}/lib/libpq.lib")

    if (EXISTS "${_pg_inc}/libpq-fe.h")
        set(TASQLY_PG_AVAILABLE TRUE)
        set(TASQLY_PG_INCLUDE_DIRS ${_pg_inc})
        set(TASQLY_PG_LIBRARIES ${_pg_lib})
        message(STATUS "[Tasqly::PostgreSQL] Fallback applied: ${_pg_inc}")
        return()
    endif()
endif()

# ❌ Not found anywhere
message(WARNING "[Tasqly::PostgreSQL] PostgreSQL client library not found on this system")
set(TASQLY_PG_AVAILABLE FALSE)
