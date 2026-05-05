#include "repositories/ISessionRepository.h"
#include "database/ConnectionPool.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace conference {
namespace repositories {

class SessionRepository : public ISessionRepository {
private:
    std::shared_ptr<database::ConnectionPool> connectionPool;
    std::shared_ptr<spdlog::logger> logger;

public:
    SessionRepository(std::shared_ptr<database::ConnectionPool> pool) : connectionPool(pool) {
        logger = spdlog::get("session_repo");
        if (!logger) {
            try {
                spdlog::sink_ptr sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                logger = std::make_shared<spdlog::logger>("session_repo", sink);
                spdlog::register_logger(logger);
            } catch (...) {
                logger = spdlog::get("conference");
            }
        }
    }

    virtual bool insertSession(const json& session) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query =
            "INSERT INTO sessions (id, title, description, start_time, end_time, location, capacity, chair_id, is_active) "
            "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)";

        std::vector<std::string> params = {
            session["id"].get<std::string>(),
            session["title"].get<std::string>(),
            session.value("description", ""),
            session.value("startTime", ""),
            session.value("endTime", ""),
            session.value("location", ""),
            std::to_string(session.value("capacity", 0)),
            session.value("chairId", ""),
            "true"
        };

        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual std::optional<json> getSessionById(const std::string& sessionId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return std::nullopt;

        std::string query = "SELECT * FROM sessions WHERE id = $1";
        auto results = conn->executeQueryWithParams(query, {sessionId});
        connectionPool->releaseConnection(conn);

        if (results.empty()) return std::nullopt;

        auto& row = results[0];
        json s = {
            {"id", row["id"]},
            {"title", row["title"]},
            {"description", row["description"]},
            {"startTime", row["start_time"]},
            {"endTime", row["end_time"]},
            {"location", row["location"]},
            {"capacity", std::stoi(row["capacity"].get<std::string>())},
            {"chairId", row["chair_id"]},
            {"isActive", row["is_active"] == "t" || row["is_active"] == "true"},
            {"createdAt", row["created_at"]}
        };
        return s;
    }

    virtual bool updateSession(const std::string& sessionId, const json& updates) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query =
            "UPDATE sessions SET title = $1, description = $2, start_time = $3, end_time = $4, "
            "location = $5, capacity = $6, chair_id = $7, updated_at = CURRENT_TIMESTAMP WHERE id = $8";

        std::vector<std::string> params = {
            updates.value("title", ""),
            updates.value("description", ""),
            updates.value("startTime", ""),
            updates.value("endTime", ""),
            updates.value("location", ""),
            std::to_string(updates.value("capacity", 0)),
            updates.value("chairId", ""),
            sessionId
        };

        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual bool deleteSession(const std::string& sessionId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "UPDATE sessions SET is_active = false WHERE id = $1";
        bool result = conn->executeWithParams(query, {sessionId});
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual std::vector<json> getAllSessions() override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        std::string query = "SELECT * FROM sessions WHERE is_active = true ORDER BY start_time ASC";
        auto results = conn->executeQueryWithParams(query, {});
        connectionPool->releaseConnection(conn);

        std::vector<json> sessions;
        for (const auto& row : results) {
            try {
                sessions.push_back(json{
                    {"id", row.at("id")},
                    {"title", row.at("title")},
                    {"description", row.at("description")},
                    {"startTime", row.at("start_time")},
                    {"endTime", row.at("end_time")},
                    {"location", row.at("location")},
                    {"capacity", std::stoi(row.at("capacity").get<std::string>())},
                    {"chairId", row.at("chair_id")},
                    {"createdAt", row.at("created_at")}
                });
            } catch (...) {
                logger->error("Error parsing session data for row: {}", row.dump());
            }
        }
        return sessions;
    }

    virtual bool assignPaperToSession(const std::string& sessionPaperId,
                                      const std::string& sessionId,
                                      const std::string& submissionId,
                                      int presentationOrder) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query =
            "INSERT INTO session_papers (id, session_id, submission_id, presentation_order) "
            "VALUES ($1, $2, $3, $4)";

        std::vector<std::string> params = {sessionPaperId, sessionId, submissionId, std::to_string(presentationOrder)};
        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual bool removePaperFromSession(const std::string& sessionId,
                                        const std::string& submissionId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "DELETE FROM session_papers WHERE session_id = $1 AND submission_id = $2";
        bool result = conn->executeWithParams(query, {sessionId, submissionId});
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual std::vector<json> getPapersForSession(const std::string& sessionId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        std::string query =
            "SELECT sp.*, s.title as submission_title, s.status as submission_status "
            "FROM session_papers sp JOIN submissions s ON sp.submission_id = s.id "
            "WHERE sp.session_id = $1 ORDER BY sp.presentation_order ASC";

        auto results = conn->executeQueryWithParams(query, {sessionId});
        connectionPool->releaseConnection(conn);

        std::vector<json> papers;
        for (const auto& row : results) {
            try {
                papers.push_back(json{
                    {"id", row.at("id")},
                    {"sessionId", row.at("session_id")},
                    {"submissionId", row.at("submission_id")},
                    {"submissionTitle", row.at("submission_title")},
                    {"submissionStatus", row.at("submission_status")},
                    {"presentationOrder", std::stoi(row.at("presentation_order").get<std::string>())}
                });
            } catch (...) {
                logger->error("Error parsing paper data for session {}", sessionId);
            }
        }
        return papers;
    }

    virtual int getCurrentParticipantCount(const std::string& sessionId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return 0;

        std::string query = "SELECT COUNT(*) as cnt FROM session_participants WHERE session_id = $1";
        auto results = conn->executeQueryWithParams(query, {sessionId});
        connectionPool->releaseConnection(conn);

        if (results.empty()) return 0;
        return std::stoi(results[0]["cnt"].get<std::string>());
    }

    virtual std::vector<json> getSessionAnalytics() override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        std::string query = 
            "SELECT s.id, s.title, s.capacity, "
            "(SELECT COUNT(*) FROM session_participants sp WHERE sp.session_id = s.id) as participant_count, "
            "(SELECT COUNT(*) FROM session_papers spap WHERE spap.session_id = s.id) as paper_count, "
            "(SELECT AVG(rating) FROM feedback f WHERE f.session_id = s.id) as avg_rating "
            "FROM sessions s WHERE s.is_active = true";
        
        auto results = conn->executeQueryWithParams(query, {});
        connectionPool->releaseConnection(conn);

        std::vector<json> analytics;
        for (const auto& row : results) {
            try {
                analytics.push_back(json{
                    {"sessionId", row.at("id")},
                    {"title", row.at("title")},
                    {"capacity", std::stoi(row.at("capacity").get<std::string>())},
                    {"participantCount", std::stoi(row.at("participant_count").get<std::string>())},
                    {"paperCount", std::stoi(row.at("paper_count").get<std::string>())},
                    {"avgRating", row.at("avg_rating").is_null() ? 0.0 : std::stod(row.at("avg_rating").get<std::string>())}
                });
            } catch (const std::exception& e) {
                logger->error("Error parsing session analytics: {}", e.what());
            }
        }
        return analytics;
    }
};

} // namespace repositories
} // namespace conference

std::shared_ptr<conference::repositories::ISessionRepository>
createSessionRepository(std::shared_ptr<conference::database::ConnectionPool> pool) {
    return std::make_shared<conference::repositories::SessionRepository>(pool);
}
