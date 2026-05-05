#pragma once
#include "database/ConnectionPool.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <optional>

using json = nlohmann::json;

namespace conference {
namespace repositories {

/**
 * @interface IConfigRepository
 * @brief Data access for system configuration settings
 */
class IConfigRepository {
public:
    virtual ~IConfigRepository() = default;

    /**
     * @brief Get a configuration value by key
     */
    virtual std::optional<std::string> getConfigValue(const std::string& key) = 0;

    /**
     * @brief Set or update a configuration value
     */
    virtual bool setConfigValue(const std::string& key, const std::string& value) = 0;

    /**
     * @brief Get all configuration settings
     */
    virtual std::vector<json> getAllConfigs() = 0;

    /**
     * @brief Delete a configuration setting
     */
    virtual bool deleteConfig(const std::string& key) = 0;
};

} // namespace repositories
} // namespace conference

// Factory function
std::shared_ptr<conference::repositories::IConfigRepository> 
createConfigRepository(std::shared_ptr<conference::database::ConnectionPool> pool);
