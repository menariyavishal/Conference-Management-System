#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace services {

/**
 * @interface IFeedbackService
 * @brief Contract for session feedback collection and analytics (FR6)
 */
class IFeedbackService {
public:
    virtual ~IFeedbackService() = default;

    virtual json submitFeedback(const std::string& sessionId,
                                const std::string& participantId,
                                int rating,
                                const std::string& comment,
                                bool isAnonymous) = 0;

    virtual json getSessionFeedbackSummary(const std::string& sessionId,
                                           const std::string& requesterId) = 0;

    virtual json listAllFeedback(const std::string& organizerId) = 0;

    virtual std::string getLastError() const = 0;
};

} // namespace services

// Forward declarations for repositories
namespace repositories {
    class IFeedbackRepository;
    class IParticipantRepository;
}

} // namespace conference

std::shared_ptr<conference::services::IFeedbackService>
createFeedbackService(
    std::shared_ptr<conference::repositories::IFeedbackRepository> feedbackRepo,
    std::shared_ptr<conference::repositories::IParticipantRepository> participantRepo
);
