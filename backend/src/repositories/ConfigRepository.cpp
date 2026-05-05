#include "repositories/IConfigRepository.h"
#include "database/ConnectionPool.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace conference {
namespace repositories {

class ConfigRepository : public IConfigRepository {
private:
    std::shared_ptr<database::ConnectionPool> connectionPool;
    std::shared_ptr<spdlog::logger> logger;

public:
    ConfigRepository(std::shared_ptr<database::ConnectionPool> pool) : connectionPool(pool) {
        logger = spdlog::get("config_repo");
        if (!logger) {
            try {
                spdlog::sink_ptr sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                logger = std::make_shared<spdlog::logger>("config_repo", sink);
                spdlog::register_logger(logger);
            } catch (...) {
                logger = spdlog::get("conference");
            }
        }
    }

    virtual std::optional<std::string> getConfigValue(const std::string& key) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return std::nullopt;

        std::string query = "SELECT value FROM system_config WHERE key = $1";
        auto results = conn->executeQueryWithParams(query, {key});
        connectionPool->releaseConnection(conn);

        if (results.empty()) return std::nullopt;
        return results[0].at("value").get<std::string>();
    }

    virtual bool setConfigValue(const std::string& key, const std::string& value) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = 
            "INSERT INTO system_config (key, value, updated_at) "
            "VALUES ($1, $2, CURRENT_TIMESTAMP) "
            "ON CONFLICT (key) DO UPDATE SET value = $2, updated_at = CURRENT_TIMESTAMP";

        bool result = conn->executeWithParams(query, {key, value});
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual std::vector<json> getAllConfigs() override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        std::string query = "SELECT * FROM system_config ORDER BY key ASC";
        auto results = conn->executeQueryWithParams(query, {});
        connectionPool->releaseConnection(conn);

        std::vector<json> configs;
        for (const auto& row : results) {
            configs.push_back({
                {"key", row.at("key")},
                {"value", row.at("value")},
                {"description", row.at("description")},
                {"updatedAt", row.at("updated_at")}
            });
        }
        return configs;
    }

    virtual bool deleteConfig(const std::string& key) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "DELETE FROM system_config WHERE key = $1";
        bool result = conn->executeWithParams(query, {key});
        connectionPool->releaseConnection(conn);
        return result;
    }
};

} // namespace repositories
} // namespace conference

std::shared_ptr<conference::repositories::IConfigRepository> 
createConfigRepository(std::shared_ptr<conference::database::ConnectionPool> pool) {
    return std::make_shared<conference::repositories::ConfigRepository>(pool);
}
