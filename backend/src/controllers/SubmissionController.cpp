#include "controllers/SubmissionController.h"
#include "controllers/ControllerHelpers.h"

namespace conference {
namespace controllers {

SubmissionController::SubmissionController(
    std::shared_ptr<services::ISubmissionService> submissionSvc,
    std::shared_ptr<services::IReviewService> reviewSvc,
    std::shared_ptr<services::IAuthService> authSvc,
    std::shared_ptr<services::IFileService> fileSvc)
    : submissionService(submissionSvc), reviewService(reviewSvc),
      authService(authSvc), fileService(fileSvc) {}

std::string SubmissionController::extractUserId(const httplib::Request& req) {
    auto token = extractBearerToken(req);
    if (token.empty()) return "";
    if (!authService->validateToken(token)) return "";
    return authService->extractUserId(token);
}

std::string SubmissionController::extractUserRole(const httplib::Request& req) {
    auto token = extractBearerToken(req);
    if (token.empty()) return "";
    if (!authService->validateToken(token)) return "";
    return authService->extractRole(token);
}

void SubmissionController::handleCreate(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    auto role = extractUserRole(req);
    if (role != "author" && role != "admin") {
        sendError(res, 403, "Only authors can create submissions", "FORBIDDEN"); return;
    }

    auto body = parseBody(req);
    if (!body.contains("title") || !body.contains("abstract")) {
        sendError(res, 400, "title and abstract are required", "VALIDATION_ERROR"); return;
    }

    auto result = submissionService->createSubmission(
        userId,
        body["title"].get<std::string>(),
        body["abstract"].get<std::string>(),
        body.value("keywords", ""),
        body.value("conferenceId", "")
    );
    sendJson(res, result["success"].get<bool>() ? 201 : 400, result);
}

void SubmissionController::handleList(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    int page = 1, pageSize = 20;
    std::string status = "", authorId = "";

    if (req.has_param("page")) page = std::stoi(req.get_param_value("page"));
    if (req.has_param("pageSize")) pageSize = std::stoi(req.get_param_value("pageSize"));
    if (req.has_param("status")) status = req.get_param_value("status");
    if (req.has_param("authorId")) authorId = req.get_param_value("authorId");

    json result;
    if (!authorId.empty()) {
        result = submissionService->getSubmissionsByAuthor(authorId);
    } else {
        result = submissionService->listSubmissions(page, pageSize, status);
    }
    sendJson(res, 200, result);
}

void SubmissionController::handleGet(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    std::string submissionId = req.matches[1].str();
    try {
        auto result = submissionService->getSubmissionDetails(submissionId);
        sendJson(res, 200, result);
    } catch (const std::exception& ex) {
        sendError(res, 404, ex.what(), "NOT_FOUND");
    }
}

void SubmissionController::handleUpdate(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    std::string submissionId = req.matches[1].str();
    auto body = parseBody(req);
    auto result = submissionService->updateSubmission(submissionId, body, userId);
    sendJson(res, result["success"].get<bool>() ? 200 : 400, result);
}

void SubmissionController::handleWithdraw(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    std::string submissionId = req.matches[1].str();
    auto result = submissionService->withdrawSubmission(submissionId, userId);
    sendJson(res, result["success"].get<bool>() ? 200 : 400, result);
}

void SubmissionController::handleGetReviews(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    std::string submissionId = req.matches[1].str();
    try {
        auto result = reviewService->getSubmissionReviews(submissionId, true);
        sendJson(res, 200, result);
    } catch (const std::exception& ex) {
        sendError(res, 404, ex.what(), "NOT_FOUND");
    }
}

void SubmissionController::handleUploadFile(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    std::string submissionId = req.matches[1].str();
    auto body = parseBody(req);

    if (!body.contains("filePath")) {
        sendError(res, 400, "filePath is required", "VALIDATION_ERROR"); return;
    }

    auto result = submissionService->uploadFile(
        submissionId, body["filePath"].get<std::string>(), userId);
    sendJson(res, result["success"].get<bool>() ? 200 : 400, result);
}

void SubmissionController::handleDownloadFile(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    std::string submissionId = req.matches[1].str();
    std::string outputPath;
    // In a real system, fileId would be fetched from the submission record
    sendJson(res, 200, {{"success", true}, {"message", "File download endpoint — integrate with FileService"}});
}

} // namespace controllers
} // namespace conference
