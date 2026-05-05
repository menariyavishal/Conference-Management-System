#ifndef I_USER_SERVICE_H
#define I_USER_SERVICE_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace services {

/**
 * @class IUserService
 * @brief Interface for user management operations
 * 
 * Provides services for:
 * - User registration with validation
 * - User authentication (login)
 * - Profile management
 * - Password changes
 * - Role assignment
 * - User listing with filters
 * - User deactivation (soft delete)
 */
class IUserService {
public:
    virtual ~IUserService() = default;

    /**
     * @brief Register new user
     * @param email User email (unique)
     * @param password Plain password (will be hashed)
     * @param fullName User full name
     * @param role User role (admin, organizer, author, reviewer)
     * @return JSON response with user data and token
     * @throws ApplicationException if validation fails or email exists
     */
    virtual json registerUser(
        const std::string& email,
        const std::string& password,
        const std::string& fullName,
        const std::string& role = "author"
    ) = 0;

    /**
     * @brief Authenticate user and generate token
     * @param email User email
     * @param password Plain password to verify
     * @return JSON response with user data and JWT token
     * @throws ApplicationException if credentials invalid
     */
    virtual json loginUser(
        const std::string& email,
        const std::string& password
    ) = 0;

    /**
     * @brief Get user profile information
     * @param userId User ID
     * @return JSON with user details (exclude password)
     * @throws UserNotFoundException if user not found
     */
    virtual json getUserProfile(const std::string& userId) = 0;

    /**
     * @brief Update user profile (name, email, bio, etc)
     * @param userId User ID
     * @param updateData JSON with fields to update
     * @return Updated user JSON
     * @throws UserNotFoundException or ValidationException
     */
    virtual json updateUserProfile(
        const std::string& userId,
        const json& updateData
    ) = 0;

    /**
     * @brief Change user password
     * @param userId User ID
     * @param oldPassword Current password for verification
     * @param newPassword New password
     * @return Success JSON response
     * @throws ApplicationException if old password invalid
     */
    virtual json changePassword(
        const std::string& userId,
        const std::string& oldPassword,
        const std::string& newPassword
    ) = 0;

    /**
     * @brief List all users with pagination
     * @param page Page number (1-indexed)
     * @param pageSize Results per page
     * @param roleFilter Filter by role (empty for all)
     * @param searchQuery Search by name/email
     * @return JSON array of users with pagination info
     */
    virtual json listUsers(
        int page = 1,
        int pageSize = 20,
        const std::string& roleFilter = "",
        const std::string& searchQuery = ""
    ) = 0;

    /**
     * @brief Assign or update user role
     * @param userId User ID
     * @param newRole New role (admin, organizer, author, reviewer)
     * @param adminId ID of admin performing the action
     * @return Updated user JSON
     * @throws UnauthorizedException if requester not admin
     */
    virtual json assignRole(
        const std::string& userId,
        const std::string& newRole,
        const std::string& adminId
    ) = 0;

    /**
     * @brief Soft delete user (mark inactive)
     * @param userId User ID
     * @param adminId ID of admin performing deletion
     * @return Success JSON response
     * @throws UnauthorizedException if requester not admin
     */
    virtual json deactivateUser(
        const std::string& userId,
        const std::string& adminId
    ) = 0;

    /**
     * @brief Check if user exists
     * @param email User email
     * @return true if user with email exists
     */
    virtual bool userExists(const std::string& email) = 0;

    /**
     * @brief Get user by email
     * @param email User email
     * @return User JSON if found
     * @throws UserNotFoundException if not found
     */
    virtual json getUserByEmail(const std::string& email) = 0;

    /**
     * @brief Get last error message
     * @return Error message from last operation
     */
    virtual std::string getLastError() const = 0;
};

} // namespace services

// Forward declarations for repositories and services
namespace repositories {
    class IUserRepository;
}
namespace services {
    class IAuthService;
}

} // namespace conference

std::shared_ptr<conference::services::IUserService>
createUserService(
    std::shared_ptr<conference::services::IAuthService> authSvc,
    std::shared_ptr<conference::repositories::IUserRepository> userRepo
);

#endif  // I_USER_SERVICE_H
