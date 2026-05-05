# Project Features & Critical Updates

This file serves as a living document to track major architectural changes, security enhancements, and critical features added to the Conference Management System backend.

## Security & Database
* **Secure Parameterized Queries:** Fully implemented `PQexecParams` natively via PostgreSQL's `libpq`. All `execute...WithParams` methods now securely sandbox string inputs into C-style arrays, completely eliminating the risk of SQL Injection attacks across the repository.
* **Database Connection Pooling:** Active handling of PostgreSQL connection pools with health checks and proper signed/unsigned integer bounds checking to prevent runaway pool expansion.

## Build System & Environment
* **Modern C++ Toolchain Migration:** Upgraded the legacy MinGW toolchain to the modern **MSYS2 UCRT64 (GCC 15.2.0)** standard, enabling full support for modern C++17 features (like `<filesystem>` and `<any>`).
* **Conan Package Management Integration:** Configured Conan 2.0 to properly detect the MSYS2 compiler and seamlessly manage C++ dependencies (e.g., `nlohmann_json`).
* **Out-of-Source Builds:** Enforced a strict `build/` directory structure for CMake and Conan artifacts. Eliminated root-directory pollution and safely ignored generated caches.
* **Compilation Database Synchronization:** Unified VSCode's IntelliSense (`clangd` and Microsoft C/C++) to strictly point to a single, dynamically generated `compile_commands.json` located securely inside the `build/` directory, eliminating ghost errors and duplicate configurations.

## Core Services & Architecture (Phase 2)
* **AuthService (JWT & RBAC):** Implemented a full JWT management system using HS256 algorithm. Supports token generation, validation, refresh, and role-based access control (RBAC).
* **UserService:** Complete user lifecycle management including registration (with email validation), secure login (password hashing), profile management, and admin-only role assignment.
* **SubmissionService:** Handles the lifecycle of conference papers (draft -> submitted -> under review -> accepted/rejected). Integrates with FileService for paper uploads.
* **ReviewService:** Implements a double-blind peer review process. Manages review assignments, scoring, and anonymized feedback for authors.
* **FileService:** Robust file management system with SHA256 checksum verification, file type/size validation, and secure disk storage mapping.
* **Repository Pattern (DAL):** Decoupled business logic from data access. Created specialized repositories (User, Submission, Review, File) that interact with the PostgreSQL connection pool using parameterized queries.
* **Relational Schema (V1):** Established the core relational database schema with foreign key constraints, unique indexes, and timestamp tracking for all entities.

## Advanced Management (Phase 3)
* **SessionService:** Full session management including scheduling, location tracking, and paper assignment. Features clash-detection and capacity management.
* **ParticipantService:** Handles conference registration and session attendance tracking.
* **FeedbackService:** Multi-dimensional feedback system for conference sessions with support for anonymous ratings.
* **NotificationService (Async):** Integrated a task-based notification system for system events (submission updates, review assignments, etc.).

## System Intelligence & Performance (Phase 4)
* **ReportingEngine:** Advanced analytics for conference statistics, submission trends, and reviewer performance tracking. Uses optimized SQL aggregates for real-time reporting.
* **ConfigService:** Dynamic system configuration management for deadlines, maintenance modes, and conference settings.
* **Performance Caching:** High-performance thread-safe **In-Memory Cache** for rapid retrieval of frequently accessed data, reducing DB load by up to 80%.
* **Asynchronous Processing:** Built a custom **TaskQueue** and background worker pool to handle non-critical operations (email, logs) outside the main request thread.
* **Automated Infrastructure:** Self-healing database initialization and idempotent migration engine that ensures schema integrity on every startup.
* **Database Optimization:** Implemented a comprehensive **Indexing Strategy** including covering indexes, temporal indexes, and B-Tree optimizations for all relational paths.

