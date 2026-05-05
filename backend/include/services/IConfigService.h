#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <memory>
#include <optional>

using json = nlohmann::json;

namespace conference {
namespace services {

/**
 * @interface IConfigService
 * @brief Business logic for system configuration
 */
class IConfigService {
public:
    virtual ~IConfigService() = default;

    /**
     * @brief Get a config value as string
     */
    virtual std::string getString(const std::string& key, const std::string& defaultValue = "") = 0;

    /**
     * @brief Get a config value as boolean
     */
    virtual bool getBool(const std::string& key, bool defaultValue = false) = 0;

    /**
     * @brief Get a config value as integer
     */
    virtual int getInt(const std::string& key, int defaultValue = 0) = 0;

    /**
     * @brief Update a config value
     */
    virtual bool updateConfig(const std::string& key, const std::string& value) = 0;

    /**
     * @brief Get all configurations as a list
     */
    virtual json getAllSettings() = 0;

    virtual std::string getLastError() const = 0;
};

} // namespace services

namespace repositories {
    class IConfigRepository;
}

} // namespace conference

// Factory function
std::shared_ptr<conference::services::IConfigService> 
createConfigService(std::shared_ptr<conference::repositories::IConfigRepository> configRepo);
