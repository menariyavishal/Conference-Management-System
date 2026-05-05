#pragma once
#include <string>

namespace conference {
namespace middleware {

class LoggingMiddleware {
public:
    static void logRequest(const std::string& method,
                           const std::string& path,
                           const std::string& clientIp);

    static void logResponse(const std::string& method,
                            const std::string& path,
                            int statusCode,
                            long long durationMs);

    static long long nowMs();
};

} // namespace middleware
} // namespace conference
