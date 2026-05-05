#ifndef I_FILE_REPOSITORY_H
#define I_FILE_REPOSITORY_H

#include "database/ConnectionPool.h"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace conference {
namespace repositories {

/**
 * @class IFileRepository
 * @brief Interface for file database operations
 */
class IFileRepository {
public:
  virtual ~IFileRepository() = default;

  virtual bool insertFileMetadata(const json &metadata) = 0;
  virtual std::optional<json> getFileById(const std::string &fileId) = 0;
  virtual bool deactivateFile(const std::string &fileId) = 0;
  virtual std::vector<json>
  getFilesByContext(const std::string &context,
                    const std::string &uploadedBy = "") = 0;
};

} // namespace repositories
} // namespace conference

// Factory function
std::shared_ptr<conference::repositories::IFileRepository> createFileRepository(
    std::shared_ptr<conference::database::ConnectionPool> pool);

#endif // I_FILE_REPOSITORY_H
