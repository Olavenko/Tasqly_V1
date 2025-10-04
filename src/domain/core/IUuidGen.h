/*
 * 🧱 File: IUuidGen.h
 * ---------------------
 * 📌 Purpose   : Abstraction for UUID generation.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Defines an interface for generating unique identifiers.
 * Used by repositories and entities to assign deterministic IDs.
 */

#pragma once

#include <QString>

// ➕ Generates unique identifiers (UUIDs)
class IUuidGen
{
public:
  virtual ~IUuidGen() = default;

  // 🔑 Generate a new UUID as QString
  virtual QString generate() = 0;
};
