#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace services {

/**
 * @interface IParticipantService
 * @brief Contract for session registration and attendance tracking (FR5)
 */
class IParticipantService {
public:
    virtual ~IParticipantService() = default;

    virtual json registerForSession(const std::string& sessionId, const std::string& userId) = 0;
    virtual json deregisterFromSession(const std::string& sessionId, const std::string& userId) = 0;
    virtual json getUserSessions(const std::string& userId) = 0;
    virtual json getSessionParticipants(const std::string& sessionId) = 0;
    virtual json markAttendance(const std::string& sessionId,
                                const std::string& userId,
                                bool attended,
                                const std::string& organizerId) = 0;

    virtual std::string getLastError() const = 0;
};

} // namespace services

// Forward declarations for repositories
namespace repositories {
    class ISessionRepository;
    class IParticipantRepository;
}

} // namespace conference

std::shared_ptr<conference::services::IParticipantService>
createParticipantService(
    std::shared_ptr<conference::repositories::IParticipantRepository> participantRepo,
    std::shared_ptr<conference::repositories::ISessionRepository> sessionRepo
);
