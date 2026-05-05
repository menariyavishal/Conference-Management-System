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
 * @interface IFeedbackRepository
 * @brief Data access contract for session feedback collection
 */
class IFeedbackRepository {
public:
  virtual ~IFeedbackRepository() = default;

  virtual bool insertFeedback(const json &feedback) = 0;
  virtual bool hasSubmittedFeedback(const std::string &sessionId,
                                    const std::string &participantId) = 0;
  virtual std::vector<json>
  getFeedbackForSession(const std::string &sessionId) = 0;

  /**
   * @brief Returns aggregate: {"sessionId", "averageRating", "totalResponses"}
   */
  virtual json getFeedbackSummary(const std::string &sessionId) = 0;

  // For organizer list view
  virtual std::vector<json> getAllFeedback() = 0;
};

} // namespace repositories
} // namespace conference

// Factory function
std::shared_ptr<conference::repositories::IFeedbackRepository>
createFeedbackRepository(
    std::shared_ptr<conference::database::ConnectionPool> pool);
