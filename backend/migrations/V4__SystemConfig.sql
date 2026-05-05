-- Phase 4: System Configuration Table
CREATE TABLE IF NOT EXISTS system_config (
    key VARCHAR(100) PRIMARY KEY,
    value TEXT NOT NULL,
    description TEXT,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_by UUID -- Reference to users table if needed
);

-- Initial Configuration Seeds
INSERT INTO system_config (key, value, description) VALUES 
('conference_name', 'Global Tech Conference 2026', 'The official name of the conference'),
('submission_deadline', '2026-06-01T23:59:59Z', 'Final date for paper submissions'),
('registration_open', 'true', 'Toggle for user registration'),
('maintenance_mode', 'false', 'Disable system for maintenance'),
('max_submissions_per_author', '3', 'Limit on number of papers per user')
ON CONFLICT (key) DO NOTHING;
