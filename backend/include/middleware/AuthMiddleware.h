#ifndef AUTH_MIDDLEWARE_H
#define AUTH_MIDDLEWARE_H

#include <string>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

namespace conference {
namespace middleware {

struct AuthContext {
    int userId;
    std::string email;
    std::string role;
    std::string token;
    long tokenExpiry;
    bool isAuthenticated;
};

class AuthMiddleware {
public:
    // Validate JWT token
    static bool validateToken(const std::string& token, AuthContext& context);
    static bool validateBearerToken(const std::string& authHeader, AuthContext& context);
    
    // Token parsing
    static json parseTokenPayload(const std::string& token);
    static bool extractUserIdFromToken(const std::string& token, int& userId);
    static bool extractRoleFromToken(const std::string& token, std::string& role);
    static bool extractEmailFromToken(const std::string& token, std::string& email);
    
    // Token verification
    static bool verifyTokenSignature(const std::string& token);
    static bool isTokenExpired(const std::string& token);
    static long getTokenExpiry(const std::string& token);
    
    // Token generation (typically in AuthService, but can be here)
    static std::string generateToken(int userId, const std::string& email, const std::string& role, int expiryHours);
    static std::string generateRefreshToken(int userId);
    
    // Permission checking
    static bool hasRole(const AuthContext& context, const std::string& requiredRole);
    static bool hasAnyRole(const AuthContext& context, const std::vector<std::string>& requiredRoles);
    static bool hasAllRoles(const AuthContext& context, const std::vector<std::string>& requiredRoles);
    
    // Extract token from header
    static bool extractTokenFromHeader(const std::string& authHeader, std::string& token);
    
    // Set JWT secret (should be called during initialization)
    static void setJWTSecret(const std::string& secret);
    
    // Validate request
    static bool validateAuthorizationHeader(const std::string& authHeader);
    
    // Get error message
    static std::string getLastError();

private:
    static std::string jwtSecret;
    static std::string lastError;
};

}  // namespace middleware
}  // namespace conference

#endif  // AUTH_MIDDLEWARE_H
