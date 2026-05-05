-- Phase 4: Performance Optimization (Indexes)

-- Submissions indexes
CREATE INDEX IF NOT EXISTS idx_submissions_author_id ON submissions(author_id);
CREATE INDEX IF NOT EXISTS idx_submissions_status ON submissions(status);
CREATE INDEX IF NOT EXISTS idx_submissions_created_at ON submissions(created_at DESC);

-- Reviews indexes
CREATE INDEX IF NOT EXISTS idx_reviews_submission_id ON reviews(submission_id);
CREATE INDEX IF NOT EXISTS idx_reviews_reviewer_id ON reviews(reviewer_id);
CREATE INDEX IF NOT EXISTS idx_reviews_status ON reviews(status);

-- Session indexes
CREATE INDEX IF NOT EXISTS idx_sessions_start_time ON sessions(start_time ASC);
CREATE INDEX IF NOT EXISTS idx_sessions_is_active ON sessions(is_active);

-- Participant and Feedback indexes
CREATE INDEX IF NOT EXISTS idx_session_participants_session_id ON session_participants(session_id);
CREATE INDEX IF NOT EXISTS idx_session_participants_user_id ON session_participants(user_id);
CREATE INDEX IF NOT EXISTS idx_feedback_session_id ON feedback(session_id);
CREATE INDEX IF NOT EXISTS idx_feedback_participant_id ON feedback(participant_id);

-- User indexes
CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);
CREATE INDEX IF NOT EXISTS idx_users_role ON users(role);
