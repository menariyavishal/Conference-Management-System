#pragma once
#include "database/ConnectionPool.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace conference {
namespace repositories {

/**
 * @interface INotificationRepository
 * @brief Data access contract for in-app notifications
 */
class INotificationRepository {
public:
  virtual ~INotificationRepository() = default;

  virtual bool insertNotification(const json &notification) = 0;
  virtual std::vector<json>
  getNotificationsForUser(const std::string &userId) = 0;
  virtual std::vector<json>
  getUnreadNotificationsForUser(const std::string &userId) = 0;
  virtual bool markAsRead(const std::string &notificationId) = 0;
  virtual bool markAllAsReadForUser(const std::string &userId) = 0;
  virtual int getUnreadCount(const std::string &userId) = 0;
};

} // namespace repositories
} // namespace conference

// Factory function
std::shared_ptr<conference::repositories::INotificationRepository>
createNotificationRepository(
    std::shared_ptr<conference::database::ConnectionPool> pool);
