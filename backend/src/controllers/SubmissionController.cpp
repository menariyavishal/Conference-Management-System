#include "controllers/SubmissionController.h"
#include "controllers/ControllerHelpers.h"
#include <fstream>

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

    std::string title, abstract, authors, keywords, conferenceId;
    bool hasFile = false;
    std::string fileContent, fileName, contentType;

    if (req.is_multipart_form_data()) {
        if (req.form.has_field("title")) title = req.form.get_field("title");
        if (req.form.has_field("abstract")) abstract = req.form.get_field("abstract");
        if (req.form.has_field("authors")) authors = req.form.get_field("authors");
        if (req.form.has_field("keywords")) keywords = req.form.get_field("keywords");
        if (req.form.has_field("conferenceId")) conferenceId = req.form.get_field("conferenceId");

        if (req.form.has_file("file")) {
            auto file = req.form.get_file("file");
            fileContent = file.content;
            fileName = file.filename;
            contentType = file.content_type;
            hasFile = true;
        }
    } else {
        auto body = parseBody(req);
        title = body.value("title", "");
        abstract = body.value("abstract", "");
        authors = body.value("authors", "");
        keywords = body.value("keywords", "");
        conferenceId = body.value("conferenceId", "");
    }

    if (title.empty() || abstract.empty()) {
        sendError(res, 400, "title and abstract are required", "VALIDATION_ERROR"); return;
    }

    auto result = submissionService->createSubmission(userId, title, abstract, keywords, conferenceId);
    
    if (result["success"].get<bool>() && hasFile) {
        std::string submissionId = result["data"]["submissionId"].get<std::string>();
        
        // Save file locally first to a temp location for the service to pick up
        std::string tempPath = "uploads/temp_" + fileName;
        std::ofstream ofs(tempPath, std::ios::binary);
        ofs.write(fileContent.data(), fileContent.size());
        ofs.close();

        auto uploadResult = submissionService->uploadFile(submissionId, tempPath, userId);
        if (!uploadResult["success"].get<bool>()) {
            // Note: submission was created but file failed. 
            // In a real app we might want to rollback or notify user.
            result["message"] = result["message"].get<std::string>() + " (But file upload failed: " + uploadResult["error"].get<std::string>() + ")";
        } else {
            result["data"]["fileId"] = uploadResult["data"]["fileId"];
        }
    }

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
