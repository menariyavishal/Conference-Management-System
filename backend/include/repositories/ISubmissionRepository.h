#ifndef I_SUBMISSION_REPOSITORY_H
#define I_SUBMISSION_REPOSITORY_H

#include "database/ConnectionPool.h"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace conference {
namespace repositories {

/**
 * @class ISubmissionRepository
 * @brief Interface for submission database operations
 */
class ISubmissionRepository {
public:
  virtual ~ISubmissionRepository() = default;

  virtual bool insertSubmission(const json &submission) = 0;
  virtual std::optional<json>
  getSubmissionById(const std::string &submissionId) = 0;
  virtual bool updateSubmission(const std::string &submissionId,
                                const json &updateData) = 0;
  virtual bool updateStatus(const std::string &submissionId,
                            const std::string &status) = 0;
  virtual std::vector<json>
  getSubmissionsByAuthor(const std::string &authorId) = 0;
  virtual std::vector<json> getSubmissions(int limit, int offset,
                                           const std::string &statusFilter,
                                           const std::string &searchQuery) = 0;
  virtual int getSubmissionCount(const std::string &statusFilter,
                                 const std::string &searchQuery) = 0;
  
  /**
   * @brief Get counts of submissions grouped by status
   */
  virtual json getStatusStatistics() = 0;
};

} // namespace repositories
} // namespace conference

// Factory function
std::shared_ptr<conference::repositories::ISubmissionRepository>
createSubmissionRepository(
    std::shared_ptr<conference::database::ConnectionPool> pool);

#endif // I_SUBMISSION_REPOSITORY_H
