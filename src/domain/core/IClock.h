/*
 * 🧱 File: IClock.h
 * ---------------------
 * 📌 Purpose   : Abstraction for system clock access.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Defines an interface for retrieving the current time.
 * Allows deterministic testing with fake clocks.
 */

#pragma once

#include <QDateTime>

// ⏰ Provides current time access
class IClock
{
public:
  virtual ~IClock() = default;

  // 🕑 Get the current time
  virtual QDateTime now() = 0;
};
