#pragma once
#include "services/IUserService.h"
#include "services/IAuthService.h"
#include "httplib.h"

namespace conference {
namespace controllers {

/**
 * @class AuthController
 * @brief HTTP handler for authentication endpoints
 *
 * Routes:
 *   POST /api/v1/auth/register
 *   POST /api/v1/auth/login
 *   POST /api/v1/auth/refresh-token
 *   POST /api/v1/auth/logout
 */
class AuthController {
private:
    std::shared_ptr<services::IUserService> userService;
    std::shared_ptr<services::IAuthService> authService;

public:
    AuthController(std::shared_ptr<services::IUserService> userSvc,
                   std::shared_ptr<services::IAuthService> authSvc);

    void handleRegister(const httplib::Request& req, httplib::Response& res);
    void handleLogin(const httplib::Request& req, httplib::Response& res);
    void handleRefreshToken(const httplib::Request& req, httplib::Response& res);
    void handleLogout(const httplib::Request& req, httplib::Response& res);
};

} // namespace controllers
} // namespace conference
