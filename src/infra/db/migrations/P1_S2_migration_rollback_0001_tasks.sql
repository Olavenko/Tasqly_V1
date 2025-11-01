/*
 * 🧱 File: P1_S2_migration_rollback_0001_tasks.sql
 * -----------------------------------------------
 * 📌 Purpose   : Rollback script for `P1_S2_migration_0001_tasks.sql`.
 * 🧱 Layer     : Infrastructure (Database / Migration)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-19
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Drops the trigger, function, indices, and the `tasks` table.
 * - Used when rolling back a failed migration or resetting test DB.
 */

BEGIN;

DROP TRIGGER IF EXISTS trg_update_task_timestamp ON tasks;
DROP FUNCTION IF EXISTS update_task_timestamp();

DROP INDEX IF EXISTS idx_tasks_composite_status_priority;
DROP INDEX IF EXISTS idx_tasks_deadline;
DROP INDEX IF EXISTS idx_tasks_priority;
DROP INDEX IF EXISTS idx_tasks_status;

DROP TABLE IF EXISTS tasks;

COMMIT;
