#include "middleware/CORSMiddleware.h"
#include <algorithm>
#include <sstream>

namespace conference {
namespace middleware {

// Initialize static members
CORSConfig CORSMiddleware::corsConfig;
bool CORSMiddleware::isConfigured = false;

void CORSMiddleware::configureCORS(const CORSConfig& config) {
    corsConfig = config;
    isConfigured = true;
}

void CORSMiddleware::addAllowedOrigin(const std::string& origin) {
    corsConfig.allowedOrigins.push_back(origin);
}

void CORSMiddleware::addAllowedMethod(const std::string& method) {
    corsConfig.allowedMethods.push_back(method);
}

void CORSMiddleware::addAllowedHeader(const std::string& header) {
    corsConfig.allowedHeaders.push_back(header);
}

void CORSMiddleware::addExposedHeader(const std::string& header) {
    corsConfig.exposedHeaders.push_back(header);
}

void CORSMiddleware::setAllowCredentials(bool allow) {
    corsConfig.allowCredentials = allow;
}

void CORSMiddleware::setMaxAge(int seconds) {
    corsConfig.maxAge = seconds;
}

void CORSMiddleware::resetConfiguration() {
    corsConfig = CORSConfig{};
    isConfigured = false;
}

bool CORSMiddleware::validateOrigin(const std::string& origin) {
    if (!isConfigured) return true;
    if (corsConfig.allowedOrigins.empty()) return false;
    
    for (const auto& pattern : corsConfig.allowedOrigins) {
        if (pattern == "*" || pattern == origin || wildCardMatches(pattern, origin)) {
            return true;
        }
    }
    return false;
}

bool CORSMiddleware::validateMethod(const std::string& method) {
    if (!isConfigured) return true;
    if (corsConfig.allowedMethods.empty()) return false;
    
    auto it = std::find(corsConfig.allowedMethods.begin(), corsConfig.allowedMethods.end(), method);
    return it != corsConfig.allowedMethods.end();
}

bool CORSMiddleware::validateHeaders(const std::vector<std::string>& headers) {
    if (!isConfigured) return true;
    if (corsConfig.allowedHeaders.empty()) return true;
    
    for (const auto& header : headers) {
        auto it = std::find(corsConfig.allowedHeaders.begin(), corsConfig.allowedHeaders.end(), header);
        if (it == corsConfig.allowedHeaders.end()) return false;
    }
    return true;
}

std::map<std::string, std::string> CORSMiddleware::getCORSHeaders(const std::string& origin) {
    std::map<std::string, std::string> headers;
    if (!isConfigured) return headers;

    if (validateOrigin(origin)) {
        headers["Access-Control-Allow-Origin"] = getCORSOrigin(origin);
        headers["Access-Control-Allow-Methods"] = getCORSMethods();
        headers["Access-Control-Allow-Headers"] = getCORSHeaders();
        
        std::string exposed = getCORSExposedHeaders();
        if (!exposed.empty()) {
            headers["Access-Control-Expose-Headers"] = exposed;
        }
        
        headers["Access-Control-Allow-Credentials"] = getAllowCredentials();
        headers["Access-Control-Max-Age"] = getMaxAge();
    }
    
    return headers;
}

std::string CORSMiddleware::getCORSOrigin(const std::string& origin) {
    if (corsConfig.allowedOrigins.empty()) return "";
    if (corsConfig.allowedOrigins[0] == "*") return "*";
    return origin;
}

std::string CORSMiddleware::getCORSMethods() {
    std::string methods;
    for (size_t i = 0; i < corsConfig.allowedMethods.size(); ++i) {
        if (i > 0) methods += ", ";
        methods += corsConfig.allowedMethods[i];
    }
    return methods;
}

std::string CORSMiddleware::getCORSHeaders() {
    std::string headers;
    for (size_t i = 0; i < corsConfig.allowedHeaders.size(); ++i) {
        if (i > 0) headers += ", ";
        headers += corsConfig.allowedHeaders[i];
    }
    return headers;
}

std::string CORSMiddleware::getCORSExposedHeaders() {
    std::string headers;
    for (size_t i = 0; i < corsConfig.exposedHeaders.size(); ++i) {
        if (i > 0) headers += ", ";
        headers += corsConfig.exposedHeaders[i];
    }
    return headers;
}

std::string CORSMiddleware::getMaxAge() {
    return std::to_string(corsConfig.maxAge);
}

std::string CORSMiddleware::getAllowCredentials() {
    return corsConfig.allowCredentials ? "true" : "false";
}

std::map<std::string, std::string> CORSMiddleware::handlePreflightRequest(const std::string& origin, 
                                                                        const std::string& requestMethod,
                                                                        const std::string& requestHeaders) {
    if (!validateOrigin(origin) || !validateMethod(requestMethod)) {
        return {};
    }
    
    // Validate requested headers if provided
    if (!requestHeaders.empty()) {
        std::vector<std::string> headers;
        std::stringstream ss(requestHeaders);
        std::string item;
        while (std::getline(ss, item, ',')) {
            // Trim whitespace
            size_t first = item.find_first_not_of(" ");
            if (std::string::npos != first) {
                size_t last = item.find_last_not_of(" ");
                item = item.substr(first, (last - first + 1));
            }
            if (!item.empty()) headers.push_back(item);
        }
        
        if (!validateHeaders(headers)) {
            return {}; // Reject if any requested header is not allowed
        }
    }
    
    return getCORSHeaders(origin);
}

bool CORSMiddleware::isOriginAllowed(const std::string& origin) {
    return validateOrigin(origin);
}

CORSConfig CORSMiddleware::getConfiguration() {
    return corsConfig;
}

void CORSMiddleware::configureForDevelopment() {
    CORSConfig config;
    config.allowedOrigins = {"*"};
    config.allowedMethods = {"GET", "POST", "PUT", "DELETE", "OPTIONS", "PATCH"};
    config.allowedHeaders = {"Content-Type", "Authorization", "X-Requested-With"};
    config.exposedHeaders = {"X-Total-Count", "Content-Range"};
    config.allowCredentials = true;
    config.maxAge = 3600;
    configureCORS(config);
}

void CORSMiddleware::configureForProduction(const std::vector<std::string>& allowedDomains) {
    CORSConfig config;
    config.allowedOrigins = allowedDomains;
    config.allowedMethods = {"GET", "POST", "PUT", "DELETE", "OPTIONS"};
    config.allowedHeaders = {"Content-Type", "Authorization"};
    config.allowCredentials = true;
    config.maxAge = 86400;
    configureCORS(config);
}

bool CORSMiddleware::wildCardMatches(const std::string& pattern, const std::string& value) {
    // Simple wildcard implementation (only supports * at the beginning or end for now)
    if (pattern == "*") return true;
    if (pattern.empty()) return value.empty();
    
    if (pattern[0] == '*') {
        std::string suffix = pattern.substr(1);
        if (value.length() < suffix.length()) return false;
        return value.compare(value.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
    
    if (pattern.back() == '*') {
        std::string prefix = pattern.substr(0, pattern.length() - 1);
        if (value.length() < prefix.length()) return false;
        return value.compare(0, prefix.length(), prefix) == 0;
    }
    
    return pattern == value;
}

}  // namespace middleware
}  // namespace conference
