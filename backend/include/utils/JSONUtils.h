#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace utils {

class JSONUtils {
public:
    // Parse JSON
    static json parseJSON(const std::string& jsonString);
    static bool tryParseJSON(const std::string& jsonString, json& result);
    static json parseJSONFile(const std::string& filePath);
    
    // Convert to JSON
    static std::string toJSONString(const json& j, bool pretty = false);
    static std::string toPrettyJSONString(const json& j);
    static std::string toCompactJSONString(const json& j);
    
    // JSON manipulation
    static json mergeJSON(const json& j1, const json& j2);
    static json filterJSON(const json& j, const std::vector<std::string>& keys);
    static json removeKeys(const json& j, const std::vector<std::string>& keys);
    
    // JSON validation
    static bool isValidJSON(const std::string& jsonString);
    static bool hasKey(const json& j, const std::string& key);
    static bool hasKeys(const json& j, const std::vector<std::string>& keys);
    
    // JSON type checking
    static bool isNull(const json& j);
    static bool isBoolean(const json& j);
    static bool isNumber(const json& j);
    static bool isInteger(const json& j);
    static bool isFloat(const json& j);
    static bool isString(const json& j);
    static bool isArray(const json& j);
    static bool isObject(const json& j);
    
    // JSON value extraction with defaults
    static std::string getString(const json& j, const std::string& key, const std::string& defaultValue = "");
    static int getInt(const json& j, const std::string& key, int defaultValue = 0);
    static double getDouble(const json& j, const std::string& key, double defaultValue = 0.0);
    static bool getBoolean(const json& j, const std::string& key, bool defaultValue = false);
    static json getArray(const json& j, const std::string& key, const json& defaultValue = json::array());
    static json getObject(const json& j, const std::string& key, const json& defaultValue = json::object());
    
    // Array operations
    static int getArraySize(const json& j);
    static json getArrayElement(const json& array, int index);
    static bool appendToArray(json& array, const json& element);
    static bool removeFromArray(json& array, int index);
    static json filterArray(const json& array, const std::string& key, const std::string& value);
    
    // Response building
    static json buildSuccessResponse(const json& data, const std::string& message = "Operation successful");
    static json buildErrorResponse(const std::string& errorCode, const std::string& message, const json& details = nullptr);
    static json buildPaginatedResponse(const json& data, int page, int pageSize, int totalItems);
    static json buildListResponse(const json& data, const std::string& message = "Data retrieved successfully");
    
    // JSON file operations
    static bool saveJSONToFile(const std::string& filePath, const json& j, bool pretty = true);
    static bool loadJSONFromFile(const std::string& filePath, json& result);
    static bool appendJSONToFile(const std::string& filePath, const json& j);
    
    // JSON schema validation (future enhancement)
    static bool validateAgainstSchema(const json& j, const json& schema);
    
    // Common response wrappers
    static json respondSuccess(const json& data);
    static json respondCreated(const json& data);
    static json respondNotFound(const std::string& message = "Resource not found");
    static json respondUnauthorized(const std::string& message = "Unauthorized access");
    static json respondForbidden(const std::string& message = "Access forbidden");
    static json respondBadRequest(const std::string& message, const json& details = nullptr);
    static json respondInternalError(const std::string& message = "Internal server error");
    
private:
    static json createErrorObject(const std::string& code, const std::string& message, const json& details);
};

}  // namespace utils
}  // namespace conference

#endif  // JSON_UTILS_H
