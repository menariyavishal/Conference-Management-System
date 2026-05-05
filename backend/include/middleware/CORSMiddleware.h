#ifndef CORS_MIDDLEWARE_H
#define CORS_MIDDLEWARE_H

#include <string>
#include <vector>
#include <map>

namespace conference {
namespace middleware {

struct CORSConfig {
    std::vector<std::string> allowedOrigins;
    std::vector<std::string> allowedMethods;
    std::vector<std::string> allowedHeaders;
    std::vector<std::string> exposedHeaders;
    bool allowCredentials;
    int maxAge;
};

class CORSMiddleware {
private:
    static CORSConfig corsConfig;
    static bool isConfigured;

public:
    // Configure CORS
    static void configureCORS(const CORSConfig& config);
    static void addAllowedOrigin(const std::string& origin);
    static void addAllowedMethod(const std::string& method);
    static void addAllowedHeader(const std::string& header);
    static void addExposedHeader(const std::string& header);
    static void setAllowCredentials(bool allow);
    static void setMaxAge(int seconds);
    
    // Reset configuration
    static void resetConfiguration();
    
    // Validate request
    static bool validateOrigin(const std::string& origin);
    static bool validateMethod(const std::string& method);
    static bool validateHeaders(const std::vector<std::string>& headers);
    
    // Build CORS headers
    static std::map<std::string, std::string> getCORSHeaders(const std::string& origin);
    static std::string getCORSOrigin(const std::string& origin);
    static std::string getCORSMethods();
    static std::string getCORSHeaders();
    static std::string getCORSExposedHeaders();
    static std::string getMaxAge();
    static std::string getAllowCredentials();
    
    // Handle preflight requests
    static std::map<std::string, std::string> handlePreflightRequest(const std::string& origin, 
                                                                      const std::string& requestMethod,
                                                                      const std::string& requestHeaders);
    
    // Check if origin is allowed
    static bool isOriginAllowed(const std::string& origin);
    
    // Get configuration
    static CORSConfig getConfiguration();
    
    // Default configuration (for development)
    static void configureForDevelopment();
    
    // Production configuration
    static void configureForProduction(const std::vector<std::string>& allowedDomains);

private:
    static bool wildCardMatches(const std::string& pattern, const std::string& value);
};

}  // namespace middleware
}  // namespace conference

#endif  // CORS_MIDDLEWARE_H
