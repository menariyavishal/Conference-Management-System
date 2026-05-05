#pragma once
#include "services/IParticipantService.h"
#include "services/IAuthService.h"
#include "httplib.h"

namespace conference {
namespace controllers {

/**
 * @class ParticipantController
 * @brief HTTP handler for session registration endpoints
 *
 * Routes:
 *   POST   /api/v1/sessions/:id/register
 *   DELETE /api/v1/sessions/:id/register
 *   GET    /api/v1/users/:id/sessions
 */
class ParticipantController {
private:
    std::shared_ptr<services::IParticipantService> participantService;
    std::shared_ptr<services::IAuthService> authService;

    std::string extractUserId(const httplib::Request& req);

public:
    ParticipantController(std::shared_ptr<services::IParticipantService> participantSvc,
                          std::shared_ptr<services::IAuthService> authSvc);

    void handleRegister(const httplib::Request& req, httplib::Response& res);
    void handleDeregister(const httplib::Request& req, httplib::Response& res);
    void handleGetUserSessions(const httplib::Request& req, httplib::Response& res);
};

} // namespace controllers
} // namespace conference
