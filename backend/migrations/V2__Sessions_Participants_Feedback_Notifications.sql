-- V2__Sessions_Participants_Feedback_Notifications.sql
-- Phase 3 Schema Extension for Conference Management System

-- 1. Sessions Table
CREATE TABLE IF NOT EXISTS sessions (
    id VARCHAR(50) PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    description TEXT,
    start_time TIMESTAMP,
    end_time TIMESTAMP,
    location VARCHAR(255),
    capacity INTEGER DEFAULT 0,
    chair_id VARCHAR(50),
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_session_chair FOREIGN KEY (chair_id) REFERENCES users(id) ON DELETE SET NULL
);

-- 2. Session Papers Table (accepted submissions assigned to sessions)
CREATE TABLE IF NOT EXISTS session_papers (
    id VARCHAR(50) PRIMARY KEY,
    session_id VARCHAR(50) NOT NULL,
    submission_id VARCHAR(50) NOT NULL,
    presentation_order INTEGER DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_sp_session FOREIGN KEY (session_id) REFERENCES sessions(id) ON DELETE CASCADE,
    CONSTRAINT fk_sp_submission FOREIGN KEY (submission_id) REFERENCES submissions(id) ON DELETE CASCADE,
    CONSTRAINT uq_session_submission UNIQUE(session_id, submission_id)
);

-- 3. Session Participants Table (registration)
CREATE TABLE IF NOT EXISTS session_participants (
    id VARCHAR(50) PRIMARY KEY,
    session_id VARCHAR(50) NOT NULL,
    user_id VARCHAR(50) NOT NULL,
    registered_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    attended BOOLEAN DEFAULT FALSE,
    CONSTRAINT fk_spart_session FOREIGN KEY (session_id) REFERENCES sessions(id) ON DELETE CASCADE,
    CONSTRAINT fk_spart_user FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    CONSTRAINT uq_session_user UNIQUE(session_id, user_id)
);

-- 4. Feedback Table
CREATE TABLE IF NOT EXISTS feedback (
    id VARCHAR(50) PRIMARY KEY,
    session_id VARCHAR(50) NOT NULL,
    participant_id VARCHAR(50) NOT NULL,
    rating INTEGER CHECK (rating >= 1 AND rating <= 5),
    comment TEXT,
    is_anonymous BOOLEAN DEFAULT TRUE,
    submitted_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_fb_session FOREIGN KEY (session_id) REFERENCES sessions(id) ON DELETE CASCADE,
    CONSTRAINT fk_fb_participant FOREIGN KEY (participant_id) REFERENCES users(id) ON DELETE CASCADE,
    CONSTRAINT uq_session_feedback UNIQUE(session_id, participant_id)
);

-- 5. Notifications Table
CREATE TABLE IF NOT EXISTS notifications (
    id VARCHAR(50) PRIMARY KEY,
    user_id VARCHAR(50) NOT NULL,
    type VARCHAR(50) NOT NULL,
    message TEXT NOT NULL,
    is_read BOOLEAN DEFAULT FALSE,
    related_id VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_notif_user FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- Indexes for performance
CREATE INDEX IF NOT EXISTS idx_sessions_chair ON sessions(chair_id);
CREATE INDEX IF NOT EXISTS idx_session_papers_session ON session_papers(session_id);
CREATE INDEX IF NOT EXISTS idx_session_papers_submission ON session_papers(submission_id);
CREATE INDEX IF NOT EXISTS idx_spart_session ON session_participants(session_id);
CREATE INDEX IF NOT EXISTS idx_spart_user ON session_participants(user_id);
CREATE INDEX IF NOT EXISTS idx_feedback_session ON feedback(session_id);
CREATE INDEX IF NOT EXISTS idx_notif_user ON notifications(user_id);
CREATE INDEX IF NOT EXISTS idx_notif_is_read ON notifications(is_read);
