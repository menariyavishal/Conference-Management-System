#pragma once
#include "httplib.h"
#include "services/IReportingService.h"
#include <memory>

namespace conference {
namespace controllers {

/**
 * @class ReportingController
 * @brief Handles API requests for conference analytics and reports
 */
class ReportingController {
private:
    std::shared_ptr<services::IReportingService> reportingService;

public:
    ReportingController(std::shared_ptr<services::IReportingService> service);

    /**
     * @brief Register routes with the HTTP server
     */
    void registerRoutes(httplib::Server& svr);

private:
    // Handlers
    void handleGetGlobalStats(const httplib::Request& req, httplib::Response& res);
    void handleGetSubmissionAnalytics(const httplib::Request& req, httplib::Response& res);
    void handleGetReviewerStats(const httplib::Request& req, httplib::Response& res);
    void handleGetSessionAnalytics(const httplib::Request& req, httplib::Response& res);
    void handleGetSessionReport(const httplib::Request& req, httplib::Response& res);
};

} // namespace controllers
} // namespace conference
