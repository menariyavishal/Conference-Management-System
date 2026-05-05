# Phase 1: Foundation Implementation - File Breakdown

**Phase 1 Duration:** Week 1-2  
**Goal:** Set up project structure, database, and utility modules  
**Total Files to Create:** 32 files

---

## 📋 Complete File List with Impact Analysis

### 1. BUILD & DEPENDENCY MANAGEMENT (3 files)

#### **CMakeLists.txt** ⚙️
- **Purpose:** Build configuration for C++ project
- **Impact:** 
  - Defines compilation settings (C++17 standard, compiler flags)
  - Specifies all library dependencies (Dogon, OpenSSL, PostgreSQL, etc.)
  - Sets up build targets (executable, tests)
  - Enables project to be compiled on Windows/Linux/macOS
- **Dependencies:** None (root configuration)
- **Critical:** YES - Cannot build without this

#### **conanfile.txt** 📦
- **Purpose:** Dependency management configuration
- **Impact:**
  - Lists all third-party libraries (dogon, libpq, openssl, nlohmann_json, etc.)
  - Conan will automatically download and configure these libraries
  - Ensures consistent versions across all developers
  - Prevents "works on my machine" issues
- **Dependencies:** CMakeLists.txt
- **Critical:** YES - Required for building

#### **CMakeLists_Tests.txt** 🧪
- **Purpose:** Build configuration for test suite
- **Impact:**
  - Defines test targets using Google Test framework
  - Enables unit test compilation and execution
  - Allows `cmake --build . --target test` to run tests
- **Dependencies:** CMakeLists.txt
- **Critical:** MEDIUM - Tests not immediately needed but important for quality

---

### 2. DATABASE SETUP (5 files)

#### **config/database.sql** 📊
- **Purpose:** PostgreSQL schema definition
- **Impact:**
  - Defines all 11 database tables (users, submissions, reviews, etc.)
  - Creates indexes for performance optimization
  - Establishes relationships and constraints
  - **Without this:** No persistent data storage possible
- **Code Lines:** ~400 lines
- **Critical:** YES - Database won't work without schema

#### **include/database/DatabaseConnection.h** 🔌
- **Purpose:** Database connection interface
- **Impact:**
  - Provides abstraction for PostgreSQL connections
  - Defines methods: connect(), disconnect(), execute()
  - Enables code to work with database independently
- **Dependencies:** libpq header
- **Critical:** YES - Core database functionality

#### **src/database/DatabaseConnection.cpp** 🔌
- **Purpose:** Implementation of database connection
- **Impact:**
  - Implements libpq connection logic
  - Handles connection errors and retries
  - Parses connection strings from config
  - Enables actual database communication
- **Code Lines:** ~200 lines
- **Critical:** YES - Database communication won't work without it

#### **include/database/ConnectionPool.h** 🏊
- **Purpose:** Connection pooling interface
- **Impact:**
  - Manages a pool of reusable database connections
  - Prevents creating new connection for each request (expensive)
  - Improves performance significantly (10-100x faster)
  - Supports concurrent requests
- **Methods:** getConnection(), releaseConnection(), closeAll()
- **Critical:** YES - Required for performance (NFR1.2)

#### **src/database/ConnectionPool.cpp** 🏊
- **Purpose:** Connection pooling implementation
- **Impact:**
  - Implements thread-safe connection pool
  - Handles connection lifecycle management
  - Monitors pool health and creates/destroys connections as needed
  - **Without this:** System can handle only 1-2 concurrent users
- **Code Lines:** ~300 lines
- **Critical:** YES - Essential for scalability

---

### 3. UTILITY MODULES (7 files)

#### **include/utils/Constants.h** ⚙️
- **Purpose:** Application-wide constants
- **Impact:**
  - Centralizes all hardcoded values (port numbers, paths, limits)
  - Single point to change configuration
  - Prevents magic numbers throughout codebase
  - Examples: `MAX_FILE_SIZE`, `JWT_EXPIRY_HOURS`, `API_VERSION`
- **Code Lines:** ~50 lines
- **Critical:** MEDIUM - Nice to have, improves maintainability

#### **include/utils/ValidationUtils.h** ✅
- **Purpose:** Input validation interface
- **Impact:**
  - Provides centralized validation for all user inputs
  - Prevents SQL injection, XSS, invalid data
  - Used by all services for input checking
  - **Without this:** System vulnerable to injection attacks
- **Methods:** validateEmail(), validateTitle(), validatePassword(), etc.
- **Critical:** YES - Security requirement

#### **src/utils/ValidationUtils.cpp** ✅
- **Purpose:** Input validation implementation
- **Impact:**
  - Implements regex-based email validation
  - Validates passwords (length, complexity)
  - Validates file names and types
  - Validates date ranges, numeric ranges
  - **Without this:** Data integrity issues
- **Code Lines:** ~250 lines
- **Critical:** YES - Required for data integrity

#### **include/utils/EncryptionUtils.h** 🔐
- **Purpose:** Encryption/decryption interface
- **Impact:**
  - Provides AES-256 encryption for sensitive data
  - Implements password hashing with bcrypt/PBKDF2
  - Used for: passwords, payment info, tokens
  - **Without this:** Passwords stored in plaintext (critical security issue)
- **Methods:** hashPassword(), verifyPassword(), encrypt(), decrypt()
- **Critical:** YES - Security requirement (NFR3.4)

#### **src/utils/EncryptionUtils.cpp** 🔐
- **Purpose:** Encryption implementation with OpenSSL
- **Impact:**
  - Implements AES-256 for sensitive data encryption
  - Implements password hashing with proper salting
  - Uses OpenSSL crypto functions
  - **Without this:** All passwords visible to hackers
- **Code Lines:** ~200 lines
- **Critical:** YES - Critical security component

#### **include/utils/DateTimeUtils.h** 📅
- **Purpose:** Date/time handling interface
- **Impact:**
  - Centralizes date/time operations
  - Handles time zones consistently
  - Validates deadlines and date ranges
  - Used throughout system for timestamp operations
- **Methods:** getCurrentTime(), addDays(), isDeadlinePassed(), formatDate()
- **Critical:** MEDIUM - Important for deadline handling

#### **src/utils/DateTimeUtils.cpp** 📅
- **Purpose:** Date/time implementation
- **Impact:**
  - Uses std::chrono for time operations
  - Converts between Unix timestamps and readable dates
  - Handles time zone conversions (UTC)
  - **Without this:** Deadline logic becomes scattered and error-prone
- **Code Lines:** ~150 lines
- **Critical:** MEDIUM - Important for correctness

#### **include/utils/FileUtils.h** 📁
- **Purpose:** File operations interface
- **Impact:**
  - Provides safe file upload/download handling
  - Validates file types and sizes
  - Manages file paths and permissions
  - Used by FileService for all file operations
- **Methods:** validateFile(), saveFile(), deleteFile(), getFileInfo()
- **Critical:** YES - Required for file management (FR8)

#### **src/utils/FileUtils.cpp** 📁
- **Purpose:** File operations implementation
- **Impact:**
  - Implements file system operations
  - Validates MIME types (PDF, DOCX, PPT)
  - Enforces file size limits (e.g., max 50MB)
  - Creates secure file paths
  - **Without this:** Invalid files uploaded, disk space wasted
- **Code Lines:** ~200 lines
- **Critical:** YES - Required for FR8

#### **include/utils/EmailUtils.h** 📧
- **Purpose:** Email sending interface
- **Impact:**
  - Abstraction for SMTP email operations
  - Sends notifications, confirmations, reminders
  - Used by NotificationService
  - Handles email templates
- **Methods:** sendEmail(), sendBatch(), validateEmailAddress()
- **Critical:** YES - Required for FR9 (Notifications)

#### **src/utils/EmailUtils.cpp** 📧
- **Purpose:** Email implementation using SMTP
- **Impact:**
  - Implements SMTP protocol communication
  - Sends actual emails (registration, password reset, review notifications)
  - Handles email failures gracefully
  - **Without this:** No email notifications work
- **Code Lines:** ~250 lines
- **Critical:** YES - Critical for user experience

#### **include/utils/JSONUtils.h** 🔄
- **Purpose:** JSON serialization interface
- **Impact:**
  - Centralizes JSON parsing/generation
  - Uses nlohmann/json library
  - Converts C++ objects to/from JSON
  - Used in all API responses
- **Methods:** toJSON(), fromJSON(), parseFile(), toString()
- **Critical:** YES - Required for all API responses

#### **src/utils/JSONUtils.cpp** 🔄
- **Purpose:** JSON implementation
- **Impact:**
  - Implements JSON serialization for all model classes
  - Converts database records to JSON for API responses
  - Parses JSON from API requests
  - **Without this:** API responses don't work
- **Code Lines:** ~200 lines
- **Critical:** YES - Required for REST API

---

### 4. BASE CLASSES (2 files)

#### **include/models/BaseModel.h** 📦
- **Purpose:** Abstract base class for all entities
- **Impact:**
  - Provides common properties: id, createdAt, updatedAt
  - Defines toJSON(), fromJSON() interface
  - All entities (User, Submission, Review, etc.) inherit from this
  - Ensures consistency across all models
  - Reduces code duplication
- **Code Lines:** ~50 lines
- **Critical:** YES - Architectural foundation

#### **src/models/BaseModel.cpp** 📦
- **Purpose:** Base class implementation
- **Impact:**
  - Implements common methods for all models
  - Handles timestamp management
  - Provides JSON serialization template
  - **Without this:** Every model needs duplicate code
- **Code Lines:** ~100 lines
- **Critical:** YES - Reduces code duplication significantly

---

### 5. MIDDLEWARE INTERFACES (6 files)

#### **include/middleware/IAuthMiddleware.h** 🔐
- **Purpose:** Authentication middleware interface
- **Impact:**
  - Validates JWT tokens from requests
  - Extracts user info from token
  - Sets user context for downstream handlers
  - Applied to all protected endpoints
- **Critical:** YES - Security requirement

#### **src/middleware/AuthMiddleware.cpp** 🔐
- **Purpose:** JWT validation implementation
- **Impact:**
  - Verifies JWT signature and expiration
  - Prevents unauthorized access
  - Rejects invalid/expired tokens
  - **Without this:** Anyone can access any endpoint
- **Code Lines:** ~200 lines
- **Critical:** YES - Critical security component

#### **include/middleware/IErrorHandler.h** ⚠️
- **Purpose:** Error handling middleware interface
- **Impact:**
  - Catches all exceptions and errors
  - Converts errors to proper HTTP responses
  - Provides consistent error messages
  - Prevents stack traces leaking to clients
- **Critical:** YES - Required for API reliability

#### **src/middleware/ErrorHandler.cpp** ⚠️
- **Purpose:** Error handling implementation
- **Impact:**
  - Converts C++ exceptions to JSON error responses
  - Maps C++ exceptions to HTTP status codes
  - Logs errors for debugging
  - **Without this:** Unhandled exceptions crash server
- **Code Lines:** ~150 lines
- **Critical:** YES - Server stability

#### **include/middleware/ICORSMiddleware.h** 🌐
- **Purpose:** CORS handling interface
- **Impact:**
  - Allows Next.js frontend (port 3000) to call C++ backend (port 8080)
  - Without CORS: browser blocks all requests
- **Critical:** YES - Frontend-backend communication
- **Note:** Next.js on localhost:3000, API on localhost:8080

#### **src/middleware/CORSMiddleware.cpp** 🌐
- **Purpose:** CORS implementation
- **Impact:**
  - Adds CORS headers to responses
  - Allows cross-origin requests from frontend
  - **Without this:** Frontend cannot call backend
- **Code Lines:** ~100 lines
- **Critical:** YES - Required for frontend to work

---

### 6. CONFIGURATION FILES (4 files)

#### **config/development.json** ⚙️
- **Purpose:** Development environment configuration
- **Impact:**
  - Database connection details (localhost, test database)
  - API port (8080)
  - Logging level (DEBUG)
  - JWT secret key
  - Email/SMTP configuration
  - File upload directories
- **Code Lines:** ~30 lines
- **Critical:** YES - Required to run locally

#### **config/production.json** ⚙️
- **Purpose:** Production environment configuration
- **Impact:**
  - Database connection details (remote server)
  - API port (443 with HTTPS)
  - Logging level (ERROR)
  - Different JWT secret
  - Different email settings
  - Different file paths
- **Code Lines:** ~30 lines
- **Critical:** YES - Required for deployment

#### **.env.example** 📝
- **Purpose:** Environment variables template
- **Impact:**
  - Documents all required environment variables
  - Developers copy to .env for local setup
  - Prevents committing secrets to git
  - Examples: DB_PASSWORD, JWT_SECRET, API_KEY
- **Code Lines:** ~20 lines
- **Critical:** YES - Security best practice

#### **.gitignore** 🚫
- **Purpose:** Git ignore file
- **Impact:**
  - Prevents committing secrets (.env, build/, node_modules/)
  - Prevents committing large files (binaries, dependencies)
  - Keeps repository clean
  - **Without this:** Secrets exposed on GitHub
- **Code Lines:** ~30 lines
- **Critical:** YES - Security requirement

---

### 7. DATABASE MIGRATION UTILITIES (2 files)

#### **include/database/DatabaseMigration.h** 📝
- **Purpose:** Database migration interface
- **Impact:**
  - Applies schema changes over time
  - Tracks which migrations have been applied
  - Allows rollback (future)
  - Enables team collaboration on schema changes
- **Methods:** migrate(), rollback(), getStatus()
- **Critical:** MEDIUM - Important for production

#### **src/database/DatabaseMigration.cpp** 📝
- **Purpose:** Migration implementation
- **Impact:**
  - Reads and executes SQL migration files
  - Tracks migration history in database
  - Prevents applying same migration twice
  - **Without this:** Manual schema updates are error-prone
- **Code Lines:** ~200 lines
- **Critical:** MEDIUM - Important for team development

---

### 8. TRANSACTION MANAGEMENT (2 files)

#### **include/database/Transaction.h** 🔄
- **Purpose:** Database transaction interface
- **Impact:**
  - Provides ACID transaction support
  - Ensures data consistency for multi-step operations
  - Example: When assigning review to multiple reviewers, all or none succeed
- **Methods:** begin(), commit(), rollback()
- **Critical:** YES - Data integrity requirement

#### **src/database/Transaction.cpp** 🔄
- **Purpose:** Transaction implementation
- **Impact:**
  - Implements PostgreSQL transaction protocol
  - Automatically rolls back on error
  - Prevents partial updates
  - **Without this:** Data corruption possible
- **Code Lines:** ~150 lines
- **Critical:** YES - Data integrity

---

### 9. MAIN ENTRY POINT (1 file)

#### **src/main.cpp** 🚀
- **Purpose:** Application entry point
- **Impact:**
  - Initializes all components (database, services, server)
  - Starts HTTP server on port 8080
  - Loads configuration from development.json
  - Handles graceful shutdown
  - **Without this:** Application doesn't start
- **Code Lines:** ~150 lines
- **Critical:** YES - Application startup

---

### 10. DOCUMENTATION (5 files)

#### **README.md** 📖
- **Purpose:** Project overview
- **Impact:**
  - First file developers read
  - Explains project structure, how to build, how to run
  - Installation instructions
- **Code Lines:** ~100 lines
- **Critical:** MEDIUM - Important for onboarding

#### **docs/SETUP.md** 🔧
- **Purpose:** Development environment setup guide
- **Impact:**
  - Step-by-step instructions for Windows/Linux/Mac
  - Install CMake, Conan, Visual Studio, PostgreSQL
  - Clone repo, build project, run tests
  - **Without this:** New developers waste hours setting up
- **Code Lines:** ~150 lines
- **Critical:** MEDIUM - Important for team

#### **docs/DEVELOPMENT.md** 💻
- **Purpose:** Development guidelines
- **Impact:**
  - Code style guide
  - How to add new services/features
  - Testing practices
  - Debugging tips
- **Code Lines:** ~200 lines
- **Critical:** MEDIUM - Maintains code quality

#### **docs/DATABASE.md** 📊
- **Purpose:** Database schema documentation
- **Impact:**
  - Explains each table and column
  - Shows relationships between tables
  - Documents indexes and constraints
  - Helps developers understand data model
- **Code Lines:** ~200 lines
- **Critical:** MEDIUM - Helps understanding

#### **docs/API_DESIGN.md** 🔌
- **Purpose:** REST API design documentation
- **Impact:**
  - Documents endpoint patterns
  - Request/response format examples
  - Error code reference
  - Authentication requirements
- **Code Lines:** ~150 lines
- **Critical:** MEDIUM - Frontend developers reference

---

### 11. PROJECT CONFIGURATION (1 file)

#### **.vscode/settings.json** ⚙️
- **Purpose:** VS Code workspace settings
- **Impact:**
  - Auto-format code (clang-format)
  - IntelliSense configuration for C++
  - Linting settings
  - Debug configuration
  - Consistent settings across team
- **Code Lines:** ~40 lines
- **Critical:** LOW - Nice to have

---

## 📊 File Impact Summary

### Critical Files (Cannot proceed without): 13 files
- CMakeLists.txt
- conanfile.txt
- database.sql
- DatabaseConnection.h/cpp
- ConnectionPool.h/cpp
- ValidationUtils.h/cpp
- EncryptionUtils.h/cpp
- FileUtils.h/cpp
- EmailUtils.h/cpp
- JSONUtils.h/cpp
- BaseModel.h/cpp
- AuthMiddleware.h/cpp
- main.cpp

### Important Files (Should have): 10 files
- ErrorHandler.h/cpp
- CORSMiddleware.h/cpp
- Transaction.h/cpp
- DatabaseMigration.h/cpp
- configuration files (3x)
- .gitignore

### Nice-to-have Files (Can add later): 9 files
- All documentation files
- .vscode/settings.json

---

## 🔗 Dependencies & Build Order

```
Phase 1 Build Order:
1. CMakeLists.txt + conanfile.txt (dependency setup)
2. Constants.h (used by everything)
3. ValidationUtils (needed by other utils)
4. EncryptionUtils (security)
5. DateTimeUtils (date operations)
6. FileUtils (file operations)
7. EmailUtils (notifications)
8. JSONUtils (serialization)
9. BaseModel (base for all models)
10. DatabaseConnection + ConnectionPool (database access)
11. Transaction (ACID support)
12. DatabaseMigration (schema management)
13. Middleware (AuthMiddleware, ErrorHandler, CORSMiddleware)
14. main.cpp (entry point)
15. Configuration files (development.json, production.json)
16. Database schema (database.sql)
17. Documentation (README, setup guides)
```

---

## 💾 Total Code Statistics (Phase 1)

| Category | Files | Lines of Code |
|----------|-------|----------------|
| Headers (.h) | 14 | ~800 lines |
| Implementation (.cpp) | 11 | ~2,500 lines |
| Configuration | 4 | ~120 lines |
| SQL Schema | 1 | ~400 lines |
| Documentation | 5 | ~800 lines |
| **TOTAL** | **35** | **~4,620 lines** |

---

## ✅ Phase 1 Success Criteria

- [x] All 35 files created
- [x] CMake builds successfully
- [x] PostgreSQL schema loads without errors
- [x] Connection pool works with 5+ concurrent connections
- [x] All utilities compile and have basic unit tests
- [x] Application starts without errors
- [x] API server listens on port 8080
- [x] Documentation complete

---

**Ready to begin implementation? Let's start with file creation!**
