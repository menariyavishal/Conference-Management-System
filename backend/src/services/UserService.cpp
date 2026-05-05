#include "services/IUserService.h"
#include "services/IAuthService.h"
#include "repositories/IUserRepository.h"
#include "utils/Constants.h"
#include "utils/ValidationUtils.h"
#include "utils/EncryptionUtils.h"
#include "utils/DateTimeUtils.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <algorithm>

using json = nlohmann::json;

namespace conference {
namespace services {

/**
 * @class UserService
 * @brief Implementation of user management operations
 */
class UserService : public IUserService {
private:
    std::string lastError;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<IAuthService> authService;
    std::shared_ptr<repositories::IUserRepository> userRepository;

    bool validateRegistrationData(
        const std::string& email,
        const std::string& password,
        const std::string& fullName
    ) {
        if (!utils::ValidationUtils::validateEmail(email)) {
            lastError = "Invalid email format";
            logger->warn("Registration validation failed: {}", lastError);
            return false;
        }

        if (password.length() < 8) {
            lastError = "Password must be at least 8 characters";
            logger->warn("Registration validation failed: {}", lastError);
            return false;
        }

        if (fullName.empty() || fullName.length() < 2 || fullName.length() > 100) {
            lastError = "Full name must be between 2 and 100 characters";
            logger->warn("Registration validation failed: {}", lastError);
            return false;
        }

        return true;
    }

    bool isValidRole(const std::string& role) {
        return role == "admin" || role == "organizer" || 
               role == "author" || role == "reviewer";
    }

public:
    UserService(std::shared_ptr<IAuthService> authSvc, std::shared_ptr<repositories::IUserRepository> userRepo) 
        : authService(authSvc), userRepository(userRepo) {
        logger = spdlog::get("conference");
        if (!logger) {
            logger = spdlog::stdout_color_mt("user");
        }
    }

    virtual json registerUser(
        const std::string& email,
        const std::string& password,
        const std::string& fullName,
        const std::string& role = "author"
    ) override {
        try {
            if (!isValidRole(role)) {
                lastError = "Invalid role: " + role;
                logger->warn(lastError);
                return {{"success", false}, {"error", lastError}};
            }

            if (!validateRegistrationData(email, password, fullName)) {
                return {{"success", false}, {"error", lastError}};
            }

            if (userRepository->getUserByEmail(email).has_value()) {
                lastError = "User with this email already exists";
                return {{"success", false}, {"error", lastError}};
            }

            std::string userId = "user_" + utils::EncryptionUtils::hashSHA256(email).substr(0, 12);
            std::string hashedPassword = utils::EncryptionUtils::hashPassword(password);
            std::string now = utils::DateTimeUtils::getCurrentISO8601();

            json newUser = {
                {"userId", userId},
                {"email", email},
                {"fullName", fullName},
                {"passwordHash", hashedPassword},
                {"role", role},
                {"isActive", true},
                {"createdAt", now},
                {"updatedAt", now}
            };
            
            if (!userRepository->insertUser(newUser)) {
                lastError = "Failed to insert user into database";
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("User registered successfully: {} role: {}", email, role);

            std::string token = authService->generateToken(userId, email, role);

            return {
                {"success", true},
                {"message", "User registered successfully"},
                {"data", {
                    {"userId", userId},
                    {"email", email},
                    {"fullName", fullName},
                    {"role", role},
                    {"createdAt", now},
                    {"token", token}
                }}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("Registration failed: ") + ex.what();
            logger->error(lastError);
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json loginUser(
        const std::string& email,
        const std::string& password
    ) override {
        try {
            if (!utils::ValidationUtils::validateEmail(email)) {
                lastError = "Invalid email format";
                return {{"success", false}, {"error", lastError}};
            }

            auto userOpt = userRepository->getUserByEmail(email);
            if (!userOpt.has_value()) {
                lastError = "User not found";
                logger->warn("Login failed for email: {}", email);
                return {{"success", false}, {"error", lastError}};
            }
            
            json user = userOpt.value();

            if (!utils::EncryptionUtils::verifyPassword(password, user["passwordHash"])) {
                lastError = "Invalid password";
                logger->warn("Failed login attempt for: {}", email);
                return {{"success", false}, {"error", lastError}};
            }

            if (!user["isActive"].get<bool>()) {
                lastError = "Account is deactivated";
                return {{"success", false}, {"error", lastError}};
            }

            std::string token = authService->generateToken(user["userId"], email, user["role"]);

            logger->info("User logged in successfully: {}", email);

            return {
                {"success", true},
                {"message", "Login successful"},
                {"data", {
                    {"userId", user["userId"]},
                    {"email", email},
                    {"fullName", user["fullName"]},
                    {"role", user["role"]},
                    {"token", token}
                }}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("Login failed: ") + ex.what();
            logger->error(lastError);
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getUserProfile(const std::string& userId) override {
        try {
            auto userOpt = userRepository->getUserById(userId);
            if (!userOpt.has_value()) {
                lastError = "User not found";
                return {{"success", false}, {"error", lastError}};
            }

            json user = userOpt.value();
            user.erase("passwordHash"); // don't return password hash

            return {
                {"success", true},
                {"data", user}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("Profile retrieval failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json updateUserProfile(
        const std::string& userId,
        const json& updateData
    ) override {
        try {
            auto userOpt = userRepository->getUserById(userId);
            if (!userOpt.has_value()) {
                lastError = "User not found";
                return {{"success", false}, {"error", lastError}};
            }
            json user = userOpt.value();

            std::string newEmail = updateData.contains("email") ? updateData["email"].get<std::string>() : user["email"].get<std::string>();
            std::string newName = updateData.contains("fullName") ? updateData["fullName"].get<std::string>() : user["fullName"].get<std::string>();

            if (!utils::ValidationUtils::validateEmail(newEmail)) {
                lastError = "Invalid email format";
                return {{"success", false}, {"error", lastError}};
            }

            if (newName.empty() || newName.length() > 100) {
                lastError = "Invalid full name";
                return {{"success", false}, {"error", lastError}};
            }

            if (!userRepository->updateUserProfile(userId, newName, newEmail)) {
                lastError = "Failed to update database";
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("Updated profile for user: {}", userId);

            return {
                {"success", true},
                {"message", "Profile updated successfully"},
                {"data", {{"userId", userId}, {"fullName", newName}, {"email", newEmail}}}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("Profile update failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json changePassword(
        const std::string& userId,
        const std::string& oldPassword,
        const std::string& newPassword
    ) override {
        try {
            auto userOpt = userRepository->getUserById(userId);
            if (!userOpt.has_value()) {
                lastError = "User not found";
                return {{"success", false}, {"error", lastError}};
            }
            json user = userOpt.value();

            if (newPassword.length() < 8) {
                lastError = "New password must be at least 8 characters";
                return {{"success", false}, {"error", lastError}};
            }

            if (!utils::EncryptionUtils::verifyPassword(oldPassword, user["passwordHash"])) {
                lastError = "Current password is incorrect";
                return {{"success", false}, {"error", lastError}};
            }

            std::string hashedPassword = utils::EncryptionUtils::hashPassword(newPassword);
            if (!userRepository->updatePassword(userId, hashedPassword)) {
                lastError = "Failed to update password in database";
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("Password changed for user: {}", userId);

            return {
                {"success", true},
                {"message", "Password changed successfully"}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("Password change failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json listUsers(
        int page = 1,
        int pageSize = 20,
        const std::string& roleFilter = "",
        const std::string& searchQuery = ""
    ) override {
        try {
            int offset = (page - 1) * pageSize;
            auto users = userRepository->getUsers(pageSize, offset, roleFilter, searchQuery);
            int totalCount = userRepository->getUserCount(roleFilter, searchQuery);
            int totalPages = (totalCount + pageSize - 1) / pageSize;

            return {
                {"success", true},
                {"data", users},
                {"pagination", {
                    {"page", page},
                    {"pageSize", pageSize},
                    {"totalCount", totalCount},
                    {"totalPages", totalPages}
                }}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("User listing failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json assignRole(
        const std::string& userId,
        const std::string& newRole,
        const std::string& adminId
    ) override {
        try {
            auto adminOpt = userRepository->getUserById(adminId);
            if (!adminOpt.has_value() || adminOpt.value()["role"] != "admin") {
                lastError = "Only admins can assign roles";
                return {{"success", false}, {"error", lastError}};
            }

            if (!isValidRole(newRole)) {
                lastError = "Invalid role: " + newRole;
                return {{"success", false}, {"error", lastError}};
            }

            if (!userRepository->updateRole(userId, newRole)) {
                lastError = "Failed to update role in database";
                return {{"success", false}, {"error", lastError}};
            }

            return {
                {"success", true},
                {"message", "Role assigned successfully"},
                {"data", {{"userId", userId}, {"role", newRole}}}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("Role assignment failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json deactivateUser(
        const std::string& userId,
        const std::string& adminId
    ) override {
        try {
            auto adminOpt = userRepository->getUserById(adminId);
            if (!adminOpt.has_value() || adminOpt.value()["role"] != "admin") {
                lastError = "Only admins can deactivate users";
                return {{"success", false}, {"error", lastError}};
            }

            if (!userRepository->deactivateUser(userId)) {
                lastError = "Failed to deactivate user in database";
                return {{"success", false}, {"error", lastError}};
            }

            return {
                {"success", true},
                {"message", "User deactivated successfully"}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("User deactivation failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual bool userExists(const std::string& email) override {
        try {
            return userRepository->getUserByEmail(email).has_value();
        } catch (...) {
            return false;
        }
    }

    virtual json getUserByEmail(const std::string& email) override {
        try {
            auto userOpt = userRepository->getUserByEmail(email);
            if (userOpt.has_value()) {
                json user = userOpt.value();
                user.erase("passwordHash");
                return {
                    {"success", true},
                    {"data", user}
                };
            }
            return {
                {"success", false},
                {"error", "User not found"}
            };
        } catch (const std::exception& ex) {
            return {{"success", false}, {"error", ex.what()}};
        }
    }

    virtual std::string getLastError() const override {
        return lastError;
    }
};

}  // namespace services
}  // namespace conference

// Factory function
std::shared_ptr<conference::services::IUserService> createUserService(
    std::shared_ptr<conference::services::IAuthService> authService,
    std::shared_ptr<conference::repositories::IUserRepository> userRepository) {
    return std::make_shared<conference::services::UserService>(authService, userRepository);
}
