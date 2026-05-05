#pragma once
#include "httplib.h"
#include "services/IConfigService.h"
#include <memory>

namespace conference {
namespace controllers {

/**
 * @class ConfigController
 * @brief Handles API requests for system configuration
 */
class ConfigController {
private:
    std::shared_ptr<services::IConfigService> configService;

public:
    ConfigController(std::shared_ptr<services::IConfigService> service);

    /**
     * @brief Register routes with the HTTP server
     */
    void registerRoutes(httplib::Server& svr);

private:
    // Handlers
    void handleGetAllConfigs(const httplib::Request& req, httplib::Response& res);
    void handleGetConfig(const httplib::Request& req, httplib::Response& res);
    void handleUpdateConfig(const httplib::Request& req, httplib::Response& res);
};

} // namespace controllers
} // namespace conference
