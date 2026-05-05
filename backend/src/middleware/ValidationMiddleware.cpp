#include "middleware/ValidationMiddleware.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace conference {
namespace middleware {

/**
 * @brief Validates that required fields are present in the request body.
 * @param body  Parsed JSON request body
 * @param requiredFields List of required field names
 * @param missingField  Output: the first missing field name (if any)
 * @return true if all required fields are present, false otherwise
 */
bool ValidationMiddleware::validateRequiredFields(const json& body,
                                                   const std::vector<std::string>& requiredFields,
                                                   std::string& missingField) {
    for (const auto& field : requiredFields) {
        if (!body.contains(field) || body[field].is_null()) {
            missingField = field;
            return false;
        }
        // Reject empty strings for string fields
        if (body[field].is_string() && body[field].get<std::string>().empty()) {
            missingField = field;
            return false;
        }
    }
    return true;
}

/**
 * @brief Validates that a string is a valid email address format.
 */
bool ValidationMiddleware::validateEmail(const std::string& email) {
    if (email.empty()) return false;
    auto atPos = email.find('@');
    if (atPos == std::string::npos || atPos == 0) return false;
    auto dotPos = email.rfind('.');
    if (dotPos == std::string::npos || dotPos < atPos + 2) return false;
    if (dotPos == email.size() - 1) return false;
    return true;
}

/**
 * @brief Validates password meets minimum security requirements.
 *        Min 8 chars with at least one digit.
 */
bool ValidationMiddleware::validatePassword(const std::string& password) {
    if (password.size() < 8) return false;
    bool hasDigit = false;
    for (char c : password) {
        if (std::isdigit(c)) { hasDigit = true; break; }
    }
    return hasDigit;
}

/**
 * @brief Validates integer is within inclusive range.
 */
bool ValidationMiddleware::validateRange(int value, int min, int max) {
    return value >= min && value <= max;
}

} // namespace middleware
} // namespace conference
