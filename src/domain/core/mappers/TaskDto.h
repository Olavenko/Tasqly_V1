/*
 * 🧱 File: TaskDto.h
 * ------------------
 * 📌 Purpose   : Defines lightweight data transfer object (DTO) for Task entity.
 * 🧱 Layer     : Domain (Core) — DTO / Mappers
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-09
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * TaskDto is a plain data structure designed for serialization and persistence.
 * It mirrors the core Task entity but uses only primitive and STL types.
 * It ensures no dependency on Qt or DB-specific types.
 *
 * ⚙️ Usage:
 * - Used by TaskMapper to convert between Task <-> TaskDto.
 * - Used by repository implementations to interface with persistence layer.
 *
 * ✅ Properties:
 *   - id            : std::string (UUID string)
 *   - title         : std::string
 *   - notes         : optional<std::string>
 *   - status        : std::string ("Todo", "Doing", "Done")
 *   - priority      : std::string ("Low", "Normal", "High")
 *   - deadline      : optional<std::string> (ISO 8601)
 *   - createdAt     : std::string (ISO 8601)
 *   - updatedAt     : std::string (ISO 8601)
 */

#pragma once
#include <optional>
#include <string>

namespace tasqly::domain::core {

struct TaskDto
{
  std::string id;
  std::string title;
  std::optional<std::string> notes;
  std::string status;                  // e.g., "Todo"
  std::string priority;                // e.g., "Normal"
  std::optional<std::string> deadline; // ISO8601 string or nullopt
  std::string createdAt;               // ISO8601 format (e.g., 2025-10-09T14:00:00Z)
  std::string updatedAt;               // same as above
};

} // namespace tasqly::domain::core
