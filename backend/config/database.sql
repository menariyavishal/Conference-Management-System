-- Conference Management System - PostgreSQL Schema
-- Created: May 5, 2026
-- Version: 1.0.0

-- ============================================================================
-- USERS TABLE
-- ============================================================================
CREATE TABLE IF NOT EXISTS users (
    user_id SERIAL PRIMARY KEY,
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    first_name VARCHAR(100),
    last_name VARCHAR(100),
    affiliation VARCHAR(255),
    role VARCHAR(50) NOT NULL CHECK (role IN ('ADMIN', 'ORGANIZER', 'AUTHOR', 'REVIEWER', 'PARTICIPANT')),
    is_active BOOLEAN DEFAULT TRUE,
    email_verified BOOLEAN DEFAULT FALSE,
    verification_token VARCHAR(255),
    verification_token_expires_at TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_users_email ON users(email);
CREATE INDEX idx_users_role ON users(role);
CREATE INDEX idx_users_created_at ON users(created_at);

-- ============================================================================
-- CONFERENCES TABLE
-- ============================================================================
CREATE TABLE IF NOT EXISTS conferences (
    conference_id SERIAL PRIMARY KEY,
    organizer_id INTEGER NOT NULL REFERENCES users(user_id),
    name VARCHAR(255) NOT NULL,
    description TEXT,
    start_date DATE,
    end_date DATE,
    location VARCHAR(255),
    submission_deadline DATE,
    review_deadline DATE,
    final_decision_deadline DATE,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_conferences_organizer ON conferences(organizer_id);
CREATE INDEX idx_conferences_dates ON conferences(start_date, end_date);
CREATE INDEX idx_conferences_is_active ON conferences(is_active);

-- ============================================================================
-- SUBMISSIONS TABLE
-- ============================================================================
CREATE TABLE IF NOT EXISTS submissions (
    submission_id SERIAL PRIMARY KEY,
    conference_id INTEGER NOT NULL REFERENCES conferences(conference_id),
    author_id INTEGER NOT NULL REFERENCES users(user_id),
    title VARCHAR(255) NOT NULL,
    abstract TEXT NOT NULL,
    presentation_file_path VARCHAR(255),
    keywords VARCHAR(500),
    status VARCHAR(50) NOT NULL DEFAULT 'DRAFT' CHECK (status IN ('DRAFT', 'SUBMITTED', 'UNDER_REVIEW', 'ACCEPTED', 'REJECTED')),
    submitted_at TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_submissions_conference ON submissions(conference_id);
CREATE INDEX idx_submissions_author ON submissions(author_id);
CREATE INDEX idx_submissions_status ON submissions(status);
CREATE INDEX idx_submissions_submitted_at ON submissions(submitted_at);

-- ============================================================================
-- REVIEWS TABLE
-- ============================================================================
CREATE TABLE IF NOT EXISTS reviews (
    review_id SERIAL PRIMARY KEY,
    submission_id INTEGER NOT NULL REFERENCES submissions(submission_id),
    reviewer_id INTEGER NOT NULL REFERENCES users(user_id),
    score INTEGER CHECK (score >= 1 AND score <= 5),
    feedback TEXT,
    is_anonymous BOOLEAN DEFAULT TRUE,
    status VARCHAR(50) DEFAULT 'PENDING' CHECK (status IN ('PENDING', 'SUBMITTED', 'WITHDRAWN')),
    submitted_at TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(submission_id, reviewer_id)
);

CREATE INDEX idx_reviews_submission ON reviews(submission_id);
CREATE INDEX idx_reviews_reviewer ON reviews(reviewer_id);
CREATE INDEX idx_reviews_status ON reviews(status);

-- ============================================================================
-- SESSIONS TABLE
-- ============================================================================
CREATE TABLE IF NOT EXISTS sessions (
    session_id SERIAL PRIMARY KEY,
    conference_id INTEGER NOT NULL REFERENCES conferences(conference_id),
    title VARCHAR(255) NOT NULL,
    description TEXT,
    start_time TIMESTAMP,
    end_time TIMESTAMP,
    location VARCHAR(255),
    capacity INTEGER,
    chair_id INTEGER REFERENCES users(user_id),
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_sessions_conference ON sessions(conference_id);
CREATE INDEX idx_sessions_start_time ON sessions(start_time);
CREATE INDEX idx_sessions_chair ON sessions(chair_id);

-- ============================================================================
-- SESSION PARTICIPANTS TABLE
-- ============================================================================
CREATE TABLE IF NOT EXISTS session_participants (
    participant_id SERIAL PRIMARY KEY,
    session_id INTEGER NOT NULL REFERENCES sessions(session_id),
    user_id INTEGER NOT NULL REFERENCES users(user_id),
    registered_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    attended BOOLEAN DEFAULT FALSE,
    UNIQUE(session_id, user_id)
);

CREATE INDEX idx_session_participants_session ON session_participants(session_id);
CREATE INDEX idx_session_participants_user ON session_participants(user_id);

-- ============================================================================
-- SESSION PAPERS TABLE
-- ============================================================================
CREATE TABLE IF NOT EXISTS session_papers (
    session_paper_id SERIAL PRIMARY KEY,
    session_id INTEGER NOT NULL REFERENCES sessions(session_id),
    submission_id INTEGER NOT NULL REFERENCES submissions(submission_id),
    presentation_order INTEGER,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(session_id, submission_id)
);

CREATE INDEX idx_session_papers_session ON session_papers(session_id);
CREATE INDEX idx_session_papers_submission ON session_papers(submission_id);

-- ============================================================================
-- FEEDBACK TABLE
-- ============================================================================
CREATE TABLE IF NOT EXISTS feedback (
    feedback_id SERIAL PRIMARY KEY,
    session_id INTEGER NOT NULL REFERENCES sessions(session_id),
    participant_id INTEGER NOT NULL REFERENCES users(user_id),
    rating INTEGER CHECK (rating >= 1 AND rating <= 5),
    comment TEXT,
    is_anonymous BOOLEAN DEFAULT TRUE,
    submitted_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_feedback_session ON feedback(session_id);
CREATE INDEX idx_feedback_participant ON feedback(participant_id);
CREATE INDEX idx_feedback_submitted_at ON feedback(submitted_at);

-- ============================================================================
-- NOTIFICATIONS TABLE
-- ============================================================================
CREATE TABLE IF NOT EXISTS notifications (
    notification_id SERIAL PRIMARY KEY,
    user_id INTEGER NOT NULL REFERENCES users(user_id),
    type VARCHAR(50) NOT NULL CHECK (type IN ('SUBMISSION', 'REVIEW', 'SESSION', 'FEEDBACK', 'SYSTEM')),
    message TEXT NOT NULL,
    is_read BOOLEAN DEFAULT FALSE,
    related_id INTEGER,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_notifications_user ON notifications(user_id);
CREATE INDEX idx_notifications_is_read ON notifications(is_read);
CREATE INDEX idx_notifications_created_at ON notifications(created_at);

-- ============================================================================
-- FILE UPLOADS TABLE
-- ============================================================================
CREATE TABLE IF NOT EXISTS file_uploads (
    file_id SERIAL PRIMARY KEY,
    submission_id INTEGER REFERENCES submissions(submission_id),
    uploaded_by INTEGER NOT NULL REFERENCES users(user_id),
    original_filename VARCHAR(255),
    stored_path VARCHAR(255),
    file_type VARCHAR(50),
    file_size BIGINT,
    uploaded_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_file_uploads_submission ON file_uploads(submission_id);
CREATE INDEX idx_file_uploads_uploaded_by ON file_uploads(uploaded_by);

-- ============================================================================
-- SYSTEM LOGS TABLE
-- ============================================================================
CREATE TABLE IF NOT EXISTS system_logs (
    log_id SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES users(user_id),
    action VARCHAR(50),
    entity VARCHAR(50),
    details TEXT,
    ip_address VARCHAR(45),
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    status VARCHAR(50) DEFAULT 'SUCCESS' CHECK (status IN ('SUCCESS', 'FAILURE'))
);

CREATE INDEX idx_system_logs_timestamp ON system_logs(timestamp);
CREATE INDEX idx_system_logs_user_id ON system_logs(user_id);
CREATE INDEX idx_system_logs_action ON system_logs(action);

-- ============================================================================
-- SYSTEM CONFIGURATION TABLE
-- ============================================================================
CREATE TABLE IF NOT EXISTS system_config (
    config_id SERIAL PRIMARY KEY,
    config_key VARCHAR(100) UNIQUE NOT NULL,
    config_value TEXT,
    description VARCHAR(255),
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_system_config_key ON system_config(config_key);

-- ============================================================================
-- MIGRATIONS TABLE (for database versioning)
-- ============================================================================
CREATE TABLE IF NOT EXISTS database_migrations (
    id SERIAL PRIMARY KEY,
    migration_name VARCHAR(255) NOT NULL UNIQUE,
    applied_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    execution_time_ms INTEGER
);

-- ============================================================================
-- Initial Configuration Data
-- ============================================================================
INSERT INTO system_config (config_key, config_value, description) VALUES
    ('app_version', '1.0.0', 'Application version'),
    ('database_version', '1.0.0', 'Database schema version'),
    ('maintenance_mode', 'false', 'System maintenance mode'),
    ('maintenance_message', 'System is under maintenance', 'Maintenance mode message')
ON CONFLICT (config_key) DO NOTHING;

-- ============================================================================
-- Triggers for automatic timestamp updates
-- ============================================================================
CREATE OR REPLACE FUNCTION update_timestamp()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER users_update_timestamp BEFORE UPDATE ON users
FOR EACH ROW EXECUTE FUNCTION update_timestamp();

CREATE TRIGGER conferences_update_timestamp BEFORE UPDATE ON conferences
FOR EACH ROW EXECUTE FUNCTION update_timestamp();

CREATE TRIGGER submissions_update_timestamp BEFORE UPDATE ON submissions
FOR EACH ROW EXECUTE FUNCTION update_timestamp();

CREATE TRIGGER reviews_update_timestamp BEFORE UPDATE ON reviews
FOR EACH ROW EXECUTE FUNCTION update_timestamp();

CREATE TRIGGER sessions_update_timestamp BEFORE UPDATE ON sessions
FOR EACH ROW EXECUTE FUNCTION update_timestamp();

-- ============================================================================
-- Comments for documentation
-- ============================================================================
COMMENT ON TABLE users IS 'Stores all users (authors, reviewers, organizers, participants)';
COMMENT ON TABLE conferences IS 'Stores conference information and deadlines';
COMMENT ON TABLE submissions IS 'Stores paper submissions for conferences';
COMMENT ON TABLE reviews IS 'Stores review information for submissions';
COMMENT ON TABLE sessions IS 'Stores conference sessions/tracks';
COMMENT ON TABLE session_participants IS 'Tracks which users registered for which sessions';
COMMENT ON TABLE session_papers IS 'Maps papers to sessions';
COMMENT ON TABLE feedback IS 'Stores session feedback from participants';
COMMENT ON TABLE notifications IS 'Stores user notifications';
COMMENT ON TABLE file_uploads IS 'Tracks uploaded files';
COMMENT ON TABLE system_logs IS 'Audit trail of system activities';
COMMENT ON TABLE system_config IS 'System configuration key-value pairs';
COMMENT ON TABLE database_migrations IS 'Database migration history';
