#include "controllers/ParticipantController.h"
#include "controllers/ControllerHelpers.h"

namespace conference {
namespace controllers {

ParticipantController::ParticipantController(
    std::shared_ptr<services::IParticipantService> participantSvc,
    std::shared_ptr<services::IAuthService> authSvc)
    : participantService(participantSvc), authService(authSvc) {}

std::string ParticipantController::extractUserId(const httplib::Request& req) {
    auto token = extractBearerToken(req);
    if (token.empty()) return "";
    if (!authService->validateToken(token)) return "";
    return authService->extractUserId(token);
}

void ParticipantController::handleRegister(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    std::string sessionId = req.matches[1].str();
    auto result = participantService->registerForSession(sessionId, userId);
    sendJson(res, result["success"].get<bool>() ? 201 : 400, result);
}

void ParticipantController::handleDeregister(const httplib::Request& req, httplib::Response& res) {
    auto userId = extractUserId(req);
    if (userId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    std::string sessionId = req.matches[1].str();
    auto result = participantService->deregisterFromSession(sessionId, userId);
    sendJson(res, result["success"].get<bool>() ? 200 : 400, result);
}

void ParticipantController::handleGetUserSessions(const httplib::Request& req, httplib::Response& res) {
    auto requesterId = extractUserId(req);
    if (requesterId.empty()) { sendError(res, 401, "Authentication required", "UNAUTHORIZED"); return; }

    std::string userId = req.matches[1].str();
    auto result = participantService->getUserSessions(userId);
    sendJson(res, 200, result);
}

} // namespace controllers
} // namespace conference
