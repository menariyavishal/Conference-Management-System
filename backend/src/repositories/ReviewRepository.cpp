#include "repositories/IReviewRepository.h"
#include "database/ConnectionPool.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace conference {
namespace repositories {

class ReviewRepository : public IReviewRepository {
private:
    std::shared_ptr<database::ConnectionPool> connectionPool;
    std::shared_ptr<spdlog::logger> logger;

public:
    ReviewRepository(std::shared_ptr<database::ConnectionPool> pool) : connectionPool(pool) {
        logger = spdlog::get("conference");
        if (!logger) {
            logger = spdlog::stdout_color_mt("review_repo");
        }
    }

    virtual bool insertReview(const json& review) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "INSERT INTO reviews (id, submission_id, reviewer_id, score, comments, status, assigned_at) "
                            "VALUES ($1, $2, $3, $4, $5, $6, $7)";
        
        std::vector<std::string> params = {
            review["reviewId"].get<std::string>(),
            review["submissionId"].get<std::string>(),
            review["reviewerId"].get<std::string>(),
            review.contains("score") && !review["score"].is_null() ? std::to_string(review["score"].get<int>()) : "",
            review.contains("comments") ? review["comments"].get<std::string>() : "",
            review["status"].get<std::string>(),
            review["assignedAt"].get<std::string>()
        };

        // Note: PQexecParams expects NULL for empty params if we want DB NULL, but here we just pass empty strings
        // Ideally we'd use a more robust param binding that handles NULLs, but empty string works for TEXT fields.
        // For score (INTEGER), an empty string might cause an error in Postgres if not handled.
        // Let's pass NULL explicitly if score is empty. Wait, the wrapper executeWithParams currently takes vector of strings.
        // We might just update the query to handle this or assume score has a default.
        // Actually, if score is empty, we can just insert it as 0, or we need to refine the executeWithParams to handle NULLs.
        // For now, let's just pass '0' if it's null, since status is 'assigned' initially.
        if (params[3].empty()) params[3] = "0";

        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual std::optional<json> getReviewById(const std::string& reviewId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return std::nullopt;

        std::string query = "SELECT * FROM reviews WHERE id = $1";
        std::vector<std::string> params = {reviewId};
        
        auto results = conn->executeQueryWithParams(query, params);
        connectionPool->releaseConnection(conn);

        if (results.empty()) return std::nullopt;

        auto row = results[0];
        json review = {
            {"reviewId", row["id"]},
            {"submissionId", row["submission_id"]},
            {"reviewerId", row["reviewer_id"]},
            {"score", (row["score"].is_null() || row["score"].get<std::string>().empty()) ? 0 : std::stoi(row["score"].get<std::string>())},
            {"comments", row["comments"]},
            {"status", row["status"]},
            {"assignedAt", row["assigned_at"]},
            {"submittedAt", row["submitted_at"]}
        };
        return review;
    }

    virtual std::optional<json> getReviewBySubmissionAndReviewer(const std::string& submissionId, const std::string& reviewerId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return std::nullopt;

        std::string query = "SELECT * FROM reviews WHERE submission_id = $1 AND reviewer_id = $2";
        std::vector<std::string> params = {submissionId, reviewerId};
        
        auto results = conn->executeQueryWithParams(query, params);
        connectionPool->releaseConnection(conn);

        if (results.empty()) return std::nullopt;

        auto row = results[0];
        json review = {
            {"reviewId", row["id"]},
            {"submissionId", row["submission_id"]},
            {"reviewerId", row["reviewer_id"]},
            {"score", (row["score"].is_null() || row["score"].get<std::string>().empty()) ? 0 : std::stoi(row["score"].get<std::string>())},
            {"comments", row["comments"]},
            {"status", row["status"]},
            {"assignedAt", row["assigned_at"]},
            {"submittedAt", row["submitted_at"]}
        };
        return review;
    }

    virtual bool updateReviewScoreAndComments(const std::string& reviewId, int score, const std::string& comments) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "UPDATE reviews SET score = $1, comments = $2, status = 'submitted', submitted_at = CURRENT_TIMESTAMP WHERE id = $3";
        std::vector<std::string> params = {std::to_string(score), comments, reviewId};
        
        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual bool updateStatus(const std::string& reviewId, const std::string& status) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "UPDATE reviews SET status = $1 WHERE id = $2";
        std::vector<std::string> params = {status, reviewId};
        
        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual std::vector<json> getReviewsBySubmission(const std::string& submissionId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        std::string query = "SELECT * FROM reviews WHERE submission_id = $1 ORDER BY assigned_at DESC";
        std::vector<std::string> params = {submissionId};
        
        auto results = conn->executeQueryWithParams(query, params);
        connectionPool->releaseConnection(conn);

        std::vector<json> reviews;
        for (const auto& row : results) {
            json review = {
                {"reviewId", row.at("id")},
                {"submissionId", row.at("submission_id")},
                {"reviewerId", row.at("reviewer_id")},
                {"score", (row.at("score").is_null() || row.at("score").get<std::string>().empty()) ? 0 : std::stoi(row.at("score").get<std::string>())},
                {"comments", row.at("comments")},
                {"status", row.at("status")},
                {"assignedAt", row.at("assigned_at")},
                {"submittedAt", row.at("submitted_at")}
            };
            reviews.push_back(review);
        }
        return reviews;
    }

    virtual std::vector<json> getReviewsByReviewer(const std::string& reviewerId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        std::string query = "SELECT * FROM reviews WHERE reviewer_id = $1 ORDER BY assigned_at DESC";
        std::vector<std::string> params = {reviewerId};
        
        auto results = conn->executeQueryWithParams(query, params);
        connectionPool->releaseConnection(conn);

        std::vector<json> reviews;
        for (const auto& row : results) {
            json review = {
                {"reviewId", row.at("id")},
                {"submissionId", row.at("submission_id")},
                {"reviewerId", row.at("reviewer_id")},
                {"score", (row.at("score").is_null() || row.at("score").get<std::string>().empty()) ? 0 : std::stoi(row.at("score").get<std::string>())},
                {"comments", row.at("comments")},
                {"status", row.at("status")},
                {"assignedAt", row.at("assigned_at")},
                {"submittedAt", row.at("submitted_at")}
            };
            reviews.push_back(review);
        }
        return reviews;
    }

    virtual json getGlobalReviewStats() override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {{"total", 0}, {"avgScore", 0.0}};

        // Using CASE WHEN for better compatibility than FILTER
        std::string query = "SELECT COUNT(*) as total, AVG(score) as avg_score FROM reviews WHERE status = 'submitted'";
        auto results = conn->executeQueryWithParams(query, {});
        connectionPool->releaseConnection(conn);

        if (results.empty() || results[0].at("total").is_null()) {
            return {{"total", 0}, {"avgScore", 0.0}};
        }

        try {
            int total = std::stoi(results[0].at("total").get<std::string>());
            double avg = results[0].at("avg_score").is_null() ? 0.0 : std::stod(results[0].at("avg_score").get<std::string>());
            return {{"total", total}, {"avgScore", avg}};
        } catch (...) {
            return {{"total", 0}, {"avgScore", 0.0}};
        }
    }

    virtual std::vector<json> getReviewerPerformanceStats() override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        // Replace FILTER (Postgres 9.4+) with CASE WHEN for maximum compatibility
        std::string query = 
            "SELECT reviewer_id, COUNT(*) as total_assigned, "
            "COUNT(CASE WHEN status = 'submitted' THEN 1 END) as total_submitted, "
            "AVG(CASE WHEN status = 'submitted' THEN score END) as avg_score_given "
            "FROM reviews GROUP BY reviewer_id";
        
        auto results = conn->executeQueryWithParams(query, {});
        connectionPool->releaseConnection(conn);

        std::vector<json> stats;
        for (const auto& row : results) {
            try {
                stats.push_back({
                    {"reviewerId", row.at("reviewer_id")},
                    {"totalAssigned", std::stoi(row.at("total_assigned").get<std::string>())},
                    {"totalSubmitted", row.at("total_submitted").is_null() ? 0 : std::stoi(row.at("total_submitted").get<std::string>())},
                    {"avgScoreGiven", row.at("avg_score_given").is_null() ? 0.0 : std::stod(row.at("avg_score_given").get<std::string>())}
                });
            } catch (const std::exception& e) {
                logger->error("Error parsing reviewer performance stats: {}", e.what());
            }
        }
        return stats;
    }
};

} // namespace repositories
} // namespace conference

// Factory function
std::shared_ptr<conference::repositories::IReviewRepository> createReviewRepository(std::shared_ptr<conference::database::ConnectionPool> pool) {
    return std::make_shared<conference::repositories::ReviewRepository>(pool);
}
