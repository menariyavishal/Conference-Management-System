#include "repositories/INotificationRepository.h"
#include "database/ConnectionPool.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace conference {
namespace repositories {

class NotificationRepository : public INotificationRepository {
private:
    std::shared_ptr<database::ConnectionPool> connectionPool;
    std::shared_ptr<spdlog::logger> logger;

public:
    NotificationRepository(std::shared_ptr<database::ConnectionPool> pool) : connectionPool(pool) {
        logger = spdlog::get("notification_repo");
        if (!logger) {
            try {
                spdlog::sink_ptr sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                logger = std::make_shared<spdlog::logger>("notification_repo", sink);
                spdlog::register_logger(logger);
            } catch (...) {
                logger = spdlog::get("conference");
            }
        }
    }

    virtual bool insertNotification(const json& notification) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query =
            "INSERT INTO notifications (id, user_id, type, message, is_read, related_id) "
            "VALUES ($1, $2, $3, $4, $5, $6)";

        std::vector<std::string> params = {
            notification["id"].get<std::string>(),
            notification["userId"].get<std::string>(),
            notification["type"].get<std::string>(),
            notification["message"].get<std::string>(),
            "false",
            notification.value("relatedId", "")
        };

        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual std::vector<json> getNotificationsForUser(const std::string& userId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        std::string query =
            "SELECT * FROM notifications WHERE user_id = $1 ORDER BY created_at DESC LIMIT 50";

        auto results = conn->executeQueryWithParams(query, {userId});
        connectionPool->releaseConnection(conn);

        std::vector<json> items;
        for (const auto& row : results) {
            items.push_back({
                {"id", row.at("id")},
                {"userId", row.at("user_id")},
                {"type", row.at("type")},
                {"message", row.at("message")},
                {"isRead", row.at("is_read") == "t" || row.at("is_read") == "true"},
                {"relatedId", row.at("related_id")},
                {"createdAt", row.at("created_at")}
            });
        }
        return items;
    }

    virtual std::vector<json> getUnreadNotificationsForUser(const std::string& userId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        std::string query =
            "SELECT * FROM notifications WHERE user_id = $1 AND is_read = false ORDER BY created_at DESC";

        auto results = conn->executeQueryWithParams(query, {userId});
        connectionPool->releaseConnection(conn);

        std::vector<json> items;
        for (const auto& row : results) {
            items.push_back({
                {"id", row.at("id")},
                {"type", row.at("type")},
                {"message", row.at("message")},
                {"relatedId", row.at("related_id")},
                {"createdAt", row.at("created_at")}
            });
        }
        return items;
    }

    virtual bool markAsRead(const std::string& notificationId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "UPDATE notifications SET is_read = true WHERE id = $1";
        bool result = conn->executeWithParams(query, {notificationId});
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual bool markAllAsReadForUser(const std::string& userId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "UPDATE notifications SET is_read = true WHERE user_id = $1";
        bool result = conn->executeWithParams(query, {userId});
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual int getUnreadCount(const std::string& userId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return 0;

        std::string query =
            "SELECT COUNT(*) as cnt FROM notifications WHERE user_id = $1 AND is_read = false";

        auto results = conn->executeQueryWithParams(query, {userId});
        connectionPool->releaseConnection(conn);

        if (results.empty()) return 0;
        return std::stoi(results[0]["cnt"].get<std::string>());
    }
};

} // namespace repositories
} // namespace conference

std::shared_ptr<conference::repositories::INotificationRepository>
createNotificationRepository(std::shared_ptr<conference::database::ConnectionPool> pool) {
    return std::make_shared<conference::repositories::NotificationRepository>(pool);
}
