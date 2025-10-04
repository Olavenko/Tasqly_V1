/*
 * 🧱 File: SystemClock.h
 * ----------------------
 * 📌 Purpose   : Production clock returning current system time.
 * 🧱 Layer     : Infrastructure (Utils)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Implements IClock using QDateTime::currentDateTime().
 */

#pragma once

#include "domain/core/IClock.h"
#include <QDateTime>

class SystemClock : public IClock
{
public:
  // 🕑 Get the current system time
  QDateTime now() override { return QDateTime::currentDateTime(); }
};
