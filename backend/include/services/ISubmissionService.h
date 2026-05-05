#ifndef I_SUBMISSION_SERVICE_H
#define I_SUBMISSION_SERVICE_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace services {

/**
 * @class ISubmissionService
 * @brief Interface for submission management operations
 * 
 * Provides services for:
 * - Paper/abstract submission creation
 * - File upload for submissions
 * - Submission editing (before deadline)
 * - Submission withdrawal
 * - Submission status tracking
 * - Submission listing with filters
 */
class ISubmissionService {
public:
    virtual ~ISubmissionService() = default;

    /**
     * @brief Create new submission
     * @param authorId Author user ID
     * @param title Paper title
     * @param abstract Paper abstract
     * @param keywords Comma-separated keywords
     * @param conferenceId Conference ID to submit to
     * @return JSON with submission data and submission ID
     * @throws ApplicationException if validation fails
     */
    virtual json createSubmission(
        const std::string& authorId,
        const std::string& title,
        const std::string& abstract,
        const std::string& keywords,
        const std::string& conferenceId
    ) = 0;

    /**
     * @brief Upload presentation file for submission
     * @param submissionId Submission ID
     * @param filePath Path to file to upload
     * @param authorId Author ID (for authorization)
     * @return JSON with file ID and metadata
     * @throws SubmissionNotFoundException or UnauthorizedException
     */
    virtual json uploadFile(
        const std::string& submissionId,
        const std::string& filePath,
        const std::string& authorId
    ) = 0;

    /**
     * @brief Update submission (title, abstract, keywords)
     * @param submissionId Submission ID
     * @param updateData JSON with fields to update
     * @param authorId Author ID (for authorization)
     * @return Updated submission JSON
     * @throws DeadlinePassedException if submission past edit deadline
     */
    virtual json updateSubmission(
        const std::string& submissionId,
        const json& updateData,
        const std::string& authorId
    ) = 0;

    /**
     * @brief Withdraw submission
     * @param submissionId Submission ID
     * @param authorId Author ID (for authorization)
     * @return Success JSON response
     * @throws SubmissionNotFoundException or UnauthorizedException
     */
    virtual json withdrawSubmission(
        const std::string& submissionId,
        const std::string& authorId
    ) = 0;

    /**
     * @brief Get submission details
     * @param submissionId Submission ID
     * @return Complete submission JSON (files, reviews if authorized)
     * @throws SubmissionNotFoundException
     */
    virtual json getSubmissionDetails(const std::string& submissionId) = 0;

    /**
     * @brief List submissions with pagination and filters
     * @param page Page number (1-indexed)
     * @param pageSize Results per page
     * @param status Filter by status (draft, submitted, accepted, rejected, withdrawn)
     * @param conferenceId Filter by conference
     * @param sortBy Sort field (submittedAt, score, etc)
     * @return JSON array of submissions with pagination info
     */
    virtual json listSubmissions(
        int page = 1,
        int pageSize = 20,
        const std::string& status = "",
        const std::string& conferenceId = "",
        const std::string& sortBy = "submittedAt"
    ) = 0;

    /**
     * @brief Get submissions by author
     * @param authorId Author user ID
     * @param conferenceId Optional conference filter
     * @return JSON array of author's submissions
     */
    virtual json getSubmissionsByAuthor(
        const std::string& authorId,
        const std::string& conferenceId = ""
    ) = 0;

    /**
     * @brief Get submissions available for review
     * @param reviewerId Reviewer user ID
     * @return JSON array of assigned submissions for review
     */
    virtual json getSubmissionsForReview(
        const std::string& reviewerId
    ) = 0;

    /**
     * @brief Update submission status
     * @param submissionId Submission ID
     * @param newStatus New status (accepted, rejected, withdrawn)
     * @param adminId Admin user ID (for authorization)
     * @return Updated submission JSON
     */
    virtual json updateSubmissionStatus(
        const std::string& submissionId,
        const std::string& newStatus,
        const std::string& adminId
    ) = 0;

    /**
     * @brief Get submission count for conference
     * @param conferenceId Conference ID
     * @param status Optional status filter
     * @return Total count
     */
    virtual int getSubmissionCount(
        const std::string& conferenceId,
        const std::string& status = ""
    ) = 0;

    /**
     * @brief Get last error message
     * @return Error message from last operation
     */
    virtual std::string getLastError() const = 0;
};

} // namespace services

// Forward declarations for repositories and services
namespace repositories {
    class ISubmissionRepository;
}
namespace services {
    class IFileService;
}

} // namespace conference

std::shared_ptr<conference::services::ISubmissionService>
createSubmissionService(
    std::shared_ptr<conference::services::IFileService> fileService,
    std::shared_ptr<conference::repositories::ISubmissionRepository> submissionRepo
);

#endif  // I_SUBMISSION_SERVICE_H
