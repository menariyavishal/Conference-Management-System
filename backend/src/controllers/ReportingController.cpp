#include "controllers/ReportingController.h"
#include "controllers/ControllerHelpers.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace controllers {

ReportingController::ReportingController(std::shared_ptr<services::IReportingService> service)
    : reportingService(service) {}

void ReportingController::registerRoutes(httplib::Server& svr) {
    svr.Get("/api/reports/global", [this](const httplib::Request& req, httplib::Response& res) {
        handleGetGlobalStats(req, res);
    });

    svr.Get("/api/reports/submissions", [this](const httplib::Request& req, httplib::Response& res) {
        handleGetSubmissionAnalytics(req, res);
    });

    svr.Get("/api/reports/reviewers", [this](const httplib::Request& req, httplib::Response& res) {
        handleGetReviewerStats(req, res);
    });

    svr.Get("/api/reports/sessions", [this](const httplib::Request& req, httplib::Response& res) {
        handleGetSessionAnalytics(req, res);
    });

    svr.Get("/api/reports/sessions/:id", [this](const httplib::Request& req, httplib::Response& res) {
        handleGetSessionReport(req, res);
    });
}

void ReportingController::handleGetGlobalStats(const httplib::Request& req, httplib::Response& res) {
    // Note: In a real app, we'd check for ADMIN role here
    auto result = reportingService->getGlobalStats();
    if (result["success"].get<bool>()) {
        res.set_content(result["data"].dump(), "application/json");
    } else {
        res.status = 500;
        res.set_content(json({{"error", result["error"]}}).dump(), "application/json");
    }
}

void ReportingController::handleGetSubmissionAnalytics(const httplib::Request& req, httplib::Response& res) {
    auto result = reportingService->getSubmissionAnalytics();
    if (result["success"].get<bool>()) {
        res.set_content(result["data"].dump(), "application/json");
    } else {
        res.status = 500;
        res.set_content(json({{"error", result["error"]}}).dump(), "application/json");
    }
}

void ReportingController::handleGetReviewerStats(const httplib::Request& req, httplib::Response& res) {
    auto result = reportingService->getReviewerStats();
    if (result["success"].get<bool>()) {
        res.set_content(result["data"].dump(), "application/json");
    } else {
        res.status = 500;
        res.set_content(json({{"error", result["error"]}}).dump(), "application/json");
    }
}

void ReportingController::handleGetSessionAnalytics(const httplib::Request& req, httplib::Response& res) {
    auto result = reportingService->getSessionAnalytics();
    if (result["success"].get<bool>()) {
        res.set_content(result["data"].dump(), "application/json");
    } else {
        res.status = 500;
        res.set_content(json({{"error", result["error"]}}).dump(), "application/json");
    }
}

void ReportingController::handleGetSessionReport(const httplib::Request& req, httplib::Response& res) {
    std::string sessionId = req.path_params.at("id");
    auto result = reportingService->getSessionReport(sessionId);
    
    if (result["success"].get<bool>()) {
        res.set_content(result["data"].dump(), "application/json");
    } else {
        if (result["error"] == "Session not found") {
            res.status = 404;
        } else {
            res.status = 500;
        }
        res.set_content(json({{"error", result["error"]}}).dump(), "application/json");
    }
}

} // namespace controllers
} // namespace conference
