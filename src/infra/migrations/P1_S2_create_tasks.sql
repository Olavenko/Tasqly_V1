
BEGIN;

CREATE TABLE IF NOT EXISTS tasks (
    id VARCHAR(64) PRIMARY KEY,                         
    title TEXT NOT NULL,                             
    notes TEXT,                                      
    status TEXT NOT NULL DEFAULT 'Todo',             
    priority TEXT NOT NULL DEFAULT 'Normal',         
    deadline TIMESTAMPTZ NULL,                       
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),   
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW()    
);

CREATE INDEX IF NOT EXISTS idx_tasks_status
    ON tasks (status);

CREATE INDEX IF NOT EXISTS idx_tasks_priority
    ON tasks (priority);

CREATE INDEX IF NOT EXISTS idx_tasks_deadline
    ON tasks (deadline);

CREATE INDEX IF NOT EXISTS idx_tasks_status_priority_deadline
    ON tasks (status, priority, deadline);

COMMIT;
