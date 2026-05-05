#pragma once
#include "services/IFeedbackService.h"
#include "services/IAuthService.h"
#include "httplib.h"

namespace conference {
namespace controllers {

/**
 * @class FeedbackController
 * @brief HTTP handler for session feedback endpoints
 *
 * Routes:
 *   POST /api/v1/sessions/:id/feedback           (Participant)
 *   GET  /api/v1/sessions/:id/feedback/summary   (Organizer)
 *   GET  /api/v1/feedback                         (Organizer)
 */
class FeedbackController {
private:
    std::shared_ptr<services::IFeedbackService> feedbackService;
    std::shared_ptr<services::IAuthService> authService;

    std::string extractUserId(const httplib::Request& req);
    std::string extractUserRole(const httplib::Request& req);

public:
    FeedbackController(std::shared_ptr<services::IFeedbackService> feedbackSvc,
                       std::shared_ptr<services::IAuthService> authSvc);

    void handleSubmit(const httplib::Request& req, httplib::Response& res);
    void handleGetSummary(const httplib::Request& req, httplib::Response& res);
    void handleListAll(const httplib::Request& req, httplib::Response& res);
};

} // namespace controllers
} // namespace conference
