#include "controllers/AuthController.h"
#include "controllers/ControllerHelpers.h"

namespace conference {
namespace controllers {

AuthController::AuthController(std::shared_ptr<services::IUserService> userSvc,
                               std::shared_ptr<services::IAuthService> authSvc)
    : userService(userSvc), authService(authSvc) {}

void AuthController::handleRegister(const httplib::Request& req, httplib::Response& res) {
    auto body = parseBody(req);

    if (!body.contains("email") || !body.contains("password") || !body.contains("fullName")) {
        sendError(res, 400, "email, password, and fullName are required", "VALIDATION_ERROR");
        return;
    }

    auto result = userService->registerUser(
        body["email"].get<std::string>(),
        body["password"].get<std::string>(),
        body["fullName"].get<std::string>(),
        body.value("role", "author")
    );

    if (result["success"].get<bool>()) {
        sendJson(res, 201, result);
    } else {
        sendError(res, 409, result.value("error", "Registration failed"), "REGISTRATION_FAILED");
    }
}

void AuthController::handleLogin(const httplib::Request& req, httplib::Response& res) {
    auto body = parseBody(req);

    if (!body.contains("email") || !body.contains("password")) {
        sendError(res, 400, "email and password are required", "VALIDATION_ERROR");
        return;
    }

    auto result = userService->loginUser(
        body["email"].get<std::string>(),
        body["password"].get<std::string>()
    );

    if (result["success"].get<bool>()) {
        sendJson(res, 200, result);
    } else {
        sendError(res, 401, result.value("error", "Login failed"), "AUTH_FAILED");
    }
}

void AuthController::handleRefreshToken(const httplib::Request& req, httplib::Response& res) {
    auto body = parseBody(req);

    if (!body.contains("refreshToken")) {
        sendError(res, 400, "refreshToken is required", "VALIDATION_ERROR");
        return;
    }

    try {
        std::string token = authService->refreshToken(body["refreshToken"].get<std::string>());
        if (!token.empty()) {
            sendJson(res, 200, {{"success", true}, {"data", {{"token", token}}}});
        } else {
            sendError(res, 401, "Invalid or expired refresh token", "TOKEN_INVALID");
        }
    } catch (const std::exception& ex) {
        sendError(res, 401, ex.what(), "TOKEN_INVALID");
    }
}

void AuthController::handleLogout(const httplib::Request& req, httplib::Response& res) {
    // JWT is stateless — client discards the token.
    // Future: add token blacklist via Redis.
    (void)req;
    sendJson(res, 200, {{"success", true}, {"message", "Logged out successfully"}});
}

} // namespace controllers
} // namespace conference
