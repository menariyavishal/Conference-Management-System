#pragma once
#include "services/IUserService.h"
#include "services/IAuthService.h"
#include "services/INotificationService.h"
#include "httplib.h"

namespace conference {
namespace controllers {

/**
 * @class UserController
 * @brief HTTP handler for user management endpoints
 *
 * Routes:
 *   GET    /api/v1/users                   (Admin)
 *   GET    /api/v1/users/:id
 *   PUT    /api/v1/users/:id
 *   DELETE /api/v1/users/:id               (Admin)
 *   PUT    /api/v1/users/:id/role          (Admin)
 *   GET    /api/v1/users/:id/notifications
 */
class UserController {
private:
    std::shared_ptr<services::IUserService> userService;
    std::shared_ptr<services::IAuthService> authService;
    std::shared_ptr<services::INotificationService> notificationService;

    std::string extractUserId(const httplib::Request& req);
    std::string extractUserRole(const httplib::Request& req);

public:
    UserController(std::shared_ptr<services::IUserService> userSvc,
                   std::shared_ptr<services::IAuthService> authSvc,
                   std::shared_ptr<services::INotificationService> notifSvc);

    void handleListUsers(const httplib::Request& req, httplib::Response& res);
    void handleGetUser(const httplib::Request& req, httplib::Response& res);
    void handleUpdateUser(const httplib::Request& req, httplib::Response& res);
    void handleDeactivateUser(const httplib::Request& req, httplib::Response& res);
    void handleUpdateRole(const httplib::Request& req, httplib::Response& res);
    void handleGetNotifications(const httplib::Request& req, httplib::Response& res);
};

} // namespace controllers
} // namespace conference
