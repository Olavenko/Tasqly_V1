-- ============================================================
--  P1_S2_create_tasks.sql
--  Phase 1 — Slice 2.5
--  Create table: tasks
-- ============================================================

BEGIN;

CREATE TABLE IF NOT EXISTS public.tasks (
    id UUID PRIMARY KEY,                               -- Domain: Task.id (UUID)
    title VARCHAR(255) NOT NULL,                       -- Task.title
    description TEXT,                                  -- Task.notes
    status INT NOT NULL,                               -- TaskStatus enum (int)
    priority INT NOT NULL,                             -- TaskPriority enum (int)
    deadline TIMESTAMP NULL,                           -- Optional deadline
    created_at TIMESTAMP NOT NULL,                     -- Task.createdAt
    updated_at TIMESTAMP NOT NULL                      -- Task.updatedAt
);

-- ------------------------------------------------------------
-- Indexes (optimized for listing / filtering use cases)
-- ------------------------------------------------------------

CREATE INDEX IF NOT EXISTS idx_tasks_status
    ON public.tasks (status);

CREATE INDEX IF NOT EXISTS idx_tasks_priority
    ON public.tasks (priority);

CREATE INDEX IF NOT EXISTS idx_tasks_deadline
    ON public.tasks (deadline);

CREATE INDEX IF NOT EXISTS idx_tasks_status_priority_deadline
    ON public.tasks (status, priority, deadline);

COMMIT;
