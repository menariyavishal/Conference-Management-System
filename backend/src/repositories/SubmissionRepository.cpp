#include "repositories/ISubmissionRepository.h"
#include "database/ConnectionPool.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace conference {
namespace repositories {

class SubmissionRepository : public ISubmissionRepository {
private:
    std::shared_ptr<database::ConnectionPool> connectionPool;
    std::shared_ptr<spdlog::logger> logger;

public:
    SubmissionRepository(std::shared_ptr<database::ConnectionPool> pool) : connectionPool(pool) {
        logger = spdlog::get("conference");
        if (!logger) {
            logger = spdlog::stdout_color_mt("submission_repo");
        }
    }

    virtual bool insertSubmission(const json& submission) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "INSERT INTO submissions (id, title, abstract, keywords, status, author_id, created_at, updated_at) "
                            "VALUES ($1, $2, $3, $4, $5, $6, $7, $8)";
        
        std::vector<std::string> params = {
            submission["submissionId"].get<std::string>(),
            submission["title"].get<std::string>(),
            submission["abstract"].get<std::string>(),
            submission.contains("keywords") ? submission["keywords"].get<std::string>() : "",
            submission["status"].get<std::string>(),
            submission["authorId"].get<std::string>(),
            submission["createdAt"].get<std::string>(),
            submission["updatedAt"].get<std::string>()
        };

        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual std::optional<json> getSubmissionById(const std::string& submissionId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return std::nullopt;

        std::string query = "SELECT * FROM submissions WHERE id = $1";
        std::vector<std::string> params = {submissionId};
        
        auto results = conn->executeQueryWithParams(query, params);
        connectionPool->releaseConnection(conn);

        if (results.empty()) return std::nullopt;

        auto row = results[0];
        json submission = {
            {"submissionId", row["id"]},
            {"title", row["title"]},
            {"abstract", row["abstract"]},
            {"keywords", row["keywords"]},
            {"status", row["status"]},
            {"authorId", row["author_id"]},
            {"createdAt", row["created_at"]},
            {"updatedAt", row["updated_at"]}
        };
        return submission;
    }

    virtual bool updateSubmission(const std::string& submissionId, const json& updateData) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "UPDATE submissions SET title = $1, abstract = $2, keywords = $3, updated_at = CURRENT_TIMESTAMP WHERE id = $4";
        std::vector<std::string> params = {
            updateData["title"].get<std::string>(),
            updateData["abstract"].get<std::string>(),
            updateData.contains("keywords") ? updateData["keywords"].get<std::string>() : "",
            submissionId
        };
        
        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual bool updateStatus(const std::string& submissionId, const std::string& status) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "UPDATE submissions SET status = $1, updated_at = CURRENT_TIMESTAMP WHERE id = $2";
        std::vector<std::string> params = {status, submissionId};
        
        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual std::vector<json> getSubmissionsByAuthor(const std::string& authorId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        std::string query = "SELECT * FROM submissions WHERE author_id = $1 ORDER BY created_at DESC";
        std::vector<std::string> params = {authorId};
        
        auto results = conn->executeQueryWithParams(query, params);
        connectionPool->releaseConnection(conn);

        std::vector<json> submissions;
        for (const auto& row : results) {
            json submission = {
                {"submissionId", row.at("id")},
                {"title", row.at("title")},
                {"abstract", row.at("abstract")},
                {"keywords", row.at("keywords")},
                {"status", row.at("status")},
                {"authorId", row.at("author_id")},
                {"createdAt", row.at("created_at")},
                {"updatedAt", row.at("updated_at")}
            };
            submissions.push_back(submission);
        }
        return submissions;
    }

    virtual std::vector<json> getSubmissions(int limit, int offset, const std::string& statusFilter, const std::string& searchQuery) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        std::string query = "SELECT * FROM submissions WHERE 1=1";
        std::vector<std::string> params;
        int paramCount = 1;

        if (!statusFilter.empty()) {
            query += " AND status = $" + std::to_string(paramCount++);
            params.push_back(statusFilter);
        }

        if (!searchQuery.empty()) {
            query += " AND (title ILIKE $" + std::to_string(paramCount) + " OR abstract ILIKE $" + std::to_string(paramCount) + ")";
            params.push_back("%" + searchQuery + "%");
            paramCount++;
        }

        query += " ORDER BY created_at DESC LIMIT $" + std::to_string(paramCount++) + " OFFSET $" + std::to_string(paramCount++);
        params.push_back(std::to_string(limit));
        params.push_back(std::to_string(offset));

        auto results = conn->executeQueryWithParams(query, params);
        connectionPool->releaseConnection(conn);

        std::vector<json> submissions;
        for (const auto& row : results) {
            json submission = {
                {"submissionId", row.at("id")},
                {"title", row.at("title")},
                {"abstract", row.at("abstract")},
                {"keywords", row.at("keywords")},
                {"status", row.at("status")},
                {"authorId", row.at("author_id")},
                {"createdAt", row.at("created_at")},
                {"updatedAt", row.at("updated_at")}
            };
            submissions.push_back(submission);
        }
        return submissions;
    }

    virtual int getSubmissionCount(const std::string& statusFilter, const std::string& searchQuery) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return 0;

        std::string query = "SELECT COUNT(*) FROM submissions WHERE 1=1";
        std::vector<std::string> params;
        int paramCount = 1;

        if (!statusFilter.empty()) {
            query += " AND status = $" + std::to_string(paramCount++);
            params.push_back(statusFilter);
        }

        if (!searchQuery.empty()) {
            query += " AND (title ILIKE $" + std::to_string(paramCount) + " OR abstract ILIKE $" + std::to_string(paramCount) + ")";
            params.push_back("%" + searchQuery + "%");
            paramCount++;
        }

        auto resultStr = conn->executeScalarWithParams(query, params);
        connectionPool->releaseConnection(conn);

        try {
            return std::stoi(resultStr);
        } catch (...) {
            return 0;
        }
    }

    virtual json getStatusStatistics() override {
        auto conn = connectionPool->getConnection();
        if (!conn) return json::object();

        std::string query = "SELECT status, COUNT(*) as count FROM submissions GROUP BY status";
        auto results = conn->executeQueryWithParams(query, {});
        connectionPool->releaseConnection(conn);

        json stats = json::object();
        for (const auto& row : results) {
            try {
                stats[row.at("status").get<std::string>()] = std::stoi(row.at("count").get<std::string>());
            } catch (...) {
                stats[row.at("status").get<std::string>()] = 0;
            }
        }
        return stats;
    }
};

} // namespace repositories
} // namespace conference

// Factory function
std::shared_ptr<conference::repositories::ISubmissionRepository> createSubmissionRepository(std::shared_ptr<conference::database::ConnectionPool> pool) {
    return std::make_shared<conference::repositories::SubmissionRepository>(pool);
}
