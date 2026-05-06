<div align="center">

# 🏛️ Conference Management System

<p align="center">
  A high-performance, full-stack platform for managing academic conferences — featuring a robust C++17 REST API backend and a modern React/TypeScript frontend with a premium glassmorphism UI.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++17"/>
  <img src="https://img.shields.io/badge/CMake-3.15%2B-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake"/>
  <img src="https://img.shields.io/badge/PostgreSQL-336791?style=for-the-badge&logo=postgresql&logoColor=white" alt="PostgreSQL"/>
  <img src="https://img.shields.io/badge/React-18-20232A?style=for-the-badge&logo=react&logoColor=61DAFB" alt="React"/>
  <img src="https://img.shields.io/badge/TypeScript-5-3178C6?style=for-the-badge&logo=typescript&logoColor=white" alt="TypeScript"/>
  <img src="https://img.shields.io/badge/Vite-6-646CFF?style=for-the-badge&logo=vite&logoColor=white" alt="Vite"/>
  <img src="https://img.shields.io/badge/Compiler-MSYS2_UCRT64-8A2BE2?style=for-the-badge&logo=gnu&logoColor=white" alt="MSYS2"/>
</p>

</div>

---

## 📑 Table of Contents

1. [🏗️ Project Architecture](#️-project-architecture)
2. [⚙️ Environment Setup (Critical)](#️-environment-setup-critical)
3. [🚀 Quick Start](#-quick-start)
4. [✨ Implemented Features](#-implemented-features)
5. [📚 Documentation Index](#-documentation-index)
6. [🛡️ Security & Performance](#️-security--performance)
7. [🛠️ Troubleshooting](#️-troubleshooting)

---

## 🏗️ Project Architecture

This is a monorepo containing two primary applications:

```
Conference Management System/
├── backend/          # C++17 REST API (cpp-httplib, PostgreSQL, JWT)
│   ├── src/
│   │   ├── controllers/   # Request handlers (Auth, Submissions, Reviews...)
│   │   ├── services/      # Business logic layer
│   │   ├── repositories/  # Database access layer (Repository Pattern)
│   │   ├── middleware/     # JWT Auth, CORS, Logging
│   │   └── utils/          # Encryption, DateTime, ID generation
│   ├── include/            # Header files and interfaces
│   ├── docs/               # API design, DB schema, dev guide
│   └── build/              # CMake out-of-source build directory
│
└── frontend/         # React 18 + TypeScript + Vite SPA
    ├── src/
    │   ├── components/    # Auth forms, Dashboard UIs, Layout
    │   ├── pages/         # AuthPage, DashboardPage
    │   ├── services/      # Axios API service layer
    │   ├── context/       # React Context (AuthContext)
    │   ├── types/         # Shared TypeScript type definitions
    │   └── hooks/         # Custom React hooks
    └── public/
```

The frontend communicates with the backend via a typed Axios service layer, with all API requests proxied through Vite's dev server to avoid CORS issues during development.

---

## ⚙️ Environment Setup (Critical)

> <span style="color: #ff6b6b">**IMPORTANT: DO NOT USE LEGACY MinGW.**</span> The legacy `MinGW.org` (GCC 6.3.0) toolchain is obsolete and lacks modern C++17 library support (like `<any>`, `<filesystem>`, and modern thread pooling).

This project requires a **modern MSYS2 UCRT64** environment for the backend.

### 1. Install MSYS2 & Toolchain
1. Download and install [MSYS2](https://www.msys2.org/).
2. Open the **MSYS2 UCRT64** terminal and run the following to install the modern GCC 15+ toolchain and CMake:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc
   pacman -S mingw-w64-ucrt-x86_64-cmake
   pacman -S mingw-w64-ucrt-x86_64-make
   ```
3. Add `C:\msys64\ucrt64\bin` to your Windows System `PATH` environment variable.

### 2. Node.js for Frontend
Ensure **Node.js 18+** and **npm** are installed. This is required for the React frontend.

### 3. IDE Configuration (VSCode)
Ensure your `.vscode/settings.json` is configured to use the UCRT64 toolchain, and that the **CMake Tools** extension is pointing to `C:/msys64/ucrt64/bin/gcc.exe`.

---

## 🚀 Quick Start

### 1. Backend — Build the C++ API

```bash
cd backend

# Create the out-of-source build directory
mkdir build && cd build

# Configure with MSYS2 MinGW Makefiles generator
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Compile the project
cmake --build .
```

> **Important:** If you need to rebuild, stop the running server first. The linker cannot overwrite the `.exe` while it is executing.

### 2. Backend — Configure Environment
Create a `.env` file inside the `backend/` directory (copy from `.env.example`):

```env
DB_HOST=localhost
DB_PORT=5432
DB_NAME=conference_db
DB_USER=postgres
DB_PASSWORD=your_password
JWT_SECRET=your-very-long-and-secret-key-here
API_PORT=8080
```

### 3. Backend — Run the Server
The system features an **Automated Database Initializer** — it creates the `conference_db` database and applies all schema migrations automatically on the first run.

```bash
./build/bin/conference_api.exe
```

The server will be available at: `http://localhost:8080/api/v1`

### 4. Frontend — Install & Run

```bash
cd frontend
npm install
npm run dev
```

The frontend dev server will be available at: `http://localhost:5173`

---

## ✨ Implemented Features

### <span style="color: #4CAF50">✅ Phase 1–4: Full C++ Backend</span>

#### 🔐 Authentication & Authorization
- **JWT-based auth engine** with access tokens signed using HMAC-SHA256.
- **Role-Based Access Control (RBAC)** supporting three roles: `author`, `reviewer`, and `organizer`.
- **Secure registration** with SHA-256 password hashing via a custom `EncryptionUtils` module.
- **Auth middleware** that intercepts every protected route, validates the JWT, and injects the user identity into the request context (`req.user_data`).
- **Token validation pipeline** with detailed error reporting for expired, malformed, or unauthorized tokens.

#### 📄 Paper Submission System
- **Full paper submission lifecycle**: create, update, withdraw, and track status (draft → submitted → accepted/rejected).
- **Multipart/Form-Data upload support**: Authors can attach PDF or DOCX manuscript files during submission. The C++ backend parses the `cpp-httplib` `MultipartFormData` structure and saves files to a dedicated `uploads/` directory.
- **Submission validation**: enforces title length, abstract minimum length, and keyword requirements on the backend service layer.
- **Author-scoped access**: each author can only view and manage their own submissions.

#### 🔬 Review System
- **Double-blind peer review assignment** — reviewers are assigned to papers without knowledge of the author's identity.
- **Review scoring logic** with configurable score rubrics.
- **Review status tracking**: assigned → in-progress → completed.

#### 📅 Session & Conference Management
- **Conference creation and management** by organizers.
- **Session scheduling** with paper assignment and time-slot management.
- **Participant registration** and attendance tracking.

#### 📊 Analytics & Reporting
- **Global reporting engine** providing statistics on submission counts by status, reviewer performance, and conference activity.
- **Status statistics endpoint** for dashboard widgets, aggregating counts per state (submitted, accepted, rejected, withdrawn).

#### ⚡ Infrastructure & Performance
- **In-Memory Cache**: High-speed caching layer for frequently accessed data (session info, analytics) ensuring sub-10ms response times.
- **Async Task Queue**: Dedicated background worker pool for non-blocking notifications, email triggers, and audit logs.
- **Auto-Migration Engine**: On startup, the server connects to PostgreSQL, creates the database if absent, and applies all pending schema migrations atomically.
- **Database Connection Pool**: Dynamic pool of 5–20 connections to handle concurrent requests efficiently.
- **RAII Memory Management**: All resources follow strict RAII patterns — no manual `new`/`delete`, no memory leaks.

---

### <span style="color: #4CAF50">✅ Phase 5: React Frontend</span>

#### 🎨 Design System & UI/UX
- **Glassmorphism Design Language**: The entire UI is built on a dark-mode glassmorphism theme with `backdrop-blur`, semi-transparent backgrounds, and glowing border effects.
- **Framer Motion Animations**: All page elements, modals, and cards use physics-based spring animations (`stiffness`, `damping`) for a premium feel.
- **Tailwind CSS Utility-First Styling**: Cohesive design tokens (color palette, spacing, border radius) applied consistently across all components.
- **Custom Scrollbars**: Styled scrollable areas for modal forms and submission lists.
- **Google Fonts (Inter)**: Modern, readable sans-serif typography.

#### 🔑 Authentication Flows
- **Login Page** (`LoginForm.tsx`): Animated form with gradient background, email/password inputs, and toast notifications for success and error states.
- **Registration Page** (`RegisterForm.tsx`): Multi-field registration with role selection (Author / Reviewer / Organizer), client-side validation, and styled error feedback.
- **Auth Page** (`AuthPage.tsx`): A unified page that switches between Login and Register views with smooth animated transitions.
- **Protected Routes** (`ProtectedRoute.tsx`): HOC that reads the auth context and redirects unauthenticated users to the login page, with role-based route guarding.
- **Auth Context** (`AuthContext.tsx`): Global React context providing `user`, `token`, `login`, and `logout` state to the entire component tree.

#### 📊 Role-Based Dashboards
- **Author Dashboard** (`AuthorDashboard.tsx`):
  - Animated stats cards showing: Total Submissions, Pending Review, Accepted, and Rejected counts.
  - A styled submissions table listing all the author's papers with status badges (color-coded per state).
  - A "New Submission" button that opens the manuscript upload modal.
  - Data is fetched on mount via the `submission.service.ts` and re-fetched after a successful submission.
- **Organizer Dashboard** (`OrganizerDashboard.tsx`): Conference management overview with session, submission, and attendance stats.
- **Reviewer Dashboard** (`ReviewerDashboard.tsx`): Lists assigned papers for review with quick-action buttons.

#### 📝 New Submission Modal (`NewSubmissionModal.tsx`)
- **Multi-field form**: Title, Co-Authors, Keywords (comma-separated), and Abstract fields with proper validation.
- **File Upload Interface**: A drag-and-drop-style file selector with client-side validation for:
  - **File type**: Only `.pdf`, `.doc`, and `.docx` are accepted.
  - **File size**: Maximum 10 MB per upload.
  - **Visual feedback**: Shows a green success state with the filename and size once a file is selected.
- **FormData Submission**: On submit, builds a `multipart/form-data` payload and sends it to the backend via `submission.service.ts`.
- **Form-to-Footer Link**: The submit button is in a sticky footer outside the scrollable form area, linked via the `form="submission-form"` HTML attribute to correctly trigger `onSubmit`.
- **Framer Motion**: The modal entrance/exit is animated with `scale` and `y` transitions.

#### 🌐 API Service Layer (`services/`)
- **`api.ts`**: Configured Axios instance with the base URL pointing to the backend, and request/response interceptors that automatically attach the JWT from `localStorage` to every outgoing request.
- **`auth.service.ts`**: `login()` and `register()` methods.
- **`submission.service.ts`**: `create()` (supports both JSON and `FormData`), `list()`, `get()`, and `update()` methods.
- **`review.service.ts`**: Methods for fetching assigned reviews and submitting review scores.
- **`session.service.ts`**: Methods for fetching conference sessions.
- **`reporting.service.ts`**: Methods for fetching analytics data.

#### 🏛️ Layout System (`components/layout/`)
- **`DashboardLayout.tsx`**: A persistent shell layout with a sticky glassmorphism navigation bar, role-based sidebar navigation links, user avatar/role badge, and a logout button. Uses `<Outlet>` for nested route rendering.

---

## 📚 Documentation Index

- 📐 **[API Design & Routes](backend/docs/API_DESIGN.md)** — Details on all 42 REST endpoints, request payload structures, and response schemas.
- 🗄️ **[Database Schema](backend/docs/DATABASE.md)** — Relational schemas, indexing strategies, and connection pooling details.
- 🚀 **[Features Tracker](FEATURES.md)** — Granular feature tracking across all development phases.
- 🏗️ **[Development Guide](backend/docs/DEVELOPMENT.md)** — Coding standards, middleware architecture, and contribution guidelines.

---

## 🛡️ Security & Performance

**Backend Security:**
- All passwords are hashed using **SHA-256** with a salt before being stored in the database. Plain-text passwords are never persisted.
- Every protected API endpoint requires a valid **JWT Bearer token** in the `Authorization` header.
- The auth middleware validates the token's signature, expiry, and role claims before the request reaches the controller.

**Indexing & Optimization Strategy:**
- **Lookup Optimization**: B-Tree indexes on all foreign keys (`author_id`, `submission_id`, `user_id`).
- **Scan Optimization**: Covering indexes for frequent status-based queries (`status`, `is_active`).
- **Temporal Efficiency**: Descending indexes on `created_at` for rapid retrieval of latest events.
- **Full-Text Support**: Optimized search for paper titles and abstracts.

**Performance Characteristics:**
- **Zero-Latency Fallback**: Multi-tier caching strategy (In-Memory default) ensuring sub-10ms response times for cached resources.
- **Resource Management**: Strict RAII memory models preventing resource leaks.
- **Database Pooling**: Dynamic connection pool (5–20 connections) to handle high concurrency without exhausting database resources.

**Frontend Type Safety:**
- The project enforces `verbatimModuleSyntax` in TypeScript, requiring all type-only imports to use `import type { ... }`. This ensures the build output is clean and no unused type code is ever emitted.

---

## 🛠️ Troubleshooting

### ❌ `Permission Denied` when rebuilding the backend
**Cause:** The `conference_api.exe` process is still running and the linker cannot overwrite it.
**Fix:** Stop the running server process first, then retry the build.

### ❌ `file not found` or `unknown type name` in VSCode
**Cause:** Your IDE's `compile_commands.json` is out of sync or caching an old compiler.
**Fix:** Run `CMake: Delete Cache and Reconfigure` from the VSCode Command Palette, then run `Developer: Reload Window`.

### ❌ `Failed to initialize database`
**Cause:** PostgreSQL credentials mismatch or the PostgreSQL service is not running.
**Fix:** Double-check your `.env` file and ensure the Postgres service is running locally (`pg_ctl status`).

### ❌ `Submission validation failed` in the server logs
**Cause:** The title is too short (under 3 characters) or the abstract is too short (under 10 characters).
**Fix:** Ensure all fields in the submission form meet the minimum length requirements. Keywords must also be provided.

### ❌ `Token validation error: parse_error.101`
**Cause:** A stale or corrupted token is stored in `localStorage` from a previous session.
**Fix:** Open the browser's DevTools → Application → Local Storage → clear the `token` and `user` entries, then log in again.
