#include "services/IParticipantService.h"
#include "repositories/IParticipantRepository.h"
#include "repositories/ISessionRepository.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using json = nlohmann::json;

namespace conference {
namespace services {

class ParticipantService : public IParticipantService {
private:
    std::string lastError;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<repositories::IParticipantRepository> participantRepository;
    std::shared_ptr<repositories::ISessionRepository> sessionRepository;

    std::string generateId() {
        auto now = std::chrono::system_clock::now().time_since_epoch().count();
        return "part_" + std::to_string(now);
    }

public:
    ParticipantService(
        std::shared_ptr<repositories::IParticipantRepository> participantRepo,
        std::shared_ptr<repositories::ISessionRepository> sessionRepo
    ) : participantRepository(participantRepo), sessionRepository(sessionRepo) {
        logger = spdlog::get("participant_service");
        if (!logger) {
            try {
                spdlog::sink_ptr sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                logger = std::make_shared<spdlog::logger>("participant_service", sink);
                spdlog::register_logger(logger);
            } catch (...) {
                logger = spdlog::get("conference");
            }
        }
    }

    virtual json registerForSession(const std::string& sessionId, const std::string& userId) override {
        try {
            // Check session exists
            auto sessionOpt = sessionRepository->getSessionById(sessionId);
            if (!sessionOpt.has_value()) {
                lastError = "Session not found";
                return {{"success", false}, {"error", lastError}};
            }

            // Check if already registered
            if (participantRepository->isRegistered(sessionId, userId)) {
                lastError = "Already registered for this session";
                return {{"success", false}, {"error", lastError}};
            }

            // Enforce capacity (FR5.4)
            auto session = sessionOpt.value();
            int capacity = session.value("capacity", 0);
            if (capacity > 0) {
                int current = sessionRepository->getCurrentParticipantCount(sessionId);
                if (current >= capacity) {
                    lastError = "Session is at full capacity";
                    return {{"success", false}, {"error", lastError}};
                }
            }

            std::string recordId = generateId();
            if (!participantRepository->registerParticipant(recordId, sessionId, userId)) {
                lastError = "Failed to register for session";
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("User {} registered for session {}", userId, sessionId);
            return {{"success", true}, {"message", "Registered for session successfully"}};

        } catch (const std::exception& ex) {
            lastError = std::string("Registration failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json deregisterFromSession(const std::string& sessionId, const std::string& userId) override {
        try {
            if (!participantRepository->isRegistered(sessionId, userId)) {
                lastError = "Not registered for this session";
                return {{"success", false}, {"error", lastError}};
            }

            if (!participantRepository->deregisterParticipant(sessionId, userId)) {
                lastError = "Failed to deregister from session";
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("User {} deregistered from session {}", userId, sessionId);
            return {{"success", true}, {"message", "Deregistered from session successfully"}};

        } catch (const std::exception& ex) {
            lastError = std::string("Deregistration failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getUserSessions(const std::string& userId) override {
        try {
            auto sessions = participantRepository->getSessionsForUser(userId);
            return {{"success", true}, {"data", sessions}};
        } catch (const std::exception& ex) {
            lastError = std::string("Get user sessions failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getSessionParticipants(const std::string& sessionId) override {
        try {
            auto participants = participantRepository->getParticipantsForSession(sessionId);
            return {{"success", true}, {"data", participants}};
        } catch (const std::exception& ex) {
            lastError = std::string("Get participants failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json markAttendance(const std::string& sessionId,
                                const std::string& userId,
                                bool attended,
                                const std::string& organizerId) override {
        try {
            if (!participantRepository->isRegistered(sessionId, userId)) {
                lastError = "User is not registered for this session";
                return {{"success", false}, {"error", lastError}};
            }

            if (!participantRepository->markAttendance(sessionId, userId, attended)) {
                lastError = "Failed to mark attendance";
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("Attendance marked for user {} in session {} by {}", userId, sessionId, organizerId);
            return {{"success", true}, {"message", "Attendance updated"}};

        } catch (const std::exception& ex) {
            lastError = std::string("Mark attendance failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual std::string getLastError() const override { return lastError; }
};

} // namespace services
} // namespace conference

std::shared_ptr<conference::services::IParticipantService>
createParticipantService(
    std::shared_ptr<conference::repositories::IParticipantRepository> participantRepo,
    std::shared_ptr<conference::repositories::ISessionRepository> sessionRepo
) {
    return std::make_shared<conference::services::ParticipantService>(participantRepo, sessionRepo);
}
