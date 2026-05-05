#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace middleware {

class ValidationMiddleware {
public:
    static bool validateRequiredFields(const json& body,
                                       const std::vector<std::string>& requiredFields,
                                       std::string& missingField);
    static bool validateEmail(const std::string& email);
    static bool validatePassword(const std::string& password);
    static bool validateRange(int value, int min, int max);
};

} // namespace middleware
} // namespace conference
