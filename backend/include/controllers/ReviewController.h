#pragma once
#include "services/IReviewService.h"
#include "services/IAuthService.h"
#include "httplib.h"

namespace conference {
namespace controllers {

/**
 * @class ReviewController
 * @brief HTTP handler for review management endpoints
 *
 * Routes:
 *   POST  /api/v1/reviews/assign    (Organizer)
 *   POST  /api/v1/reviews           (Reviewer)
 *   GET   /api/v1/reviews
 *   GET   /api/v1/reviews/:id
 *   GET   /api/v1/reviews/statistics (Organizer)
 */
class ReviewController {
private:
    std::shared_ptr<services::IReviewService> reviewService;
    std::shared_ptr<services::IAuthService> authService;

    std::string extractUserId(const httplib::Request& req);
    std::string extractUserRole(const httplib::Request& req);

public:
    ReviewController(std::shared_ptr<services::IReviewService> reviewSvc,
                     std::shared_ptr<services::IAuthService> authSvc);

    void handleAssignReview(const httplib::Request& req, httplib::Response& res);
    void handleSubmitReview(const httplib::Request& req, httplib::Response& res);
    void handleListReviews(const httplib::Request& req, httplib::Response& res);
    void handleGetReview(const httplib::Request& req, httplib::Response& res);
    void handleGetStatistics(const httplib::Request& req, httplib::Response& res);
};

} // namespace controllers
} // namespace conference
