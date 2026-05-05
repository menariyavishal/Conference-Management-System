#include "services/ISessionService.h"
#include "repositories/ISessionRepository.h"
#include "repositories/IParticipantRepository.h"
#include "database/ICacheProvider.h"
#include "utils/DateTimeUtils.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using json = nlohmann::json;

namespace conference {
namespace services {

class SessionService : public ISessionService {
private:
    std::string lastError;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<repositories::ISessionRepository> sessionRepository;
    std::shared_ptr<repositories::IParticipantRepository> participantRepository;
    std::shared_ptr<database::ICacheProvider> cache;
    
    const std::string SESSIONS_CACHE_KEY = "sessions_list";

    std::string generateId() {
        auto now = std::chrono::system_clock::now().time_since_epoch().count();
        return "sess_" + std::to_string(now);
    }

    void invalidateCache() {
        if (cache) cache->remove(SESSIONS_CACHE_KEY);
    }

public:
    SessionService(
        std::shared_ptr<repositories::ISessionRepository> sessionRepo,
        std::shared_ptr<repositories::IParticipantRepository> participantRepo,
        std::shared_ptr<database::ICacheProvider> cacheProvider
    ) : sessionRepository(sessionRepo), participantRepository(participantRepo), cache(cacheProvider) {
        logger = spdlog::get("session_service");
        if (!logger) {
            try {
                spdlog::sink_ptr sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                logger = std::make_shared<spdlog::logger>("session_service", sink);
                spdlog::register_logger(logger);
            } catch (...) {
                logger = spdlog::get("conference");
            }
        }
    }

    virtual json createSession(const json& sessionData, const std::string& organizerId) override {
        try {
            if (sessionData.value("title", "").empty()) {
                lastError = "Session title is required";
                return {{"success", false}, {"error", lastError}};
            }

            std::string sessionId = generateId();
            json session = sessionData;
            session["id"] = sessionId;

            if (!sessionRepository->insertSession(session)) {
                lastError = "Failed to create session in database";
                return {{"success", false}, {"error", lastError}};
            }

            invalidateCache();
            logger->info("Session created: {} by organizer: {}", sessionId, organizerId);
            return {{"success", true}, {"data", {{"sessionId", sessionId}}}, {"message", "Session created successfully"}};

        } catch (const std::exception& ex) {
            lastError = std::string("Create session failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getSession(const std::string& sessionId) override {
        try {
            // We could cache individual sessions too, but let's start with the list
            auto sessionOpt = sessionRepository->getSessionById(sessionId);
            if (!sessionOpt.has_value()) {
                lastError = "Session not found";
                return {{"success", false}, {"error", lastError}};
            }
            auto session = sessionOpt.value();
            session["papers"] = sessionRepository->getPapersForSession(sessionId);
            return {{"success", true}, {"data", session}};

        } catch (const std::exception& ex) {
            lastError = std::string("Get session failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json updateSession(const std::string& sessionId, const json& updates, const std::string& organizerId) override {
        try {
            auto sessionOpt = sessionRepository->getSessionById(sessionId);
            if (!sessionOpt.has_value()) {
                lastError = "Session not found";
                return {{"success", false}, {"error", lastError}};
            }

            if (!sessionRepository->updateSession(sessionId, updates)) {
                lastError = "Failed to update session";
                return {{"success", false}, {"error", lastError}};
            }

            invalidateCache();
            logger->info("Session updated: {} by: {}", sessionId, organizerId);
            return {{"success", true}, {"message", "Session updated successfully"}};

        } catch (const std::exception& ex) {
            lastError = std::string("Update session failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json deleteSession(const std::string& sessionId, const std::string& organizerId) override {
        try {
            if (!sessionRepository->deleteSession(sessionId)) {
                lastError = "Failed to delete session";
                return {{"success", false}, {"error", lastError}};
            }

            invalidateCache();
            logger->info("Session deleted (soft): {} by: {}", sessionId, organizerId);
            return {{"success", true}, {"message", "Session deleted successfully"}};

        } catch (const std::exception& ex) {
            lastError = std::string("Delete session failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json listSessions() override {
        try {
            if (cache) {
                auto cachedData = cache->get(SESSIONS_CACHE_KEY);
                if (cachedData.has_value()) {
                    logger->debug("Serving session list from cache");
                    return json::parse(cachedData.value());
                }
            }

            auto sessions = sessionRepository->getAllSessions();
            json response = {{"success", true}, {"data", sessions}, {"total", sessions.size()}, {"source", "database"}};
            
            if (cache) {
                response["source"] = "cache"; // Mark for next time
                cache->set(SESSIONS_CACHE_KEY, response.dump(), 300); // Cache for 5 minutes
                response["source"] = "database"; // Reset for current response
            }

            return response;
        } catch (const std::exception& ex) {
            lastError = std::string("List sessions failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json assignPaperToSession(const std::string& sessionId,
                                      const std::string& submissionId,
                                      int presentationOrder,
                                      const std::string& organizerId) override {
        try {
            auto sessionOpt = sessionRepository->getSessionById(sessionId);
            if (!sessionOpt.has_value()) {
                lastError = "Session not found";
                return {{"success", false}, {"error", lastError}};
            }

            auto now = std::chrono::system_clock::now().time_since_epoch().count();
            std::string paperId = "sp_" + std::to_string(now);

            if (!sessionRepository->assignPaperToSession(paperId, sessionId, submissionId, presentationOrder)) {
                lastError = "Failed to assign paper (may already be assigned)";
                return {{"success", false}, {"error", lastError}};
            }

            invalidateCache();
            logger->info("Paper {} assigned to session {} by {}", submissionId, sessionId, organizerId);
            return {{"success", true}, {"message", "Paper assigned to session successfully"}};

        } catch (const std::exception& ex) {
            lastError = std::string("Assign paper failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json removePaperFromSession(const std::string& sessionId,
                                        const std::string& submissionId,
                                        const std::string& organizerId) override {
        try {
            if (!sessionRepository->removePaperFromSession(sessionId, submissionId)) {
                lastError = "Failed to remove paper from session";
                return {{"success", false}, {"error", lastError}};
            }

            invalidateCache();
            logger->info("Paper {} removed from session {} by {}", submissionId, sessionId, organizerId);
            return {{"success", true}, {"message", "Paper removed from session"}};

        } catch (const std::exception& ex) {
            lastError = std::string("Remove paper failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getPapersForSession(const std::string& sessionId) override {
        try {
            auto papers = sessionRepository->getPapersForSession(sessionId);
            return {{"success", true}, {"data", papers}};
        } catch (const std::exception& ex) {
            lastError = std::string("Get papers failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getParticipantsForSession(const std::string& sessionId) override {
        try {
            auto participants = participantRepository->getParticipantsForSession(sessionId);
            return {{"success", true}, {"data", participants}};
        } catch (const std::exception& ex) {
            lastError = std::string("Get participants failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual std::string getLastError() const override { return lastError; }
};

} // namespace services
} // namespace conference

std::shared_ptr<conference::services::ISessionService>
createSessionService(
    std::shared_ptr<conference::repositories::ISessionRepository> sessionRepo,
    std::shared_ptr<conference::repositories::IParticipantRepository> participantRepo,
    std::shared_ptr<conference::database::ICacheProvider> cacheProvider
) {
    return std::make_shared<conference::services::SessionService>(sessionRepo, participantRepo, cacheProvider);
}
