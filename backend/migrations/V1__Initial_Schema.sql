-- V1__Initial_Schema.sql
-- Core Phase 2 Data Schema for Conference Management System

-- 1. Users Table
CREATE TABLE IF NOT EXISTS users (
    id VARCHAR(50) PRIMARY KEY,
    email VARCHAR(255) UNIQUE NOT NULL,
    full_name VARCHAR(100) NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    role VARCHAR(20) NOT NULL DEFAULT 'author',
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 2. Submissions (Papers) Table
CREATE TABLE IF NOT EXISTS submissions (
    id VARCHAR(50) PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    abstract TEXT NOT NULL,
    keywords VARCHAR(255),
    status VARCHAR(20) NOT NULL DEFAULT 'draft',
    author_id VARCHAR(50) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_author FOREIGN KEY (author_id) REFERENCES users(id) ON DELETE CASCADE
);

-- 3. Files Table
CREATE TABLE IF NOT EXISTS files (
    id VARCHAR(50) PRIMARY KEY,
    original_name VARCHAR(255) NOT NULL,
    secure_filename VARCHAR(255) NOT NULL,
    file_path TEXT NOT NULL,
    file_size BIGINT NOT NULL,
    extension VARCHAR(20) NOT NULL,
    checksum VARCHAR(64) NOT NULL,
    uploaded_by VARCHAR(50) NOT NULL,
    context VARCHAR(50) NOT NULL,
    is_active BOOLEAN DEFAULT TRUE,
    uploaded_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_uploader FOREIGN KEY (uploaded_by) REFERENCES users(id) ON DELETE CASCADE
);

-- 4. Reviews Table
CREATE TABLE IF NOT EXISTS reviews (
    id VARCHAR(50) PRIMARY KEY,
    submission_id VARCHAR(50) NOT NULL,
    reviewer_id VARCHAR(50) NOT NULL,
    score INTEGER,
    comments TEXT,
    status VARCHAR(20) NOT NULL DEFAULT 'assigned',
    assigned_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    submitted_at TIMESTAMP,
    CONSTRAINT fk_submission FOREIGN KEY (submission_id) REFERENCES submissions(id) ON DELETE CASCADE,
    CONSTRAINT fk_reviewer FOREIGN KEY (reviewer_id) REFERENCES users(id) ON DELETE CASCADE,
    CONSTRAINT uq_review UNIQUE(submission_id, reviewer_id)
);
