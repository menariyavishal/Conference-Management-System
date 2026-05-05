#ifndef I_AUTH_SERVICE_H
#define I_AUTH_SERVICE_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace services {

/**
 * @class IAuthService
 * @brief Interface for authentication and JWT token management
 * 
 * Provides services for:
 * - JWT token generation with user claims
 * - Token validation and signature verification
 * - Token refresh
 * - User claim extraction
 * - Permission checking
 */
class IAuthService {
public:
    virtual ~IAuthService() = default;

    /**
     * @brief Generate JWT token for authenticated user
     * @param userId User ID to embed in token
     * @param email User email for identification
     * @param role User role for permissions
     * @param expiryHours Token expiration in hours (default 24)
     * @return Generated JWT token string
     * @throws ApplicationException if token generation fails
     */
    virtual std::string generateToken(
        const std::string& userId,
        const std::string& email,
        const std::string& role,
        int expiryHours = 24
    ) = 0;

    /**
     * @brief Validate JWT token signature and expiry
     * @param token JWT token to validate
     * @return true if token is valid and not expired, false otherwise
     */
    virtual bool validateToken(const std::string& token) = 0;

    /**
     * @brief Refresh expired token with new token
     * @param token Current token (even if expired)
     * @return New JWT token
     * @throws ApplicationException if refresh fails
     */
    virtual std::string refreshToken(const std::string& token) = 0;

    /**
     * @brief Extract user ID from valid token
     * @param token JWT token
     * @return User ID if token valid, empty string if invalid
     */
    virtual std::string extractUserId(const std::string& token) = 0;

    /**
     * @brief Extract email from valid token
     * @param token JWT token
     * @return User email if token valid, empty string if invalid
     */
    virtual std::string extractEmail(const std::string& token) = 0;

    /**
     * @brief Extract role from valid token
     * @param token JWT token
     * @return User role if token valid, empty string if invalid
     */
    virtual std::string extractRole(const std::string& token) = 0;

    /**
     * @brief Check if token has specific role/permission
     * @param token JWT token
     * @param requiredRole Role to check (admin, organizer, author, reviewer)
     * @return true if token contains required role
     */
    virtual bool hasRole(const std::string& token, const std::string& requiredRole) = 0;

    /**
     * @brief Get last error message
     * @return Error message from last operation
     */
    virtual std::string getLastError() const = 0;
};

}  // namespace services
} // namespace conference

std::shared_ptr<conference::services::IAuthService> createAuthService();

#endif // I_AUTH_SERVICE_H
