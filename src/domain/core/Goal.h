/*
 * 🧱 File: Goal.h
 * ---------------------
 * 📌 Purpose   : Domain entity representing a goal grouping multiple tasks.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Represents a Goal entity that groups tasks together, with
 * unique identifier, metadata, and timestamps.
 */

#pragma once

#include <optional>
#include <QDateTime>
#include <QString>

namespace tasqly::domain::core {

// 📝 Goal entity
struct Goal
{
  QString id;
  QString title;
  QString description;
  QDateTime createdAt;
  QDateTime updatedAt;
  std::optional<QDateTime> dueDate;

  // 🆕 Default constructor
  Goal() = default;

  // 🆕 Parameterized constructor
  Goal(QString id,
       QString title,
       QString description,
       QDateTime createdAt,
       QDateTime updatedAt,
       std::optional<QDateTime> dueDate = std::nullopt)
      : id(std::move(id))
      , title(std::move(title))
      , description(std::move(description))
      , createdAt(std::move(createdAt))
      , updatedAt(std::move(updatedAt))
      , dueDate(std::move(dueDate))
  {}

  // 🆕 Copy constructor / assignment (default)
  Goal(const Goal&) = default;
  Goal& operator=(const Goal&) = default;

  // ⚡ Move constructor
  Goal(Goal&& other) noexcept
      : id(std::move(other.id))
      , title(std::move(other.title))
      , description(std::move(other.description))
      , createdAt(std::move(other.createdAt))
      , updatedAt(std::move(other.updatedAt))
      , dueDate(std::move(other.dueDate))
  {}

  // ⚡ Move assignment
  Goal& operator=(Goal&& other) noexcept
  {
    if (this != &other) {
      id = std::move(other.id);
      title = std::move(other.title);
      description = std::move(other.description);
      createdAt = std::move(other.createdAt);
      updatedAt = std::move(other.updatedAt);
      dueDate = std::move(other.dueDate);
    }
    return *this;
  }
};

} // namespace tasqly::domain::core
