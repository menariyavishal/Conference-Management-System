#include "database/ConnectionPool.h"
#include "repositories/IFeedbackRepository.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace conference {
namespace repositories {

class FeedbackRepository : public IFeedbackRepository {
private:
  std::shared_ptr<database::ConnectionPool> connectionPool;
  std::shared_ptr<spdlog::logger> logger;

public:
  FeedbackRepository(std::shared_ptr<database::ConnectionPool> pool)
      : connectionPool(pool) {
    logger = spdlog::get("feedback_repo");
    if (!logger) {
      try {
        spdlog::sink_ptr sink =
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        logger = std::make_shared<spdlog::logger>("feedback_repo", sink);
        spdlog::register_logger(logger);
      } catch (...) {
        logger = spdlog::get("conference");
      }
    }
  }

  virtual bool insertFeedback(const json &feedback) override {
    auto conn = connectionPool->getConnection();
    if (!conn)
      return false;

    std::string query = "INSERT INTO feedback (id, session_id, participant_id, "
                        "rating, comment, is_anonymous) "
                        "VALUES ($1, $2, $3, $4, $5, $6)";

    std::vector<std::string> params = {
        feedback["id"].get<std::string>(),
        feedback["sessionId"].get<std::string>(),
        feedback["participantId"].get<std::string>(),
        std::to_string(feedback["rating"].get<int>()),
        feedback.value("comment", ""),
        feedback.value("isAnonymous", true) ? "true" : "false"};

    bool result = conn->executeWithParams(query, params);
    connectionPool->releaseConnection(conn);
    return result;
  }

  virtual bool hasSubmittedFeedback(const std::string &sessionId,
                                    const std::string &participantId) override {
    auto conn = connectionPool->getConnection();
    if (!conn)
      return false;

    std::string query = "SELECT COUNT(*) as cnt FROM feedback WHERE session_id "
                        "= $1 AND participant_id = $2";

    auto results =
        conn->executeQueryWithParams(query, {sessionId, participantId});
    connectionPool->releaseConnection(conn);

    if (results.empty())
      return false;
    return std::stoi(results[0]["cnt"].get<std::string>()) > 0;
  }

  virtual std::vector<json>
  getFeedbackForSession(const std::string &sessionId) override {
    auto conn = connectionPool->getConnection();
    if (!conn)
      return {};

    std::string query = "SELECT * FROM feedback WHERE session_id = $1 ORDER BY "
                        "submitted_at DESC";

    auto results = conn->executeQueryWithParams(query, {sessionId});
    connectionPool->releaseConnection(conn);

    std::vector<json> items;
    for (const auto &row : results) {
      bool anon =
          row.at("is_anonymous") == "t" || row.at("is_anonymous") == "true";
      json fb = {{"id", row.at("id")},
                 {"sessionId", row.at("session_id")},
                 {"rating", std::stoi(row.at("rating").get<std::string>())},
                 {"comment", row.at("comment")},
                 {"isAnonymous", anon},
                 {"submittedAt", row.at("submitted_at")}};
      if (!anon)
        fb["participantId"] = row.at("participant_id");
      items.push_back(fb);
    }
    return items;
  }

  virtual json getFeedbackSummary(const std::string &sessionId) override {
    auto conn = connectionPool->getConnection();
    if (!conn)
      return {{"sessionId", sessionId},
              {"averageRating", 0},
              {"totalResponses", 0}};

    std::string query = "SELECT COUNT(*) as total, AVG(rating) as avg_rating "
                        "FROM feedback WHERE session_id = $1";

    auto results = conn->executeQueryWithParams(query, {sessionId});
    connectionPool->releaseConnection(conn);

    if (results.empty()) {
      return {{"sessionId", sessionId},
              {"averageRating", 0.0},
              {"totalResponses", 0}};
    }

    double avg = 0.0;
    int total = 0;
    try {
      std::string avgStr = results[0]["avg_rating"].get<std::string>();
      if (!avgStr.empty() && avgStr != "null")
        avg = std::stod(avgStr);
      total = std::stoi(results[0]["total"].get<std::string>());
    } catch (...) {
    }

    return {{"sessionId", sessionId},
            {"averageRating", avg},
            {"totalResponses", total}};
  }

  virtual std::vector<json> getAllFeedback() override {
    auto conn = connectionPool->getConnection();
    if (!conn)
      return {};

    std::string query = "SELECT f.*, s.title as session_title FROM feedback f "
                        "JOIN sessions s ON f.session_id = s.id "
                        "ORDER BY f.submitted_at DESC";

    auto results = conn->executeQueryWithParams(query, {});
    connectionPool->releaseConnection(conn);

    std::vector<json> items;
    for (const auto &row : results) {
      items.push_back(
          {{"id", row.at("id")},
           {"sessionId", row.at("session_id")},
           {"sessionTitle", row.at("session_title")},
           {"rating", std::stoi(row.at("rating").get<std::string>())},
           {"comment", row.at("comment")},
           {"submittedAt", row.at("submitted_at")}});
    }
    return items;
  }
};

} // namespace repositories
} // namespace conference

std::shared_ptr<conference::repositories::IFeedbackRepository>
createFeedbackRepository(
    std::shared_ptr<conference::database::ConnectionPool> pool) {
  return std::make_shared<conference::repositories::FeedbackRepository>(pool);
}
