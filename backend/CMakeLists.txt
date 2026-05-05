cmake_minimum_required(VERSION 3.15)
project(ConferenceManagementSystem VERSION 1.0.0 LANGUAGES CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set PostgreSQL root on Windows
if(WIN32)
    set(PostgreSQL_ROOT "C:/Program Files/PostgreSQL/18")
    set(PostgreSQL_INCLUDE_DIR "C:/Program Files/PostgreSQL/18/include")
    set(PostgreSQL_LIBRARY_DIR "C:/Program Files/PostgreSQL/18/lib")
endif()

# Enable testing
enable_testing()

# Add conan dependencies (if available)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
if(EXISTS ${CMAKE_BINARY_DIR}/conandeps_legacy.cmake)
    include(${CMAKE_BINARY_DIR}/conandeps_legacy.cmake)
elseif(EXISTS ${CMAKE_BINARY_DIR}/conandeps.cmake)
    include(${CMAKE_BINARY_DIR}/conandeps.cmake)
endif()

# Define source files
set(HEADERS
    include/utils/Constants.h
    include/utils/ValidationUtils.h
    include/utils/EncryptionUtils.h
    include/utils/DateTimeUtils.h
    include/utils/FileUtils.h
    include/utils/EmailUtils.h
    include/utils/JSONUtils.h
    include/utils/EnvUtils.h
    include/models/BaseModel.h
    include/database/DatabaseConnection.h
    include/database/ConnectionPool.h
    include/database/Transaction.h
    include/database/DatabaseMigration.h
    include/middleware/AuthMiddleware.h
    include/middleware/ErrorHandler.h
    include/middleware/CORSMiddleware.h
    include/middleware/LoggingMiddleware.h
    include/middleware/ValidationMiddleware.h
    include/services/IAuthService.h
    include/services/IUserService.h
    include/services/IReportingService.h
    include/services/IConfigService.h
    include/services/INotificationService.h
    include/repositories/IUserRepository.h
    include/repositories/IFileRepository.h
    include/repositories/ISubmissionRepository.h
    include/repositories/IReviewRepository.h
    include/repositories/ISessionRepository.h
    include/repositories/IParticipantRepository.h
    include/repositories/IFeedbackRepository.h
    include/repositories/INotificationRepository.h
    include/repositories/IConfigRepository.h
    include/controllers/ControllerHelpers.h
    include/controllers/AuthController.h
    include/controllers/UserController.h
    include/controllers/SubmissionController.h
    include/controllers/ReviewController.h
    include/controllers/SessionController.h
    include/controllers/ParticipantController.h
    include/controllers/FeedbackController.h
    include/controllers/ReportingController.h
    include/controllers/ConfigController.h
    include/database/ICacheProvider.h
    include/utils/TaskQueue.h
)

set(SOURCES
    src/utils/ValidationUtils.cpp
    src/utils/EncryptionUtils.cpp
    src/utils/DateTimeUtils.cpp
    src/utils/FileUtils.cpp
    src/utils/EmailUtils.cpp
    src/utils/JSONUtils.cpp
    src/utils/TaskQueue.cpp
    src/utils/EnvUtils.cpp
    src/models/BaseModel.cpp
    src/database/DatabaseConnection.cpp
    src/database/ConnectionPool.cpp
    src/database/Transaction.cpp
    src/database/DatabaseMigration.cpp
    src/database/InMemoryCache.cpp
    src/middleware/AuthMiddleware.cpp
    src/middleware/ErrorHandler.cpp
    src/middleware/CORSMiddleware.cpp
    src/middleware/LoggingMiddleware.cpp
    src/middleware/ValidationMiddleware.cpp
    src/services/AuthService.cpp
    src/services/UserService.cpp
    src/services/FileService.cpp
    src/services/SubmissionService.cpp
    src/services/ReviewService.cpp
    src/services/SessionService.cpp
    src/services/ParticipantService.cpp
    src/services/FeedbackService.cpp
    src/services/NotificationService.cpp
    src/services/ReportingService.cpp
    src/services/ConfigService.cpp
    src/repositories/UserRepository.cpp
    src/repositories/FileRepository.cpp
    src/repositories/SubmissionRepository.cpp
    src/repositories/ReviewRepository.cpp
    src/repositories/SessionRepository.cpp
    src/repositories/ParticipantRepository.cpp
    src/repositories/FeedbackRepository.cpp
    src/repositories/NotificationRepository.cpp
    src/repositories/ConfigRepository.cpp
    src/controllers/AuthController.cpp
    src/controllers/UserController.cpp
    src/controllers/SubmissionController.cpp
    src/controllers/ReviewController.cpp
    src/controllers/SessionController.cpp
    src/controllers/ParticipantController.cpp
    src/controllers/FeedbackController.cpp
    src/controllers/ReportingController.cpp
    src/controllers/ConfigController.cpp
    src/main.cpp
)

# Create executable
add_executable(conference_api ${HEADERS} ${SOURCES})

# Link libraries
target_include_directories(conference_api PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Find required packages
find_package(nlohmann_json 3.11 REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
if(spdlog_FOUND)
    # Link to the header-only target which correctly sets up all sub-include paths
    target_link_libraries(conference_api PRIVATE spdlog::spdlog_header_only fmt::fmt)
    # Force header-only mode and use external fmt
    target_compile_definitions(conference_api PRIVATE SPDLOG_HEADER_ONLY SPDLOG_FMT_EXTERNAL)
endif()
find_package(OpenSSL QUIET)

# Force CMake to find the MinGW-compatible PostgreSQL library installed via MSYS2
set(PostgreSQL_ROOT "C:/msys64/ucrt64")
find_package(PostgreSQL QUIET)

# Build target link libraries list
set(TARGET_LIBS nlohmann_json::nlohmann_json)
if(OpenSSL_FOUND)
    list(APPEND TARGET_LIBS OpenSSL::Crypto OpenSSL::SSL)
endif()
if(PostgreSQL_FOUND)
    list(APPEND TARGET_LIBS PostgreSQL::PostgreSQL)
else()
    list(APPEND TARGET_LIBS pq)
endif()
if(WIN32)
    list(APPEND TARGET_LIBS ws2_32 wsock32 stdc++fs)
else()
    list(APPEND TARGET_LIBS pthread stdc++fs)
endif()

target_link_libraries(conference_api PRIVATE ${TARGET_LIBS})

# Set output directory
set_target_properties(conference_api PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Tests (optional)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Install targets (optional)
install(TARGETS conference_api DESTINATION bin)
