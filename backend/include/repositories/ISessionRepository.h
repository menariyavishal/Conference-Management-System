#pragma once
#include "database/ConnectionPool.h"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace conference {
namespace repositories {

/**
 * @interface ISessionRepository
 * @brief Data access contract for conference session management
 */
class ISessionRepository {
public:
  virtual ~ISessionRepository() = default;

  // Core CRUD
  virtual bool insertSession(const json &session) = 0;
  virtual std::optional<json> getSessionById(const std::string &sessionId) = 0;
  virtual bool updateSession(const std::string &sessionId,
                             const json &updates) = 0;
  virtual bool deleteSession(const std::string &sessionId) = 0;

  // Listing
  virtual std::vector<json> getAllSessions() = 0;

  // Paper assignment
  virtual bool assignPaperToSession(const std::string &sessionPaperId,
                                    const std::string &sessionId,
                                    const std::string &submissionId,
                                    int presentationOrder) = 0;
  virtual bool removePaperFromSession(const std::string &sessionId,
                                      const std::string &submissionId) = 0;
  virtual std::vector<json>
  getPapersForSession(const std::string &sessionId) = 0;

  // Capacity helpers
  virtual int getCurrentParticipantCount(const std::string &sessionId) = 0;

  /**
   * @brief Get aggregated analytics for all sessions (paper counts, participant counts)
   */
  virtual std::vector<json> getSessionAnalytics() = 0;
};

} // namespace repositories
} // namespace conference

// Factory function
std::shared_ptr<conference::repositories::ISessionRepository>
createSessionRepository(
    std::shared_ptr<conference::database::ConnectionPool> pool);
