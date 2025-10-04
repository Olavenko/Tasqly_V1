/*
 * 🧱 File: AppContext.cpp
 * -----------------------
 * 📌 Purpose   : Implementation of AppContext service registry.
 * 🧱 Layer     : Application (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-07
 *
 * 🧠 Description:
 * Stores initialized repositories and utilities, providing a central
 * access point for application use cases.
 */

#include "app/core/AppContext.h"

// 📝 Setters
void AppContext::setTaskRepository(std::shared_ptr<tasqly::domain::core::ITaskRepository> repo)
{
  m_taskRepo = std::move(repo);
}
void AppContext::setGoalRepository(std::shared_ptr<tasqly::domain::core::IGoalRepository> repo)
{
  m_goalRepo = std::move(repo);
}
void AppContext::setUuidGen(std::shared_ptr<IUuidGen> gen)
{
  m_uuidGen = std::move(gen);
}
void AppContext::setClock(std::shared_ptr<IClock> clock)
{
  m_clock = std::move(clock);
}

// 🔎 Getters
std::shared_ptr<tasqly::domain::core::ITaskRepository> AppContext::taskRepository() const
{
  return m_taskRepo;
}
std::shared_ptr<tasqly::domain::core::IGoalRepository> AppContext::goalRepository() const
{
  return m_goalRepo;
}
std::shared_ptr<IUuidGen> AppContext::uuidGen() const
{
  return m_uuidGen;
}
std::shared_ptr<IClock> AppContext::clock() const
{
  return m_clock;
}
