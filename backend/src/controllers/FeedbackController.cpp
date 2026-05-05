#include "controllers/FeedbackController.h"
#include "controllers/ControllerHelpers.h"

namespace conference {
namespace controllers {

FeedbackController::FeedbackController(
    std::shared_ptr<services::IFeedbackService> feedbackSvc,
    std::shared_ptr<services::IAuthService> authSvc)
    : feedbackService(feedbackSvc), authService(authSvc) {}

std::string FeedbackController::extractUserId(const httplib::Request& req) {
    auto token = extractBearerToken(req);
    if (token.empty()) return "";
    if (!authService->validateToken(token)) return "";
    return authService->extractUserId(token);
}

std::string FeedbackController::extractUserRole(const httplib::Request& req) {
    auto token = extractBearerToken(req);
    if (token.empty()) return "";
    if (!authService->validateToken(token)) return "";
    return authService->extractRole(token);
}

void FeedbackController::handleSubmit(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    std::string sessionId = req.matches[1].str();
    auto body = parseBody(req);

    if (!body.contains("rating")) {
        sendError(res, 400, "rating is required", "VALIDATION_ERROR"); return;
    }

    auto result = feedbackService->submitFeedback(
        sessionId, userId,
        body["rating"].get<int>(),
        body.value("comment", ""),
        body.value("isAnonymous", true)
    );
    sendJson(res, result["success"].get<bool>() ? 201 : 400, result);
}

void FeedbackController::handleGetSummary(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    auto role = extractUserRole(req);
    if (role != "organizer" && role != "admin") {
        sendError(res, 403, "Organizer access required", "FORBIDDEN"); return;
    }

    std::string sessionId = req.matches[1].str();
    auto result = feedbackService->getSessionFeedbackSummary(sessionId, userId);
    sendJson(res, 200, result);
}

void FeedbackController::handleListAll(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    auto role = extractUserRole(req);
    if (role != "organizer" && role != "admin") {
        sendError(res, 403, "Organizer access required", "FORBIDDEN"); return;
    }
    auto result = feedbackService->listAllFeedback(userId);
    sendJson(res, 200, result);
}

} // namespace controllers
} // namespace conference
