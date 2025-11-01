/*
 * 🧱 File: DatabaseIntegrationFixture.cpp
 * ---------------------------------------
 * 📌 Purpose   : Definitions for static members of DatabaseIntegrationFixture.
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-11-01
 */

#include "tests/integration/common/DatabaseIntegrationFixture.h"

namespace tasqly::testing::infra {
PGconn* DatabaseIntegrationFixture::adminConn = nullptr;
PGconn* DatabaseIntegrationFixture::conn = nullptr;
} // namespace tasqly::testing::infra
