#include "controllers/ConfigController.h"
#include "controllers/ControllerHelpers.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace controllers {

ConfigController::ConfigController(std::shared_ptr<services::IConfigService> service)
    : configService(service) {}

void ConfigController::registerRoutes(httplib::Server& svr) {
    svr.Get("/api/config", [this](const httplib::Request& req, httplib::Response& res) {
        handleGetAllConfigs(req, res);
    });

    svr.Get("/api/config/:key", [this](const httplib::Request& req, httplib::Response& res) {
        handleGetConfig(req, res);
    });

    svr.Put("/api/config/:key", [this](const httplib::Request& req, httplib::Response& res) {
        handleUpdateConfig(req, res);
    });
}

void ConfigController::handleGetAllConfigs(const httplib::Request& req, httplib::Response& res) {
    auto result = configService->getAllSettings();
    res.set_content(result["data"].dump(), "application/json");
}

void ConfigController::handleGetConfig(const httplib::Request& req, httplib::Response& res) {
    std::string key = req.path_params.at("key");
    std::string value = configService->getString(key, "");
    
    if (value.empty()) {
        res.status = 404;
        res.set_content(json({{"error", "Config key not found"}}).dump(), "application/json");
    } else {
        res.set_content(json({{"key", key}, {"value", value}}).dump(), "application/json");
    }
}

void ConfigController::handleUpdateConfig(const httplib::Request& req, httplib::Response& res) {
    std::string key = req.path_params.at("key");
    
    try {
        auto body = json::parse(req.body);
        if (!body.contains("value")) {
            res.status = 400;
            res.set_content(json({{"error", "Missing 'value' field"}}).dump(), "application/json");
            return;
        }

        std::string value = body["value"].is_string() ? body["value"].get<std::string>() : body["value"].dump();
        
        if (configService->updateConfig(key, value)) {
            res.set_content(json({{"success", true}, {"key", key}, {"value", value}}).dump(), "application/json");
        } else {
            res.status = 500;
            res.set_content(json({{"error", configService->getLastError()}}).dump(), "application/json");
        }
    } catch (const std::exception& ex) {
        res.status = 400;
        res.set_content(json({{"error", "Invalid JSON body"}}).dump(), "application/json");
    }
}

} // namespace controllers
} // namespace conference
