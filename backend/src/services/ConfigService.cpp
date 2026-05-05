#include "services/IConfigService.h"
#include "repositories/IConfigRepository.h"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace conference {
namespace services {

class ConfigService : public IConfigService {
private:
    std::shared_ptr<repositories::IConfigRepository> configRepo;
    std::string lastError;

public:
    ConfigService(std::shared_ptr<repositories::IConfigRepository> repo) : configRepo(repo) {}

    virtual std::string getString(const std::string& key, const std::string& defaultValue) override {
        auto val = configRepo->getConfigValue(key);
        return val.value_or(defaultValue);
    }

    virtual bool getBool(const std::string& key, bool defaultValue) override {
        auto val = configRepo->getConfigValue(key);
        if (!val.has_value()) return defaultValue;
        
        std::string s = val.value();
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return (s == "true" || s == "1" || s == "yes" || s == "on");
    }

    virtual int getInt(const std::string& key, int defaultValue) override {
        auto val = configRepo->getConfigValue(key);
        if (!val.has_value()) return defaultValue;
        
        try {
            return std::stoi(val.value());
        } catch (...) {
            return defaultValue;
        }
    }

    virtual bool updateConfig(const std::string& key, const std::string& value) override {
        if (!configRepo->setConfigValue(key, value)) {
            lastError = "Failed to update configuration in database";
            return false;
        }
        return true;
    }

    virtual json getAllSettings() override {
        auto configs = configRepo->getAllConfigs();
        return {{"success", true}, {"data", configs}};
    }

    virtual std::string getLastError() const override { return lastError; }
};

} // namespace services
} // namespace conference

std::shared_ptr<conference::services::IConfigService> 
createConfigService(std::shared_ptr<conference::repositories::IConfigRepository> configRepo) {
    return std::make_shared<conference::services::ConfigService>(configRepo);
}
