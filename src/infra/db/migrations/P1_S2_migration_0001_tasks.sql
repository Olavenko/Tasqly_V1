/*
 * 🧱 File: P1_S2_migration_0001_tasks.sql
 * ---------------------------------------
 * 📌 Purpose   : Create the `tasks` table and performance indices for Tasqly Phase 1 Slice 2.
 * 🧱 Layer     : Infrastructure (Database / Migration)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-19
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Creates the main `tasks` table used for persistence in the Postgres repository.
 * - Uses UUID primary key for scalability.
 * - Adds default timestamps for created_at and updated_at.
 * - Includes indices for query optimization (status, priority, deadline).
 *
 * 🔒 Security:
 * - No hardcoded credentials, migration runs under the active DB user.
 */

BEGIN;

CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

CREATE TABLE IF NOT EXISTS tasks (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    title TEXT NOT NULL CHECK (char_length(title) > 0),
    notes TEXT DEFAULT '',
    status INT NOT NULL DEFAULT 0,
    priority INT NOT NULL DEFAULT 0,
    deadline TIMESTAMPTZ NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

-- ✅ Add automatic updated_at trigger for modification timestamp
CREATE OR REPLACE FUNCTION update_task_timestamp()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = NOW();
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_update_task_timestamp
BEFORE UPDATE ON tasks
FOR EACH ROW
EXECUTE FUNCTION update_task_timestamp();

-- ✅ Indices for faster query filtering and sorting
CREATE INDEX IF NOT EXISTS idx_tasks_status ON tasks(status);
CREATE INDEX IF NOT EXISTS idx_tasks_priority ON tasks(priority);
CREATE INDEX IF NOT EXISTS idx_tasks_deadline ON tasks(deadline);
CREATE INDEX IF NOT EXISTS idx_tasks_composite_status_priority
    ON tasks(status, priority);

COMMIT;
