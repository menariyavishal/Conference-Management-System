#include "services/INotificationService.h"
#include "repositories/INotificationRepository.h"
#include "utils/TaskQueue.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using json = nlohmann::json;

namespace conference {
namespace services {

class NotificationService : public INotificationService {
private:
    std::string lastError;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<repositories::INotificationRepository> notificationRepository;
    std::shared_ptr<utils::TaskQueue> taskQueue;

    std::string generateId() {
        auto now = std::chrono::system_clock::now().time_since_epoch().count();
        return "notif_" + std::to_string(now);
    }

public:
    NotificationService(
        std::shared_ptr<repositories::INotificationRepository> notificationRepo,
        std::shared_ptr<utils::TaskQueue> queue
    ) : notificationRepository(notificationRepo), taskQueue(queue) {
        logger = spdlog::get("notification_service");
        if (!logger) {
            try {
                spdlog::sink_ptr sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                logger = std::make_shared<spdlog::logger>("notification_service", sink);
                spdlog::register_logger(logger);
            } catch (...) {
                logger = spdlog::get("conference");
            }
        }
    }

    virtual bool createNotification(const std::string& userId,
                                    const std::string& type,
                                    const std::string& message,
                                    const std::string& relatedId) override {
        try {
            std::string notifId = generateId();
            json notif = {
                {"id", notifId},
                {"userId", userId},
                {"type", type},
                {"message", message},
                {"relatedId", relatedId}
            };

            // Database insertion is synchronous to ensure record existence
            bool result = notificationRepository->insertNotification(notif);
            
            if (result && taskQueue) {
                // Background "sending" simulation
                taskQueue->enqueue([this, userId, type, message]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate latency
                    logger->info("ASYNC NOTIFICATION SENT to User {}: [{}] {}", userId, type, message);
                });
            }

            return result;

        } catch (const std::exception& ex) {
            lastError = std::string("Create notification failed: ") + ex.what();
            logger->error(lastError);
            return false;
        }
    }

    virtual json getUserNotifications(const std::string& userId) override {
        try {
            auto notifications = notificationRepository->getNotificationsForUser(userId);
            int unreadCount = notificationRepository->getUnreadCount(userId);
            return {
                {"success", true},
                {"data", notifications},
                {"unreadCount", unreadCount}
            };
        } catch (const std::exception& ex) {
            lastError = std::string("Get notifications failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getUnreadNotifications(const std::string& userId) override {
        try {
            auto notifications = notificationRepository->getUnreadNotificationsForUser(userId);
            return {{"success", true}, {"data", notifications}, {"count", notifications.size()}};
        } catch (const std::exception& ex) {
            lastError = std::string("Get unread notifications failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json markAsRead(const std::string& notificationId, const std::string& userId) override {
        try {
            (void)userId;
            if (!notificationRepository->markAsRead(notificationId)) {
                lastError = "Failed to mark notification as read";
                return {{"success", false}, {"error", lastError}};
            }
            return {{"success", true}, {"message", "Notification marked as read"}};
        } catch (const std::exception& ex) {
            lastError = std::string("Mark as read failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json markAllAsRead(const std::string& userId) override {
        try {
            if (!notificationRepository->markAllAsReadForUser(userId)) {
                lastError = "Failed to mark all notifications as read";
                return {{"success", false}, {"error", lastError}};
            }
            logger->info("All notifications marked as read for user: {}", userId);
            return {{"success", true}, {"message", "All notifications marked as read"}};
        } catch (const std::exception& ex) {
            lastError = std::string("Mark all as read failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual std::string getLastError() const override { return lastError; }
};

} // namespace services
} // namespace conference

std::shared_ptr<conference::services::INotificationService>
createNotificationService(
    std::shared_ptr<conference::repositories::INotificationRepository> notificationRepo,
    std::shared_ptr<conference::utils::TaskQueue> taskQueue
) {
    return std::make_shared<conference::services::NotificationService>(notificationRepo, taskQueue);
}
