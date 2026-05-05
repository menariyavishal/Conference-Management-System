#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace services {

/**
 * @interface INotificationService
 * @brief Contract for in-app notification management (FR9.2)
 */
class INotificationService {
public:
    virtual ~INotificationService() = default;

    virtual bool createNotification(const std::string& userId,
                                    const std::string& type,
                                    const std::string& message,
                                    const std::string& relatedId = "") = 0;

    virtual json getUserNotifications(const std::string& userId) = 0;
    virtual json getUnreadNotifications(const std::string& userId) = 0;
    virtual json markAsRead(const std::string& notificationId, const std::string& userId) = 0;
    virtual json markAllAsRead(const std::string& userId) = 0;

    virtual std::string getLastError() const = 0;
};

} // namespace services

// Forward declarations for repositories
namespace repositories {
    class INotificationRepository;
}

// Forward declarations for utils
namespace utils {
    class TaskQueue;
}

} // namespace conference

// Factory function
std::shared_ptr<conference::services::INotificationService> 
createNotificationService(
    std::shared_ptr<conference::repositories::INotificationRepository> notificationRepo,
    std::shared_ptr<conference::utils::TaskQueue> taskQueue
);
