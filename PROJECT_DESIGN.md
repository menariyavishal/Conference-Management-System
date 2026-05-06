# Conference Management System (C++ + Dogon)
## OOAD Mini Project - Complete Design Document

**Project Date:** May 5, 2026  
**Technology Stack:** C++ Backend + React Frontend (Vite) + Tailwind CSS + Framer Motion  
**Architecture:** Modular (Single Feature Per File) + REST API Interaction

---

## Table of Contents
1. [Reference Repository Analysis](#reference-repository-analysis)
2. [Functional Requirements (FR)](#functional-requirements-fr)
3. [Non-Functional Requirements (NFR)](#non-functional-requirements-nfr)
4. [Use Cases & User Scenarios](#use-cases--user-scenarios)
5. [UML Diagrams](#uml-diagrams)
6. [Data Flow Diagrams (DFD)](#data-flow-diagrams-dfd)
7. [Database Schema](#database-schema)
8. [System Architecture](#system-architecture)
9. [Modular Structure](#modular-structure)
10. [API Design](#api-design)
11. [Technology Stack](#technology-stack)
12. [Implementation Roadmap](#implementation-roadmap)

---

## Reference Repository Analysis

### Existing System Overview
The reference repository (gorkemturkut57/Conference-Management-System) is a web-based PHP/MSSQL system with the following characteristics:

**Current Tech Stack:**
- **Frontend:** HTML, CSS, JavaScript (Bootstrap 62.7%)
- **Backend:** PHP (21.9%)
- **Database:** MSSQL
- **Session Management:** PHP native sessions
- **File Storage:** Local filesystem (uploads/ directory)
- **Architecture:** Monolithic web application

**Core Functionalities Implemented:**
1. ✅ User Authentication & Role-Based Access Control
2. ✅ Multi-role User Management (Organizer, Author, Reviewer, Participant)
3. ✅ Submission Management System
4. ✅ Double-blind Review Process
5. ✅ Conference Session Scheduling
6. ✅ Maintenance Mode & Logging
7. ✅ User Feedback System
8. ✅ Role-based Dashboards

**Key Insights:**
- Well-defined user roles with distinct responsibilities
- Clear separation of concerns per user type
- File-based submission storage requiring proper access control
- Real-time logging and monitoring requirements
- Role-based access control is critical

### Lessons for Our C++ Implementation
- Adopt the same user role hierarchy
- Enhance security with JWT/Token-based auth instead of sessions
- Implement proper file management with database integration
- Use microservices/modular approach instead of monolithic
- Separate backend API from frontend concerns

---

## Functional Requirements (FR)

### FR1: User Management Module
- **FR1.1** User Registration: Self-registration with email verification
- **FR1.2** User Authentication: Login with role assignment
- **FR1.3** User Role Management: Assign/update user roles (Admin only)
- **FR1.4** User Profile Management: Update personal information, change password
- **FR1.5** User Listing: Organizers can view all users with filters
- **FR1.6** User Deactivation: Soft delete users (preserve data integrity)

### FR2: Submission Management Module
- **FR2.1** Submit Abstract: Authors can submit paper abstracts
- **FR2.2** Submit Presentation: Authors can upload presentation files
- **FR2.3** Edit Submission: Authors can modify submissions before deadline
- **FR2.4** Withdraw Submission: Authors can withdraw submissions
- **FR2.5** View Submission Status: Authors can track submission status
- **FR2.6** List Submissions: Reviewers/Organizers can view submissions

### FR3: Review Process Module
- **FR3.1** Assign Reviews: Organizers assign submissions to reviewers
- **FR3.2** Submit Review: Reviewers can submit reviews with scores/feedback
- **FR3.3** Double-blind Review: Anonymize author information to reviewers
- **FR3.4** Review Tracking: Track review deadlines and completion status
- **FR3.5** View Reviews: Authors can see anonymized reviews and scores
- **FR3.6** Review Statistics: Generate review metrics and summaries

### FR4: Conference Session Management Module
- **FR4.1** Create Session: Organizers can create conference sessions
- **FR4.2** Edit Session: Organizers can update session details
- **FR4.3** Delete Session: Organizers can remove sessions
- **FR4.4** Assign Papers to Sessions: Assign accepted papers to sessions
- **FR4.5** View Session Schedule: All users can view conference schedule
- **FR4.6** Session Details: Display session info (time, room, speakers, papers)

### FR5: Participant Management Module
- **FR5.1** Session Registration: Participants can register for sessions
- **FR5.2** Deregister Session: Participants can unregister from sessions
- **FR5.3** View My Sessions: Participants can see registered sessions
- **FR5.4** Capacity Management: Enforce session capacity limits
- **FR5.5** Attendance Tracking: Record attendance (optional)

### FR6: Feedback System Module
- **FR6.1** Submit Feedback: Participants can rate sessions/speakers
- **FR6.2** Feedback Anonymity: Keep feedback anonymous
- **FR6.3** View Feedback Summary: Organizers can see aggregated feedback
- **FR6.4** Feedback Analytics: Generate feedback reports

### FR7: System Maintenance Module
- **FR7.1** Maintenance Mode Toggle: Enable/disable maintenance mode
- **FR7.2** Maintenance Messages: Display custom messages
- **FR7.3** System Logging: Log all system activities
- **FR7.4** Log Retrieval: Retrieve logs for auditing
- **FR7.5** Database Backup: Automated backup scheduling

### FR8: File Management Module
- **FR8.1** Upload Files: Support abstract/presentation uploads
- **FR8.2** Download Files: Users can download submissions
- **FR8.3** File Validation: Validate file types and sizes
- **FR8.4** File Storage: Secure storage with access control
- **FR8.5** File Deletion: Remove files with user permissions

### FR9: Notification System Module
- **FR9.1** Email Notifications: Send emails for events (submission, review, etc.)
- **FR9.2** In-App Notifications: Display system notifications
- **FR9.3** Notification Preferences: Users can manage notification settings
- **FR9.4** Reminder Notifications: Automatic reminders for deadlines

### FR10: Reporting & Analytics Module
- **FR10.1** Submission Reports: Generate submission statistics
- **FR10.2** Review Reports: Review completion and metrics
- **FR10.3** Attendance Reports: Attendance statistics
- **FR10.4** Export Reports: Export reports in CSV/PDF format

---

## Non-Functional Requirements (NFR)

### NFR1: Performance
- **NFR1.1** Response Time: API responses < 200ms (95th percentile)
- **NFR1.2** Database Queries: Indexed queries complete < 100ms
- **NFR1.3** Concurrent Users: Support 500+ concurrent users
- **NFR1.4** Throughput: Process 100+ requests/second

### NFR2: Scalability
- **NFR2.1** Horizontal Scaling: Support load balancing
- **NFR2.2** Database Scaling: Support read replicas
- **NFR2.3** File Storage: Scalable file storage solution
- **NFR2.4** API Versioning: Support multiple API versions

### NFR3: Security
- **NFR3.1** Authentication: JWT/OAuth2 token-based auth
- **NFR3.2** Authorization: Role-based access control (RBAC)
- **NFR3.3** Encryption: TLS 1.3 for data in transit
- **NFR3.4** Data Encryption: AES-256 for sensitive data at rest
- **NFR3.5** SQL Injection Prevention: Parameterized queries
- **NFR3.6** XSS Prevention: Input validation and output encoding
- **NFR3.7** CSRF Protection: CSRF tokens for state-changing operations
- **NFR3.8** Rate Limiting: API rate limiting per user/IP
- **NFR3.9** Audit Trail: Log all user activities

### NFR4: Availability
- **NFR4.1** Uptime: 99.5% service availability
- **NFR4.2** Recovery Time: RTO < 1 hour for critical failures
- **NFR4.3** Recovery Point: RPO < 15 minutes
- **NFR4.4** Health Checks: Continuous service health monitoring

### NFR5: Maintainability
- **NFR5.1** Code Quality: Cyclomatic complexity < 10
- **NFR5.2** Documentation: API documentation with Swagger/OpenAPI
- **NFR5.3** Testing: 70%+ code coverage with unit/integration tests
- **NFR5.4** Logging: Structured logging for debugging
- **NFR5.5** Error Handling: Graceful error handling with meaningful messages

### NFR6: Usability
- **NFR6.1** UI/UX: Responsive design (mobile-friendly)
- **NFR6.2** Accessibility: WCAG 2.1 Level AA compliance
- **NFR6.3** Localization: Multi-language support
- **NFR6.4** Help Documentation: User guides and FAQs

### NFR7: Data Integrity
- **NFR7.1** ACID Compliance: Database ACID properties
- **NFR7.2** Data Validation: Server-side validation
- **NFR7.3** Backup Strategy: Daily automated backups
- **NFR7.4** Data Retention: Comply with data privacy regulations (GDPR)

### NFR8: Interoperability
- **NFR8.1** REST API: RESTful API design
- **NFR8.2** JSON Format: JSON for data exchange
- **NFR8.3** Cross-Platform: Windows, Linux, macOS support

---

## Use Cases & User Scenarios

### Use Case Diagram Analysis

```
Conference Management System

Actors:
1. Organizer (extends User)
2. Author (extends User)
3. Reviewer (extends User)
4. Participant (extends User)
5. System Administrator
6. Email Service

Use Cases:

ORGANIZER:
- Manage Conference (Create, Edit, Delete Sessions)
- Manage Users (Add, Update, Delete, Assign Roles)
- Assign Reviews (Assign papers to reviewers)
- View Reports (Submission, Review, Attendance stats)
- System Maintenance (Enable maintenance mode, view logs)
- Manage Deadlines (Set submission/review deadlines)

AUTHOR:
- Register/Login
- Submit Abstract
- Submit Presentation
- Edit Submission (before deadline)
- Withdraw Submission
- View Submission Status
- View Reviews and Scores

REVIEWER:
- Register/Login
- View Assigned Submissions
- Download Abstract/Presentation
- Submit Review (with score and feedback)
- Track Review Deadlines
- View Review History

PARTICIPANT:
- Register/Login
- View Conference Schedule
- View Session Details
- Register for Sessions
- Deregister from Sessions
- Provide Session Feedback
- Download Conference Materials

SYSTEM ADMIN:
- Manage System Configuration
- Monitor System Health
- Manage Backups
- View System Logs
- Manage API Keys/Tokens
```

### Key User Scenarios

**Scenario 1: Author Submission Flow**
1. Author registers → Email verification → Login
2. Create new submission → Add abstract + presentation files
3. Submit for review → View submission status
4. Receive reviews → Accept/Reject decision notification
5. If accepted → Papers assigned to session

**Scenario 2: Reviewer Workflow**
1. Reviewer receives invitation → Email confirmation
2. Login → View assigned papers dashboard
3. Download paper → Read and evaluate
4. Fill review form → Submit score (1-5) and feedback
5. Track deadline progress

**Scenario 3: Organizer Session Management**
1. Login to organizer dashboard
2. Create conference → Set deadlines (submission, review, final)
3. Create session slots → Assign accepted papers
4. Manage user roles → Promote reviewers, authors, etc.
5. View analytics → Monitor submission/review progress
6. Send notifications → Email reminders to authors/reviewers

**Scenario 4: Participant Conference Experience**
1. Register as participant
2. Browse conference schedule
3. Register for 3-4 interesting sessions
4. Attend sessions (if physical) or view online
5. Provide feedback on sessions
6. Download conference materials

---

## UML Diagrams

### 1. Class Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                          USER (Abstract)                             │
├─────────────────────────────────────────────────────────────────────┤
│ - userId: int (PK)                                                   │
│ - email: string (UNIQUE)                                            │
│ - password: string (hashed)                                         │
│ - firstName: string                                                 │
│ - lastName: string                                                  │
│ - affiliation: string                                               │
│ - role: enum(ORGANIZER, AUTHOR, REVIEWER, PARTICIPANT, ADMIN)       │
│ - createdAt: timestamp                                              │
│ - updatedAt: timestamp                                              │
│ - isActive: boolean                                                 │
├─────────────────────────────────────────────────────────────────────┤
│ + register(): void                                                   │
│ + login(): Token                                                     │
│ + updateProfile(): void                                              │
│ + changePassword(): void                                             │
│ + getNotifications(): List<Notification>                             │
└─────────────────────────────────────────────────────────────────────┘
         ▲              ▲              ▲              ▲
         │              │              │              │
    ┌────┴──────┐  ┌──────┴──────┐ ┌──┴──────────┐ ┌┴──────────────┐
    │  Organizer│  │  Author     │ │ Reviewer   │ │ Participant   │
    └───────────┘  └─────────────┘ └────────────┘ └───────────────┘


┌─────────────────────────────────────────────────────────────────────┐
│                        SUBMISSION                                    │
├─────────────────────────────────────────────────────────────────────┤
│ - submissionId: int (PK)                                             │
│ - authorId: int (FK → User)                                         │
│ - title: string                                                     │
│ - abstract: text                                                    │
│ - presentationPath: string                                          │
│ - status: enum(DRAFT, SUBMITTED, UNDER_REVIEW, ACCEPTED, REJECTED) │
│ - submittedAt: timestamp                                            │
│ - updatedAt: timestamp                                              │
│ - conferenceId: int (FK → Conference)                               │
├─────────────────────────────────────────────────────────────────────┤
│ + submit(): void                                                     │
│ + edit(): void                                                       │
│ + withdraw(): void                                                   │
│ + getReviews(): List<Review>                                         │
│ + calculateAverageScore(): float                                     │
└─────────────────────────────────────────────────────────────────────┘


┌─────────────────────────────────────────────────────────────────────┐
│                          REVIEW                                      │
├─────────────────────────────────────────────────────────────────────┤
│ - reviewId: int (PK)                                                 │
│ - submissionId: int (FK → Submission)                               │
│ - reviewerId: int (FK → User)                                       │
│ - score: int (1-5)                                                   │
│ - feedback: text                                                    │
│ - submittedAt: timestamp                                            │
│ - isAnonymous: boolean                                              │
├─────────────────────────────────────────────────────────────────────┤
│ + submitReview(): void                                               │
│ + updateReview(): void                                               │
│ + getAuthorFeedback(): string (anonymized)                           │
└─────────────────────────────────────────────────────────────────────┘


┌─────────────────────────────────────────────────────────────────────┐
│                       CONFERENCE                                     │
├─────────────────────────────────────────────────────────────────────┤
│ - conferenceId: int (PK)                                             │
│ - organizerId: int (FK → User)                                      │
│ - name: string                                                      │
│ - description: text                                                 │
│ - startDate: date                                                   │
│ - endDate: date                                                     │
│ - location: string                                                  │
│ - submissionDeadline: date                                          │
│ - reviewDeadline: date                                              │
│ - isActive: boolean                                                 │
│ - createdAt: timestamp                                              │
├─────────────────────────────────────────────────────────────────────┤
│ + createSession(): void                                              │
│ + updateDeadline(): void                                             │
│ + getSessions(): List<Session>                                       │
│ + getStatistics(): ConferenceStats                                   │
└─────────────────────────────────────────────────────────────────────┘


┌─────────────────────────────────────────────────────────────────────┐
│                         SESSION                                      │
├─────────────────────────────────────────────────────────────────────┤
│ - sessionId: int (PK)                                                │
│ - conferenceId: int (FK → Conference)                               │
│ - title: string                                                     │
│ - description: text                                                 │
│ - startTime: timestamp                                              │
│ - endTime: timestamp                                                │
│ - location: string                                                  │
│ - capacity: int                                                     │
│ - chairId: int (FK → User, nullable)                                │
├─────────────────────────────────────────────────────────────────────┤
│ + addPaper(): void                                                   │
│ + removePaper(): void                                                │
│ + registerParticipant(): void                                        │
│ + deregisterParticipant(): void                                      │
│ + getParticipants(): List<User>                                      │
│ + isFull(): boolean                                                  │
└─────────────────────────────────────────────────────────────────────┘


┌─────────────────────────────────────────────────────────────────────┐
│                         FEEDBACK                                     │
├─────────────────────────────────────────────────────────────────────┤
│ - feedbackId: int (PK)                                               │
│ - sessionId: int (FK → Session)                                     │
│ - participantId: int (FK → User)                                    │
│ - rating: int (1-5)                                                  │
│ - comment: text                                                     │
│ - submittedAt: timestamp                                            │
│ - isAnonymous: boolean                                              │
├─────────────────────────────────────────────────────────────────────┤
│ + submitFeedback(): void                                             │
│ + updateFeedback(): void                                             │
│ + getAggregateStats(): FeedbackStats                                 │
└─────────────────────────────────────────────────────────────────────┘


┌─────────────────────────────────────────────────────────────────────┐
│                      NOTIFICATION                                    │
├─────────────────────────────────────────────────────────────────────┤
│ - notificationId: int (PK)                                           │
│ - userId: int (FK → User)                                           │
│ - type: enum(SUBMISSION, REVIEW, SESSION, FEEDBACK, SYSTEM)         │
│ - message: text                                                     │
│ - isRead: boolean                                                   │
│ - createdAt: timestamp                                              │
│ - relatedId: int (nullable, references relevant entity)             │
├─────────────────────────────────────────────────────────────────────┤
│ + markAsRead(): void                                                 │
│ + getUnreadCount(): int                                              │
│ + deleteNotification(): void                                         │
└─────────────────────────────────────────────────────────────────────┘


┌─────────────────────────────────────────────────────────────────────┐
│                       SYSTEM_LOG                                     │
├─────────────────────────────────────────────────────────────────────┤
│ - logId: int (PK)                                                    │
│ - userId: int (FK → User, nullable)                                 │
│ - action: enum(LOGIN, CREATE, UPDATE, DELETE, EXPORT)              │
│ - entity: string                                                    │
│ - details: text                                                     │
│ - ipAddress: string                                                 │
│ - timestamp: timestamp                                              │
│ - status: enum(SUCCESS, FAILURE)                                     │
├─────────────────────────────────────────────────────────────────────┤
│ + logAction(): void                                                  │
│ + getAuditTrail(): List<SystemLog>                                   │
│ + exportLogs(): File                                                 │
└─────────────────────────────────────────────────────────────────────┘
```

### 2. Sequence Diagram - Author Submission Flow

```
Author          UI            API Server      Database    File Service
  │              │              │               │             │
  │─ Registers ──→│              │               │             │
  │              │─ API Call ───→│               │             │
  │              │              │─ Validate ───→│             │
  │              │              │←─ User ID ────│             │
  │              │←─ Token ─────│               │             │
  │              │              │               │             │
  │─ Login ──────→│              │               │             │
  │              │─ API Call ───→│               │             │
  │              │              │─ Verify ─────→│             │
  │              │              │←─ OK ────────│             │
  │              │←─ JWT ───────│               │             │
  │              │              │               │             │
  │─ Submit ─────→│              │               │             │
  │  (Abstract)   │─ API Call ───→│               │             │
  │              │              │─ Create ─────→│             │
  │              │              │←─ Submission ─│             │
  │              │              │               │             │
  │─ Upload ─────→│              │               │       ┌────→│
  │(Presentation) │─ API Call ───→│               │       │     │
  │              │              │─ Validate ───→│       │     │
  │              │              │←─ OK ────────│       │     │
  │              │              │               │    Save File│
  │              │              │               │       │←────│
  │              │              │─ Update ─────→│       │     │
  │              │              │←─ OK ────────│       │     │
  │              │←─ Success ───│               │       │     │
  │              │              │               │       │     │
  │─ View Status─→│              │               │       │     │
  │              │─ API Call ───→│               │       │     │
  │              │              │─ Query ──────→│       │     │
  │              │              │←─ Status ────│       │     │
  │              │←─ Status ────│               │       │     │
  │              │              │               │       │     │

```

### 3. State Diagram - Submission Lifecycle

```
                          ┌──────────────┐
                          │ DRAFT        │
                          └──────────────┘
                                 │
                           [submit]
                                 │
                                 ▼
                          ┌──────────────┐
                          │ SUBMITTED    │◄─────────┐
                          └──────────────┘          │
                                 │              [edit]
                           [assign]                 │
                                 │                  │
                                 ▼                  │
                          ┌──────────────┐          │
                          │UNDER_REVIEW  │──────────┘
                          └──────────────┘  (before deadline)
                                 │
                    ┌────────────┬┴────────────┐
                    │            │            │
              [accept]    [reject]      [withdraw]
                    │            │            │
                    ▼            ▼            ▼
            ┌──────────────┐ ┌──────────────┐
            │  ACCEPTED    │ │   REJECTED   │
            └──────────────┘ └──────────────┘
                    │
              [assign_to_session]
                    │
                    ▼
            ┌──────────────┐
            │IN_SESSION    │
            └──────────────┘
```

---

## Data Flow Diagrams (DFD)

### Level 0 - Context Diagram

```
                          ┌─────────────────────────┐
                          │ Conference Management   │
                          │ System                  │
                          └─────────────────────────┘
                                     ▲
                    ┌────────────────┼────────────────┐
                    │                │                │
        ┌───────────▼───────────┐   │     ┌──────────▼──────────┐
        │ Email Service         │   │     │ File Storage Service │
        └───────────────────────┘   │     └─────────────────────┘
                                    │
        ┌──────────────┐     ┌──────▼──────┐     ┌──────────────┐
        │  Authors     │     │  Organizer  │     │   Reviewers  │
        └──────────────┘     └─────────────┘     └──────────────┘
                                    │
        ┌──────────────┐     ┌──────▼──────┐     ┌──────────────┐
        │  System      │     │ Participant │     │    Admin     │
        │  Admins      │     │             │     │              │
        └──────────────┘     └─────────────┘     └──────────────┘
```

### Level 1 - Main Processes

```
                    ┌─────────────────────────────────────┐
                    │   Conference Management System      │
                    └─────────────────────────────────────┘
                                   │
                ┌──────────────┬────┼────┬──────────────┬──────────────┐
                │              │        │              │              │
                ▼              ▼        ▼              ▼              ▼
        ┌──────────────┐  ┌──────────────────┐  ┌──────────────┐  ┌──────────────┐
        │1.User        │  │2.Submission      │  │3.Review      │  │4.Session     │
        │Management    │  │Management        │  │Management    │  │Management    │
        └──────────────┘  └──────────────────┘  └──────────────┘  └──────────────┘
                │              │                      │              │
                │              │                      │              │
        ┌──────────────┐  ┌──────────────────┐  ┌──────────────┐  ┌──────────────┐
        │5.Feedback    │  │6.Notification   │  │7.Logging &  │  │8.File        │
        │Management    │  │System           │  │Maintenance  │  │Management    │
        └──────────────┘  └──────────────────┘  └──────────────┘  └──────────────┘

Data Stores:
    D1: User Database
    D2: Submission Database
    D3: Review Database
    D4: Session Database
    D5: Feedback Database
    D6: Notification Queue
    D7: System Logs
    D8: File Storage
```

### Level 2 - Submission Management Detail

```
                    Author/Reviewer            System
                          │                      │
                          │ Submit Abstract      │
                          ├─────────────────────→│
                          │                      │ Validate
                          │                      ├──────→ D2 (Save)
                          │  Confirmation        │
                          │←─────────────────────┤
                          │                      │
                          │ Upload Presentation  │
                          ├─────────────────────→│
                          │                      │ Validate File
                          │                      ├──────→ D8 (Store)
                          │                      │ Update D2
                          │  Upload Confirmed    │
                          │←─────────────────────┤
                          │                      │ Send Notification
                          │                      ├──────→ D6 (Queue)
                          │                      │
                          │   Email Notification │
                          │←─────────────────────┤
                          │                      │
                          │  View Status         │
                          ├─────────────────────→│
                          │                      │ Query D2
                          │  Status & Reviews    │
                          │←─────────────────────┤
```

---

## Database Schema

### Entity-Relationship Diagram (Conceptual)

```
USER (1) ───────────────── (M) SUBMISSION
  │                                  │
  │                                  │ (1)
  │ (M)                              │
  └─ (1) CONFERENCE             (M) REVIEW
       │                             │
       │ (1)                         │ (M)
       │                    USER (1) │
  (M) SESSION                        
       │
       │ (1)
  (M) SESSION_PARTICIPANT ←─ (M) USER
       │
  (1)  │
       └─ (M) FEEDBACK

  └─ (1) CONFERENCE (1) ──── (M) SYSTEM_LOG

NOTIFICATION ──(M)─→ USER (1)
```

### SQL Schema

```sql
-- Users Table
CREATE TABLE users (
    user_id INT PRIMARY KEY AUTO_INCREMENT,
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    first_name VARCHAR(100),
    last_name VARCHAR(100),
    affiliation VARCHAR(255),
    role ENUM('ORGANIZER', 'AUTHOR', 'REVIEWER', 'PARTICIPANT', 'ADMIN') NOT NULL,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_email (email),
    INDEX idx_role (role)
);

-- Conferences Table
CREATE TABLE conferences (
    conference_id INT PRIMARY KEY AUTO_INCREMENT,
    organizer_id INT NOT NULL,
    name VARCHAR(255) NOT NULL,
    description TEXT,
    start_date DATE,
    end_date DATE,
    location VARCHAR(255),
    submission_deadline DATE,
    review_deadline DATE,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (organizer_id) REFERENCES users(user_id),
    INDEX idx_organizer_id (organizer_id),
    INDEX idx_dates (start_date, end_date)
);

-- Submissions Table
CREATE TABLE submissions (
    submission_id INT PRIMARY KEY AUTO_INCREMENT,
    conference_id INT NOT NULL,
    author_id INT NOT NULL,
    title VARCHAR(255) NOT NULL,
    abstract TEXT NOT NULL,
    presentation_file_path VARCHAR(255),
    status ENUM('DRAFT', 'SUBMITTED', 'UNDER_REVIEW', 'ACCEPTED', 'REJECTED') DEFAULT 'DRAFT',
    submitted_at TIMESTAMP NULL,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (conference_id) REFERENCES conferences(conference_id),
    FOREIGN KEY (author_id) REFERENCES users(user_id),
    INDEX idx_conference_id (conference_id),
    INDEX idx_author_id (author_id),
    INDEX idx_status (status)
);

-- Reviews Table
CREATE TABLE reviews (
    review_id INT PRIMARY KEY AUTO_INCREMENT,
    submission_id INT NOT NULL,
    reviewer_id INT NOT NULL,
    score INT CHECK (score >= 1 AND score <= 5),
    feedback TEXT,
    is_anonymous BOOLEAN DEFAULT TRUE,
    submitted_at TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE KEY unique_review (submission_id, reviewer_id),
    FOREIGN KEY (submission_id) REFERENCES submissions(submission_id),
    FOREIGN KEY (reviewer_id) REFERENCES users(user_id),
    INDEX idx_submission_id (submission_id),
    INDEX idx_reviewer_id (reviewer_id)
);

-- Sessions Table
CREATE TABLE sessions (
    session_id INT PRIMARY KEY AUTO_INCREMENT,
    conference_id INT NOT NULL,
    title VARCHAR(255) NOT NULL,
    description TEXT,
    start_time TIMESTAMP,
    end_time TIMESTAMP,
    location VARCHAR(255),
    capacity INT,
    chair_id INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (conference_id) REFERENCES conferences(conference_id),
    FOREIGN KEY (chair_id) REFERENCES users(user_id),
    INDEX idx_conference_id (conference_id),
    INDEX idx_start_time (start_time)
);

-- Session Participants Table
CREATE TABLE session_participants (
    participant_id INT PRIMARY KEY AUTO_INCREMENT,
    session_id INT NOT NULL,
    user_id INT NOT NULL,
    registered_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    attended BOOLEAN DEFAULT FALSE,
    UNIQUE KEY unique_registration (session_id, user_id),
    FOREIGN KEY (session_id) REFERENCES sessions(session_id),
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    INDEX idx_session_id (session_id),
    INDEX idx_user_id (user_id)
);

-- Session Papers Table (for assigning accepted submissions to sessions)
CREATE TABLE session_papers (
    session_paper_id INT PRIMARY KEY AUTO_INCREMENT,
    session_id INT NOT NULL,
    submission_id INT NOT NULL,
    presentation_order INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY unique_assignment (session_id, submission_id),
    FOREIGN KEY (session_id) REFERENCES sessions(session_id),
    FOREIGN KEY (submission_id) REFERENCES submissions(submission_id),
    INDEX idx_session_id (session_id)
);

-- Feedback Table
CREATE TABLE feedback (
    feedback_id INT PRIMARY KEY AUTO_INCREMENT,
    session_id INT NOT NULL,
    participant_id INT NOT NULL,
    rating INT CHECK (rating >= 1 AND rating <= 5),
    comment TEXT,
    is_anonymous BOOLEAN DEFAULT TRUE,
    submitted_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (session_id) REFERENCES sessions(session_id),
    FOREIGN KEY (participant_id) REFERENCES users(user_id),
    INDEX idx_session_id (session_id),
    INDEX idx_participant_id (participant_id)
);

-- Notifications Table
CREATE TABLE notifications (
    notification_id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL,
    type ENUM('SUBMISSION', 'REVIEW', 'SESSION', 'FEEDBACK', 'SYSTEM') NOT NULL,
    message TEXT NOT NULL,
    is_read BOOLEAN DEFAULT FALSE,
    related_id INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    INDEX idx_user_id (user_id),
    INDEX idx_is_read (is_read),
    INDEX idx_created_at (created_at)
);

-- System Logs Table
CREATE TABLE system_logs (
    log_id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT,
    action VARCHAR(50),
    entity VARCHAR(50),
    details TEXT,
    ip_address VARCHAR(45),
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    status ENUM('SUCCESS', 'FAILURE') DEFAULT 'SUCCESS',
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    INDEX idx_timestamp (timestamp),
    INDEX idx_user_id (user_id),
    INDEX idx_action (action)
);

-- File Management Table
CREATE TABLE file_uploads (
    file_id INT PRIMARY KEY AUTO_INCREMENT,
    submission_id INT,
    uploaded_by INT NOT NULL,
    original_filename VARCHAR(255),
    stored_path VARCHAR(255),
    file_type VARCHAR(50),
    file_size BIGINT,
    uploaded_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (submission_id) REFERENCES submissions(submission_id),
    FOREIGN KEY (uploaded_by) REFERENCES users(user_id),
    INDEX idx_submission_id (submission_id)
);

-- System Configuration Table
CREATE TABLE system_config (
    config_id INT PRIMARY KEY AUTO_INCREMENT,
    config_key VARCHAR(100) UNIQUE NOT NULL,
    config_value TEXT,
    description VARCHAR(255),
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);
```

---

## System Architecture

### High-Level Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                        CLIENT LAYER                                  │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │  Web UI (HTML/CSS/JavaScript - Responsive)                   │   │
│  │  Mobile UI (Optional React/Vue)                              │   │
│  └──────────────────────────────────────────────────────────────┘   │
└────────────────────┬────────────────────────────────────────────────┘
                     │ HTTP/HTTPS
┌────────────────────▼────────────────────────────────────────────────┐
│                    API GATEWAY LAYER (C++ Dogon)                    │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │ - Request Routing & Load Balancing                           │   │
│  │ - Authentication (JWT/OAuth2)                                │   │
│  │ - Rate Limiting & CORS                                       │   │
│  │ - API Versioning                                             │   │
│  │ - Request/Response Validation                                │   │
│  └──────────────────────────────────────────────────────────────┘   │
└────────────────────┬────────────────────────────────────────────────┘
                     │ Internal APIs
┌────────────────────▼────────────────────────────────────────────────┐
│                  BUSINESS LOGIC LAYER (C++)                          │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │  Service Modules (Each File = One Feature)                  │    │
│  │  ┌──────────────────────────────────────────────────────┐   │    │
│  │  │ 1. UserService          - Registration, Auth, Roles  │   │    │
│  │  │ 2. SubmissionService    - Create, Edit, Withdraw    │   │    │
│  │  │ 3. ReviewService        - Assignment, Submission     │   │    │
│  │  │ 4. SessionService       - Create, Schedule Sessions  │   │    │
│  │  │ 5. ParticipantService   - Registration, Attendance   │   │    │
│  │  │ 6. FeedbackService      - Collect, Aggregate         │   │    │
│  │  │ 7. NotificationService  - Send Emails/In-app         │   │    │
│  │  │ 8. FileService          - Upload, Download, Validate │   │    │
│  │  │ 9. LoggingService       - Audit Trail, Logs          │   │    │
│  │  │ 10. ReportingService    - Analytics, Export           │   │    │
│  │  │ 11. ConfigService       - System Settings             │   │    │
│  │  │ 12. AuthService         - Token Generation, Validation│   │    │
│  │  └──────────────────────────────────────────────────────┘   │    │
│  └─────────────────────────────────────────────────────────────┘    │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │  Utility Modules                                            │    │
│  │  - ValidationUtils      - Input validation               │    │
│  │  - EncryptionUtils      - Encryption/Decryption          │    │
│  │  - DateTimeUtils        - Date/Time operations           │    │
│  │  - FileUtils            - File operations                │    │
│  │  - EmailUtils           - Email sending                  │    │
│  │  - JSONUtils            - JSON parsing/serialization     │    │
│  └─────────────────────────────────────────────────────────────┘    │
└────────────────────┬────────────────────────────────────────────────┘
                     │ SQL Queries
┌────────────────────▼────────────────────────────────────────────────┐
│                     DATA ACCESS LAYER                                │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │  ORM/Query Builder (C++ Database Library)                   │   │
│  │  - Connection Pooling                                      │   │
│  │  - Query Caching                                           │   │
│  │  - Transaction Management                                 │   │
│  │  - Prepared Statements (SQL Injection Prevention)          │   │
│  └──────────────────────────────────────────────────────────────┘   │
└────────────────────┬────────────────────────────────────────────────┘
                     │ JDBC/Native
┌────────────────────▼────────────────────────────────────────────────┐
│                      DATABASE LAYER                                  │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │  MySQL / PostgreSQL (Primary Database)                      │   │
│  │  - User Management                                          │   │
│  │  - Submission Management                                    │   │
│  │  - Review Management                                        │   │
│  │  - Session Management                                       │   │
│  │  - Notifications & Logs                                     │   │
│  │  - Read Replicas for Analytics                              │   │
│  └──────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────┘

                     ┌──────────────────────────────────┐
                     │  EXTERNAL SERVICES               │
                     ├──────────────────────────────────┤
                     │ - SMTP (Email Service)           │
                     │ - File Storage (AWS S3/Local)    │
                     │ - Cache (Redis)                  │
                     │ - Message Queue (RabbitMQ/Kafka) │
                     │ - Logging (ELK Stack)            │
                     └──────────────────────────────────┘
```

### Deployment Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    LOAD BALANCER (Nginx)                        │
│  - SSL/TLS Termination                                         │
│  - Request Routing                                             │
│  - Rate Limiting                                               │
└──────────────────┬──────────────────────────────────────────────┘
                   │
        ┌──────────┼──────────┐
        │          │          │
    ┌───▼──┐  ┌───▼──┐  ┌───▼──┐
    │API 1 │  │API 2 │  │API 3 │  (Multiple Instances)
    └───┬──┘  └───┬──┘  └───┬──┘  (Container/VM)
        │         │         │
        └────┬────┴────┬────┘
             │         │
        ┌────▼──┐  ┌──▼─────┐
        │Cache  │  │Message  │
        │(Redis)│  │Queue    │
        └───────┘  └─────────┘
             │         │
             └────┬────┘
                  │
        ┌─────────▼──────────┐
        │  Database Cluster  │
        │ (MySQL + Replicas) │
        └────────────────────┘
```

---

## Modular Structure

### C++ Project Structure (Feature-Based Modular Approach)

```
conference-management-system/
├── CMakeLists.txt                          # Build configuration
├── conanfile.txt                           # Dependencies (Dogon, MySQL, etc.)
│
├── include/                                # Public headers
│   ├── models/
│   │   ├── User.h
│   │   ├── Submission.h
│   │   ├── Review.h
│   │   ├── Session.h
│   │   ├── Feedback.h
│   │   ├── Notification.h
│   │   └── SystemLog.h
│   │
│   ├── services/
│   │   ├── IUserService.h
│   │   ├── ISubmissionService.h
│   │   ├── IReviewService.h
│   │   ├── ISessionService.h
│   │   ├── IParticipantService.h
│   │   ├── IFeedbackService.h
│   │   ├── INotificationService.h
│   │   ├── IFileService.h
│   │   ├── ILoggingService.h
│   │   ├── IReportingService.h
│   │   ├── IAuthService.h
│   │   └── IConfigService.h
│   │
│   ├── repositories/
│   │   ├── IUserRepository.h
│   │   ├── ISubmissionRepository.h
│   │   ├── IReviewRepository.h
│   │   ├── ISessionRepository.h
│   │   ├── IFeedbackRepository.h
│   │   └── INotificationRepository.h
│   │
│   ├── utils/
│   │   ├── ValidationUtils.h
│   │   ├── EncryptionUtils.h
│   │   ├── DateTimeUtils.h
│   │   ├── FileUtils.h
│   │   ├── EmailUtils.h
│   │   ├── JSONUtils.h
│   │   └── Constants.h
│   │
│   ├── controllers/
│   │   ├── UserController.h
│   │   ├── SubmissionController.h
│   │   ├── ReviewController.h
│   │   ├── SessionController.h
│   │   ├── ParticipantController.h
│   │   ├── FeedbackController.h
│   │   └── ReportController.h
│   │
│   └── middleware/
│       ├── AuthMiddleware.h
│       ├── ErrorHandler.h
│       ├── CORSMiddleware.h
│       ├── ValidationMiddleware.h
│       └── LoggingMiddleware.h
│
├── src/                                    # Implementation
│   ├── models/
│   │   ├── User.cpp
│   │   ├── Submission.cpp
│   │   ├── Review.cpp
│   │   ├── Session.cpp
│   │   ├── Feedback.cpp
│   │   ├── Notification.cpp
│   │   └── SystemLog.cpp
│   │
│   ├── services/                          # ONE FILE PER SERVICE
│   │   ├── UserService.cpp                # User registration, auth, profile
│   │   ├── SubmissionService.cpp          # Submission CRUD, workflow
│   │   ├── ReviewService.cpp              # Review assignment, submission
│   │   ├── SessionService.cpp             # Session creation, scheduling
│   │   ├── ParticipantService.cpp         # Participant registration
│   │   ├── FeedbackService.cpp            # Feedback collection, analytics
│   │   ├── NotificationService.cpp        # Email, in-app notifications
│   │   ├── FileService.cpp                # File upload, download, storage
│   │   ├── LoggingService.cpp             # Audit trail, system logs
│   │   ├── ReportingService.cpp           # Analytics, report generation
│   │   ├── AuthService.cpp                # JWT, token management
│   │   └── ConfigService.cpp              # System configuration
│   │
│   ├── repositories/
│   │   ├── UserRepository.cpp             # User data access
│   │   ├── SubmissionRepository.cpp       # Submission data access
│   │   ├── ReviewRepository.cpp           # Review data access
│   │   ├── SessionRepository.cpp          # Session data access
│   │   ├── FeedbackRepository.cpp         # Feedback data access
│   │   └── NotificationRepository.cpp     # Notification data access
│   │
│   ├── utils/
│   │   ├── ValidationUtils.cpp
│   │   ├── EncryptionUtils.cpp
│   │   ├── DateTimeUtils.cpp
│   │   ├── FileUtils.cpp
│   │   ├── EmailUtils.cpp
│   │   └── JSONUtils.cpp
│   │
│   ├── controllers/
│   │   ├── UserController.cpp
│   │   ├── SubmissionController.cpp
│   │   ├── ReviewController.cpp
│   │   ├── SessionController.cpp
│   │   ├── ParticipantController.cpp
│   │   ├── FeedbackController.cpp
│   │   └── ReportController.cpp
│   │
│   ├── middleware/
│   │   ├── AuthMiddleware.cpp
│   │   ├── ErrorHandler.cpp
│   │   ├── CORSMiddleware.cpp
│   │   ├── ValidationMiddleware.cpp
│   │   └── LoggingMiddleware.cpp
│   │
│   ├── database/
│   │   ├── DatabaseConnection.cpp         # Connection pooling
│   │   ├── DatabaseMigration.cpp          # Schema migration
│   │   └── Transaction.cpp                # Transaction management
│   │
│   └── main.cpp                           # Application entry point
│
├── frontend/                               # Web UI (Separate)
│   ├── public/
│   │   ├── index.html
│   │   ├── css/
│   │   ├── js/
│   │   └── images/
│   │
│   ├── src/
│   │   ├── components/
│   │   ├── pages/
│   │   ├── services/
│   │   └── utils/
│   │
│   ├── package.json
│   └── webpack.config.js
│
├── tests/                                  # Unit & Integration tests
│   ├── unit/
│   │   ├── UserServiceTest.cpp
│   │   ├── SubmissionServiceTest.cpp
│   │   ├── ReviewServiceTest.cpp
│   │   └── ...
│   │
│   └── integration/
│       ├── APIEndpointTest.cpp
│       ├── DatabaseTest.cpp
│       └── ...
│
├── config/
│   ├── development.json                   # Development config
│   ├── production.json                    # Production config
│   ├── database.sql                       # Database schema
│   └── email_templates/                   # Email templates
│
├── docs/
│   ├── API.md                             # API documentation
│   ├── ARCHITECTURE.md                    # Architecture guide
│   ├── DEPLOYMENT.md                      # Deployment guide
│   └── DEVELOPMENT.md                     # Development guide
│
└── README.md                              # Project README
```

### Key Principles for Modular Structure:
1. **One Feature Per File** - Each service/utility serves a single purpose
2. **Interface-Based Design** - Use abstract classes (IUserService, etc.)
3. **Dependency Injection** - Inject dependencies via constructors
4. **Clear Separation** - Models, Services, Repositories, Controllers are distinct
5. **No Cross-Feature Dependencies** - Services are independent
6. **Comprehensive Testing** - Each module tested independently

---

## API Design

### RESTful API Endpoints

#### Authentication
```
POST   /api/v1/auth/register          - User registration
POST   /api/v1/auth/login             - User login (returns JWT)
POST   /api/v1/auth/refresh-token     - Refresh JWT token
POST   /api/v1/auth/logout            - User logout
POST   /api/v1/auth/forgot-password   - Request password reset
POST   /api/v1/auth/reset-password    - Reset password with token
```

#### Users
```
GET    /api/v1/users/{userId}         - Get user profile
PUT    /api/v1/users/{userId}         - Update user profile
DELETE /api/v1/users/{userId}         - Deactivate user (soft delete)
GET    /api/v1/users                  - List users (Admin only)
POST   /api/v1/users                  - Create user (Admin only)
PUT    /api/v1/users/{userId}/role    - Update user role (Admin only)
GET    /api/v1/users/{userId}/notifications - Get user notifications
```

#### Submissions
```
POST   /api/v1/submissions            - Create submission
GET    /api/v1/submissions            - List submissions
GET    /api/v1/submissions/{subId}    - Get submission details
PUT    /api/v1/submissions/{subId}    - Edit submission
DELETE /api/v1/submissions/{subId}    - Withdraw submission
GET    /api/v1/submissions/{subId}/reviews - Get submission reviews
POST   /api/v1/submissions/{subId}/file/upload - Upload presentation
GET    /api/v1/submissions/{subId}/file/download - Download presentation
```

#### Reviews
```
POST   /api/v1/reviews                - Submit review
GET    /api/v1/reviews/{reviewId}     - Get review details
PUT    /api/v1/reviews/{reviewId}     - Update review
GET    /api/v1/reviews                - List assigned reviews
POST   /api/v1/reviews/assign         - Assign review (Organizer only)
GET    /api/v1/reviews/statistics     - Review statistics (Organizer only)
```

#### Sessions
```
POST   /api/v1/conferences/{confId}/sessions - Create session
GET    /api/v1/conferences/{confId}/sessions - List conference sessions
GET    /api/v1/sessions/{sessionId}   - Get session details
PUT    /api/v1/sessions/{sessionId}   - Update session
DELETE /api/v1/sessions/{sessionId}   - Delete session
POST   /api/v1/sessions/{sessionId}/papers - Add paper to session
DELETE /api/v1/sessions/{sessionId}/papers/{paperId} - Remove paper
GET    /api/v1/sessions/{sessionId}/participants - Get participants
```

#### Participants
```
POST   /api/v1/sessions/{sessionId}/register - Register for session
DELETE /api/v1/sessions/{sessionId}/register - Unregister from session
GET    /api/v1/users/{userId}/sessions - Get user's sessions
```

#### Feedback
```
POST   /api/v1/sessions/{sessionId}/feedback - Submit feedback
GET    /api/v1/sessions/{sessionId}/feedback/summary - Get feedback summary
GET    /api/v1/feedback                - List user feedback (Organizer)
```

#### Reports
```
GET    /api/v1/reports/submissions    - Submission report
GET    /api/v1/reports/reviews        - Review report
GET    /api/v1/reports/attendance     - Attendance report
GET    /api/v1/reports/feedback       - Feedback report
GET    /api/v1/reports/export         - Export report (CSV/PDF)
```

#### Conferences
```
POST   /api/v1/conferences            - Create conference (Organizer)
GET    /api/v1/conferences            - List conferences
GET    /api/v1/conferences/{confId}   - Get conference details
PUT    /api/v1/conferences/{confId}   - Update conference
DELETE /api/v1/conferences/{confId}   - Delete conference
GET    /api/v1/conferences/{confId}/statistics - Get statistics
```

### API Response Format

**Success Response (200):**
```json
{
  "success": true,
  "data": {
    // Response data
  },
  "message": "Operation successful"
}
```

**Error Response (400/500):**
```json
{
  "success": false,
  "error": {
    "code": "VALIDATION_ERROR",
    "message": "Invalid input",
    "details": [
      {
        "field": "email",
        "message": "Invalid email format"
      }
    ]
  }
}
```

---

## Technology Stack

### Backend (C++)
| Component | Technology | Version | Purpose |
|-----------|-----------|---------|---------|
| Framework | Dogon Framework | Latest | Web framework for HTTP handling |
| Build Tool | CMake | 3.15+ | Build automation |
| Package Manager | Conan | 2.x | Dependency management |
| Database Driver | libpq (PostgreSQL C++ Adapter) | 13.0+ | PostgreSQL connectivity |
| JSON Library | nlohmann/json | 3.11+ | JSON parsing/serialization |
| Encryption | OpenSSL | 1.1.1+ | SSL/TLS, cryptography |
| Logging | spdlog | 1.x | Structured logging |
| Testing | Google Test | Latest | Unit testing framework |
| HTTP Client | cpp-httplib | Latest | HTTP requests (internal APIs) |
| Connection Pool | Boost.Pool | Latest | Database connection pooling |
| Threading | std::thread, Boost.Asio | C++17 | Async operations |

### Frontend (Web)
| Component | Technology | Version | Purpose |
|-----------|-----------|---------|---------|
| Framework | Next.js | 14.0+ | React framework with SSR/SSG |
| CSS | Tailwind CSS | 3.3+ | Utility-first CSS framework |
| UI Components | Headless UI / Radix UI | Latest | Accessible UI components |
| HTTP Client | Fetch API / Axios | Latest | API communication |
| State Management | React Context / Zustand | Latest | State management |
| Build Tool | Next.js Built-in | - | Module bundling (integrated) |

### Database
| Component | Technology | Version | Purpose |
|-----------|-----------|---------|---------|
| Primary DB | PostgreSQL | 13.0+ | Data storage with advanced features |
| Read Replica | PostgreSQL | 13.0+ | Analytics/Reporting |
| Connection Pool | pgBouncer / Boost.Pool | - | Connection management |
| Backup | pg_dump / pg_basebackup | - | Database backup |
| Replication | Streaming Replication | - | Master-slave replication |

### DevOps & Deployment
| Component | Technology | Purpose |
|-----------|-----------|---------|
| Containerization | Docker | Container deployment |
| Orchestration | Docker Compose | Local multi-container setup |
| Load Balancer | Nginx | Request routing, SSL termination |
| CI/CD | GitHub Actions | Automated testing & deployment |
| Monitoring | Prometheus + Grafana | Performance monitoring |
| Logging | ELK Stack (Elasticsearch, Logstash, Kibana) | Centralized logging |
| Version Control | Git/GitHub | Source code management |

### External Services
| Service | Purpose | Integration |
|---------|---------|-------------|
| SMTP Server | Email notifications | SMTP library in C++ |
| AWS S3 / MinIO | File storage | HTTP API |
| Redis | Caching | C++ Redis client library |
| RabbitMQ / Kafka | Message queue (async tasks) | C++ client library |

---

## C++ to JavaScript Communication Architecture

### Overview
The C++ backend and Next.js frontend communicate through **RESTful HTTP/HTTPS APIs** using **JSON** for data exchange. This is a standard client-server architecture with clear separation of concerns.

### Communication Flow

```
┌────────────────────────────────────────────────────────────────┐
│                    Next.js Frontend (JavaScript)                │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ - React Components                                       │  │
│  │ - Fetch API / Axios HTTP Client                          │  │
│  │ - State Management (React Context / Zustand)             │  │
│  │ - Tailwind CSS Styling                                   │  │
│  └──────────────────────────────────────────────────────────┘  │
│                              │                                   │
│                              │ HTTP/HTTPS Requests               │
│                              │ JSON Data                         │
│                              │ JWT Token (Authorization Header)  │
│                              ▼                                   │
└────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────┐
│                    C++ Backend (Dogon Framework)                │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ - HTTP Request Handler (Dogon)                           │  │
│  │ - JWT Token Validation (AuthMiddleware)                  │  │
│  │ - Request Routing & Processing                           │  │
│  │ - Business Logic (Services)                              │  │
│  │ - Database Operations (PostgreSQL)                       │  │
│  │ - JSON Response Serialization                            │  │
│  └──────────────────────────────────────────────────────────┘  │
│                              │                                   │
│                              │ HTTP/HTTPS Responses              │
│                              │ JSON Data                         │
│                              │ HTTP Status Codes                 │
│                              ▼                                   │
└────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────┐
│                  PostgreSQL Database                            │
│  - Persistent Data Storage                                     │
│  - ACID Transactions                                           │
└────────────────────────────────────────────────────────────────┘
```

### Detailed Communication Mechanism

#### 1. **Frontend to Backend Request**
```javascript
// Next.js Frontend (React Component)
async function submitPaper() {
  const response = await fetch(
    'https://api.conference.local/api/v1/submissions',
    {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${jwtToken}`,  // JWT Token
        'X-Request-ID': requestId,
      },
      body: JSON.stringify({
        title: 'Paper Title',
        abstract: 'Paper abstract...',
        conferenceId: 1
      })
    }
  );
  
  const data = await response.json();
  return data;
}
```

#### 2. **Backend Processing (C++ with Dogon)**
```cpp
// C++ Backend Handler
class SubmissionController {
public:
  void createSubmission(Request &req, Response &res) {
    // 1. Extract & Validate JWT Token from headers
    auto token = req.headers["Authorization"];
    if (!AuthService::validateToken(token)) {
      res.setStatusCode(401);
      res.json({{"error", "Unauthorized"}});
      return;
    }
    
    // 2. Parse JSON Request Body
    auto body = req.getJsonBody();
    string title = body["title"];
    string abstract = body["abstract"];
    int conferenceId = body["conferenceId"];
    
    // 3. Validate Input
    if (!ValidationUtils::validateTitle(title)) {
      res.setStatusCode(400);
      res.json({{"error", "Invalid title"}});
      return;
    }
    
    // 4. Call Business Logic Service
    auto submission = SubmissionService::createSubmission(
      title, 
      abstract, 
      conferenceId
    );
    
    // 5. Serialize Response to JSON
    res.setStatusCode(201);
    res.setHeader("Content-Type", "application/json");
    res.json({
      {"success", true},
      {"data", submission.toJSON()},
      {"message", "Submission created successfully"}
    });
  }
};
```

#### 3. **Backend Response to Frontend**
```json
{
  "success": true,
  "data": {
    "submissionId": 123,
    "title": "Paper Title",
    "abstract": "Paper abstract...",
    "status": "DRAFT",
    "createdAt": "2026-05-05T10:30:00Z"
  },
  "message": "Submission created successfully"
}
```

#### 4. **Frontend Processing**
```javascript
// Next.js Frontend - Handling Response
try {
  const data = await submitPaper();
  
  if (data.success) {
    // Update local state
    setSubmission(data.data);
    
    // Show success message
    showNotification('Paper submitted successfully!');
    
    // Redirect user
    router.push(`/submissions/${data.data.submissionId}`);
  } else {
    // Show error message
    showError(data.error.message);
  }
} catch (error) {
  console.error('API Error:', error);
  showError('Failed to submit paper');
}
```

### Key Communication Patterns

#### Pattern 1: Authentication (JWT)
```
1. User logs in via /auth/login endpoint
2. Backend validates credentials
3. Backend generates JWT token: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
4. Frontend stores JWT in localStorage/sessionStorage
5. Frontend includes JWT in Authorization header for all subsequent requests
6. Backend validates JWT token in AuthMiddleware before processing requests
```

#### Pattern 2: File Upload
```
Frontend (Next.js):
1. User selects file via HTML form
2. Create FormData object
3. POST to /api/v1/submissions/{id}/file/upload with file

Backend (C++):
1. Receive multipart form data
2. Validate file type and size
3. Save file to storage service
4. Update database with file reference
5. Return success response with file metadata
```

#### Pattern 3: Real-Time Notifications (Optional Enhancement)
```
Current: Polling with fetch() every N seconds
Future: WebSocket upgrade for real-time updates

// Next.js WebSocket client
const ws = new WebSocket('wss://api.conference.local/api/v1/notifications');
ws.onmessage = (event) => {
  const notification = JSON.parse(event.data);
  setNotifications(prev => [notification, ...prev]);
};
```

#### Pattern 4: Error Handling
```
Backend Returns:
{
  "success": false,
  "error": {
    "code": "SUBMISSION_DEADLINE_PASSED",
    "message": "Cannot submit after deadline",
    "details": {
      "deadline": "2026-04-30T23:59:59Z",
      "currentTime": "2026-05-05T10:30:00Z"
    }
  }
}

Frontend Handles:
- HTTP 401: Redirect to login
- HTTP 403: Show permission denied error
- HTTP 400: Show validation errors
- HTTP 500: Show server error message
- Network error: Show "No internet connection" message
```

### Technology Details

#### Frontend Axios Setup
```javascript
// Next.js API client configuration
import axios from 'axios';

const apiClient = axios.create({
  baseURL: process.env.NEXT_PUBLIC_API_URL || 'http://localhost:8080/api/v1',
  headers: {
    'Content-Type': 'application/json',
  },
});

// Add JWT token to all requests
apiClient.interceptors.request.use((config) => {
  const token = localStorage.getItem('jwtToken');
  if (token) {
    config.headers.Authorization = `Bearer ${token}`;
  }
  return config;
});

// Handle response errors
apiClient.interceptors.response.use(
  (response) => response,
  (error) => {
    if (error.response?.status === 401) {
      // Redirect to login
      window.location.href = '/login';
    }
    return Promise.reject(error);
  }
);

export default apiClient;
```

#### C++ HTTP Server Setup (Dogon)
```cpp
// C++ backend HTTP server
#include <dogon/server.hpp>
#include <dogon/request.hpp>
#include <dogon/response.hpp>

int main() {
  dogon::Server server;
  
  // Enable CORS for Next.js frontend
  server.enableCORS({
    .allowOrigin = "http://localhost:3000", // Next.js dev server
    .allowMethods = {"GET", "POST", "PUT", "DELETE"},
    .allowHeaders = {"Content-Type", "Authorization"}
  });
  
  // API Endpoints
  server.post("/api/v1/submissions", SubmissionController::create);
  server.get("/api/v1/submissions/:id", SubmissionController::getById);
  server.put("/api/v1/submissions/:id", SubmissionController::update);
  server.delete("/api/v1/submissions/:id", SubmissionController::remove);
  
  // Middleware
  server.use(LoggingMiddleware::handler);
  server.use(AuthMiddleware::handler);
  server.use(ErrorHandler::handler);
  
  // Start server on port 8080
  server.listen(8080, [](const std::string& addr) {
    std::cout << "Server running on " << addr << std::endl;
  });
  
  return 0;
}
```

### Data Format Examples

#### User Registration Request-Response
```
Frontend Request:
POST /api/v1/auth/register
Content-Type: application/json

{
  "email": "author@example.com",
  "password": "SecurePassword123",
  "firstName": "John",
  "lastName": "Doe",
  "affiliation": "MIT",
  "role": "AUTHOR"
}

Backend Response:
{
  "success": true,
  "data": {
    "userId": 42,
    "email": "author@example.com",
    "firstName": "John",
    "lastName": "Doe",
    "role": "AUTHOR",
    "createdAt": "2026-05-05T10:30:00Z"
  },
  "message": "User registered successfully"
}
```

#### Submission List Request-Response
```
Frontend Request:
GET /api/v1/submissions?conferenceId=1&status=SUBMITTED
Authorization: Bearer <JWT_TOKEN>

Backend Response:
{
  "success": true,
  "data": [
    {
      "submissionId": 1,
      "title": "AI in Education",
      "authorId": 42,
      "status": "SUBMITTED",
      "submittedAt": "2026-05-01T08:00:00Z",
      "reviewCount": 2,
      "averageScore": 4.5
    },
    ...more submissions...
  ],
  "pagination": {
    "page": 1,
    "pageSize": 20,
    "totalItems": 150,
    "totalPages": 8
  }
}
```

### Security Considerations

1. **JWT Token Management**
   - Tokens stored in localStorage (or httpOnly cookies for better security)
   - Tokens include expiration (typ. 1 hour)
   - Refresh tokens for extended sessions
   - Tokens validated on every backend request

2. **CORS (Cross-Origin Resource Sharing)**
   - Whitelist frontend domain in C++ backend
   - Only allow necessary HTTP methods
   - Restrict headers and credentials

3. **HTTPS/TLS**
   - All communication encrypted end-to-end
   - SSL certificates managed by Nginx reverse proxy
   - HSTS headers enforced

4. **Input Validation**
   - Frontend: Client-side validation with libraries (Formik, React Hook Form)
   - Backend: Server-side validation (critical, cannot be bypassed)
   - JSON schema validation in Dogon middleware

5. **Rate Limiting**
   - Frontend: Prevent rapid-fire requests with debouncing/throttling
   - Backend: Rate limit by IP/user to prevent DDoS

6. **Error Messages**
   - Frontend: User-friendly error messages
   - Backend: Don't expose sensitive system details in error responses

### Performance Optimization

1. **Frontend Optimization**
   - Code splitting with Next.js dynamic imports
   - Image optimization with Next.js Image component
   - CSS minification with Tailwind
   - Caching responses locally

2. **Backend Optimization**
   - Request caching with Redis
   - Database query optimization with indexes
   - Connection pooling for PostgreSQL
   - Gzip compression for responses

3. **Network Optimization**
   - HTTP/2 protocol for multiplexing
   - CDN for static assets
   - API pagination for large datasets

---

## Implementation Roadmap

### Phase 1: Foundation (Week 1-2)
- [x] Set up C++ project structure with CMake/Conan
- [x] Configure project with modern C++17 standards
- [x] Set up database schema and migrations
- [x] Implement database connection pooling
- [x] Create utility modules (Validation, Encryption, DateTime, etc.)

### Phase 2: Core Services (Week 3-4)
- [x] AuthService - JWT token generation/validation
- [x] UserService - Registration, login, profile management
- [x] SubmissionService - CRUD operations for submissions
- [x] ReviewService - Review assignment and submission
- [x] FileService - File upload/download with validation

### Phase 3: Business Logic (Week 5-6)
- [x] SessionService - Conference session management
- [x] ParticipantService - Session registration/attendance
- [x] FeedbackService - Feedback collection and analytics
- [x] NotificationService - Email and in-app notifications
- [x] LoggingService - Audit trail and system logs

### Phase 4: Advanced Features (Week 7-8)
- [x] ReportingService - Analytics and report generation
- [x] ConfigService - System configuration management
- [x] Implement caching (In-memory fallback for performance)
- [x] Async task processing (Internal worker queue)
- [x] Performance optimization and indexing

### Phase 5: API & Frontend (Week 9-10)
- [ ] Implement all RESTful endpoints
- [ ] API documentation (Swagger/OpenAPI)
- [ ] Frontend design (HTML/CSS/JS)
- [ ] Frontend integration with API
- [ ] User authentication UI

### Phase 6: Testing & Deployment (Week 11-12)
- [ ] Unit tests for all services
- [ ] Integration tests for API endpoints
- [ ] Performance testing and optimization
- [ ] Security testing (SQL injection, XSS, CSRF)
- [ ] Docker containerization
- [ ] Deployment documentation
- [ ] Final project report

---

## Key Design Decisions

1. **JWT Token-Based Authentication** - More scalable than session-based
2. **Modular Architecture** - One file per feature for maintainability
3. **Repository Pattern** - Abstraction between services and data access
4. **Async Processing** - Message queues for notifications and logging
5. **Caching Layer** - Redis for frequently accessed data
6. **Role-Based Access Control** - Fine-grained permission management
7. **Double-Blind Reviews** - Anonymize reviewers and authors
8. **Soft Deletes** - Preserve data integrity and audit trails
9. **Database Indexing** - Performance optimization on critical queries
10. **API Versioning** - Support for backward compatibility

---

## Success Criteria

- [x] Reference repository analyzed
- [x] All FRs and NFRs documented
- [x] UML diagrams created (Class, Sequence, State)
- [x] DFD (Level 0, 1, 2) completed
- [x] Database schema finalized with SQL
- [x] Modular structure planned (12+ services)
- [x] API endpoints defined (42+ endpoints)
- [x] Technology stack finalized
- [x] Implementation roadmap established
- [x] Code quality and testing strategy defined

---

## Next Steps

1. **Review this document** - Ensure all requirements are captured
2. **Refine architecture** - Make any adjustments needed
3. **Set up development environment** - Install Dogon, CMake, dependencies
4. **Create project structure** - Generate C++ project skeleton
5. **Begin Phase 1** - Foundation setup and utility modules
6. **Iterative development** - Follow the roadmap for implementation

---

**Document Version:** 1.0  
**Last Updated:** May 5, 2026  
**Status:** Ready for Implementation
