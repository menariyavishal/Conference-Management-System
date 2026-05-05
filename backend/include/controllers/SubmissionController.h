#pragma once
#include "services/ISubmissionService.h"
#include "services/IReviewService.h"
#include "services/IAuthService.h"
#include "services/IFileService.h"
#include "httplib.h"

namespace conference {
namespace controllers {

/**
 * @class SubmissionController
 * @brief HTTP handler for submission management endpoints
 *
 * Routes:
 *   POST   /api/v1/submissions
 *   GET    /api/v1/submissions
 *   GET    /api/v1/submissions/:id
 *   PUT    /api/v1/submissions/:id
 *   DELETE /api/v1/submissions/:id
 *   GET    /api/v1/submissions/:id/reviews
 *   POST   /api/v1/submissions/:id/file/upload
 *   GET    /api/v1/submissions/:id/file/download
 */
class SubmissionController {
private:
    std::shared_ptr<services::ISubmissionService> submissionService;
    std::shared_ptr<services::IReviewService> reviewService;
    std::shared_ptr<services::IAuthService> authService;
    std::shared_ptr<services::IFileService> fileService;

    std::string extractUserId(const httplib::Request& req);
    std::string extractUserRole(const httplib::Request& req);

public:
    SubmissionController(std::shared_ptr<services::ISubmissionService> submissionSvc,
                         std::shared_ptr<services::IReviewService> reviewSvc,
                         std::shared_ptr<services::IAuthService> authSvc,
                         std::shared_ptr<services::IFileService> fileSvc);

    void handleCreate(const httplib::Request& req, httplib::Response& res);
    void handleList(const httplib::Request& req, httplib::Response& res);
    void handleGet(const httplib::Request& req, httplib::Response& res);
    void handleUpdate(const httplib::Request& req, httplib::Response& res);
    void handleWithdraw(const httplib::Request& req, httplib::Response& res);
    void handleGetReviews(const httplib::Request& req, httplib::Response& res);
    void handleUploadFile(const httplib::Request& req, httplib::Response& res);
    void handleDownloadFile(const httplib::Request& req, httplib::Response& res);
};

} // namespace controllers
} // namespace conference
