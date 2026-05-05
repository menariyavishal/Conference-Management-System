#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <exception>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace middleware {

enum class ErrorCode {
    // Client errors (4xx)
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    CONFLICT = 409,
    
    // Server errors (5xx)
    INTERNAL_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    SERVICE_UNAVAILABLE = 503,
};

struct ErrorResponse {
    int statusCode;
    std::string code;
    std::string message;
    json details;
};

class ApplicationException : public std::exception {
protected:
    int statusCode;
    std::string errorCode;
    std::string message;
    json details;

public:
    explicit ApplicationException(int code, const std::string& msg, const json& det = nullptr);
    
    const char* what() const noexcept override;
    int getStatusCode() const;
    std::string getErrorCode() const;
    std::string getMessage() const;
    json getDetails() const;
};

class ValidationException : public ApplicationException {
public:
    explicit ValidationException(const std::string& message, const json& details = nullptr);
};

class AuthenticationException : public ApplicationException {
public:
    explicit AuthenticationException(const std::string& message = "Authentication failed");
};

class AuthorizationException : public ApplicationException {
public:
    explicit AuthorizationException(const std::string& message = "Access forbidden");
};

class NotFoundException : public ApplicationException {
public:
    explicit NotFoundException(const std::string& message = "Resource not found");
};

class ConflictException : public ApplicationException {
public:
    explicit ConflictException(const std::string& message);
};

class DatabaseException : public ApplicationException {
public:
    explicit DatabaseException(const std::string& message);
};

class FileException : public ApplicationException {
public:
    explicit FileException(const std::string& message);
};

class EmailException : public ApplicationException {
public:
    explicit EmailException(const std::string& message);
};

// Error Handler class
class ErrorHandler {
public:
    // Handle exceptions and convert to JSON response
    static ErrorResponse handleException(const std::exception& ex);
    static ErrorResponse handleApplicationException(const ApplicationException& ex);
    static ErrorResponse handleStdException(const std::exception& ex);
    static ErrorResponse handleUnknownException();
    
    // Build error response
    static json buildErrorJSON(const ErrorResponse& response);
    static json buildErrorJSON(int statusCode, const std::string& code, const std::string& message, 
                               const json& details = nullptr);
    
    // Log error
    static void logError(const std::string& context, const std::exception& ex);
    static void logError(const ErrorResponse& response);
    
    // Validate error
    static bool isClientError(int statusCode);
    static bool isServerError(int statusCode);
    
    // Get error message
    static std::string getErrorMessage(int statusCode);
    static std::string getErrorCode(const std::string& exceptionType);

private:
    static std::string exceptionTypeToErrorCode(const std::type_info& typeInfo);
};

}  // namespace middleware
}  // namespace conference

#endif  // ERROR_HANDLER_H
