#include "services/ISubmissionService.h"
#include "services/IFileService.h"
#include "repositories/ISubmissionRepository.h"
#include "utils/ValidationUtils.h"
#include "utils/Constants.h"
#include "utils/DateTimeUtils.h"
#include "utils/EncryptionUtils.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using json = nlohmann::json;

namespace conference {
namespace services {

class SubmissionService : public ISubmissionService {
private:
    std::string lastError;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<IFileService> fileService;
    std::shared_ptr<repositories::ISubmissionRepository> submissionRepository;

    bool validateSubmissionData(
        const std::string& title,
        const std::string& abstract,
        const std::string& keywords
    ) {
        if (title.empty() || title.length() < 5 || title.length() > 250) {
            lastError = "Title must be between 5 and 250 characters";
            return false;
        }

        if (abstract.empty() || abstract.length() < 50 || abstract.length() > 1000) {
            lastError = "Abstract must be between 50 and 1000 characters";
            return false;
        }

        if (keywords.empty() || keywords.length() > 200) {
            lastError = "Keywords must be provided and under 200 characters";
            return false;
        }

        return true;
    }

    bool isEditableStatus(const std::string& status) {
        return status == "draft" || status == "submitted";
    }

    std::string getNextStatus(const std::string& currentStatus) {
        if (currentStatus == "draft") return "submitted";
        if (currentStatus == "submitted") return "under_review";
        return currentStatus;
    }

public:
    SubmissionService(std::shared_ptr<IFileService> fileSvc, std::shared_ptr<repositories::ISubmissionRepository> repo) 
        : fileService(fileSvc), submissionRepository(repo) {
        logger = spdlog::get("conference");
        if (!logger) {
            logger = spdlog::stdout_color_mt("submission");
        }
    }

    virtual json createSubmission(
        const std::string& authorId,
        const std::string& title,
        const std::string& abstract,
        const std::string& keywords,
        const std::string& conferenceId
    ) override {
        try {
            if (!validateSubmissionData(title, abstract, keywords)) {
                logger->warn("Submission validation failed: {}", lastError);
                return {{"success", false}, {"error", lastError}};
            }

            std::string submissionId = "sub_" + 
                utils::EncryptionUtils::hashSHA256(
                    authorId + title + utils::DateTimeUtils::getCurrentISO8601()
                ).substr(0, 12);

            std::string now = utils::DateTimeUtils::getCurrentISO8601();

            json newSubmission = {
                {"submissionId", submissionId},
                {"authorId", authorId},
                {"title", title},
                {"abstract", abstract},
                {"keywords", keywords},
                {"status", "draft"},
                {"createdAt", now},
                {"updatedAt", now}
            };
            
            if (!submissionRepository->insertSubmission(newSubmission)) {
                lastError = "Failed to insert submission into database";
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("Submission created: {} author: {}", submissionId, authorId);

            return {
                {"success", true},
                {"message", "Submission created successfully"},
                {"data", newSubmission}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("Submission creation failed: ") + ex.what();
            logger->error(lastError);
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json uploadFile(
        const std::string& submissionId,
        const std::string& filePath,
        const std::string& authorId
    ) override {
        try {
            auto submissionOpt = submissionRepository->getSubmissionById(submissionId);
            if (!submissionOpt.has_value() || submissionOpt.value()["authorId"] != authorId) {
                lastError = "Only author can upload files to this submission";
                return {{"success", false}, {"error", lastError}};
            }

            json uploadResult = fileService->uploadFile(filePath, authorId, "submission_" + submissionId);

            if (!uploadResult["success"].get<bool>()) {
                lastError = uploadResult["error"].get<std::string>();
                return uploadResult;
            }

            logger->info("File uploaded for submission: {} fileId: {}", submissionId, uploadResult["data"]["fileId"].get<std::string>());
            return uploadResult;

        } catch (const std::exception& ex) {
            lastError = std::string("File upload failed: ") + ex.what();
            logger->error(lastError);
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json updateSubmission(
        const std::string& submissionId,
        const json& updateData,
        const std::string& authorId
    ) override {
        try {
            auto submissionOpt = submissionRepository->getSubmissionById(submissionId);
            if (!submissionOpt.has_value() || submissionOpt.value()["authorId"] != authorId) {
                lastError = "Only author can update submission";
                return {{"success", false}, {"error", lastError}};
            }

            json submission = submissionOpt.value();
            if (!isEditableStatus(submission["status"])) {
                lastError = "Cannot edit submission in current status: " + submission["status"].get<std::string>();
                return {{"success", false}, {"error", lastError}};
            }

            std::string title = updateData.contains("title") ? updateData["title"].get<std::string>() : submission["title"].get<std::string>();
            std::string abstract = updateData.contains("abstract") ? updateData["abstract"].get<std::string>() : submission["abstract"].get<std::string>();
            std::string keywords = updateData.contains("keywords") ? updateData["keywords"].get<std::string>() : submission["keywords"].get<std::string>();
            
            if (!validateSubmissionData(title, abstract, keywords)) {
                return {{"success", false}, {"error", lastError}};
            }

            json updatedFields = {
                {"title", title},
                {"abstract", abstract},
                {"keywords", keywords}
            };

            if (!submissionRepository->updateSubmission(submissionId, updatedFields)) {
                lastError = "Failed to update submission in database";
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("Submission updated: {} author: {}", submissionId, authorId);

            return {
                {"success", true},
                {"message", "Submission updated successfully"},
                {"data", updatedFields}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("Submission update failed: ") + ex.what();
            logger->error(lastError);
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json withdrawSubmission(
        const std::string& submissionId,
        const std::string& authorId
    ) override {
        try {
            auto submissionOpt = submissionRepository->getSubmissionById(submissionId);
            if (!submissionOpt.has_value() || submissionOpt.value()["authorId"] != authorId) {
                lastError = "Only author can withdraw";
                return {{"success", false}, {"error", lastError}};
            }

            if (!submissionRepository->updateStatus(submissionId, "withdrawn")) {
                lastError = "Failed to withdraw submission";
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("Submission withdrawn: {} author: {}", submissionId, authorId);

            return {
                {"success", true},
                {"message", "Submission withdrawn successfully"}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("Submission withdrawal failed: ") + ex.what();
            logger->error(lastError);
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getSubmissionDetails(const std::string& submissionId) override {
        try {
            auto submissionOpt = submissionRepository->getSubmissionById(submissionId);
            if (!submissionOpt.has_value()) {
                return {{"success", false}, {"error", "Submission not found"}};
            }

            logger->debug("Retrieved submission details: {}", submissionId);
            return {
                {"success", true},
                {"data", submissionOpt.value()}
            };

        } catch (const std::exception& ex) {
            return {{"success", false}, {"error", ex.what()}};
        }
    }

    virtual json listSubmissions(
        int page = 1,
        int pageSize = 20,
        const std::string& status = "",
        const std::string& conferenceId = "",
        const std::string& sortBy = "submittedAt"
    ) override {
        try {
            int offset = (page - 1) * pageSize;
            auto submissions = submissionRepository->getSubmissions(pageSize, offset, status, "");
            int totalCount = submissionRepository->getSubmissionCount(status, "");
            int totalPages = (totalCount + pageSize - 1) / pageSize;

            logger->debug("Listed submissions - page: {}, status: {}", page, status);

            return {
                {"success", true},
                {"data", submissions},
                {"pagination", {
                    {"page", page},
                    {"pageSize", pageSize},
                    {"totalCount", totalCount},
                    {"totalPages", totalPages}
                }}
            };

        } catch (const std::exception& ex) {
            return {{"success", false}, {"error", ex.what()}};
        }
    }

    virtual json getSubmissionsByAuthor(
        const std::string& authorId,
        const std::string& conferenceId = ""
    ) override {
        try {
            auto submissions = submissionRepository->getSubmissionsByAuthor(authorId);
            logger->debug("Retrieved submissions for author: {}", authorId);
            return {
                {"success", true},
                {"data", submissions}
            };
        } catch (const std::exception& ex) {
            return {{"success", false}, {"error", ex.what()}};
        }
    }

    virtual json getSubmissionsForReview(
        const std::string& reviewerId
    ) override {
        try {
            // Ideally this queries a joint view. Since ReviewService handles reviews, we might need ReviewRepository here, 
            // but for now we return not implemented or empty since it's typically handled in ReviewService.
            return {{"success", true}, {"data", json::array()}};
        } catch (const std::exception& ex) {
            return {{"success", false}, {"error", ex.what()}};
        }
    }

    virtual json updateSubmissionStatus(
        const std::string& submissionId,
        const std::string& newStatus,
        const std::string& adminId
    ) override {
        try {
            if (!submissionRepository->updateStatus(submissionId, newStatus)) {
                lastError = "Failed to update status in database";
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("Submission status updated: {} newStatus: {} admin: {}", submissionId, newStatus, adminId);
            return {
                {"success", true},
                {"message", "Status updated successfully"},
                {"data", {{"submissionId", submissionId}, {"status", newStatus}}}
            };

        } catch (const std::exception& ex) {
            return {{"success", false}, {"error", ex.what()}};
        }
    }

    virtual int getSubmissionCount(
        const std::string& conferenceId,
        const std::string& status = ""
    ) override {
        try {
            return submissionRepository->getSubmissionCount(status, "");
        } catch (...) {
            return 0;
        }
    }

    virtual std::string getLastError() const override {
        return lastError;
    }
};

}  // namespace services
}  // namespace conference

// Factory function
std::shared_ptr<conference::services::ISubmissionService> createSubmissionService(
    std::shared_ptr<conference::services::IFileService> fileService,
    std::shared_ptr<conference::repositories::ISubmissionRepository> submissionRepository) {
    return std::make_shared<conference::services::SubmissionService>(fileService, submissionRepository);
}
