#include <iostream>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <httplib.h>

#include "utils/Constants.h"
#include "utils/ValidationUtils.h"
#include "utils/EncryptionUtils.h"
#include "utils/DateTimeUtils.h"
#include "utils/FileUtils.h"
#include "utils/JSONUtils.h"
#include "utils/EnvUtils.h"
#include "database/DatabaseConnection.h"
#include "database/ConnectionPool.h"
#include "database/DatabaseMigration.h"
#include "middleware/AuthMiddleware.h"
#include "middleware/CORSMiddleware.h"
#include "middleware/ErrorHandler.h"
#include "middleware/LoggingMiddleware.h"

// Repositories
#include "repositories/IUserRepository.h"
#include "repositories/ISubmissionRepository.h"
#include "repositories/IReviewRepository.h"
#include "repositories/IFileRepository.h"
#include "repositories/ISessionRepository.h"
#include "repositories/IParticipantRepository.h"
#include "repositories/IFeedbackRepository.h"
#include "repositories/INotificationRepository.h"

// Services
#include "services/IAuthService.h"
#include "services/IUserService.h"
#include "services/ISubmissionService.h"
#include "services/IReviewService.h"
#include "services/IFileService.h"
#include "services/ISessionService.h"
#include "services/IParticipantService.h"
#include "services/IFeedbackService.h"
#include "services/INotificationService.h"

// Controllers
#include "controllers/AuthController.h"
#include "controllers/UserController.h"
#include "controllers/SubmissionController.h"
#include "controllers/ReviewController.h"
#include "controllers/SessionController.h"
#include "controllers/ParticipantController.h"
#include "controllers/FeedbackController.h"
#include "controllers/ReportingController.h"
#include "controllers/ConfigController.h"

// Repositories
#include "repositories/IConfigRepository.h"

// Services
#include "services/IReportingService.h"
#include "services/IConfigService.h"

// Database & Utils
#include "database/ICacheProvider.h"
#include "utils/TaskQueue.h"

using json = nlohmann::json;

namespace conference {

class ConferenceManagementServer {
private:
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<database::ConnectionPool> connectionPool;
    middleware::CORSMiddleware corsMiddleware;

    // Controllers (instantiated after DI wiring)
    std::shared_ptr<controllers::AuthController> authController;
    std::shared_ptr<controllers::UserController> userController;
    std::shared_ptr<controllers::SubmissionController> submissionController;
    std::shared_ptr<controllers::ReviewController> reviewController;
    std::shared_ptr<controllers::SessionController> sessionController;
    std::shared_ptr<controllers::ParticipantController> participantController;
    std::shared_ptr<controllers::FeedbackController> feedbackController;
    std::shared_ptr<controllers::ReportingController> reportingController;
    std::shared_ptr<controllers::ConfigController> configController;

    // Infrastructure members
    std::shared_ptr<utils::TaskQueue> taskQueue;
    std::shared_ptr<database::ICacheProvider> cacheProvider;
    
public:
    ConferenceManagementServer() {
        initializeLogger();
        logger->info("Initializing Conference Management System Server");
    }
    
    ~ConferenceManagementServer() {
        shutdown();
    }
    
    bool initialize() {
        logger->info("=== Conference Management System v{} ===", constants::APP_VERSION);
        logger->info("API Version: {}", constants::API_VERSION);
        
        // Initialize database connection pool
        if (!initializeDatabase()) {
            logger->error("Failed to initialize database");
            return false;
        }
        
        // Apply database migrations
        if (!applyMigrations()) {
            logger->error("Failed to apply database migrations");
            return false;
        }
        
        // Configure CORS
        configureCORS();
        
        // Configure authentication
        configureAuth();
        
        // Wire all repositories, services, and controllers
        if (!wireDependencies()) {
            logger->error("Failed to wire dependencies");
            return false;
        }

        logger->info("Server initialized successfully");
        return true;
    }
    
    bool start() {
        logger->info("Starting Conference Management Server on port {}", constants::API_PORT);

        httplib::Server svr;

        // ── Global CORS pre-flight & headers ───────────────────────────────────
        svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
            // Append CORS headers to EVERY request
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Accept");

            // If it's a pre-flight request, handle it immediately and stop routing
            if (req.method == "OPTIONS") {
                res.status = 204;
                return httplib::Server::HandlerResponse::Handled;
            }

            // Otherwise, let the normal routing continue
            return httplib::Server::HandlerResponse::Unhandled;
        });


        // ── Auth routes ───────────────────────────────────────────────────────
        svr.Post("/api/v1/auth/register", [this](const httplib::Request& req, httplib::Response& res) {
            middleware::LoggingMiddleware::logRequest("POST", "/api/v1/auth/register", req.remote_addr);
            authController->handleRegister(req, res);
        });
        svr.Post("/api/v1/auth/login", [this](const httplib::Request& req, httplib::Response& res) {
            middleware::LoggingMiddleware::logRequest("POST", "/api/v1/auth/login", req.remote_addr);
            authController->handleLogin(req, res);
        });
        svr.Post("/api/v1/auth/refresh-token", [this](const httplib::Request& req, httplib::Response& res) {
            authController->handleRefreshToken(req, res);
        });
        svr.Post("/api/v1/auth/logout", [this](const httplib::Request& req, httplib::Response& res) {
            authController->handleLogout(req, res);
        });

        // ── User routes ───────────────────────────────────────────────────────
        svr.Get("/api/v1/users", [this](const httplib::Request& req, httplib::Response& res) {
            userController->handleListUsers(req, res);
        });
        svr.Get(R"(/api/v1/users/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
            userController->handleGetUser(req, res);
        });
        svr.Put(R"(/api/v1/users/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
            userController->handleUpdateUser(req, res);
        });
        svr.Delete(R"(/api/v1/users/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
            userController->handleDeactivateUser(req, res);
        });
        svr.Put(R"(/api/v1/users/([^/]+)/role)", [this](const httplib::Request& req, httplib::Response& res) {
            userController->handleUpdateRole(req, res);
        });
        svr.Get(R"(/api/v1/users/([^/]+)/notifications)", [this](const httplib::Request& req, httplib::Response& res) {
            userController->handleGetNotifications(req, res);
        });
        svr.Get(R"(/api/v1/users/([^/]+)/sessions)", [this](const httplib::Request& req, httplib::Response& res) {
            participantController->handleGetUserSessions(req, res);
        });

        // ── Submission routes ─────────────────────────────────────────────────
        svr.Post("/api/v1/submissions", [this](const httplib::Request& req, httplib::Response& res) {
            submissionController->handleCreate(req, res);
        });
        svr.Get("/api/v1/submissions", [this](const httplib::Request& req, httplib::Response& res) {
            submissionController->handleList(req, res);
        });
        svr.Get(R"(/api/v1/submissions/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
            submissionController->handleGet(req, res);
        });
        svr.Put(R"(/api/v1/submissions/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
            submissionController->handleUpdate(req, res);
        });
        svr.Delete(R"(/api/v1/submissions/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
            submissionController->handleWithdraw(req, res);
        });
        svr.Get(R"(/api/v1/submissions/([^/]+)/reviews)", [this](const httplib::Request& req, httplib::Response& res) {
            submissionController->handleGetReviews(req, res);
        });
        svr.Post(R"(/api/v1/submissions/([^/]+)/file/upload)", [this](const httplib::Request& req, httplib::Response& res) {
            submissionController->handleUploadFile(req, res);
        });
        svr.Get(R"(/api/v1/submissions/([^/]+)/file/download)", [this](const httplib::Request& req, httplib::Response& res) {
            submissionController->handleDownloadFile(req, res);
        });

        // ── Review routes ─────────────────────────────────────────────────────
        svr.Post("/api/v1/reviews/assign", [this](const httplib::Request& req, httplib::Response& res) {
            reviewController->handleAssignReview(req, res);
        });
        svr.Post("/api/v1/reviews", [this](const httplib::Request& req, httplib::Response& res) {
            reviewController->handleSubmitReview(req, res);
        });
        svr.Get("/api/v1/reviews", [this](const httplib::Request& req, httplib::Response& res) {
            reviewController->handleListReviews(req, res);
        });
        svr.Get(R"(/api/v1/reviews/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
            reviewController->handleGetReview(req, res);
        });
        svr.Get("/api/v1/reviews/statistics", [this](const httplib::Request& req, httplib::Response& res) {
            reviewController->handleGetStatistics(req, res);
        });

        // ── Session routes ────────────────────────────────────────────────────
        svr.Post("/api/v1/sessions", [this](const httplib::Request& req, httplib::Response& res) {
            sessionController->handleCreate(req, res);
        });
        svr.Get("/api/v1/sessions", [this](const httplib::Request& req, httplib::Response& res) {
            sessionController->handleList(req, res);
        });
        svr.Get(R"(/api/v1/sessions/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
            sessionController->handleGet(req, res);
        });
        svr.Put(R"(/api/v1/sessions/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
            sessionController->handleUpdate(req, res);
        });
        svr.Delete(R"(/api/v1/sessions/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
            sessionController->handleDelete(req, res);
        });
        svr.Post(R"(/api/v1/sessions/([^/]+)/papers)", [this](const httplib::Request& req, httplib::Response& res) {
            sessionController->handleAddPaper(req, res);
        });
        svr.Delete(R"(/api/v1/sessions/([^/]+)/papers/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
            sessionController->handleRemovePaper(req, res);
        });
        svr.Get(R"(/api/v1/sessions/([^/]+)/participants)", [this](const httplib::Request& req, httplib::Response& res) {
            sessionController->handleGetParticipants(req, res);
        });

        // ── Participant routes ────────────────────────────────────────────────
        svr.Post(R"(/api/v1/sessions/([^/]+)/register)", [this](const httplib::Request& req, httplib::Response& res) {
            participantController->handleRegister(req, res);
        });
        svr.Delete(R"(/api/v1/sessions/([^/]+)/register)", [this](const httplib::Request& req, httplib::Response& res) {
            participantController->handleDeregister(req, res);
        });

        // ── Feedback routes ───────────────────────────────────────────────────
        svr.Post(R"(/api/v1/sessions/([^/]+)/feedback)", [this](const httplib::Request& req, httplib::Response& res) {
            feedbackController->handleSubmit(req, res);
        });
        svr.Get(R"(/api/v1/sessions/([^/]+)/feedback/summary)", [this](const httplib::Request& req, httplib::Response& res) {
            feedbackController->handleGetSummary(req, res);
        });
        svr.Get("/api/v1/feedback", [this](const httplib::Request& req, httplib::Response& res) {
            feedbackController->handleListAll(req, res);
        });

        // ── Reporting & Config routes ─────────────────────────────────────────
        reportingController->registerRoutes(svr);
        configController->registerRoutes(svr);

        // ── Health check ──────────────────────────────────────────────────────
        svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
            res.set_header("Content-Type", "application/json");
            res.body = R"({"status":"ok","service":"conference-management-system"})";
            res.status = 200;
        });

        logger->info("All {} routes registered. Listening on port {}...",
                     42, constants::API_PORT);
        std::cout << "Conference Management API running at http://localhost:"
                  << constants::API_PORT << "/api/v1" << std::endl;
        std::cout << "Health check: http://localhost:" << constants::API_PORT << "/health" << std::endl;
        std::cout << "Press Ctrl+C to shutdown..." << std::endl;

        svr.listen("0.0.0.0", constants::API_PORT);
        return true;
    }
    
    void shutdown() {
        logger->info("Shutting down server...");
        
        if (taskQueue) {
            taskQueue->shutdown();
        }
        
        if (connectionPool) {
            connectionPool->shutdown();
        }
        
        logger->info("Server shut down successfully");
    }
    
    bool wireDependencies() {
        logger->info("Wiring repositories, services, and controllers...");
        try {
            // -- Infrastructure --
            taskQueue     = std::make_shared<utils::TaskQueue>(4);
            cacheProvider = database::createInMemoryCache();

            // -- Repositories --
            auto userRepo         = createUserRepository(connectionPool);
            auto submissionRepo   = createSubmissionRepository(connectionPool);
            auto reviewRepo       = createReviewRepository(connectionPool);
            auto fileRepo         = createFileRepository(connectionPool);
            auto sessionRepo      = createSessionRepository(connectionPool);
            auto participantRepo  = createParticipantRepository(connectionPool);
            auto feedbackRepo     = createFeedbackRepository(connectionPool);
            auto notificationRepo = createNotificationRepository(connectionPool);
            auto configRepo       = createConfigRepository(connectionPool);

            // -- Services --
            auto authSvc         = createAuthService();
            auto userSvc         = createUserService(authSvc, userRepo);
            auto fileSvc         = createFileService(fileRepo);
            auto submissionSvc   = createSubmissionService(fileSvc, submissionRepo);
            auto reviewSvc       = createReviewService(reviewRepo, submissionRepo, userRepo);
            auto sessionSvc      = createSessionService(sessionRepo, participantRepo, cacheProvider);
            auto participantSvc  = createParticipantService(participantRepo, sessionRepo);
            auto feedbackSvc     = createFeedbackService(feedbackRepo, participantRepo);
            auto notifSvc        = createNotificationService(notificationRepo, taskQueue);
            auto configSvc       = createConfigService(configRepo);
            auto reportingSvc    = createReportingService(submissionRepo, reviewRepo, userRepo, sessionRepo, participantRepo, feedbackRepo);

            // -- Controllers --
            authController        = std::make_shared<controllers::AuthController>(userSvc, authSvc);
            userController        = std::make_shared<controllers::UserController>(userSvc, authSvc, notifSvc);
            submissionController  = std::make_shared<controllers::SubmissionController>(submissionSvc, reviewSvc, authSvc, fileSvc);
            reviewController      = std::make_shared<controllers::ReviewController>(reviewSvc, authSvc);
            sessionController     = std::make_shared<controllers::SessionController>(sessionSvc, participantSvc, authSvc);
            participantController = std::make_shared<controllers::ParticipantController>(participantSvc, authSvc);
            feedbackController    = std::make_shared<controllers::FeedbackController>(feedbackSvc, authSvc);
            reportingController   = std::make_shared<controllers::ReportingController>(reportingSvc);
            configController      = std::make_shared<controllers::ConfigController>(configSvc);

            logger->info("All dependencies wired successfully");
            return true;
        } catch (const std::exception& ex) {
            logger->error("Dependency wiring failed: {}", ex.what());
            return false;
        }
    }

private:
    void initializeLogger() {
        try {
            // Create console sink with color
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::info);
            
            // Create rotating file sink (max 10 MB, 3 files)
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                "./logs/conference_system.log", 10 * 1024 * 1024, 3);
            file_sink->set_level(spdlog::level::debug);
            
            // Combine sinks
            std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
            logger = std::make_shared<spdlog::logger>("conference", sinks.begin(), sinks.end());
            logger->set_level(spdlog::level::debug);
            logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%l] %v");
            
            spdlog::register_logger(logger);
        } catch (const spdlog::spdlog_ex& ex) {
            std::cout << "Log initialization failed: " << ex.what() << std::endl;
            logger = spdlog::stdout_color_mt("conference");
        }
    }
    
    bool initializeDatabase() {
        logger->info("Initializing database connection pool");
        
        std::string host         = utils::EnvUtils::get("DB_HOST", "localhost");
        std::string port         = utils::EnvUtils::get("DB_PORT", "5432");
        std::string databaseName = utils::EnvUtils::get("DB_NAME", "conference_db");
        std::string user         = utils::EnvUtils::get("DB_USER", "postgres");
        std::string password     = utils::EnvUtils::get("DB_PASSWORD", "");
        
        if (password.empty()) {
            logger->warn("Database password is empty. Check your .env file.");
        }
        
        // 1. First connect to 'postgres' database to ensure conference_db exists
        {
            std::string adminConnStr = "host=" + host + " port=" + port + " dbname=postgres user=" + user + " password=" + password;
            auto adminConn = std::make_unique<database::DatabaseConnection>();
            if (adminConn->connect(adminConnStr)) {
                logger->info("Connected to postgres admin database to verify conference_db");
                
                std::string checkQuery = "SELECT 1 FROM pg_database WHERE datname = '" + databaseName + "'";
                auto result = adminConn->executeScalar(checkQuery);
                
                if (result != "1") {
                    logger->info("Database {} does not exist. Creating it...", databaseName);
                    if (adminConn->execute("CREATE DATABASE " + databaseName)) {
                        logger->info("Database {} created successfully", databaseName);
                    } else {
                        logger->error("Failed to create database {}: {}", databaseName, adminConn->getLastError());
                        return false;
                    }
                } else {
                    logger->info("Database {} already exists", databaseName);
                }
                adminConn->disconnect();
            } else {
                logger->error("Failed to connect to postgres admin database. Check your credentials in .env.");
                return false;
            }
        }

        // 2. Now connect to the actual conference_db
        std::string connectionString = 
            "host=" + host + 
            " port=" + port + 
            " dbname=" + databaseName + 
            " user=" + user + 
            " password=" + password;
        
        connectionPool = std::make_shared<database::ConnectionPool>(5, 20);
        
        if (!connectionPool->initialize(connectionString)) {
            logger->error("Failed to initialize connection pool for {}", databaseName);
            return false;
        }
        
        logger->info("Database connection pool initialized successfully for {}", databaseName);
        return true;
    }
    
    bool applyMigrations() {
        logger->info("Applying database migrations");
        
        auto conn = connectionPool->getConnection();
        if (!conn) {
            logger->error("Failed to get database connection for migrations");
            return false;
        }
        
        auto migration = std::make_shared<database::DatabaseMigration>(conn);
        
        if (!migration->initialize()) {
            logger->error("Failed to initialize migrations table");
            connectionPool->releaseConnection(conn);
            return false;
        }
        
        if (!migration->migrate()) {
            logger->error("Failed to apply migrations");
            connectionPool->releaseConnection(conn);
            return false;
        }
        
        logger->info("Database migrations completed successfully");
        connectionPool->releaseConnection(conn);
        
        return true;
    }
    
    void configureCORS() {
        logger->info("Configuring CORS middleware");
        
        // Development configuration
        corsMiddleware.configureForDevelopment();
        
        logger->info("CORS configured for development environment");
    }
    
    void configureAuth() {
        logger->info("Configuring authentication");
        
        // Set JWT secret from environment
        std::string jwtSecret = utils::EnvUtils::get("JWT_SECRET", "development-secret-key-change-in-production");
        middleware::AuthMiddleware::setJWTSecret(jwtSecret);
        
        logger->info("Authentication configured with JWT");
    }
};

}  // namespace conference

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    // Load .env file
    if (conference::utils::EnvUtils::loadEnv(".env")) {
        std::cout << "Loaded environment from .env file" << std::endl;
    } else if (conference::utils::EnvUtils::loadEnv("../.env")) {
        std::cout << "Loaded environment from ../.env file" << std::endl;
    } else {
        std::cout << "No .env file found, using system environment variables" << std::endl;
    }

    try {
        auto server = std::make_unique<conference::ConferenceManagementServer>();
        
        if (!server->initialize()) {
            std::cerr << "Failed to initialize server" << std::endl;
            return 1;
        }
        
        if (!server->start()) {
            std::cerr << "Failed to start server" << std::endl;
            return 1;
        }
        
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return 1;
    }
}
