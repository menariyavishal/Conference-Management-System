#ifndef I_REVIEW_REPOSITORY_H
#define I_REVIEW_REPOSITORY_H

#include "database/ConnectionPool.h"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace conference {
namespace repositories {

/**
 * @class IReviewRepository
 * @brief Interface for review database operations
 */
class IReviewRepository {
public:
  virtual ~IReviewRepository() = default;

  virtual bool insertReview(const json &review) = 0;
  virtual std::optional<json> getReviewById(const std::string &reviewId) = 0;
  virtual std::optional<json>
  getReviewBySubmissionAndReviewer(const std::string &submissionId,
                                   const std::string &reviewerId) = 0;

  virtual bool updateReviewScoreAndComments(const std::string &reviewId,
                                            int score,
                                            const std::string &comments) = 0;
  virtual bool updateStatus(const std::string &reviewId,
                            const std::string &status) = 0;

  virtual std::vector<json>
  getReviewsBySubmission(const std::string &submissionId) = 0;
  virtual std::vector<json>
  getReviewsByReviewer(const std::string &reviewerId) = 0;
  
  /**
   * @brief Get global review statistics (total reviews, average score)
   */
  virtual json getGlobalReviewStats() = 0;

  /**
   * @brief Get statistics for reviewers (reviews completed, average score given)
   */
  virtual std::vector<json> getReviewerPerformanceStats() = 0;
};

} // namespace repositories
} // namespace conference

// Factory function
std::shared_ptr<conference::repositories::IReviewRepository>
createReviewRepository(
    std::shared_ptr<conference::database::ConnectionPool> pool);

#endif // I_REVIEW_REPOSITORY_H
