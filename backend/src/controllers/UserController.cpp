#include "controllers/UserController.h"
#include "controllers/ControllerHelpers.h"

namespace conference {
namespace controllers {

UserController::UserController(std::shared_ptr<services::IUserService> userSvc,
                               std::shared_ptr<services::IAuthService> authSvc,
                               std::shared_ptr<services::INotificationService> notifSvc)
    : userService(userSvc), authService(authSvc), notificationService(notifSvc) {}

std::string UserController::extractUserId(const httplib::Request& req) {
    auto token = extractBearerToken(req);
    if (token.empty()) return "";
    if (!authService->validateToken(token)) return "";
    return authService->extractUserId(token);
}

std::string UserController::extractUserRole(const httplib::Request& req) {
    auto token = extractBearerToken(req);
    if (token.empty()) return "";
    if (!authService->validateToken(token)) return "";
    return authService->extractRole(token);
}

void UserController::handleListUsers(const httplib::Request& req, httplib::Response& res) {
    auto role = extractUserRole(req);
    if (role != "admin" && role != "organizer") {
        sendError(res, 403, "Admin or Organizer access required", "FORBIDDEN");
        return;
    }
    auto result = userService->listUsers();
    sendJson(res, 200, result);
}

void UserController::handleGetUser(const httplib::Request& req, httplib::Response& res) {
    auto requesterId = extractUserId(req);
    if (requesterId.empty()) {
        sendError(res, 401, "Authentication required", "UNAUTHORIZED");
        return;
    }
    std::string userId = req.matches[1].str();
    auto result = userService->getUserProfile(userId);
    if (result["success"].get<bool>()) {
        sendJson(res, 200, result);
    } else {
        sendError(res, 404, "User not found", "NOT_FOUND");
    }
}

void UserController::handleUpdateUser(const httplib::Request& req, httplib::Response& res) {
    auto requesterId = extractUserId(req);
    std::string userId = req.matches[1].str();

    if (requesterId.empty()) {
        sendError(res, 401, "Authentication required", "UNAUTHORIZED");
        return;
    }
    // Users can only update their own profile unless admin
    auto role = extractUserRole(req);
    if (requesterId != userId && role != "admin") {
        sendError(res, 403, "Cannot update another user's profile", "FORBIDDEN");
        return;
    }

    auto body = parseBody(req);
    auto result = userService->updateUserProfile(userId, body);
    sendJson(res, result["success"].get<bool>() ? 200 : 400, result);
}

void UserController::handleDeactivateUser(const httplib::Request& req, httplib::Response& res) {
    auto role = extractUserRole(req);
    if (role != "admin") {
        sendError(res, 403, "Admin access required", "FORBIDDEN");
        return;
    }
    std::string userId = req.matches[1].str();
    std::string adminId = extractUserId(req);
    auto result = userService->deactivateUser(userId, adminId);
    sendJson(res, result["success"].get<bool>() ? 200 : 400, result);
}

void UserController::handleUpdateRole(const httplib::Request& req, httplib::Response& res) {
    auto role = extractUserRole(req);
    if (role != "admin") {
        sendError(res, 403, "Admin access required", "FORBIDDEN");
        return;
    }
    std::string userId = req.matches[1].str();
    std::string adminId = extractUserId(req);
    auto body = parseBody(req);
    if (!body.contains("role")) {
        sendError(res, 400, "role field is required", "VALIDATION_ERROR");
        return;
    }
    auto result = userService->assignRole(userId, body["role"].get<std::string>(), adminId);
    sendJson(res, result["success"].get<bool>() ? 200 : 400, result);
}

void UserController::handleGetNotifications(const httplib::Request& req, httplib::Response& res) {
    auto requesterId = extractUserId(req);
    std::string userId = req.matches[1].str();

    if (requesterId.empty()) {
        sendError(res, 401, "Authentication required", "UNAUTHORIZED");
        return;
    }
    if (requesterId != userId) {
        sendError(res, 403, "Cannot view another user's notifications", "FORBIDDEN");
        return;
    }

    auto result = notificationService->getUserNotifications(userId);
    sendJson(res, 200, result);
}

} // namespace controllers
} // namespace conference
