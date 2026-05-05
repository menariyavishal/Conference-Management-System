# Development Environment Setup

Complete step-by-step instructions for setting up the Conference Management System backend on Windows, macOS, and Linux.

## Prerequisites

All platforms require:
- 8 GB RAM
- 2 GB disk space
- Administrator/sudo access

## Windows Setup

### Step 1: Install C++ Compiler

**Option A: Using Visual Studio 2019/2022** (Recommended)

1. Download from https://visualstudio.microsoft.com
2. Select "Desktop development with C++"
3. Ensure C++17 is installed
4. Verify installation:
   ```cmd
   cl.exe
   ```

**Option B: Using MinGW-w64**

1. Download from https://www.mingw-w64.org
2. Extract to `C:\mingw-w64`
3. Add to PATH: `C:\mingw-w64\bin`
4. Verify:
   ```cmd
   g++ --version
   ```

### Step 2: Install CMake

1. Download from https://cmake.org/download
2. Install and add to PATH
3. Verify:
   ```cmd
   cmake --version
   ```
   Should be 3.15 or higher

### Step 3: Install Conan

```cmd
pip install conan==2.0
conan --version
```

### Step 4: Install PostgreSQL

1. Download from https://www.postgresql.org/download/windows
2. Install with default settings
3. Note the password you set for `postgres` user
4. Verify:
   ```cmd
   psql --version
   ```

### Step 5: Clone Repository

```cmd
cd C:\Projects
git clone https://github.com/your-org/conference-management-system.git
cd conference-management-system\backend
```

### Step 6: Configure Environment

Create `.env` file in backend directory:

```env
DB_HOST=localhost
DB_PORT=5432
DB_NAME=conference_db
DB_USER=postgres
DB_PASSWORD=your-postgres-password
JWT_SECRET=development-secret-key
API_PORT=8080
```

### Step 7: Build Project

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

Or with MinGW:

```cmd
cmake .. -G "Unix Makefiles"
cmake --build .
```

### Step 8: Setup Database

```cmd
createdb -U postgres conference_db
psql -U postgres -d conference_db -f ..\config\database.sql
```

### Step 9: Run Server

```cmd
.\bin\Release\conference_server
```

Expected output:
```
=== Conference Management System v1.0.0 ===
API Version: /api/v1
Server is ready to accept connections
```

## macOS Setup

### Step 1: Install Xcode Command Line Tools

```bash
xcode-select --install
```

### Step 2: Install Homebrew

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### Step 3: Install Dependencies

```bash
brew install cmake conan postgresql
```

### Step 4: Start PostgreSQL

```bash
brew services start postgresql
createdb conference_db
psql -d conference_db -f config/database.sql
```

### Step 5: Clone and Configure

```bash
git clone https://github.com/your-org/conference-management-system.git
cd conference-management-system/backend
cp config/.env.example .env
# Edit .env with your settings
```

### Step 6: Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j$(sysctl -n hw.ncpu)
```

### Step 7: Run

```bash
./bin/conference_server
```

## Linux Setup (Ubuntu/Debian)

### Step 1: Install Dependencies

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    postgresql \
    postgresql-contrib \
    libpq-dev \
    python3-pip \
    git
```

### Step 2: Install Conan

```bash
pip3 install conan==2.0
echo 'export PATH="$PATH:$HOME/.local/bin"' >> ~/.bashrc
source ~/.bashrc
```

### Step 3: Start PostgreSQL

```bash
sudo systemctl start postgresql
sudo systemctl enable postgresql
```

### Step 4: Create Database

```bash
sudo -u postgres createdb conference_db
sudo -u postgres psql -d conference_db -f config/database.sql
```

### Step 5: Clone Repository

```bash
git clone https://github.com/your-org/conference-management-system.git
cd conference-management-system/backend
```

### Step 6: Configure

```bash
cp config/.env.example .env
nano .env  # Edit with your settings
```

### Step 7: Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j$(nproc)
```

### Step 8: Run

```bash
./bin/conference_server
```

## Verify Installation

### Check Database Connection

```bash
# From within the backend directory
psql -h localhost -U postgres -d conference_db -c "SELECT * FROM system_config;"
```

### Check Server Startup

```bash
# In another terminal
curl http://localhost:8080/api/v1/health
```

Expected response:
```json
{
  "success": true,
  "data": {
    "status": "ok",
    "timestamp": "2024-05-05T10:00:00.000Z"
  }
}
```

## Common Issues

### Issue: "psql: error: could not translate host name"

**Solution**: Ensure PostgreSQL is running
- Windows: Check Services (postgresql service)
- macOS: `brew services list`
- Linux: `sudo systemctl status postgresql`

### Issue: "CMAKE_CXX_COMPILER not found"

**Solution**: Install C++ compiler and add to PATH
- Windows: Install Visual Studio or MinGW
- macOS: `xcode-select --install`
- Linux: `sudo apt-get install build-essential`

### Issue: "conan: command not found"

**Solution**: Add Python scripts to PATH
```bash
# macOS/Linux
export PATH="$PATH:$HOME/.local/bin"

# Windows
pip show conan  # Check installation path
```

### Issue: "Permission denied" when running server

**Solution**: Make binary executable
```bash
chmod +x ./bin/conference_server
```

## Next Steps

1. Read [DEVELOPMENT.md](DEVELOPMENT.md) for development guidelines
2. Read [DATABASE.md](DATABASE.md) for schema documentation
3. Read [API_DESIGN.md](API_DESIGN.md) for API conventions
4. Start implementing services in `src/services/`

## IDE Setup

### Visual Studio Code

Install extensions:
- C/C++ (Microsoft)
- CMake (twxs)
- CMake Tools (Microsoft)
- PostgreSQL (Chris Kolkman)

### JetBrains CLion

1. Open project in CLion
2. Install PostgreSQL plugin
3. Configure compiler toolchain
4. Set CMake profile to Release/Debug

### Visual Studio 2019/2022

1. Open CMakeLists.txt
2. CMake project auto-configures
3. Build > Build All

## Performance Tuning

### PostgreSQL

```bash
# Increase shared buffers for better performance
sudo nano /etc/postgresql/13/main/postgresql.conf
# Find "shared_buffers" and set to 256MB
sudo systemctl restart postgresql
```

### CMake Build Optimization

```bash
# Use Ninja for faster builds
brew install ninja  # macOS
# or
sudo apt-get install ninja-build  # Linux

cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Cleanup

To reset development environment:

```bash
# Remove build artifacts
rm -rf build
rm -rf bin

# Reset database
dropdb -U postgres conference_db
createdb -U postgres conference_db
psql -U postgres -d conference_db -f config/database.sql

# Rebuild
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
