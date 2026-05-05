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
 * @interface IParticipantRepository
 * @brief Data access contract for session participant registration
 */
class IParticipantRepository {
public:
  virtual ~IParticipantRepository() = default;

  virtual bool registerParticipant(const std::string &recordId,
                                   const std::string &sessionId,
                                   const std::string &userId) = 0;

  virtual bool deregisterParticipant(const std::string &sessionId,
                                     const std::string &userId) = 0;

  virtual bool isRegistered(const std::string &sessionId,
                            const std::string &userId) = 0;

  virtual std::vector<json>
  getParticipantsForSession(const std::string &sessionId) = 0;
  virtual std::vector<json> getSessionsForUser(const std::string &userId) = 0;

  virtual bool markAttendance(const std::string &sessionId,
                              const std::string &userId, bool attended) = 0;
};

} // namespace repositories
} // namespace conference

// Factory function
std::shared_ptr<conference::repositories::IParticipantRepository>
createParticipantRepository(
    std::shared_ptr<conference::database::ConnectionPool> pool);
