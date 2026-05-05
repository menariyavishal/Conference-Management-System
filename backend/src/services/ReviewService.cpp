#include "services/IReviewService.h"
#include "repositories/IReviewRepository.h"
#include "repositories/ISubmissionRepository.h"
#include "repositories/IUserRepository.h"
#include "utils/Constants.h"
#include "utils/DateTimeUtils.h"
#include "utils/ValidationUtils.h"
#include "utils/EncryptionUtils.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using json = nlohmann::json;

namespace conference {
namespace services {

class ReviewService : public IReviewService {
private:
    std::string lastError;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<repositories::IReviewRepository> reviewRepository;
    std::shared_ptr<repositories::ISubmissionRepository> submissionRepository;
    std::shared_ptr<repositories::IUserRepository> userRepository;

    json anonymizeSubmission(const json& submission) {
        json anon = submission;
        if (anon.contains("author")) anon["author"] = "Anonymous Author";
        if (anon.contains("email")) anon.erase("email");
        if (anon.contains("authorId")) anon["authorId"] = "HIDDEN";
        if (anon.contains("affiliation")) anon["affiliation"] = "Anonymous Affiliation";
        return anon;
    }

    bool isValidScore(int score) {
        return score >= 1 && score <= 10;
    }

    bool isValidRecommendation(const std::string& rec) {
        return rec == "accept" || rec == "reject" || 
               rec == "major_revision" || rec == "minor_revision";
    }

public:
    ReviewService(
        std::shared_ptr<repositories::IReviewRepository> reviewRepo,
        std::shared_ptr<repositories::ISubmissionRepository> submissionRepo,
        std::shared_ptr<repositories::IUserRepository> userRepo
    ) : reviewRepository(reviewRepo), submissionRepository(submissionRepo), userRepository(userRepo) {
        logger = spdlog::get("conference");
        if (!logger) {
            logger = spdlog::stdout_color_mt("review");
        }
    }

    virtual json assignReview(
        const std::string& submissionId,
        const std::string& reviewerId,
        const std::string& deadline,
        const std::string& adminId
    ) override {
        try {
            auto adminOpt = userRepository->getUserById(adminId);
            if (!adminOpt.has_value() || (adminOpt.value()["role"] != "admin" && adminOpt.value()["role"] != "organizer")) {
                lastError = "Only admin/organizer can assign reviews";
                return {{"success", false}, {"error", lastError}};
            }

            auto submissionOpt = submissionRepository->getSubmissionById(submissionId);
            if (!submissionOpt.has_value()) {
                lastError = "Submission not found";
                return {{"success", false}, {"error", lastError}};
            }

            auto reviewerOpt = userRepository->getUserById(reviewerId);
            if (!reviewerOpt.has_value() || reviewerOpt.value()["role"] != "reviewer") {
                lastError = "Reviewer not found or invalid role";
                return {{"success", false}, {"error", lastError}};
            }

            std::string reviewId = "rev_" + 
                utils::EncryptionUtils::hashSHA256(
                    submissionId + reviewerId + deadline
                ).substr(0, 12);

            std::string now = utils::DateTimeUtils::getCurrentISO8601();

            json assignment = {
                {"reviewId", reviewId},
                {"submissionId", submissionId},
                {"reviewerId", reviewerId},
                {"status", "assigned"},
                {"assignedAt", now},
                {"deadline", deadline}
            };

            if (!reviewRepository->insertReview(assignment)) {
                lastError = "Failed to insert review assignment into database";
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("Review assigned: {} reviewer: {} submission: {} deadline: {}", 
                        reviewId, reviewerId, submissionId, deadline);

            return {
                {"success", true},
                {"message", "Review assignment created"},
                {"data", assignment}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("Review assignment failed: ") + ex.what();
            logger->error(lastError);
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json submitReview(
        const std::string& reviewId,
        int score,
        const std::string& comment,
        const std::string& recommendation,
        const std::string& reviewerId
    ) override {
        try {
            if (!isValidScore(score)) {
                lastError = "Score must be between 1 and 10";
                return {{"success", false}, {"error", lastError}};
            }

            if (!isValidRecommendation(recommendation)) {
                lastError = "Invalid recommendation value";
                return {{"success", false}, {"error", lastError}};
            }

            if (comment.empty() || comment.length() > 5000) {
                lastError = "Comment must be provided and under 5000 characters";
                return {{"success", false}, {"error", lastError}};
            }

            auto reviewOpt = reviewRepository->getReviewById(reviewId);
            if (!reviewOpt.has_value()) {
                lastError = "Review not found";
                return {{"success", false}, {"error", lastError}};
            }

            if (reviewOpt.value()["reviewerId"] != reviewerId) {
                lastError = "Only assigned reviewer can submit";
                return {{"success", false}, {"error", lastError}};
            }

            std::string now = utils::DateTimeUtils::getCurrentISO8601();

            // Using recommendation as a comment prefix or appending to comment, 
            // since we didn't define recommendation in DB schema, we combine it with comment.
            std::string fullComment = "[" + recommendation + "] " + comment;

            if (!reviewRepository->updateReviewScoreAndComments(reviewId, score, fullComment)) {
                lastError = "Failed to save review to database";
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("Review submitted: {} reviewer: {} score: {}", reviewId, reviewerId, score);

            return {
                {"success", true},
                {"message", "Review submitted successfully"},
                {"data", {
                    {"reviewId", reviewId},
                    {"score", score},
                    {"recommendation", recommendation},
                    {"submittedAt", now},
                    {"status", "submitted"}
                }}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("Review submission failed: ") + ex.what();
            logger->error(lastError);
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getReviewAssignments(
        const std::string& reviewerId,
        const std::string& status = ""
    ) override {
        try {
            auto assignments = reviewRepository->getReviewsByReviewer(reviewerId);

            // Filter if status is provided
            std::vector<json> filtered;
            for (const auto& a : assignments) {
                if (status.empty() || a["status"] == status) {
                    filtered.push_back(a);
                }
            }

            return {{"success", true}, {"data", filtered}};

        } catch (const std::exception& ex) {
            return {{"success", false}, {"error", ex.what()}};
        }
    }

    virtual json getSubmissionReviews(
        const std::string& submissionId,
        bool anonymized = true
    ) override {
        try {
            auto reviews = reviewRepository->getReviewsBySubmission(submissionId);

            json result = json::array();
            for (auto review : reviews) {
                if (anonymized && review.contains("reviewerId")) {
                    review["reviewerId"] = "Anonymous Reviewer";
                }
                result.push_back(review);
            }

            return {{"success", true}, {"data", result}};

        } catch (const std::exception& ex) {
            return {{"success", false}, {"error", ex.what()}};
        }
    }

    virtual json getReviewStatistics(const std::string& submissionId) override {
        try {
            auto reviews = reviewRepository->getReviewsBySubmission(submissionId);

            int totalScore = 0;
            int submittedReviews = 0;
            int acceptCount = 0;
            int rejectCount = 0;
            int majorCount = 0;
            int minorCount = 0;

            for (const auto& r : reviews) {
                if (r["status"] == "submitted") {
                    submittedReviews++;
                    totalScore += r["score"].get<int>();
                    
                    std::string comment = r["comments"].get<std::string>();
                    if (comment.find("[accept]") != std::string::npos) acceptCount++;
                    else if (comment.find("[reject]") != std::string::npos) rejectCount++;
                    else if (comment.find("[major_revision]") != std::string::npos) majorCount++;
                    else if (comment.find("[minor_revision]") != std::string::npos) minorCount++;
                }
            }

            double avgScore = submittedReviews > 0 ? (double)totalScore / submittedReviews : 0.0;

            return {
                {"success", true},
                {"data", {
                    {"averageScore", avgScore},
                    {"totalReviews", (int)reviews.size()},
                    {"submittedReviews", submittedReviews},
                    {"pendingReviews", (int)reviews.size() - submittedReviews},
                    {"acceptCount", acceptCount},
                    {"rejectCount", rejectCount},
                    {"majorRevisionCount", majorCount},
                    {"minorRevisionCount", minorCount}
                }}
            };

        } catch (const std::exception& ex) {
            return {{"success", false}, {"error", ex.what()}};
        }
    }

    virtual json updateReviewDeadline(
        const std::string& reviewId,
        const std::string& newDeadline,
        const std::string& adminId
    ) override {
        try {
            auto adminOpt = userRepository->getUserById(adminId);
            if (!adminOpt.has_value() || (adminOpt.value()["role"] != "admin" && adminOpt.value()["role"] != "organizer")) {
                return {{"success", false}, {"error", "Only admin/organizer can update deadline"}};
            }

            // We didn't define updateDeadline in IReviewRepository, and deadline isn't in our schema explicitly.
            // Our V1 schema has assigned_at and submitted_at. Deadline would need a schema change.
            // We'll return an error stating the feature requires schema update.
            return {{"success", false}, {"error", "Deadline field not available in current database schema"}};

        } catch (const std::exception& ex) {
            return {{"success", false}, {"error", ex.what()}};
        }
    }

    virtual bool isDeadlineOverdue(const std::string& reviewId) override {
        return false; // Requires deadline schema column
    }

    virtual json getReviewDetails(
        const std::string& reviewId,
        const std::string& reviewerId
    ) override {
        try {
            auto reviewOpt = reviewRepository->getReviewById(reviewId);
            if (!reviewOpt.has_value()) {
                return {{"success", false}, {"error", "Review not found"}};
            }

            if (reviewOpt.value()["reviewerId"] != reviewerId) {
                return {{"success", false}, {"error", "Unauthorized access"}};
            }

            return {{"success", true}, {"data", reviewOpt.value()}};

        } catch (const std::exception& ex) {
            return {{"success", false}, {"error", ex.what()}};
        }
    }

    virtual json getAnonymizedSubmission(
        const std::string& submissionId,
        const std::string& reviewerId
    ) override {
        try {
            auto assignmentOpt = reviewRepository->getReviewBySubmissionAndReviewer(submissionId, reviewerId);
            if (!assignmentOpt.has_value()) {
                return {{"success", false}, {"error", "Not assigned to review this"}};
            }

            auto submissionOpt = submissionRepository->getSubmissionById(submissionId);
            if (!submissionOpt.has_value()) {
                return {{"success", false}, {"error", "Submission not found"}};
            }

            json anonymized = anonymizeSubmission(submissionOpt.value());
            return {{"success", true}, {"data", anonymized}};

        } catch (const std::exception& ex) {
            return {{"success", false}, {"error", ex.what()}};
        }
    }

    virtual int getPendingReviewCount(const std::string& reviewerId) override {
        try {
            auto assignments = reviewRepository->getReviewsByReviewer(reviewerId);
            int count = 0;
            for (const auto& a : assignments) {
                if (a["status"] == "assigned" || a["status"] == "pending") count++;
            }
            return count;
        } catch (...) {
            return 0;
        }
    }

    virtual json getOverdueReviews() override {
        return {{"success", true}, {"data", json::array()}}; // Requires deadline schema
    }

    virtual std::string getLastError() const override {
        return lastError;
    }
};

}  // namespace services
}  // namespace conference

// Factory function
std::shared_ptr<conference::services::IReviewService> createReviewService(
    std::shared_ptr<conference::repositories::IReviewRepository> reviewRepository,
    std::shared_ptr<conference::repositories::ISubmissionRepository> submissionRepository,
    std::shared_ptr<conference::repositories::IUserRepository> userRepository
) {
    return std::make_shared<conference::services::ReviewService>(reviewRepository, submissionRepository, userRepository);
}
