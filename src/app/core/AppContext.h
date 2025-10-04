/*
 * 🧱 File: AppContext.h
 * ---------------------
 * 📌 Purpose   : Central registry for core application services (repositories, utils).
 * 🧱 Layer     : Application (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Provides global access to initialized repositories and utilities such as
 * UUID generator and clock. Populated during application startup via InitRepositories.
 */

#pragma once

#include "domain/core/IClock.h"
#include "domain/core/IGoalRepository.h"
#include "domain/core/ITaskRepository.h"
#include "domain/core/IUuidGen.h"

#include <memory>

class AppContext
{
public:
  AppContext() = default;
  ~AppContext() = default; // 🆕 explicit destructor

  // 🔑 Setters (called during initialization)
  void setTaskRepository(std::shared_ptr<tasqly::domain::core::ITaskRepository> repo);
  void setGoalRepository(std::shared_ptr<tasqly::domain::core::IGoalRepository> repo);
  void setUuidGen(std::shared_ptr<IUuidGen> gen);
  void setClock(std::shared_ptr<IClock> clock);

  // 🔎 Getters (used by use cases)
  std::shared_ptr<tasqly::domain::core::ITaskRepository> taskRepository() const;
  std::shared_ptr<tasqly::domain::core::IGoalRepository> goalRepository() const;
  std::shared_ptr<IUuidGen> uuidGen() const;
  std::shared_ptr<IClock> clock() const;

private:
  std::shared_ptr<tasqly::domain::core::ITaskRepository> m_taskRepo;
  std::shared_ptr<tasqly::domain::core::IGoalRepository> m_goalRepo;
  std::shared_ptr<IUuidGen> m_uuidGen;
  std::shared_ptr<IClock> m_clock;
};
