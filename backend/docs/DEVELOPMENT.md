# Development Guide

Guidelines and best practices for developing the Conference Management System backend.

## Code Style

### File Organization

**One feature per file** - Each .cpp file should contain implementation for a single, well-defined feature.

```cpp
// ❌ BAD: Multiple features in one file
class UserService {
    // 100+ methods mixing user, email, and logging
};

// ✅ GOOD: Single responsibility
// src/services/UserService.cpp - Only user management
class UserService {
    bool createUser(const User& user);
    bool updateUser(const User& user);
    // ...
};
```

### Naming Conventions

- **Classes**: PascalCase (e.g., `UserService`, `DatabaseConnection`)
- **Methods**: camelCase (e.g., `getUserById()`, `validateEmail()`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_POOL_SIZE`, `JWT_EXPIRY_HOURS`)
- **Variables**: camelCase (e.g., `userId`, `isActive`)
- **Files**: PascalCase for headers (e.g., `UserService.h`), snake_case for source (e.g., `user_service.cpp`)

### C++ Standards

- Use **C++17** features: `std::optional`, `std::variant`, structured bindings
- Use `nullptr` instead of `NULL`
- Use `auto` for obvious types, explicit types otherwise
- Prefer `std::make_shared` over `new`
- Use `const` and `constexpr` liberally

```cpp
// ❌ BAD
int* ptr = new int(42);
const char* name = NULL;
auto processData = [](User u) { ... };  // unclear what User is

// ✅ GOOD
auto ptr = std::make_shared<int>(42);
const char* name = nullptr;
auto processUser = [](const User& user) { ... };  // clear parameter
```

### Comments and Documentation

Every file should have:

```cpp
// Purpose: Brief description of what this file contains
// Usage: Where and how to use it
// Dependencies: What this depends on

/// Explain why this function exists and what problem it solves
/// @param name Description of parameter
/// @return What this function returns
/// @throws What exceptions can be thrown
bool validateEmail(const std::string& email);
```

## Adding New Features

### 1. Plan First

Create header file with complete interface:

```cpp
// include/services/YourService.h
#ifndef CONFERENCE_SERVICES_YOURSERVICE_H
#define CONFERENCE_SERVICES_YOURSERVICE_H

#include <string>
#include <memory>
#include <nlohmann/json.hpp>
#include "database/DatabaseConnection.h"

using json = nlohmann::json;

namespace conference {
namespace services {

class YourService {
public:
    YourService(std::shared_ptr<database::DatabaseConnection> conn);
    
    /// Main functionality
    bool doSomething(const std::string& param);
    
private:
    std::shared_ptr<database::DatabaseConnection> connection;
    
    /// Helper method
    json formatResponse(const json& data);
};

}  // namespace services
}  // namespace conference

#endif
```

### 2. Implement

```cpp
// src/services/YourService.cpp
#include "services/YourService.h"
#include "utils/ValidationUtils.h"
#include "utils/JSONUtils.h"
#include "middleware/ErrorHandler.h"

namespace conference {
namespace services {

YourService::YourService(std::shared_ptr<database::DatabaseConnection> conn)
    : connection(conn) {
}

bool YourService::doSomething(const std::string& param) {
    // Validate input
    if (!ValidationUtils::validateTextInput(param)) {
        throw middleware::ValidationException("Invalid input");
    }
    
    try {
        // Query database
        auto results = connection->executeQuery("SELECT ...");
        
        // Process results
        // Return success
        return true;
    } catch (const std::exception& ex) {
        throw middleware::DatabaseException(ex.what());
    }
}

}  // namespace services
}  // namespace conference
```

### 3. Update CMakeLists.txt

```cmake
# Add your source file to build
target_sources(conference_server PRIVATE
    src/services/YourService.cpp
)
```

### 4. Add to main.cpp if Needed

```cpp
// In ConferenceManagementServer::initialize()
auto userService = std::make_shared<YourService>(connection);
```

### 5. Write Tests

```cpp
// tests/YourServiceTest.cpp
#include <gtest/gtest.h>
#include "services/YourService.h"

TEST(YourService, DoSomethingWorks) {
    auto mockConn = std::make_shared<MockDatabaseConnection>();
    auto service = std::make_shared<YourService>(mockConn);
    
    EXPECT_TRUE(service->doSomething("test"));
}
```

## Error Handling

Always use exception hierarchy:

```cpp
// ✅ GOOD: Specific exception type
try {
    user = getUserById(userId);
} catch (const middleware::NotFoundException& ex) {
    return JSONUtils::buildErrorResponse(404, "USER_NOT_FOUND", ex.getMessage());
} catch (const middleware::DatabaseException& ex) {
    logger->error("DB error: {}", ex.what());
    return JSONUtils::buildErrorResponse(500, "DATABASE_ERROR", "Internal server error");
}

// ❌ BAD: Generic exception catching
try {
    user = getUserById(userId);
} catch (...) {
    // Lost all error information!
}
```

## Database Queries

Always use parameterized queries to prevent SQL injection:

```cpp
// ❌ BAD: SQL injection vulnerability
std::string query = "SELECT * FROM users WHERE email = '" + email + "'";
auto results = connection->executeQuery(query);

// ✅ GOOD: Parameterized query
std::vector<std::string> params = {email};
auto results = connection->executeQueryWithParams(
    "SELECT * FROM users WHERE email = ?", params
);
```

## Logging

Use the spdlog logger for consistent logging:

```cpp
#include <spdlog/spdlog.h>

// In your class
std::shared_ptr<spdlog::logger> logger = spdlog::get("conference");

// Usage
logger->info("User {} created successfully", userId);
logger->warn("Password attempt for non-existent user {}", email);
logger->error("Database connection failed: {}", errorMsg);
logger->debug("Query execution time: {}ms", executionTime);
```

## Testing

### Unit Tests

```cpp
TEST(ValidationUtils, ValidateEmailAcceptsValidEmails) {
    EXPECT_TRUE(ValidationUtils::validateEmail("user@example.com"));
    EXPECT_TRUE(ValidationUtils::validateEmail("test.email+tag@example.co.uk"));
}

TEST(ValidationUtils, ValidateEmailRejectsInvalidEmails) {
    EXPECT_FALSE(ValidationUtils::validateEmail(""));
    EXPECT_FALSE(ValidationUtils::validateEmail("invalid.email"));
    EXPECT_FALSE(ValidationUtils::validateEmail("@example.com"));
}
```

### Running Tests

```bash
cd build
cmake --build . --target test
```

### Code Coverage

```bash
# Enable coverage in CMake
cmake .. -DCOVERAGE=ON
cmake --build .
make coverage
# Open htmlcov/index.html
```

## Performance Considerations

### Connection Pooling

Always reuse database connections from the pool:

```cpp
// ✅ GOOD
auto conn = connectionPool->getConnection();
auto results = conn->executeQuery(...);
connectionPool->releaseConnection(conn);

// ❌ BAD: Creating new connection each time
auto conn = std::make_shared<DatabaseConnection>();
conn->connect(connectionString);
```

### Avoiding N+1 Queries

```cpp
// ❌ BAD: N+1 queries
auto submissions = getSubmissions();  // 1 query
for (auto& sub : submissions) {
    sub.author = getUser(sub.authorId);  // N queries
}

// ✅ GOOD: Join query
auto submissions = getSubmissionsWithAuthors();  // 1 query with JOIN
```

### Caching

For frequently accessed data:

```cpp
class UserService {
private:
    std::map<int, User> userCache;
    std::chrono::steady_clock::time_point cacheLastUpdate;
    
    static constexpr int CACHE_TTL_SECONDS = 3600;
};
```

## Memory Management

Use smart pointers throughout:

```cpp
// ❌ BAD: Manual memory management
User* user = new User();
// ... someone might forget to delete

// ✅ GOOD: Smart pointers
auto user = std::make_shared<User>();
// Automatically freed when last reference is released
```

## Debugging

### Enable Debug Output

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_LOGGING=ON
```

### Use Debug Assertions

```cpp
// ✅ GOOD: Catches bugs in development
assert(userId > 0);  // Will abort if userId is invalid
assert(!email.empty());

// For production checks
if (userId <= 0) {
    throw middleware::ValidationException("Invalid user ID");
}
```

### Breakpoints and Inspection

```cpp
// In debugger
break UserService.cpp:42  // Set breakpoint at line 42
print user.email  // Inspect variable
step  // Step into function
continue  // Continue execution
```

## Git Workflow

```bash
# Create feature branch
git checkout -b feature/user-management

# Make changes following code style
# Write tests
git add .
git commit -m "feat: add user management service"

# Push and create PR
git push origin feature/user-management
```

### Commit Messages

Follow conventional commits:

```
feat: add user registration endpoint
fix: prevent SQL injection in email validation
docs: update API documentation
test: add unit tests for password hashing
refactor: simplify connection pool logic
perf: optimize user query with indexes
```

## Security Checklist

Before committing:

- [ ] No hardcoded secrets (use environment variables)
- [ ] Input validation on all user inputs
- [ ] SQL injection prevention (parameterized queries)
- [ ] XSS prevention (escape HTML)
- [ ] CSRF tokens for state-changing operations
- [ ] HTTPS/TLS in production
- [ ] Password hashing (never store plaintext)
- [ ] Rate limiting on authentication endpoints

## Documentation

### Code Comments

When comments are needed:

```cpp
// ✅ GOOD: Explains why, not what
// Use lazy loading to avoid N+1 queries when fetching users with submissions
auto users = getUsersLazy();

// ❌ BAD: Restates obvious code
// Get all users
auto users = getAllUsers();
```

### API Documentation

Document all endpoints:

```
POST /api/v1/users
Create a new user account

Request:
{
  "email": "user@example.com",
  "firstName": "John",
  "lastName": "Doe"
}

Response 201:
{
  "success": true,
  "data": { "userId": 1, ... }
}

Response 409:
{
  "success": false,
  "error": { "code": "CONFLICT", "message": "Email already registered" }
}
```

## Troubleshooting

### Compilation Errors

```bash
# Clear build cache and rebuild
rm -rf build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -- VERBOSE=1
```

### Runtime Issues

```bash
# Check logs
tail -f ./logs/conference_system.log

# Run with debug output
./bin/conference_server --debug --verbose
```

### Memory Leaks

```bash
# Build with AddressSanitizer
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined"
cmake --build .
./bin/conference_server
```

## Resources

- [Modern C++ Practices](https://isocpp.org/guidelines)
- [PostgreSQL Documentation](https://www.postgresql.org/docs/)
- [Dogon Framework Docs](https://dogon.io)
- [spdlog Documentation](https://github.com/gabime/spdlog)
- [nlohmann/json API](https://nlohmann.github.io/json/)
