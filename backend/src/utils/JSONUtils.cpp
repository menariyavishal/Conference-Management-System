#include "utils/JSONUtils.h"
#include <iostream>
#include <fstream>

namespace conference {
namespace utils {

// Parse JSON
json JSONUtils::parseJSON(const std::string& jsonString) {
    try {
        return json::parse(jsonString);
    } catch (...) {
        return json::object();
    }
}

bool JSONUtils::tryParseJSON(const std::string& jsonString, json& result) {
    try {
        result = json::parse(jsonString);
        return true;
    } catch (...) {
        return false;
    }
}

json JSONUtils::parseJSONFile(const std::string& filePath) {
    try {
        std::ifstream file(filePath);
        if (file.is_open()) {
            json j;
            file >> j;
            return j;
        }
    } catch (...) {}
    return json::object();
}

// Convert to JSON
std::string JSONUtils::toJSONString(const json& j, bool pretty) {
    return pretty ? j.dump(4) : j.dump();
}

std::string JSONUtils::toPrettyJSONString(const json& j) {
    return j.dump(4);
}

std::string JSONUtils::toCompactJSONString(const json& j) {
    return j.dump();
}

// JSON manipulation
json JSONUtils::mergeJSON(const json& j1, const json& j2) {
    json merged = j1;
    if (j2.is_object()) {
        for (auto it = j2.begin(); it != j2.end(); ++it) {
            merged[it.key()] = it.value();
        }
    }
    return merged;
}

json JSONUtils::filterJSON(const json& j, const std::vector<std::string>& keys) {
    json filtered = json::object();
    if (j.is_object()) {
        for (const auto& key : keys) {
            if (j.contains(key)) {
                filtered[key] = j[key];
            }
        }
    }
    return filtered;
}

json JSONUtils::removeKeys(const json& j, const std::vector<std::string>& keys) {
    json result = j;
    if (result.is_object()) {
        for (const auto& key : keys) {
            result.erase(key);
        }
    }
    return result;
}

// JSON validation
bool JSONUtils::isValidJSON(const std::string& jsonString) {
    try {
        json::parse(jsonString);
        return true;
    } catch (...) {
        return false;
    }
}

bool JSONUtils::hasKey(const json& j, const std::string& key) {
    return j.is_object() && j.contains(key);
}

bool JSONUtils::hasKeys(const json& j, const std::vector<std::string>& keys) {
    if (!j.is_object()) return false;
    for (const auto& key : keys) {
        if (!j.contains(key)) return false;
    }
    return true;
}

// JSON type checking
bool JSONUtils::isNull(const json& j) { return j.is_null(); }
bool JSONUtils::isBoolean(const json& j) { return j.is_boolean(); }
bool JSONUtils::isNumber(const json& j) { return j.is_number(); }
bool JSONUtils::isInteger(const json& j) { return j.is_number_integer(); }
bool JSONUtils::isFloat(const json& j) { return j.is_number_float(); }
bool JSONUtils::isString(const json& j) { return j.is_string(); }
bool JSONUtils::isArray(const json& j) { return j.is_array(); }
bool JSONUtils::isObject(const json& j) { return j.is_object(); }

// JSON value extraction with defaults
std::string JSONUtils::getString(const json& j, const std::string& key, const std::string& defaultValue) {
    if (j.is_object() && j.contains(key) && j[key].is_string()) return j[key].get<std::string>();
    return defaultValue;
}

int JSONUtils::getInt(const json& j, const std::string& key, int defaultValue) {
    if (j.is_object() && j.contains(key) && j[key].is_number_integer()) return j[key].get<int>();
    return defaultValue;
}

double JSONUtils::getDouble(const json& j, const std::string& key, double defaultValue) {
    if (j.is_object() && j.contains(key) && j[key].is_number()) return j[key].get<double>();
    return defaultValue;
}

bool JSONUtils::getBoolean(const json& j, const std::string& key, bool defaultValue) {
    if (j.is_object() && j.contains(key) && j[key].is_boolean()) return j[key].get<bool>();
    return defaultValue;
}

json JSONUtils::getArray(const json& j, const std::string& key, const json& defaultValue) {
    if (j.is_object() && j.contains(key) && j[key].is_array()) return j[key];
    return defaultValue;
}

json JSONUtils::getObject(const json& j, const std::string& key, const json& defaultValue) {
    if (j.is_object() && j.contains(key) && j[key].is_object()) return j[key];
    return defaultValue;
}

// Array operations
int JSONUtils::getArraySize(const json& j) {
    return j.is_array() ? static_cast<int>(j.size()) : 0;
}

json JSONUtils::getArrayElement(const json& array, int index) {
    if (array.is_array() && index >= 0 && index < array.size()) return array[index];
    return json();
}

bool JSONUtils::appendToArray(json& array, const json& element) {
    if (array.is_array()) {
        array.push_back(element);
        return true;
    }
    return false;
}

bool JSONUtils::removeFromArray(json& array, int index) {
    if (array.is_array() && index >= 0 && index < array.size()) {
        array.erase(index);
        return true;
    }
    return false;
}

json JSONUtils::filterArray(const json& array, const std::string& key, const std::string& value) {
    json result = json::array();
    if (array.is_array()) {
        for (const auto& item : array) {
            if (item.is_object() && item.contains(key) && item[key] == value) {
                result.push_back(item);
            }
        }
    }
    return result;
}

// Response building
json JSONUtils::buildSuccessResponse(const json& data, const std::string& message) {
    return {
        {"status", "success"},
        {"message", message},
        {"data", data}
    };
}

json JSONUtils::buildErrorResponse(const std::string& errorCode, const std::string& message, const json& details) {
    json response = {
        {"status", "error"},
        {"error", {
            {"code", errorCode},
            {"message", message}
        }}
    };
    if (!details.is_null()) {
        response["error"]["details"] = details;
    }
    return response;
}

json JSONUtils::buildPaginatedResponse(const json& data, int page, int pageSize, int totalItems) {
    return {
        {"status", "success"},
        {"data", data},
        {"pagination", {
            {"page", page},
            {"pageSize", pageSize},
            {"totalItems", totalItems},
            {"totalPages", (totalItems + pageSize - 1) / pageSize}
        }}
    };
}

json JSONUtils::buildListResponse(const json& data, const std::string& message) {
    return buildSuccessResponse(data, message);
}

// JSON file operations
bool JSONUtils::saveJSONToFile(const std::string& filePath, const json& j, bool pretty) {
    try {
        std::ofstream file(filePath);
        if (file.is_open()) {
            file << (pretty ? j.dump(4) : j.dump());
            return true;
        }
    } catch (...) {}
    return false;
}

bool JSONUtils::loadJSONFromFile(const std::string& filePath, json& result) {
    try {
        std::ifstream file(filePath);
        if (file.is_open()) {
            file >> result;
            return true;
        }
    } catch (...) {}
    return false;
}

bool JSONUtils::appendJSONToFile(const std::string& filePath, const json& j) {
    json existing = parseJSONFile(filePath);
    if (existing.is_array()) {
        existing.push_back(j);
        return saveJSONToFile(filePath, existing);
    } else if (existing.is_object() || existing.is_null()) {
        json arr = json::array();
        if (!existing.is_null()) arr.push_back(existing);
        arr.push_back(j);
        return saveJSONToFile(filePath, arr);
    }
    return false;
}

bool JSONUtils::validateAgainstSchema(const json& j, const json& schema) {
    // Basic placeholder implementation since nlohmann::json doesn't have built-in schema validation
    return j.is_object() && schema.is_object();
}

// Common response wrappers
json JSONUtils::respondSuccess(const json& data) {
    return buildSuccessResponse(data);
}

json JSONUtils::respondCreated(const json& data) {
    return buildSuccessResponse(data, "Resource created successfully");
}

json JSONUtils::respondNotFound(const std::string& message) {
    return buildErrorResponse("NOT_FOUND", message);
}

json JSONUtils::respondUnauthorized(const std::string& message) {
    return buildErrorResponse("UNAUTHORIZED", message);
}

json JSONUtils::respondForbidden(const std::string& message) {
    return buildErrorResponse("FORBIDDEN", message);
}

json JSONUtils::respondBadRequest(const std::string& message, const json& details) {
    return buildErrorResponse("BAD_REQUEST", message, details);
}

json JSONUtils::respondInternalError(const std::string& message) {
    return buildErrorResponse("INTERNAL_ERROR", message);
}

json JSONUtils::createErrorObject(const std::string& code, const std::string& message, const json& details) {
    json err = {
        {"code", code},
        {"message", message}
    };
    if (!details.is_null()) err["details"] = details;
    return err;
}

}  // namespace utils
}  // namespace conference
