#include "utils/ValidationUtils.h"
#include <regex>
#include <algorithm>

namespace conference {
namespace utils {

// Email validation using RFC 5322 simplified regex
bool ValidationUtils::validateEmail(const std::string& email) {
    if (email.empty() || email.length() > 254) {
        return false;
    }
    
    static const std::regex emailRegex(
        R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})"
    );
    
    return std::regex_match(email, emailRegex);
}

// Validate password: 8+ chars, uppercase, lowercase, number, special char
bool ValidationUtils::validatePassword(const std::string& password) {
    if (password.length() < 8 || password.length() > 128) {
        return false;
    }
    
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSpecial = false;
    
    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        else if (std::islower(c)) hasLower = true;
        else if (std::isdigit(c)) hasDigit = true;
        else if (std::string("!@#$%^&*()_+-=[]{}|;:,.<>?").find(c) != std::string::npos) {
            hasSpecial = true;
        }
    }
    
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

// Validate title (non-empty, min/max length)
bool ValidationUtils::validateTitle(const std::string& title) {
    return !title.empty() && title.length() >= 5 && title.length() <= 255;
}

// Validate abstract
bool ValidationUtils::validateAbstract(const std::string& abstract) {
    return !abstract.empty() && abstract.length() >= 50 && abstract.length() <= 2000;
}

// Validate file type
bool ValidationUtils::validateFileType(const std::string& filename, const std::vector<std::string>& allowedTypes) {
    size_t dotPos = filename.find_last_of(".");
    if (dotPos == std::string::npos) return false;
    
    std::string ext = filename.substr(dotPos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return std::find(allowedTypes.begin(), allowedTypes.end(), ext) != allowedTypes.end();
}

// Check for SQL injection patterns
bool ValidationUtils::hasSQLInjectionPatterns(const std::string& input) {
    static const std::vector<std::string> patterns = {
        "UNION", "SELECT", "INSERT", "UPDATE", "DELETE", "DROP",
        "CREATE", "ALTER", "EXEC", "EXECUTE", "DECLARE", "--", ";", "/*", "*/"
    };
    
    std::string upper = input;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    for (const auto& pattern : patterns) {
        if (upper.find(pattern) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

// Check for XSS patterns
bool ValidationUtils::hasXSSPatterns(const std::string& input) {
    static const std::vector<std::string> patterns = {
        "<script", "javascript:", "onerror=", "onclick=", "onload=",
        "<iframe", "<embed", "<object", "eval(", "expression("
    };
    
    std::string lower = input;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    for (const auto& pattern : patterns) {
        if (lower.find(pattern) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

// Escape SQL string
std::string ValidationUtils::escapeSQLString(const std::string& input) {
    std::string escaped;
    for (char c : input) {
        if (c == '\'') {
            escaped += "''";
        } else if (c == '\\') {
            escaped += "\\\\";
        } else {
            escaped += c;
        }
    }
    return escaped;
}

// Escape HTML characters
std::string ValidationUtils::escapeHTMLChars(const std::string& input) {
    std::string escaped;
    for (char c : input) {
        switch (c) {
            case '<': escaped += "&lt;"; break;
            case '>': escaped += "&gt;"; break;
            case '&': escaped += "&amp;"; break;
            case '"': escaped += "&quot;"; break;
            case '\'': escaped += "&#39;"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

// Validate URL
bool ValidationUtils::validateURL(const std::string& url) {
    static const std::regex urlRegex(
        R"((https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*)))"
    );
    
    return std::regex_match(url, urlRegex);
}

// Validate phone number (basic international format)
bool ValidationUtils::validatePhoneNumber(const std::string& phone) {
    std::string cleaned;
    for (char c : phone) {
        if (std::isdigit(c) || c == '+') {
            cleaned += c;
        }
    }
    
    return cleaned.length() >= 10 && cleaned.length() <= 15 && cleaned[0] == '+';
}

// Sanitize filename
std::string ValidationUtils::sanitizeFileName(const std::string& filename) {
    std::string sanitized;
    
    for (char c : filename) {
        if (std::isalnum(c) || c == '.' || c == '-' || c == '_') {
            sanitized += c;
        } else if (c == ' ') {
            sanitized += '_';
        }
    }
    
    // Remove leading/trailing dots
    while (!sanitized.empty() && sanitized.front() == '.') {
        sanitized.erase(0, 1);
    }
    while (!sanitized.empty() && sanitized.back() == '.') {
        sanitized.pop_back();
    }
    
    return sanitized.empty() ? "file" : sanitized;
}

// Validate text input (no SQL or XSS)
bool ValidationUtils::validateTextInput(const std::string& input, int minLength, int maxLength) {
    if (input.empty() || input.length() < minLength || input.length() > maxLength) {
        return false;
    }
    
    return !hasSQLInjectionPatterns(input) && !hasXSSPatterns(input);
}

// Validate numeric input
bool ValidationUtils::validateNumericInput(const std::string& input, int minValue, int maxValue) {
    try {
        int value = std::stoi(input);
        return value >= minValue && value <= maxValue;
    } catch (...) {
        return false;
    }
}

// Validate date format (YYYY-MM-DD)
bool ValidationUtils::validateDateFormat(const std::string& date) {
    static const std::regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
    return std::regex_match(date, dateRegex);
}

// Validate ISO 8601 datetime
bool ValidationUtils::validateISO8601DateTime(const std::string& datetime) {
    static const std::regex isoRegex(
        R"(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:Z|[+-]\d{2}:\d{2})?)"
    );
    return std::regex_match(datetime, isoRegex);
}

// Normalize whitespace
std::string ValidationUtils::normalizeWhitespace(const std::string& input) {
    std::string result;
    bool prevSpace = false;
    
    for (char c : input) {
        if (std::isspace(c)) {
            if (!prevSpace) {
                result += ' ';
                prevSpace = true;
            }
        } else {
            result += c;
            prevSpace = false;
        }
    }
    
    // Trim leading/trailing spaces
    size_t start = result.find_first_not_of(" ");
    size_t end = result.find_last_not_of(" ");
    
    if (start == std::string::npos) return "";
    return result.substr(start, end - start + 1);
}

// Trim string
std::string ValidationUtils::trimString(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Validate keyword list
bool ValidationUtils::validateKeywords(const std::string& keywords) {
    std::vector<std::string> keywordList;
    size_t start = 0;
    
    while (start < keywords.length()) {
        size_t end = keywords.find(',', start);
        if (end == std::string::npos) end = keywords.length();
        
        std::string keyword = trimString(keywords.substr(start, end - start));
        if (keyword.empty() || keyword.length() > 50) {
            return false;
        }
        
        keywordList.push_back(keyword);
        start = end + 1;
    }
    
    return keywordList.size() > 0 && keywordList.size() <= 10;
}

}  // namespace utils
}  // namespace conference
