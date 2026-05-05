#include "controllers/SessionController.h"
#include "controllers/ControllerHelpers.h"

namespace conference {
namespace controllers {

SessionController::SessionController(
    std::shared_ptr<services::ISessionService> sessionSvc,
    std::shared_ptr<services::IParticipantService> participantSvc,
    std::shared_ptr<services::IAuthService> authSvc)
    : sessionService(sessionSvc), participantService(participantSvc), authService(authSvc) {}

std::string SessionController::extractUserId(const httplib::Request& req) {
    auto token = extractBearerToken(req);
    if (token.empty()) return "";
    if (!authService->validateToken(token)) return "";
    return authService->extractUserId(token);
}

std::string SessionController::extractUserRole(const httplib::Request& req) {
    auto token = extractBearerToken(req);
    if (token.empty()) return "";
    if (!authService->validateToken(token)) return "";
    return authService->extractRole(token);
}

void SessionController::handleCreate(const httplib::Request& req, httplib::Response& res) {
    auto role = extractUserRole(req);
    if (role != "organizer" && role != "admin") {
        sendError(res, 403, "Organizer access required", "FORBIDDEN"); return;
    }
    auto userId = extractUserId(req);
    auto body = parseBody(req);

    if (!body.contains("title")) {
        sendError(res, 400, "title is required", "VALIDATION_ERROR"); return;
    }

    auto result = sessionService->createSession(body, userId);
    sendJson(res, result["success"].get<bool>() ? 201 : 400, result);
}

void SessionController::handleList(const httplib::Request& req, httplib::Response& res) {
    (void)req;
    auto result = sessionService->listSessions();
    sendJson(res, 200, result);
}

void SessionController::handleGet(const httplib::Request& req, httplib::Response& res) {
    std::string sessionId = req.matches[1].str();
    auto result = sessionService->getSession(sessionId);
    sendJson(res, result["success"].get<bool>() ? 200 : 404, result);
}

void SessionController::handleUpdate(const httplib::Request& req, httplib::Response& res) {
    auto role = extractUserRole(req);
    if (role != "organizer" && role != "admin") {
        sendError(res, 403, "Organizer access required", "FORBIDDEN"); return;
    }
    auto userId = extractUserId(req);
    std::string sessionId = req.matches[1].str();
    auto body = parseBody(req);
    auto result = sessionService->updateSession(sessionId, body, userId);
    sendJson(res, result["success"].get<bool>() ? 200 : 400, result);
}

void SessionController::handleDelete(const httplib::Request& req, httplib::Response& res) {
    auto role = extractUserRole(req);
    if (role != "organizer" && role != "admin") {
        sendError(res, 403, "Organizer access required", "FORBIDDEN"); return;
    }
    auto userId = extractUserId(req);
    std::string sessionId = req.matches[1].str();
    auto result = sessionService->deleteSession(sessionId, userId);
    sendJson(res, result["success"].get<bool>() ? 200 : 400, result);
}

void SessionController::handleAddPaper(const httplib::Request& req, httplib::Response& res) {
    auto role = extractUserRole(req);
    if (role != "organizer" && role != "admin") {
        sendError(res, 403, "Organizer access required", "FORBIDDEN"); return;
    }
    auto userId = extractUserId(req);
    std::string sessionId = req.matches[1].str();
    auto body = parseBody(req);

    if (!body.contains("submissionId")) {
        sendError(res, 400, "submissionId is required", "VALIDATION_ERROR"); return;
    }

    auto result = sessionService->assignPaperToSession(
        sessionId,
        body["submissionId"].get<std::string>(),
        body.value("presentationOrder", 0),
        userId
    );
    sendJson(res, result["success"].get<bool>() ? 201 : 400, result);
}

void SessionController::handleRemovePaper(const httplib::Request& req, httplib::Response& res) {
    auto role = extractUserRole(req);
    if (role != "organizer" && role != "admin") {
        sendError(res, 403, "Organizer access required", "FORBIDDEN"); return;
    }
    auto userId = extractUserId(req);
    std::string sessionId = req.matches[1].str();
    std::string submissionId = req.matches[2].str();
    auto result = sessionService->removePaperFromSession(sessionId, submissionId, userId);
    sendJson(res, result["success"].get<bool>() ? 200 : 400, result);
}

void SessionController::handleGetParticipants(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    std::string sessionId = req.matches[1].str();
    auto result = participantService->getSessionParticipants(sessionId);
    sendJson(res, 200, result);
}

} // namespace controllers
} // namespace conference
