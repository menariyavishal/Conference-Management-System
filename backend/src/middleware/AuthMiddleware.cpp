#include "middleware/AuthMiddleware.h"
#include "utils/EncryptionUtils.h"
#include <iostream>

namespace conference {
namespace middleware {

std::string AuthMiddleware::jwtSecret = "";
std::string AuthMiddleware::lastError = "";

bool AuthMiddleware::validateToken(const std::string& token, AuthContext& context) {
    if (token.empty()) {
        lastError = "Token is empty";
        return false;
    }

    // Verify JWT signature
    if (!verifyTokenSignature(token)) {
        lastError = "Invalid JWT signature";
        return false;
    }

    // Check expiration
    if (isTokenExpired(token)) {
        lastError = "Token has expired";
        return false;
    }

    // Extract payload
    auto payload = parseTokenPayload(token);
    if (payload.is_null()) {
        lastError = "Failed to parse token payload";
        return false;
    }

    // Set context
    context.token = token;
    context.userId = payload["userId"];
    context.email = payload["email"];
    context.role = payload["role"];
    context.tokenExpiry = payload["exp"];
    context.isAuthenticated = true;

    return true;
}

bool AuthMiddleware::validateBearerToken(const std::string& authHeader, AuthContext& context) {
    std::string token;
    if (!extractTokenFromHeader(authHeader, token)) {
        lastError = "Invalid Authorization header format";
        return false;
    }
    
    return validateToken(token, context);
}

json AuthMiddleware::parseTokenPayload(const std::string& token) {
    try {
        // JWT format: header.payload.signature
        size_t dotPos = token.find('.');
        if (dotPos == std::string::npos) {
            return json::object();
        }
        
        size_t secondDot = token.find('.', dotPos + 1);
        if (secondDot == std::string::npos) {
            return json::object();
        }
        
        std::string payloadEncoded = token.substr(dotPos + 1, secondDot - dotPos - 1);
        std::string payloadDecoded = utils::EncryptionUtils::decodeBase64(payloadEncoded);
        
        return json::parse(payloadDecoded);
    } catch (...) {
        return json::object();
    }
}

bool AuthMiddleware::extractUserIdFromToken(const std::string& token, int& userId) {
    auto payload = parseTokenPayload(token);
    if (payload.contains("userId")) {
        userId = payload["userId"];
        return true;
    }
    return false;
}

bool AuthMiddleware::extractRoleFromToken(const std::string& token, std::string& role) {
    auto payload = parseTokenPayload(token);
    if (payload.contains("role")) {
        role = payload["role"];
        return true;
    }
    return false;
}

bool AuthMiddleware::extractEmailFromToken(const std::string& token, std::string& email) {
    auto payload = parseTokenPayload(token);
    if (payload.contains("email")) {
        email = payload["email"];
        return true;
    }
    return false;
}

bool AuthMiddleware::verifyTokenSignature(const std::string& token) {
    if (jwtSecret.empty()) {
        lastError = "JWT secret not configured";
        return false;
    }

    // TODO: Implement proper JWT signature verification
    // For now, basic check
    return token.find('.') != std::string::npos;
}

bool AuthMiddleware::isTokenExpired(const std::string& token) {
    auto payload = parseTokenPayload(token);
    if (!payload.contains("exp")) {
        return true;
    }
    
    long expiry = payload["exp"];
    long now = std::time(nullptr);
    
    return now > expiry;
}

long AuthMiddleware::getTokenExpiry(const std::string& token) {
    auto payload = parseTokenPayload(token);
    if (payload.contains("exp")) {
        return payload["exp"];
    }
    return 0;
}

std::string AuthMiddleware::generateToken(int userId, const std::string& email, const std::string& role, int expiryHours) {
    // TODO: Implement proper JWT token generation
    return "token_stub";
}

std::string AuthMiddleware::generateRefreshToken(int userId) {
    // TODO: Implement refresh token generation
    return utils::EncryptionUtils::generateSecureToken(32);
}

bool AuthMiddleware::hasRole(const AuthContext& context, const std::string& requiredRole) {
    return context.isAuthenticated && context.role == requiredRole;
}

bool AuthMiddleware::hasAnyRole(const AuthContext& context, const std::vector<std::string>& requiredRoles) {
    if (!context.isAuthenticated) return false;
    for (const auto& role : requiredRoles) {
        if (context.role == role) return true;
    }
    return false;
}

bool AuthMiddleware::hasAllRoles(const AuthContext& context, const std::vector<std::string>& requiredRoles) {
    // Note: A user typically has only one role, so this checks if user has any of the roles
    return hasAnyRole(context, requiredRoles);
}

bool AuthMiddleware::extractTokenFromHeader(const std::string& authHeader, std::string& token) {
    if (authHeader.substr(0, 7) != "Bearer ") {
        return false;
    }
    
    token = authHeader.substr(7);
    return !token.empty();
}

void AuthMiddleware::setJWTSecret(const std::string& secret) {
    jwtSecret = secret;
}

bool AuthMiddleware::validateAuthorizationHeader(const std::string& authHeader) {
    return authHeader.substr(0, 7) == "Bearer " && authHeader.length() > 7;
}

std::string AuthMiddleware::getLastError() {
    return lastError;
}

}  // namespace middleware
}  // namespace conference
