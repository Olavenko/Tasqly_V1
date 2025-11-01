BEGIN;

DROP INDEX IF EXISTS idx_tasks_status;
DROP INDEX IF EXISTS idx_tasks_priority;
DROP INDEX IF EXISTS idx_tasks_deadline;
DROP INDEX IF EXISTS idx_tasks_status_priority_deadline;

DROP TABLE IF EXISTS tasks;

COMMIT;
