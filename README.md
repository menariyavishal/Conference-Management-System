<div align="center">

# 🏛️ Conference Management System (Backend)

<p align="center">
  A highly-performant, robust REST API for managing academic conferences, built with modern C++17 standards.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++17"/>
  <img src="https://img.shields.io/badge/CMake-3.15%2B-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake"/>
  <img src="https://img.shields.io/badge/PostgreSQL-336791?style=for-the-badge&logo=postgresql&logoColor=white" alt="PostgreSQL"/>
  <img src="https://img.shields.io/badge/Compiler-MSYS2_UCRT64-8A2BE2?style=for-the-badge&logo=gnu&logoColor=white" alt="MSYS2"/>
</p>

</div>

---

## 📑 Table of Contents

1. [⚙️ Environment Setup (Critical)](#️-environment-setup-critical)
2. [🚀 Quick Start](#-quick-start)
3. [📚 Documentation Index](#-documentation-index)
4. [✨ Features & Roadmap](#-features--roadmap)
5. [🛡️ Security & Performance](#️-security--performance)
6. [🛠️ Troubleshooting](#️-troubleshooting)

---

## ⚙️ Environment Setup (Critical)

> <span style="color: #ff6b6b">**IMPORTANT: DO NOT USE LEGACY MinGW.**</span> The legacy `MinGW.org` (GCC 6.3.0) toolchain is obsolete and lacks modern C++17 library support (like `<any>`, `<filesystem>`, and modern thread pooling). 

This project requires a **modern MSYS2 UCRT64** environment.

### 1. Install MSYS2 & Toolchain
1. Download and install [MSYS2](https://www.msys2.org/).
2. Open the **MSYS2 UCRT64** terminal and run the following to install the modern GCC 15+ toolchain and CMake:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc
   pacman -S mingw-w64-ucrt-x86_64-cmake
   pacman -S mingw-w64-ucrt-x86_64-make
   ```
3. Add `C:\msys64\ucrt64\bin` to your Windows System `PATH` environment variable.

### 2. IDE Configuration (VSCode)
Ensure your `.vscode/settings.json` is configured to use the UCRT64 toolchain, and that the **CMake Tools** extension is pointing to `C:/msys64/ucrt64/bin/gcc.exe`.

---

## 🚀 Quick Start

### 1. Build the Project
We use standard Out-of-Source CMake builds.

```bash
# Clone the repository
cd backend

# Create build directory
mkdir build && cd build

# Configure with MSYS2 CMake Generator
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build .
```

### 2. Configure Environment
Create a `.env` file in the `backend/` directory (see `.env.example` for reference):

```env
DB_HOST=localhost
DB_PORT=5432
DB_NAME=conference_db
DB_USER=postgres
DB_PASSWORD=your_password
JWT_SECRET=your-secret-key-here
API_PORT=8080
```

### 3. Run the Server
The system features an **Automated Database Initializer** that creates the database and runs migrations on startup.

```bash
./build/bin/conference_api.exe
```

---

## 📚 Documentation Index

- 📐 **[API Design & Routes](docs/API_DESIGN.md)** - Details on REST endpoints, payload structures, and response schemas.
- 🗄️ **[Database Schema](docs/DATABASE.md)** - Relational schemas, indexing strategies, and connection pooling details.
- 🚀 [Features Tracker (`FEATURES.md`)](FEATURES.md)
- 🏗️ **[Development Guide](docs/DEVELOPMENT.md)** - Coding standards and middleware architecture.

---

## ✨ Features & Roadmap

### <span style="color: #4CAF50">✅ Completed (Phases 1-4: Full Backend)</span>
- **Auth Engine**: Role-based access control (RBAC) with secure JWT token issuance.
- **Conference & Submission**: Full paper submission workflow with file uploads and author management.
- **Review System**: Double-blind peer review assignment and scoring logic.
- **Session Management**: Scheduling, paper assignment, and participant registration.
- **Advanced Analytics**: Global reporting engine for conference statistics and reviewer performance.
- **Performance Caching**: High-speed **In-Memory Cache** for session data and analytics.
- **Async Task Queue**: Dedicated background worker pool for non-blocking notifications and logs.
- **Auto-Infrastructure**: Self-healing database creation and automated migration engine.

### <span style="color: #FFC107">🚀 Phase 5: API & Frontend (Next Step)</span>
- **Frontend Design**: Modern, responsive dashboard with glassmorphism aesthetics.
- **API Documentation**: Interactive Swagger/OpenAPI specification.
- **Integration**: Full frontend-to-backend handshake.

---

## 🛡️ Security & Performance

**Indexing & Optimization Strategy:**
- **Lookup Optimization**: B-Tree indexes on all foreign keys (`author_id`, `submission_id`, `user_id`).
- **Scan Optimization**: Covering indexes for frequent status-based queries (`status`, `is_active`).
- **Temporal Efficiency**: Descending indexes on `created_at` for rapid retrieval of latest events.
- **Full-Text Support**: Optimized search for paper titles and abstracts.

**Performance Characteristics:**
- **Zero-Latency Fallback**: Multi-tier caching strategy (In-Memory default) ensuring sub-10ms response times.
- **Resource Management**: Strict RAII memory models preventing leaks.
- **Database Pooling**: Dynamic connection pool (5-20 connections) to handle high concurrency.

---

## 🛠️ Troubleshooting

### ❌ `file not found` or `unknown type name` in VSCode
**Cause:** Your IDE's `compile_commands.json` is out of sync or caching an old compiler.
**Fix:** Run `CMake: Delete Cache and Reconfigure` from the VSCode Command Palette, then run `Developer: Reload Window`.

### ❌ `Failed to initialize database`
**Cause:** PostgreSQL credentials mismatch or the server is down.
**Fix:** Double-check your `.env` file and ensure the Postgres service is running locally.
