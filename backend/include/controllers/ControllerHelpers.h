#pragma once
#include <nlohmann/json.hpp>
#include "httplib.h"
#include <string>

using json = nlohmann::json;

namespace conference {
namespace controllers {

/**
 * @brief Shared helper functions for all controllers
 */

inline void sendJson(httplib::Response& res, int statusCode, const json& body) {
    res.status = statusCode;
    res.set_header("Content-Type", "application/json");
    res.body = body.dump();
}

inline void sendSuccess(httplib::Response& res, const json& data, const std::string& message = "Success", int statusCode = 200) {
    json response = {{"success", true}, {"data", data}, {"message", message}};
    sendJson(res, statusCode, response);
}

inline void sendError(httplib::Response& res, int statusCode, const std::string& message, const std::string& code = "") {
    json response = {{"success", false}, {"error", {{"message", message}, {"code", code.empty() ? "ERROR" : code}}}};
    sendJson(res, statusCode, response);
}

inline json parseBody(const httplib::Request& req) {
    try {
        if (req.body.empty()) return json::object();
        return json::parse(req.body);
    } catch (...) {
        return json::object();
    }
}

inline std::string extractBearerToken(const httplib::Request& req) {
    auto it = req.headers.find("Authorization");
    if (it == req.headers.end()) return "";
    std::string auth = it->second;
    if (auth.substr(0, 7) == "Bearer ") return auth.substr(7);
    return "";
}

} // namespace controllers
} // namespace conference
