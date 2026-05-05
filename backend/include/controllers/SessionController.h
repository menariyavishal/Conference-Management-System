#pragma once
#include "services/ISessionService.h"
#include "services/IParticipantService.h"
#include "services/IAuthService.h"
#include "httplib.h"

namespace conference {
namespace controllers {

/**
 * @class SessionController
 * @brief HTTP handler for conference session endpoints
 *
 * Routes:
 *   POST   /api/v1/sessions                          (Organizer)
 *   GET    /api/v1/sessions
 *   GET    /api/v1/sessions/:id
 *   PUT    /api/v1/sessions/:id                      (Organizer)
 *   DELETE /api/v1/sessions/:id                      (Organizer)
 *   POST   /api/v1/sessions/:id/papers               (Organizer)
 *   DELETE /api/v1/sessions/:id/papers/:paperId      (Organizer)
 *   GET    /api/v1/sessions/:id/participants
 */
class SessionController {
private:
    std::shared_ptr<services::ISessionService> sessionService;
    std::shared_ptr<services::IParticipantService> participantService;
    std::shared_ptr<services::IAuthService> authService;

    std::string extractUserId(const httplib::Request& req);
    std::string extractUserRole(const httplib::Request& req);

public:
    SessionController(std::shared_ptr<services::ISessionService> sessionSvc,
                      std::shared_ptr<services::IParticipantService> participantSvc,
                      std::shared_ptr<services::IAuthService> authSvc);

    void handleCreate(const httplib::Request& req, httplib::Response& res);
    void handleList(const httplib::Request& req, httplib::Response& res);
    void handleGet(const httplib::Request& req, httplib::Response& res);
    void handleUpdate(const httplib::Request& req, httplib::Response& res);
    void handleDelete(const httplib::Request& req, httplib::Response& res);
    void handleAddPaper(const httplib::Request& req, httplib::Response& res);
    void handleRemovePaper(const httplib::Request& req, httplib::Response& res);
    void handleGetParticipants(const httplib::Request& req, httplib::Response& res);
};

} // namespace controllers
} // namespace conference
