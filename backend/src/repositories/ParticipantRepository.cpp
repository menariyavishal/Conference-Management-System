#include "repositories/IParticipantRepository.h"
#include "database/ConnectionPool.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace conference {
namespace repositories {

class ParticipantRepository : public IParticipantRepository {
private:
    std::shared_ptr<database::ConnectionPool> connectionPool;
    std::shared_ptr<spdlog::logger> logger;

public:
    ParticipantRepository(std::shared_ptr<database::ConnectionPool> pool) : connectionPool(pool) {
        logger = spdlog::get("participant_repo");
        if (!logger) {
            try {
                spdlog::sink_ptr sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                logger = std::make_shared<spdlog::logger>("participant_repo", sink);
                spdlog::register_logger(logger);
            } catch (...) {
                logger = spdlog::get("conference");
            }
        }
    }

    virtual bool registerParticipant(const std::string& recordId,
                                     const std::string& sessionId,
                                     const std::string& userId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query =
            "INSERT INTO session_participants (id, session_id, user_id) VALUES ($1, $2, $3)";

        bool result = conn->executeWithParams(query, {recordId, sessionId, userId});
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual bool deregisterParticipant(const std::string& sessionId,
                                       const std::string& userId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query =
            "DELETE FROM session_participants WHERE session_id = $1 AND user_id = $2";

        bool result = conn->executeWithParams(query, {sessionId, userId});
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual bool isRegistered(const std::string& sessionId,
                               const std::string& userId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query =
            "SELECT COUNT(*) as cnt FROM session_participants WHERE session_id = $1 AND user_id = $2";

        auto results = conn->executeQueryWithParams(query, {sessionId, userId});
        connectionPool->releaseConnection(conn);

        if (results.empty()) return false;
        return std::stoi(results[0]["cnt"].get<std::string>()) > 0;
    }

    virtual std::vector<json> getParticipantsForSession(const std::string& sessionId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        std::string query =
            "SELECT sp.*, u.email, u.full_name FROM session_participants sp "
            "JOIN users u ON sp.user_id = u.id "
            "WHERE sp.session_id = $1 ORDER BY sp.registered_at ASC";

        auto results = conn->executeQueryWithParams(query, {sessionId});
        connectionPool->releaseConnection(conn);

        std::vector<json> participants;
        for (const auto& row : results) {
            participants.push_back({
                {"id", row.at("id")},
                {"sessionId", row.at("session_id")},
                {"userId", row.at("user_id")},
                {"email", row.at("email")},
                {"fullName", row.at("full_name")},
                {"registeredAt", row.at("registered_at")},
                {"attended", row.at("attended") == "t" || row.at("attended") == "true"}
            });
        }
        return participants;
    }

    virtual std::vector<json> getSessionsForUser(const std::string& userId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        std::string query =
            "SELECT sp.*, s.title, s.start_time, s.end_time, s.location FROM session_participants sp "
            "JOIN sessions s ON sp.session_id = s.id "
            "WHERE sp.user_id = $1 ORDER BY s.start_time ASC";

        auto results = conn->executeQueryWithParams(query, {userId});
        connectionPool->releaseConnection(conn);

        std::vector<json> sessions;
        for (const auto& row : results) {
            sessions.push_back({
                {"sessionId", row.at("session_id")},
                {"title", row.at("title")},
                {"startTime", row.at("start_time")},
                {"endTime", row.at("end_time")},
                {"location", row.at("location")},
                {"registeredAt", row.at("registered_at")},
                {"attended", row.at("attended") == "t" || row.at("attended") == "true"}
            });
        }
        return sessions;
    }

    virtual bool markAttendance(const std::string& sessionId,
                                const std::string& userId,
                                bool attended) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query =
            "UPDATE session_participants SET attended = $1 WHERE session_id = $2 AND user_id = $3";

        bool result = conn->executeWithParams(query, {attended ? "true" : "false", sessionId, userId});
        connectionPool->releaseConnection(conn);
        return result;
    }
};

} // namespace repositories
} // namespace conference

std::shared_ptr<conference::repositories::IParticipantRepository>
createParticipantRepository(std::shared_ptr<conference::database::ConnectionPool> pool) {
    return std::make_shared<conference::repositories::ParticipantRepository>(pool);
}
