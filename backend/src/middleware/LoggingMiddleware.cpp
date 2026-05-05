#include "middleware/LoggingMiddleware.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <chrono>
#include <string>

namespace conference {
namespace middleware {

static std::shared_ptr<spdlog::logger> getLogger() {
    auto logger = spdlog::get("http_access");
    if (!logger) {
        try {
            spdlog::sink_ptr sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            logger = std::make_shared<spdlog::logger>("http_access", sink);
            spdlog::register_logger(logger);
        } catch (...) {
            logger = spdlog::get("conference");
        }
    }
    return logger;
}

/**
 * @brief Logs an incoming HTTP request with method, path, and client IP.
 *        Call this at the start of every route handler.
 */
void LoggingMiddleware::logRequest(const std::string& method,
                                   const std::string& path,
                                   const std::string& clientIp) {
    auto logger = getLogger();
    if (logger) {
        logger->info("[REQUEST]  {} {} from {}", method, path, clientIp);
    }
}

/**
 * @brief Logs a completed HTTP response with status code and latency.
 *        Call this just before sending the response.
 */
void LoggingMiddleware::logResponse(const std::string& method,
                                    const std::string& path,
                                    int statusCode,
                                    long long durationMs) {
    auto logger = getLogger();
    if (logger) {
        if (statusCode >= 500) {
            logger->error("[RESPONSE] {} {} -> {} ({}ms)", method, path, statusCode, durationMs);
        } else if (statusCode >= 400) {
            logger->warn("[RESPONSE] {} {} -> {} ({}ms)", method, path, statusCode, durationMs);
        } else {
            logger->info("[RESPONSE] {} {} -> {} ({}ms)", method, path, statusCode, durationMs);
        }
    }
}

/**
 * @brief Returns current time as milliseconds since epoch (for latency tracking).
 */
long long LoggingMiddleware::nowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

} // namespace middleware
} // namespace conference
