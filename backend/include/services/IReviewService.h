#ifndef I_REVIEW_SERVICE_H
#define I_REVIEW_SERVICE_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace services {

/**
 * @class IReviewService
 * @brief Interface for double-blind review process management
 * 
 * Provides services for:
 * - Review assignment (reviewer to submission)
 * - Review submission and scoring
 * - Double-blind anonymization
 * - Deadline tracking
 * - Review statistics generation
 * - Access control
 */
class IReviewService {
public:
    virtual ~IReviewService() = default;

    /**
     * @brief Assign submission to reviewer for review
     * @param submissionId Submission ID
     * @param reviewerId Reviewer user ID
     * @param deadline Review deadline (ISO 8601 format)
     * @param adminId Admin user ID (for authorization)
     * @return JSON with review assignment data
     * @throws ApplicationException if assignment fails
     */
    virtual json assignReview(
        const std::string& submissionId,
        const std::string& reviewerId,
        const std::string& deadline,
        const std::string& adminId
    ) = 0;

    /**
     * @brief Submit completed review with scores and comments
     * @param reviewId Review assignment ID
     * @param score Numerical score (1-10)
     * @param comment Review feedback/comments
     * @param recommendation Accept/Reject/Major Revision/Minor Revision
     * @param reviewerId Reviewer ID (for authorization)
     * @return JSON with submitted review
     * @throws DeadlinePassedException if past review deadline
     */
    virtual json submitReview(
        const std::string& reviewId,
        int score,
        const std::string& comment,
        const std::string& recommendation,
        const std::string& reviewerId
    ) = 0;

    /**
     * @brief Get review assignments for reviewer
     * @param reviewerId Reviewer user ID
     * @param status Optional status filter (pending, submitted, overdue)
     * @return JSON array of assigned submissions
     */
    virtual json getReviewAssignments(
        const std::string& reviewerId,
        const std::string& status = ""
    ) = 0;

    /**
     * @brief Get reviews for submission (anonymized)
     * @param submissionId Submission ID
     * @param anonymized If true, remove reviewer names
     * @return JSON array of reviews
     * @throws SubmissionNotFoundException
     */
    virtual json getSubmissionReviews(
        const std::string& submissionId,
        bool anonymized = true
    ) = 0;

    /**
     * @brief Get review statistics for submission
     * @param submissionId Submission ID
     * @return JSON with average score, review count, status summary
     */
    virtual json getReviewStatistics(const std::string& submissionId) = 0;

    /**
     * @brief Update review deadline
     * @param reviewId Review ID
     * @param newDeadline New deadline (ISO 8601 format)
     * @param adminId Admin ID (for authorization)
     * @return JSON response
     */
    virtual json updateReviewDeadline(
        const std::string& reviewId,
        const std::string& newDeadline,
        const std::string& adminId
    ) = 0;

    /**
     * @brief Check if review deadline is overdue
     * @param reviewId Review ID
     * @return true if deadline passed
     */
    virtual bool isDeadlineOverdue(const std::string& reviewId) = 0;

    /**
     * @brief Get review details (for reviewer viewing their own review)
     * @param reviewId Review ID
     * @param reviewerId Reviewer ID (for authorization)
     * @return JSON with review data
     */
    virtual json getReviewDetails(
        const std::string& reviewId,
        const std::string& reviewerId
    ) = 0;

    /**
     * @brief Get anonymized submission for review
     * @param submissionId Submission ID
     * @param reviewerId Reviewer ID
     * @return JSON with submission (author names removed)
     * @throws UnauthorizedException if reviewer not assigned
     */
    virtual json getAnonymizedSubmission(
        const std::string& submissionId,
        const std::string& reviewerId
    ) = 0;

    /**
     * @brief Get pending reviews count for reviewer
     * @param reviewerId Reviewer ID
     * @return Count of pending reviews
     */
    virtual int getPendingReviewCount(const std::string& reviewerId) = 0;

    /**
     * @brief Get overdue reviews (deadline passed, not submitted)
     * @return JSON array of overdue reviews
     */
    virtual json getOverdueReviews() = 0;

    /**
     * @brief Get last error message
     * @return Error message from last operation
     */
    virtual std::string getLastError() const = 0;
};

} // namespace services

// Forward declarations for repositories
namespace repositories {
    class IReviewRepository;
    class ISubmissionRepository;
    class IUserRepository;
}

} // namespace conference

std::shared_ptr<conference::services::IReviewService>
createReviewService(
    std::shared_ptr<conference::repositories::IReviewRepository> reviewRepo,
    std::shared_ptr<conference::repositories::ISubmissionRepository> submissionRepo,
    std::shared_ptr<conference::repositories::IUserRepository> userRepo
);

#endif  // I_REVIEW_SERVICE_H
