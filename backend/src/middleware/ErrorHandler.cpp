#include "middleware/ErrorHandler.h"
#include <typeinfo>
#include <iostream>

namespace conference {
namespace middleware {

// ApplicationException Implementation
ApplicationException::ApplicationException(int code, const std::string& msg, const json& det)
    : statusCode(code), message(msg), details(det) {
    errorCode = "APPLICATION_ERROR";
}

const char* ApplicationException::what() const noexcept {
    return message.c_str();
}

int ApplicationException::getStatusCode() const {
    return statusCode;
}

std::string ApplicationException::getErrorCode() const {
    return errorCode;
}

std::string ApplicationException::getMessage() const {
    return message;
}

json ApplicationException::getDetails() const {
    return details;
}

// ValidationException Implementation
ValidationException::ValidationException(const std::string& message, const json& details)
    : ApplicationException(400, message, details) {
    errorCode = "VALIDATION_ERROR";
}

// AuthenticationException Implementation
AuthenticationException::AuthenticationException(const std::string& message)
    : ApplicationException(401, message) {
    errorCode = "AUTHENTICATION_FAILED";
}

// AuthorizationException Implementation
AuthorizationException::AuthorizationException(const std::string& message)
    : ApplicationException(403, message) {
    errorCode = "ACCESS_FORBIDDEN";
}

// NotFoundException Implementation
NotFoundException::NotFoundException(const std::string& message)
    : ApplicationException(404, message) {
    errorCode = "NOT_FOUND";
}

// ConflictException Implementation
ConflictException::ConflictException(const std::string& message)
    : ApplicationException(409, message) {
    errorCode = "CONFLICT";
}

// DatabaseException Implementation
DatabaseException::DatabaseException(const std::string& message)
    : ApplicationException(500, message) {
    errorCode = "DATABASE_ERROR";
}

// FileException Implementation
FileException::FileException(const std::string& message)
    : ApplicationException(500, message) {
    errorCode = "FILE_ERROR";
}

// EmailException Implementation
EmailException::EmailException(const std::string& message)
    : ApplicationException(500, message) {
    errorCode = "EMAIL_ERROR";
}

// ErrorHandler Implementation
ErrorResponse ErrorHandler::handleException(const std::exception& ex) {
    try {
        auto& appEx = dynamic_cast<const ApplicationException&>(ex);
        return handleApplicationException(appEx);
    } catch (...) {
        try {
            auto& stdEx = dynamic_cast<const std::exception&>(ex);
            return handleStdException(stdEx);
        } catch (...) {
            return handleUnknownException();
        }
    }
}

ErrorResponse ErrorHandler::handleApplicationException(const ApplicationException& ex) {
    ErrorResponse response;
    response.statusCode = ex.getStatusCode();
    response.code = ex.getErrorCode();
    response.message = ex.getMessage();
    response.details = ex.getDetails();
    
    logError(response);
    return response;
}

ErrorResponse ErrorHandler::handleStdException(const std::exception& ex) {
    ErrorResponse response;
    response.statusCode = 500;
    response.code = "INTERNAL_ERROR";
    response.message = ex.what();
    response.details = nullptr;
    
    logError("std::exception", ex);
    return response;
}

ErrorResponse ErrorHandler::handleUnknownException() {
    ErrorResponse response;
    response.statusCode = 500;
    response.code = "UNKNOWN_ERROR";
    response.message = "An unknown error occurred";
    response.details = nullptr;
    return response;
}

json ErrorHandler::buildErrorJSON(const ErrorResponse& response) {
    json error;
    error["success"] = false;
    error["error"]["code"] = response.code;
    error["error"]["message"] = response.message;
    if (!response.details.is_null()) {
        error["error"]["details"] = response.details;
    }
    return error;
}

json ErrorHandler::buildErrorJSON(int statusCode, const std::string& code, const std::string& message, 
                                 const json& details) {
    json error;
    error["success"] = false;
    error["error"]["code"] = code;
    error["error"]["message"] = message;
    if (!details.is_null()) {
        error["error"]["details"] = details;
    }
    return error;
}

void ErrorHandler::logError(const std::string& context, const std::exception& ex) {
    std::cerr << "[ERROR] " << context << ": " << ex.what() << std::endl;
}

void ErrorHandler::logError(const ErrorResponse& response) {
    std::cerr << "[ERROR] " << response.code << " (" << response.statusCode << "): " 
              << response.message << std::endl;
}

bool ErrorHandler::isClientError(int statusCode) {
    return statusCode >= 400 && statusCode < 500;
}

bool ErrorHandler::isServerError(int statusCode) {
    return statusCode >= 500 && statusCode < 600;
}

std::string ErrorHandler::getErrorMessage(int statusCode) {
    switch (statusCode) {
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 409: return "Conflict";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 503: return "Service Unavailable";
        default: return "Unknown Error";
    }
}

std::string ErrorHandler::getErrorCode(const std::string& exceptionType) {
    // TODO: Map exception types to error codes
    return "UNKNOWN_ERROR";
}

std::string ErrorHandler::exceptionTypeToErrorCode(const std::type_info& typeInfo) {
    // TODO: Implement type to error code mapping
    return "UNKNOWN_ERROR";
}

}  // namespace middleware
}  // namespace conference
