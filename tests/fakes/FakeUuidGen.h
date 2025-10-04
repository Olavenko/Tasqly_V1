/*
 * 🧱 File: FakeUuidGen.h
 * ----------------------
 * 📌 Purpose   : Deterministic UUID generator for testing.
 * 🧱 Layer     : Test Utilities
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Implements IUuidGen by returning sequential deterministic IDs.
 */

#pragma once

#include "domain/core/IUuidGen.h"
#include <atomic>

class FakeUuidGen : public IUuidGen
{
public:
  FakeUuidGen()
      : m_counter(0)
  {}

  QString generate() override
  {
    int id = m_counter.fetch_add(1);
    return QString("fake-%1").arg(id);
  }

private:
  std::atomic<int> m_counter;
};
