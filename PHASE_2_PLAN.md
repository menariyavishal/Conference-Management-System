# Phase 2: Core Services Implementation Plan
## Conference Management System (C++ Backend)

**Date:** May 5, 2026  
**Status:** Planning  
**Objective:** Implement 5 core services with complete CRUD operations and business logic

---

## Table of Contents
1. [Overview](#overview)
2. [Services to Create](#services-to-create)
3. [File Structure](#file-structure)
4. [Implementation Details](#implementation-details)
5. [Dependencies & Impact](#dependencies--impact)
6. [Development Order](#development-order)
7. [Testing Strategy](#testing-strategy)

---

## Overview

### Phase 2 Goals
- ✅ Implement 5 core services (AuthService, UserService, SubmissionService, ReviewService, FileService)
- ✅ Create service interfaces (header files with pure virtual methods)
- ✅ Create concrete implementations with full business logic
- ✅ Integrate with database layer (repositories)
- ✅ Support JWT authentication and role-based access control
- ✅ Handle all CRUD operations for main entities

### Key Principles
- **One file per feature** - Each service gets ONE .cpp file
- **Interface-based design** - Service interfaces (I*.h) define contracts
- **Repository pattern** - Data access abstraction layer
- **Error handling** - Consistent exception/error response handling
- **Logging** - All operations logged via spdlog

### Deliverables
- 5 service interface headers (I*.h)
- 5 service implementation files (*.cpp)
- 6 repository interface headers (I*.h)
- Business logic fully implemented
- All integrated with database layer

---

## Services to Create

### 1. AuthService (Authentication & JWT)
**Purpose:** JWT token generation, validation, and user authentication  
**File:** `backend/src/services/AuthService.cpp` (new)  
**Interface:** `backend/include/services/IAuthService.h` (new)

#### Responsibilities:
- Generate JWT tokens with user claims
- Validate JWT tokens for expiry and signature
- Refresh token generation
- Extract user ID from token
- Check token permissions

#### Key Methods:
```cpp
class IAuthService {
    virtual ~IAuthService() = default;
    virtual std::string generateToken(const User& user, int expiryHours = 24) = 0;
    virtual bool validateToken(const std::string& token) = 0;
    virtual std::string refreshToken(const std::string& token) = 0;
    virtual std::optional<std::string> extractUserId(const std::string& token) = 0;
    virtual bool hasPermission(const std::string& token, const std::string& role) = 0;
};
```

#### Input/Output:
- **Input:** User object, token string
- **Output:** JWT token, validation result, user claims
- **Errors:** InvalidTokenException, ExpiredTokenException, InvalidSignatureException
- **Logging:** Token generation/validation attempts (DEBUG), failed auth (WARN)

#### Database Usage:
- No direct DB access
- Uses EncryptionUtils for token signing
- Caches public key in memory

#### Impact:
- ✅ Enables all API endpoints to have authentication
- ✅ Supports role-based access control
- ✅ 24-hour token expiry for security

---

### 2. UserService (User Management)
**Purpose:** User registration, login, profile management, role assignment  
**File:** `backend/src/services/UserService.cpp` (new)  
**Interface:** `backend/include/services/IUserService.h` (new)

#### Responsibilities:
- User registration with email verification
- User login with password validation
- Profile updates (name, email, password)
- User role management (admin only)
- User listing with filters
- Soft delete users

#### Key Methods:
```cpp
class IUserService {
    virtual ~IUserService() = default;
    virtual json registerUser(const User& user) = 0;
    virtual json loginUser(const std::string& email, const std::string& password) = 0;
    virtual json getUserProfile(const std::string& userId) = 0;
    virtual json updateUserProfile(const std::string& userId, const User& updates) = 0;
    virtual json changePassword(const std::string& userId, const std::string& oldPassword, const std::string& newPassword) = 0;
    virtual json listUsers(int page = 1, int pageSize = 20, const std::string& role = "") = 0;
    virtual json assignRole(const std::string& userId, const std::string& newRole) = 0;
    virtual bool deactivateUser(const std::string& userId) = 0;
};
```

#### Input/Output:
- **Input:** User registration data, login credentials, profile updates
- **Output:** User object, JWT token, user list with pagination
- **Errors:** UserAlreadyExistsException, InvalidCredentialsException, UserNotFoundException
- **Logging:** Registration (INFO), login success/failure (INFO/WARN), profile changes (DEBUG)

#### Database Usage:
- **Repository:** IUserRepository for all DB operations
- **Tables:** users table (user data, passwords, roles)
- **Queries:** 
  - Insert new user
  - Select by email/ID
  - Update profile
  - Soft delete (set isActive=false)
  - List with filters

#### Impact:
- ✅ Core user management functionality
- ✅ Supports 4 user roles (Admin, Organizer, Author, Reviewer)
- ✅ Password hashing via EncryptionUtils
- ✅ Email validation via ValidationUtils

---

### 3. SubmissionService (Paper/Abstract Management)
**Purpose:** CRUD operations for conference submissions (abstracts, papers)  
**File:** `backend/src/services/SubmissionService.cpp` (new)  
**Interface:** `backend/include/services/ISubmissionService.h` (new)

#### Responsibilities:
- Create new submission (abstract + metadata)
- Upload presentation/paper files
- Edit submission (before deadline)
- Withdraw submission
- List submissions with filters
- Track submission status

#### Key Methods:
```cpp
class ISubmissionService {
    virtual ~ISubmissionService() = default;
    virtual json createSubmission(const Submission& submission, const std::string& userId) = 0;
    virtual json uploadFile(const std::string& submissionId, const std::string& filePath) = 0;
    virtual json updateSubmission(const std::string& submissionId, const Submission& updates) = 0;
    virtual json withdrawSubmission(const std::string& submissionId) = 0;
    virtual json getSubmissionDetails(const std::string& submissionId) = 0;
    virtual json listSubmissions(int page = 1, int pageSize = 20, const std::string& filters = "") = 0;
    virtual json getSubmissionsByAuthor(const std::string& authorId) = 0;
    virtual json getSubmissionsForReview(const std::string& reviewerId) = 0;
};
```

#### Input/Output:
- **Input:** Submission data, file upload, filter criteria
- **Output:** Submission object, file metadata, submission list
- **Errors:** SubmissionNotFoundException, FileUploadException, DeadlinePassedException
- **Logging:** Create/update/delete (INFO), file uploads (INFO), status changes (DEBUG)

#### Database Usage:
- **Repository:** ISubmissionRepository
- **Tables:** submissions (submission data, status, timestamps)
- **Related:** file_uploads (file metadata)
- **Queries:**
  - Insert submission
  - Update status
  - Select by author/ID
  - List with pagination and filters

#### Impact:
- ✅ Authors can submit papers
- ✅ Tracks submission lifecycle (draft → submitted → under review → accepted)
- ✅ File management via FileService
- ✅ Integrates with ReviewService for reviewer assignments

---

### 4. ReviewService (Review Assignment & Submission)
**Purpose:** Manage double-blind peer review process  
**File:** `backend/src/services/ReviewService.cpp` (new)  
**Interface:** `backend/include/services/IReviewService.h` (new)

#### Responsibilities:
- Assign submissions to reviewers
- Submit reviews with scores/feedback
- Anonymize author/reviewer information (double-blind)
- Track review deadlines and status
- Display reviews to authors (after acceptance)
- Generate review statistics

#### Key Methods:
```cpp
class IReviewService {
    virtual ~IReviewService() = default;
    virtual json assignReview(const std::string& submissionId, const std::string& reviewerId) = 0;
    virtual json submitReview(const std::string& reviewId, const Review& review) = 0;
    virtual json getReviewAssignments(const std::string& reviewerId) = 0;
    virtual json getSubmissionReviews(const std::string& submissionId, bool anonymized = true) = 0;
    virtual json getReviewStatistics(const std::string& submissionId) = 0;
    virtual json updateReviewDeadline(const std::string& reviewId, const std::string& newDeadline) = 0;
    virtual bool isDeadlineOverdue(const std::string& reviewId) = 0;
};
```

#### Input/Output:
- **Input:** Review assignment (submission + reviewer), review scores/feedback
- **Output:** Review object, anonymized review data, statistics
- **Errors:** ReviewNotFoundException, DeadlinePassedException, UnauthorizedAccessException
- **Logging:** Assignments (INFO), submissions (INFO), deadline reminders (WARN)

#### Database Usage:
- **Repository:** IReviewRepository
- **Tables:** reviews (review data, scores, comments, status)
- **Queries:**
  - Insert review assignment
  - Update review (when submitted)
  - Select by reviewer/submission
  - List assignments with filtering

#### Impact:
- ✅ Implements double-blind review process
- ✅ Prevents bias in review assignments
- ✅ Tracks review quality metrics
- ✅ Supports review deadline management

---

### 5. FileService (File Management)
**Purpose:** Handle file upload, download, validation, and storage  
**File:** `backend/src/services/FileService.cpp` (new)  
**Interface:** `backend/include/services/IFileService.h` (new)

#### Responsibilities:
- Upload files with validation (size, type, virus scan placeholder)
- Generate secure filenames
- Calculate file checksums (SHA256)
- Download files with access control
- Delete files (when soft-deleting submissions)
- Track file metadata

#### Key Methods:
```cpp
class IFileService {
    virtual ~IFileService() = default;
    virtual json uploadFile(const std::string& sourceFilePath, const std::string& userId, 
                           const std::string& context = "submission") = 0;
    virtual bool downloadFile(const std::string& fileId, const std::string& userId, 
                             std::string& outputPath) = 0;
    virtual json getFileInfo(const std::string& fileId) = 0;
    virtual bool deleteFile(const std::string& fileId) = 0;
    virtual bool validateFileSize(const std::string& filePath, long maxSizeMB = 50) = 0;
    virtual bool validateFileType(const std::string& filePath, const std::vector<std::string>& allowedTypes) = 0;
    virtual std::string calculateChecksum(const std::string& filePath) = 0;
};
```

#### Input/Output:
- **Input:** File path, file metadata, user ID
- **Output:** File ID, file info (name, size, type, uploaded date)
- **Errors:** FileUploadException, FileTooLargeException, InvalidFileTypeException
- **Logging:** Uploads (INFO), downloads (DEBUG), deletions (INFO)

#### Database Usage:
- **Repository:** IFileRepository (new - for file metadata)
- **Tables:** file_uploads (file metadata storage)
- **Disk Storage:** ./uploads/ directory structure
- **Queries:**
  - Insert file metadata
  - Select file info
  - Update access count
  - Soft delete (set isActive=false)

#### Impact:
- ✅ Secure file handling for submissions
- ✅ Access control (only owner/reviewers can access)
- ✅ Prevents malicious file uploads
- ✅ Supports audit trail for file access

---

## File Structure

### New Files to Create (Phase 2)

#### Service Interfaces (Headers)
```
backend/include/services/
├── IAuthService.h          (AUTH - JWT token management)
├── IUserService.h          (USER - Registration, login, profile)
├── ISubmissionService.h    (SUBMISSION - Paper/abstract CRUD)
├── IReviewService.h        (REVIEW - Double-blind review)
└── IFileService.h          (FILE - Upload, download, storage)
```

#### Service Implementations
```
backend/src/services/
├── AuthService.cpp         (JWT generation, validation)
├── UserService.cpp         (User CRUD with roles)
├── SubmissionService.cpp   (Submission lifecycle)
├── ReviewService.cpp       (Review assignment & anonymization)
└── FileService.cpp         (File operations with validation)
```

#### Repository Interfaces (Data Access)
```
backend/include/repositories/
├── IUserRepository.h       (User data access)
├── ISubmissionRepository.h (Submission data access)
├── IReviewRepository.h     (Review data access)
├── ISessionRepository.h    (Session data access)
├── IFeedbackRepository.h   (Feedback data access)
└── INotificationRepository.h (Notification data access)
```

#### Model Updates (if needed)
```
backend/include/models/
├── User.h                  (Already created, may need fields)
├── Submission.h            (Already created, may need fields)
├── Review.h                (Already created, may need fields)
└── Session.h               (Already created, may need fields)
```

---

## Implementation Details

### Service Architecture Pattern

```
User Request
    ↓
Controller (UserController.cpp)
    ↓
Service Interface (IUserService)
    ↓
Service Implementation (UserService.cpp)
    ├─ Business Logic
    ├─ Validation
    └─ Error Handling
    ↓
Repository Interface (IUserRepository)
    ↓
Repository Implementation (UserRepository.cpp)
    ├─ Database Queries
    ├─ SQL Execution
    └─ Result Mapping
    ↓
Database (PostgreSQL)
```

### Common Service Structure

Each service will follow this pattern:

```cpp
// IAuthService.h
class IAuthService {
public:
    virtual ~IAuthService() = default;
    // Pure virtual methods defining the contract
};

// AuthService.cpp
class AuthService : public IAuthService {
private:
    std::shared_ptr<DatabaseConnection> connection;
    std::shared_ptr<spdlog::logger> logger;
    
public:
    AuthService(std::shared_ptr<DatabaseConnection> conn);
    virtual std::string generateToken(const User& user, int expiryHours) override;
    virtual bool validateToken(const std::string& token) override;
    // ... more implementations
};
```

### Error Handling Strategy

All services use exception hierarchy from Phase 1:
```cpp
try {
    auto user = userService->getUserProfile(userId);
    // Process user
} catch (const UserNotFoundException& ex) {
    logger->warn("User not found: {}", userId);
    return {success: false, error: "User not found"};
} catch (const ApplicationException& ex) {
    logger->error("Unexpected error: {}", ex.what());
    return {success: false, error: "Internal server error"};
}
```

### Logging Strategy

- **DEBUG:** Method entry/exit, variable states, query execution
- **INFO:** User actions (registration, login, submission), successful operations
- **WARN:** Failed attempts, deadline approaching, unusual patterns
- **ERROR:** Exceptions, database failures, critical issues

Example:
```cpp
logger->info("User {} registered successfully", user.email);
logger->debug("Checking user {} permissions", userId);
logger->warn("Review deadline approaching for submission {}", submissionId);
logger->error("Database connection failed: {}", error.message());
```

---

## Dependencies & Impact

### Internal Dependencies

```
AuthService
├─ EncryptionUtils (password hashing, token signing)
├─ Constants (JWT_SECRET, TOKEN_EXPIRY)
└─ IUserRepository (user lookup)

UserService
├─ AuthService (token generation on login)
├─ ValidationUtils (email, password validation)
├─ EncryptionUtils (password hashing)
├─ DateTimeUtils (timestamps)
└─ IUserRepository (CRUD operations)

SubmissionService
├─ FileService (file uploads)
├─ ValidationUtils (submission data)
├─ DateTimeUtils (deadlines)
├─ Constants (submission status values)
└─ ISubmissionRepository (CRUD operations)

ReviewService
├─ SubmissionService (get submission details)
├─ UserService (reviewer info)
├─ ValidationUtils (review scores)
├─ DateTimeUtils (deadline tracking)
└─ IReviewRepository (CRUD operations)

FileService
├─ ValidationUtils (file type checking)
├─ EncryptionUtils (SHA256 checksums)
└─ FileUtils (file operations)
```

### Database Dependencies

| Service | Tables Used | Operations |
|---------|------------|-----------|
| AuthService | None | Read-only via UserRepository |
| UserService | users, system_logs | INSERT, UPDATE, SELECT |
| SubmissionService | submissions, file_uploads | INSERT, UPDATE, SELECT |
| ReviewService | reviews, submissions, users | INSERT, UPDATE, SELECT |
| FileService | file_uploads | INSERT, UPDATE, SELECT |

### External Dependencies

- PostgreSQL 13+ (with libpq)
- spdlog (logging)
- nlohmann_json (JSON serialization)
- OpenSSL (JWT signing)
- Boost (date/time utilities)

---

## Development Order

### Priority: Implementation Sequence

1. **AuthService** (Foundation)
   - Must exist for other services to function
   - 100 lines implementation
   - No dependencies on other services
   - **Time: 1-2 hours**

2. **UserService** (Core)
   - Depends on AuthService
   - Heavy database interaction
   - 250+ lines
   - **Time: 2-3 hours**

3. **FileService** (Support)
   - Can be implemented independently
   - File I/O heavy
   - 200+ lines
   - **Time: 2-3 hours**

4. **SubmissionService** (Business Logic)
   - Depends on UserService, FileService
   - Complex status tracking
   - 250+ lines
   - **Time: 3-4 hours**

5. **ReviewService** (Complex)
   - Depends on SubmissionService, UserService
   - Anonymization logic
   - Double-blind implementation
   - 300+ lines
   - **Time: 4-5 hours**

### Parallel Work Possible

- AuthService + FileService (independent)
- SubmissionService + ReviewService (after UserService)

---

## Testing Strategy

### Unit Tests (Per Service)

```cpp
// Test AuthService
TEST(AuthServiceTest, GenerateValidToken) {
    User user{"user1", "test@example.com", "User One", "author"};
    std::string token = authService->generateToken(user);
    EXPECT_TRUE(authService->validateToken(token));
}

TEST(AuthServiceTest, RejectExpiredToken) {
    // Token expired 1 hour ago
    EXPECT_FALSE(authService->validateToken(expiredToken));
}
```

### Integration Tests

- Test UserService with actual database
- Test SubmissionService file uploads
- Test ReviewService anonymization
- Test cross-service communication

### Mock Objects

For testing without database:
```cpp
class MockUserRepository : public IUserRepository {
    // Mock implementation for testing
};
```

---

## Summary Table

| Service | Files | Lines | Dependencies | Time |
|---------|-------|-------|--------------|------|
| AuthService | 2 (h+cpp) | 100 | Encryption, Constants | 1-2h |
| UserService | 2 | 250+ | AuthService, Validation | 2-3h |
| FileService | 2 | 200+ | Validation, Encryption | 2-3h |
| SubmissionService | 2 | 250+ | UserService, FileService | 3-4h |
| ReviewService | 2 | 300+ | SubmissionService, Anonymization | 4-5h |
| **TOTAL** | **10** | **1100+** | **Multiple** | **12-17h** |

---

## Success Criteria

- [x] All service interfaces defined with pure virtual methods
- [ ] All service implementations completed with business logic
- [ ] All repository interfaces created
- [ ] Services properly integrated with database layer
- [ ] JWT authentication fully functional
- [ ] User CRUD operations tested
- [ ] Submission lifecycle working
- [ ] Review anonymization implemented
- [ ] File upload/download working
- [ ] All logging implemented
- [ ] Error handling comprehensive
- [ ] Code follows modular design (one file per service)

---

## Next Steps

1. **Review this plan** - Confirm services and file structure
2. **Validate with project design** - Ensure alignment with requirements
3. **Begin AuthService** - Start with foundation
4. **Implement sequentially** - Follow development order
5. **Test as we go** - Unit tests for each service
6. **Move to Phase 3** - SessionService, ParticipantService, etc.

---

**Document Version:** 1.0  
**Created:** May 5, 2026  
**Status:** Ready for Implementation  
**Total Estimated Time:** 12-17 hours
