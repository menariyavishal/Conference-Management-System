#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace services {

/**
 * @interface ISessionService
 * @brief Contract for conference session lifecycle management (FR4)
 */
class ISessionService {
public:
    virtual ~ISessionService() = default;

    virtual json createSession(const json& sessionData, const std::string& organizerId) = 0;
    virtual json getSession(const std::string& sessionId) = 0;
    virtual json updateSession(const std::string& sessionId, const json& updates, const std::string& organizerId) = 0;
    virtual json deleteSession(const std::string& sessionId, const std::string& organizerId) = 0;
    virtual json listSessions() = 0;

    // Paper management
    virtual json assignPaperToSession(const std::string& sessionId,
                                      const std::string& submissionId,
                                      int presentationOrder,
                                      const std::string& organizerId) = 0;
    virtual json removePaperFromSession(const std::string& sessionId,
                                        const std::string& submissionId,
                                        const std::string& organizerId) = 0;
    virtual json getPapersForSession(const std::string& sessionId) = 0;
    virtual json getParticipantsForSession(const std::string& sessionId) = 0;

    virtual std::string getLastError() const = 0;
};

} // namespace services

// Forward declarations for repositories
namespace repositories {
    class ISessionRepository;
    class IParticipantRepository;
}

// Forward declarations for cache
namespace database {
    class ICacheProvider;
}

} // namespace conference

// Factory function
std::shared_ptr<conference::services::ISessionService> 
createSessionService(
    std::shared_ptr<conference::repositories::ISessionRepository> sessionRepo,
    std::shared_ptr<conference::repositories::IParticipantRepository> participantRepo,
    std::shared_ptr<conference::database::ICacheProvider> cacheProvider
);
