#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <memory>

using json = nlohmann::json;

namespace conference {
namespace services {

/**
 * @interface IReportingService
 * @brief Service for generating conference analytics and reports
 */
class IReportingService {
public:
    virtual ~IReportingService() = default;

    /**
     * @brief Get global conference statistics (total users, submissions, sessions)
     */
    virtual json getGlobalStats() = 0;

    /**
     * @brief Get submission trends (count by status, count by track)
     */
    virtual json getSubmissionAnalytics() = 0;

    /**
     * @brief Get reviewer performance statistics
     */
    virtual json getReviewerStats() = 0;

    /**
     * @brief Get session attendance and feedback analytics
     */
    virtual json getSessionAnalytics() = 0;

    /**
     * @brief Generate a detailed report for a specific session
     */
    virtual json getSessionReport(const std::string& sessionId) = 0;

    /**
     * @brief Get system health and activity logs (for admins)
     */
    virtual json getSystemActivityReport(int limit = 100) = 0;

    virtual std::string getLastError() const = 0;
};

} // namespace services

// Forward declarations for repositories
namespace repositories {
    class ISubmissionRepository;
    class IReviewRepository;
    class IUserRepository;
    class ISessionRepository;
    class IParticipantRepository;
    class IFeedbackRepository;
}

} // namespace conference

// Factory function
std::shared_ptr<conference::services::IReportingService> 
createReportingService(
    std::shared_ptr<conference::repositories::ISubmissionRepository> submissionRepo,
    std::shared_ptr<conference::repositories::IReviewRepository> reviewRepo,
    std::shared_ptr<conference::repositories::IUserRepository> userRepo,
    std::shared_ptr<conference::repositories::ISessionRepository> sessionRepo,
    std::shared_ptr<conference::repositories::IParticipantRepository> participantRepo,
    std::shared_ptr<conference::repositories::IFeedbackRepository> feedbackRepo
);
