#ifndef VALIDATION_UTILS_H
#define VALIDATION_UTILS_H

#include <string>
#include <vector>

namespace conference {
namespace utils {

class ValidationUtils {
public:
    // Email validation
    static bool validateEmail(const std::string& email);
    static bool isValidEmailFormat(const std::string& email);
    
    // Password validation
    static bool validatePassword(const std::string& password);
    static bool hasMinLength(const std::string& password, int minLength);
    static bool hasMaxLength(const std::string& password, int maxLength);
    static bool hasSpecialChar(const std::string& password);
    static bool hasNumber(const std::string& password);
    static bool hasUpperCase(const std::string& password);
    static bool hasLowerCase(const std::string& password);
    
    // String validation
    static bool validateTitle(const std::string& title);
    static bool validateAbstract(const std::string& abstract);
    static bool isEmpty(const std::string& str);
    static bool isOnlyWhitespace(const std::string& str);
    
    // Numeric validation
    static bool validateInteger(const std::string& str);
    static bool validatePositiveInteger(const std::string& str);
    static bool validateFloat(const std::string& str);
    
    // File validation
    static bool validateFileName(const std::string& fileName);
    static bool validateFileSize(long fileSize, long maxSize);
    static bool validateFileType(const std::string& fileName, const std::vector<std::string>& allowedTypes);
    static std::string getFileExtension(const std::string& fileName);
    
    // URL validation
    static bool validateURL(const std::string& url);
    static bool validateURI(const std::string& uri);
    
    // Date validation
    static bool validateDateFormat(const std::string& date);
    static bool validateDateRange(const std::string& startDate, const std::string& endDate);
    
    // Input sanitization
    static std::string sanitizeInput(const std::string& input);
    static std::string sanitizeFileName(const std::string& filename);
    static std::string trimWhitespace(const std::string& str);
    static std::string trimString(const std::string& str);
    static std::string normalizeWhitespace(const std::string& input);
    static std::string toLowercase(const std::string& str);
    static std::string toUppercase(const std::string& str);
    
    // Additional validation
    static bool validatePhoneNumber(const std::string& phone);
    static bool validateTextInput(const std::string& input, int minLength, int maxLength);
    static bool validateNumericInput(const std::string& input, int minValue, int maxValue);
    static bool validateISO8601DateTime(const std::string& datetime);
    static bool validateKeywords(const std::string& keywords);
    
    // SQL injection prevention
    static bool hasSQLInjectionPatterns(const std::string& input);
    static std::string escapeSQLString(const std::string& input);
    
    // XSS prevention
    static bool hasXSSPatterns(const std::string& input);
    static std::string escapeHTMLChars(const std::string& input);
    
private:
    static const std::string EMAIL_PATTERN;
    static const std::string URL_PATTERN;
    static const std::string SPECIAL_CHARS;
    static const std::vector<std::string> SQL_INJECTION_PATTERNS;
    static const std::vector<std::string> XSS_PATTERNS;
};

}  // namespace utils
}  // namespace conference

#endif  // VALIDATION_UTILS_H
