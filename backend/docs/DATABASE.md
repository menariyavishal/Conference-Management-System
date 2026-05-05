# Database Schema Documentation

Complete documentation of the PostgreSQL database schema for the Conference Management System.

## Overview

The database consists of 13 tables supporting:
- User management with role-based access
- Conference creation and management
- Paper submission and review workflows
- Session scheduling and management
- Participant feedback collection
- System logging and audit trails
- Configuration management

## Table Descriptions

### 1. users

Stores all user accounts with authentication credentials and role information.

| Column | Type | Constraints | Notes |
|--------|------|-------------|-------|
| user_id | SERIAL | PRIMARY KEY | Auto-incrementing user identifier |
| email | VARCHAR(255) | UNIQUE, NOT NULL | Email must be unique |
| password_hash | VARCHAR(255) | NOT NULL | SHA256 hash with salt |
| first_name | VARCHAR(100) | NULL | User's first name |
| last_name | VARCHAR(100) | NULL | User's last name |
| affiliation | VARCHAR(255) | NULL | University/organization |
| role | VARCHAR(50) | CHECK, NOT NULL | Must be ADMIN, ORGANIZER, AUTHOR, REVIEWER, or PARTICIPANT |
| is_active | BOOLEAN | DEFAULT TRUE | Soft delete flag |
| email_verified | BOOLEAN | DEFAULT FALSE | Email confirmation status |
| verification_token | VARCHAR(255) | NULL | Token for email verification |
| verification_token_expires_at | TIMESTAMP | NULL | Expiration time of verification token |
| created_at | TIMESTAMP | DEFAULT NOW | Account creation time |
| updated_at | TIMESTAMP | DEFAULT NOW | Last update time |

**Indexes**:
- email (for login queries)
- role (for role-based filtering)
- created_at (for user statistics)

**Sample Query**:
```sql
SELECT * FROM users WHERE email = 'user@example.com' AND is_active = true;
```

### 2. conferences

Stores conference information with deadlines and dates.

| Column | Type | Constraints | Notes |
|--------|------|-------------|-------|
| conference_id | SERIAL | PRIMARY KEY | Auto-incrementing conference ID |
| organizer_id | INTEGER | FOREIGN KEY | References users.user_id |
| name | VARCHAR(255) | NOT NULL | Conference name |
| description | TEXT | NULL | Conference description |
| start_date | DATE | NULL | Conference start date |
| end_date | DATE | NULL | Conference end date |
| location | VARCHAR(255) | NULL | Physical location |
| submission_deadline | DATE | NULL | Paper submission deadline |
| review_deadline | DATE | NULL | Review completion deadline |
| final_decision_deadline | DATE | NULL | Final decision announcement |
| is_active | BOOLEAN | DEFAULT TRUE | Active conference flag |
| created_at | TIMESTAMP | DEFAULT NOW | Creation timestamp |
| updated_at | TIMESTAMP | DEFAULT NOW | Last update timestamp |

**Indexes**:
- organizer_id (for user's conferences)
- dates (start_date, end_date for calendar queries)
- is_active (for active conference filtering)

**Sample Query**:
```sql
SELECT * FROM conferences 
WHERE is_active = true 
AND start_date <= CURRENT_DATE 
AND end_date >= CURRENT_DATE;
```

### 3. submissions

Stores paper submissions for conferences with current status.

| Column | Type | Constraints | Notes |
|--------|------|-------------|-------|
| submission_id | SERIAL | PRIMARY KEY | Auto-incrementing submission ID |
| conference_id | INTEGER | FOREIGN KEY | References conferences.conference_id |
| author_id | INTEGER | FOREIGN KEY | References users.user_id |
| title | VARCHAR(255) | NOT NULL | Paper title |
| abstract | TEXT | NOT NULL | Paper abstract (50-2000 chars) |
| presentation_file_path | VARCHAR(255) | NULL | Path to uploaded PDF |
| keywords | VARCHAR(500) | NULL | Comma-separated keywords |
| status | VARCHAR(50) | CHECK, NOT NULL | DRAFT, SUBMITTED, UNDER_REVIEW, ACCEPTED, REJECTED |
| submitted_at | TIMESTAMP | NULL | Submission timestamp |
| updated_at | TIMESTAMP | DEFAULT NOW | Last update |
| created_at | TIMESTAMP | DEFAULT NOW | Creation timestamp |

**Indexes**:
- conference_id (for conference submissions)
- author_id (for author's papers)
- status (for workflow filtering)
- submitted_at (for submission history)

**Sample Query**:
```sql
SELECT COUNT(*) FROM submissions 
WHERE conference_id = 1 AND status = 'SUBMITTED';
```

### 4. reviews

Stores peer review information for submissions.

| Column | Type | Constraints | Notes |
|--------|------|-------------|-------|
| review_id | SERIAL | PRIMARY KEY | Auto-incrementing review ID |
| submission_id | INTEGER | FOREIGN KEY | References submissions.submission_id |
| reviewer_id | INTEGER | FOREIGN KEY | References users.user_id |
| score | INTEGER | CHECK (1-5) | 1-5 point scale |
| feedback | TEXT | NULL | Review comments |
| is_anonymous | BOOLEAN | DEFAULT TRUE | Anonymity flag |
| status | VARCHAR(50) | DEFAULT PENDING | PENDING, SUBMITTED, WITHDRAWN |
| submitted_at | TIMESTAMP | NULL | Review submission time |
| updated_at | TIMESTAMP | DEFAULT NOW | Last update |
| created_at | TIMESTAMP | DEFAULT NOW | Creation timestamp |

**Constraints**:
- UNIQUE(submission_id, reviewer_id) - One review per reviewer per submission

**Indexes**:
- submission_id (for submission reviews)
- reviewer_id (for reviewer's assignments)
- status (for pending reviews)

**Sample Query**:
```sql
SELECT AVG(score) as average_score 
FROM reviews 
WHERE submission_id = 5 AND status = 'SUBMITTED';
```

### 5. sessions

Stores conference session/track information.

| Column | Type | Constraints | Notes |
|--------|------|-------------|-------|
| session_id | SERIAL | PRIMARY KEY | Auto-incrementing session ID |
| conference_id | INTEGER | FOREIGN KEY | References conferences.conference_id |
| title | VARCHAR(255) | NOT NULL | Session title |
| description | TEXT | NULL | Session description |
| start_time | TIMESTAMP | NULL | Session start time |
| end_time | TIMESTAMP | NULL | Session end time |
| location | VARCHAR(255) | NULL | Room/location |
| capacity | INTEGER | NULL | Maximum participants |
| chair_id | INTEGER | FOREIGN KEY | References users.user_id (session chair) |
| is_active | BOOLEAN | DEFAULT TRUE | Active flag |
| created_at | TIMESTAMP | DEFAULT NOW | Creation timestamp |
| updated_at | TIMESTAMP | DEFAULT NOW | Last update |

**Indexes**:
- conference_id (for conference sessions)
- start_time (for schedule queries)
- chair_id (for chair's sessions)

**Sample Query**:
```sql
SELECT * FROM sessions 
WHERE conference_id = 1 
ORDER BY start_time ASC;
```

### 6. session_participants

Junction table for participant-session relationships with attendance tracking.

| Column | Type | Constraints | Notes |
|--------|------|-------------|-------|
| participant_id | SERIAL | PRIMARY KEY | Auto-incrementing participant ID |
| session_id | INTEGER | FOREIGN KEY | References sessions.session_id |
| user_id | INTEGER | FOREIGN KEY | References users.user_id |
| registered_at | TIMESTAMP | DEFAULT NOW | Registration time |
| attended | BOOLEAN | DEFAULT FALSE | Attendance flag |

**Constraints**:
- UNIQUE(session_id, user_id) - User can register once per session

**Indexes**:
- session_id (for session participants)
- user_id (for user's sessions)

**Sample Query**:
```sql
SELECT COUNT(*) as attendee_count 
FROM session_participants 
WHERE session_id = 3 AND attended = true;
```

### 7. session_papers

Maps accepted papers to sessions with presentation order.

| Column | Type | Constraints | Notes |
|--------|------|-------------|-------|
| session_paper_id | SERIAL | PRIMARY KEY | Auto-incrementing ID |
| session_id | INTEGER | FOREIGN KEY | References sessions.session_id |
| submission_id | INTEGER | FOREIGN KEY | References submissions.submission_id |
| presentation_order | INTEGER | NULL | Order in session (1, 2, 3...) |
| created_at | TIMESTAMP | DEFAULT NOW | Creation timestamp |

**Constraints**:
- UNIQUE(session_id, submission_id) - Paper in session once

**Indexes**:
- session_id (for session papers)
- submission_id (for paper sessions)

**Sample Query**:
```sql
SELECT s.title, sp.presentation_order 
FROM session_papers sp
JOIN submissions s ON sp.submission_id = s.submission_id
WHERE sp.session_id = 2
ORDER BY sp.presentation_order;
```

### 8. feedback

Stores session feedback from participants.

| Column | Type | Constraints | Notes |
|--------|------|-------------|-------|
| feedback_id | SERIAL | PRIMARY KEY | Auto-incrementing feedback ID |
| session_id | INTEGER | FOREIGN KEY | References sessions.session_id |
| participant_id | INTEGER | FOREIGN KEY | References users.user_id |
| rating | INTEGER | CHECK (1-5) | 1-5 point scale |
| comment | TEXT | NULL | Feedback comments |
| is_anonymous | BOOLEAN | DEFAULT TRUE | Anonymity flag |
| submitted_at | TIMESTAMP | DEFAULT NOW | Submission time |

**Indexes**:
- session_id (for session feedback)
- participant_id (for user feedback)
- submitted_at (for feedback history)

**Sample Query**:
```sql
SELECT AVG(rating) as average_rating 
FROM feedback 
WHERE session_id = 1;
```

### 9. notifications

Stores user notifications for various system events.

| Column | Type | Constraints | Notes |
|--------|------|-------------|-------|
| notification_id | SERIAL | PRIMARY KEY | Auto-incrementing ID |
| user_id | INTEGER | FOREIGN KEY | References users.user_id |
| type | VARCHAR(50) | CHECK | SUBMISSION, REVIEW, SESSION, FEEDBACK, SYSTEM |
| message | TEXT | NOT NULL | Notification message |
| is_read | BOOLEAN | DEFAULT FALSE | Read status |
| related_id | INTEGER | NULL | ID of related entity |
| created_at | TIMESTAMP | DEFAULT NOW | Creation time |

**Indexes**:
- user_id (for user notifications)
- is_read (for unread notifications)
- created_at (for notification history)

**Sample Query**:
```sql
SELECT * FROM notifications 
WHERE user_id = 1 AND is_read = false
ORDER BY created_at DESC;
```

### 10. file_uploads

Tracks uploaded files for submissions and other entities.

| Column | Type | Constraints | Notes |
|--------|------|-------------|-------|
| file_id | SERIAL | PRIMARY KEY | Auto-incrementing file ID |
| submission_id | INTEGER | FOREIGN KEY | References submissions.submission_id |
| uploaded_by | INTEGER | FOREIGN KEY | References users.user_id |
| original_filename | VARCHAR(255) | NULL | Original uploaded filename |
| stored_path | VARCHAR(255) | NOT NULL | Path in file system |
| file_type | VARCHAR(50) | NULL | MIME type |
| file_size | BIGINT | NULL | File size in bytes |
| uploaded_at | TIMESTAMP | DEFAULT NOW | Upload timestamp |

**Indexes**:
- submission_id (for submission files)
- uploaded_by (for user uploads)

**Sample Query**:
```sql
SELECT * FROM file_uploads 
WHERE submission_id = 1 
ORDER BY uploaded_at DESC;
```

### 11. system_logs

Audit trail of all system activities.

| Column | Type | Constraints | Notes |
|--------|------|-------------|-------|
| log_id | SERIAL | PRIMARY KEY | Auto-incrementing log ID |
| user_id | INTEGER | FOREIGN KEY | References users.user_id |
| action | VARCHAR(50) | NULL | LOGIN, CREATE, UPDATE, DELETE, etc. |
| entity | VARCHAR(50) | NULL | USER, SUBMISSION, REVIEW, etc. |
| details | TEXT | NULL | Additional JSON details |
| ip_address | VARCHAR(45) | NULL | IPv4 or IPv6 address |
| timestamp | TIMESTAMP | DEFAULT NOW | Log timestamp |
| status | VARCHAR(50) | CHECK | SUCCESS, FAILURE |

**Indexes**:
- timestamp (for log queries)
- user_id (for user activity)
- action (for action analysis)

**Sample Query**:
```sql
SELECT * FROM system_logs 
WHERE user_id = 1 AND action = 'LOGIN'
ORDER BY timestamp DESC LIMIT 10;
```

### 12. system_config

Key-value configuration storage for system settings.

| Column | Type | Constraints | Notes |
|--------|------|-------------|-------|
| config_id | SERIAL | PRIMARY KEY | Auto-incrementing ID |
| config_key | VARCHAR(100) | UNIQUE | Configuration key |
| config_value | TEXT | NULL | Configuration value |
| description | VARCHAR(255) | NULL | Setting description |
| updated_at | TIMESTAMP | DEFAULT NOW | Last update |

**Indexes**:
- config_key (for fast lookups)

**Sample Query**:
```sql
SELECT config_value FROM system_config 
WHERE config_key = 'app_version';
```

### 13. database_migrations

Tracks applied database migrations for versioning.

| Column | Type | Constraints | Notes |
|--------|------|-------------|-------|
| id | SERIAL | PRIMARY KEY | Auto-incrementing ID |
| migration_name | VARCHAR(255) | UNIQUE | Migration filename |
| applied_at | TIMESTAMP | DEFAULT NOW | Application timestamp |
| execution_time_ms | INTEGER | NULL | Execution time in milliseconds |

**Sample Query**:
```sql
SELECT * FROM database_migrations 
ORDER BY applied_at DESC LIMIT 10;
```

## Relationships

### User-Centric

```
users (1) ----> (many) conferences (as organizer)
users (1) ----> (many) submissions (as author)
users (1) ----> (many) reviews (as reviewer)
users (1) ----> (many) notifications
```

### Conference Workflow

```
conferences (1) ----> (many) submissions
submissions (1) ----> (many) reviews
submissions (1) ----> (many) session_papers
sessions (1) ----> (many) session_participants
sessions (1) ----> (many) session_papers
sessions (1) ----> (many) feedback
```

## Common Queries

### User Dashboard - Submissions by Status

```sql
SELECT 
    COUNT(*) as total,
    status,
    COUNT(CASE WHEN status = 'ACCEPTED' THEN 1 END) as accepted,
    COUNT(CASE WHEN status = 'REJECTED' THEN 1 END) as rejected
FROM submissions
WHERE author_id = $1
GROUP BY status;
```

### Review Statistics

```sql
SELECT 
    s.submission_id,
    s.title,
    AVG(r.score) as average_score,
    COUNT(r.review_id) as review_count
FROM submissions s
LEFT JOIN reviews r ON s.submission_id = r.submission_id
WHERE s.conference_id = $1
GROUP BY s.submission_id, s.title;
```

### Conference Timeline

```sql
SELECT 
    c.conference_id,
    c.name,
    c.start_date,
    c.submission_deadline,
    CASE 
        WHEN CURRENT_DATE > c.submission_deadline THEN 'Closed'
        WHEN CURRENT_DATE <= c.submission_deadline THEN 'Open'
    END as submission_status
FROM conferences c
WHERE c.is_active = true
ORDER BY c.start_date ASC;
```

## Performance Tuning

### Index Strategy

Critical indexes are in place on:
- Foreign key columns for joins
- Filter columns (status, is_active, dates)
- Search columns (email, keywords)

### Query Optimization Tips

1. **Always filter by conference_id first** - reduces result set early
2. **Use EXPLAIN ANALYZE** to inspect query plans
3. **Avoid SELECT * ** - specify needed columns
4. **Use LIMIT** when pagination available

```bash
EXPLAIN ANALYZE
SELECT * FROM submissions WHERE conference_id = 1 AND status = 'SUBMITTED';
```

## Backup and Recovery

### Regular Backups

```bash
# Full backup
pg_dump -U postgres conference_db > backup_$(date +%Y%m%d).sql

# Compressed backup
pg_dump -U postgres -F c conference_db > backup_$(date +%Y%m%d).dump

# Restore from backup
psql -U postgres conference_db < backup_20240505.sql
```

### Point-in-Time Recovery

PostgreSQL supports continuous archiving for PITR. Configure in production.

## Maintenance

### Regular Maintenance Tasks

```sql
-- Analyze table for query optimization
ANALYZE submissions;

-- Reindex table
REINDEX TABLE submissions;

-- Vacuum to reclaim space
VACUUM submissions;

-- Full maintenance (run off-hours)
VACUUM FULL ANALYZE;
```

## Security Considerations

1. **Row-Level Security (RLS)** - Implement for multi-tenant separation
2. **Column Encryption** - Consider for sensitive data (email, passwords)
3. **Audit Triggers** - Log all data modifications
4. **Connection Limits** - Restrict connections per role
5. **Credential Rotation** - Change database passwords regularly
