#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace conference {
namespace constants {

// Application
const std::string APP_NAME = "Conference Management System";
const std::string APP_VERSION = "1.0.0";
const int API_PORT = 8080;
const std::string API_VERSION = "/api/v1";

// Database
const int DB_POOL_SIZE = 10;
const int DB_TIMEOUT_SECONDS = 30;
const int DB_MAX_RETRY_ATTEMPTS = 3;

// File Upload
const long MAX_FILE_SIZE = 52428800;  // 50 MB in bytes
const std::string UPLOAD_DIRECTORY = "./uploads";
const std::string ALLOWED_FILE_TYPES = "pdf,doc,docx,ppt,pptx,txt";

// JWT
const int JWT_EXPIRY_HOURS = 24;
const int JWT_REFRESH_EXPIRY_DAYS = 30;
const std::string JWT_ALGORITHM = "HS256";

// Email
const int EMAIL_MAX_RETRIES = 3;
const std::string SMTP_HOST = "smtp.example.com";
const int SMTP_PORT = 587;
const std::string FROM_EMAIL = "noreply@conference.local";

// Validation
const int MIN_PASSWORD_LENGTH = 8;
const int MAX_PASSWORD_LENGTH = 128;
const int MIN_TITLE_LENGTH = 5;
const int MAX_TITLE_LENGTH = 255;
const int MAX_EMAIL_LENGTH = 255;

// Pagination
const int DEFAULT_PAGE_SIZE = 20;
const int MAX_PAGE_SIZE = 100;

// HTTP Status Codes
const int HTTP_OK = 200;
const int HTTP_CREATED = 201;
const int HTTP_BAD_REQUEST = 400;
const int HTTP_UNAUTHORIZED = 401;
const int HTTP_FORBIDDEN = 403;
const int HTTP_NOT_FOUND = 404;
const int HTTP_CONFLICT = 409;
const int HTTP_INTERNAL_ERROR = 500;

// Roles
const std::string ROLE_ADMIN = "ADMIN";
const std::string ROLE_ORGANIZER = "ORGANIZER";
const std::string ROLE_AUTHOR = "AUTHOR";
const std::string ROLE_REVIEWER = "REVIEWER";
const std::string ROLE_PARTICIPANT = "PARTICIPANT";

// Submission Status
const std::string STATUS_DRAFT = "DRAFT";
const std::string STATUS_SUBMITTED = "SUBMITTED";
const std::string STATUS_UNDER_REVIEW = "UNDER_REVIEW";
const std::string STATUS_ACCEPTED = "ACCEPTED";
const std::string STATUS_REJECTED = "REJECTED";

// Review Status
const std::string REVIEW_PENDING = "PENDING";
const std::string REVIEW_SUBMITTED = "SUBMITTED";
const std::string REVIEW_WITHDRAWN = "WITHDRAWN";

// Response messages
const std::string MSG_SUCCESS = "Operation successful";
const std::string MSG_CREATED = "Resource created successfully";
const std::string MSG_UPDATED = "Resource updated successfully";
const std::string MSG_DELETED = "Resource deleted successfully";
const std::string MSG_NOT_FOUND = "Resource not found";
const std::string MSG_UNAUTHORIZED = "Unauthorized access";
const std::string MSG_INVALID_INPUT = "Invalid input provided";
const std::string MSG_SERVER_ERROR = "Internal server error";

}  // namespace constants
}  // namespace conference

#endif  // CONSTANTS_H
