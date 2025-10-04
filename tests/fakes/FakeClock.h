/*
 * 🧱 File: FakeClock.h
 * --------------------
 * 📌 Purpose   : Deterministic clock for testing.
 * 🧱 Layer     : Test Utilities
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Implements IClock with manual control over time progression.
 * Useful for deterministic tests (e.g., createdAt, updatedAt).
 */

#pragma once

#include "domain/core/IClock.h"
#include <QDateTime>

class FakeClock : public IClock
{
public:
  explicit FakeClock(QDateTime start = QDateTime::currentDateTime())
      : m_now(start)
  {}

  // 🕑 Returns the current fake time
  QDateTime now() override { return m_now; }

  // ⏩ Advance time by milliseconds
  void advance(qint64 ms) { m_now = m_now.addMSecs(ms); }

private:
  QDateTime m_now;
};
