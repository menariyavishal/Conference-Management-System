#ifndef I_USER_REPOSITORY_H
#define I_USER_REPOSITORY_H

#include "database/ConnectionPool.h"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace conference {
namespace repositories {

/**
 * @class IUserRepository
 * @brief Interface for user database operations
 */
class IUserRepository {
public:
  virtual ~IUserRepository() = default;

  virtual bool insertUser(const json &user) = 0;
  virtual std::optional<json> getUserByEmail(const std::string &email) = 0;
  virtual std::optional<json> getUserById(const std::string &id) = 0;
  virtual bool updateUserProfile(const std::string &userId,
                                 const std::string &fullName,
                                 const std::string &email) = 0;
  virtual bool updatePassword(const std::string &userId,
                              const std::string &newPasswordHash) = 0;
  virtual bool updateRole(const std::string &userId,
                          const std::string &newRole) = 0;
  virtual bool deactivateUser(const std::string &userId) = 0;

  virtual std::vector<json> getUsers(int limit, int offset,
                                     const std::string &roleFilter,
                                     const std::string &searchQuery) = 0;
  virtual int getUserCount(const std::string &roleFilter,
                           const std::string &searchQuery) = 0;
  
  /**
   * @brief Get counts of users grouped by role
   */
  virtual json getUserRoleDistribution() = 0;
};

} // namespace repositories
} // namespace conference

// Factory function
std::shared_ptr<conference::repositories::IUserRepository> createUserRepository(
    std::shared_ptr<conference::database::ConnectionPool> pool);

#endif // I_USER_REPOSITORY_H
