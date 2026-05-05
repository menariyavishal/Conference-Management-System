#include "controllers/ReviewController.h"
#include "controllers/ControllerHelpers.h"

namespace conference {
namespace controllers {

ReviewController::ReviewController(std::shared_ptr<services::IReviewService> reviewSvc,
                                   std::shared_ptr<services::IAuthService> authSvc)
    : reviewService(reviewSvc), authService(authSvc) {}

std::string ReviewController::extractUserId(const httplib::Request& req) {
    auto token = extractBearerToken(req);
    if (token.empty()) return "";
    if (!authService->validateToken(token)) return "";
    return authService->extractUserId(token);
}

std::string ReviewController::extractUserRole(const httplib::Request& req) {
    auto token = extractBearerToken(req);
    if (token.empty()) return "";
    if (!authService->validateToken(token)) return "";
    return authService->extractRole(token);
}

void ReviewController::handleAssignReview(const httplib::Request& req, httplib::Response& res) {
    auto role = extractUserRole(req);
    if (role != "organizer" && role != "admin") {
        sendError(res, 403, "Organizer access required", "FORBIDDEN"); return;
    }

    auto organizerId = extractUserId(req);
    auto body = parseBody(req);

    if (!body.contains("submissionId") || !body.contains("reviewerId")) {
        sendError(res, 400, "submissionId and reviewerId are required", "VALIDATION_ERROR"); return;
    }

    auto result = reviewService->assignReview(
        body["submissionId"].get<std::string>(),
        body["reviewerId"].get<std::string>(),
        body.value("deadline", ""),
        organizerId
    );
    sendJson(res, result["success"].get<bool>() ? 201 : 400, result);
}

void ReviewController::handleSubmitReview(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    auto role = extractUserRole(req);
    if (role != "reviewer" && role != "admin") {
        sendError(res, 403, "Reviewer access required", "FORBIDDEN"); return;
    }

    auto body = parseBody(req);
    if (!body.contains("reviewId") || !body.contains("score")) {
        sendError(res, 400, "reviewId and score are required", "VALIDATION_ERROR"); return;
    }

    try {
        auto result = reviewService->submitReview(
            body["reviewId"].get<std::string>(),
            body["score"].get<int>(),
            body.value("comments", ""),
            body.value("recommendation", ""),
            userId
        );
        sendJson(res, 200, result);
    } catch (const std::exception& ex) {
        sendError(res, 400, ex.what(), "SUBMISSION_FAILED");
    }
}

void ReviewController::handleListReviews(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    auto result = reviewService->getReviewAssignments(userId);
    sendJson(res, 200, result);
}

void ReviewController::handleGetReview(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    std::string reviewId = req.matches[1].str();
    try {
        auto result = reviewService->getReviewDetails(reviewId, userId);
        sendJson(res, 200, result);
    } catch (const std::exception& ex) {
        sendError(res, 404, ex.what(), "NOT_FOUND");
    }
}

void ReviewController::handleGetStatistics(const httplib::Request& req, httplib::Response& res) {
    auto role = extractUserRole(req);
    if (role != "organizer" && role != "admin") {
        sendError(res, 403, "Organizer access required", "FORBIDDEN"); return;
    }
    std::string submissionId = req.matches[1].str();
    auto result = reviewService->getReviewStatistics(submissionId);
    sendJson(res, 200, result);
}

} // namespace controllers
} // namespace conference
