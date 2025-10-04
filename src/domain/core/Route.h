/*
 * 🧱 File: Route.h
 * ----------------
 * 📌 Purpose   : Represents a navigation route with validation metadata.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-05
 *
 * 🧠 Description:
 * Defines the `Route` value type used by NavigationService and UI loaders.
 * Includes identifier, creation timestamp, and basic validation helpers.
 */

#pragma once

#include <QDateTime>
#include <QString>

// ➡️ Represents a navigation target (id + timestamp)
struct Route
{
  QString id;          // 🆔 Unique identifier for the route
  QDateTime timestamp; // ⏱️ Creation time

  // ✅ Checks if the route has valid metadata
  bool isValid() const { return !id.isEmpty() && timestamp.isValid(); }

  // 📋 Utility: formatted debug string
  QString toString() const
  {
    return QString("Route{id='%1', timestamp='%2'}").arg(id, timestamp.toString(Qt::ISODate));
  }
};
