#include "utils/DateTimeUtils.h"
#include <iomanip>
#include <sstream>

namespace conference {
namespace utils {

// Get current timestamp in ISO 8601 format
std::string DateTimeUtils::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time), "%Y-%m-%dT%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';
    
    return ss.str();
}

// Parse ISO 8601 to Unix timestamp
time_t DateTimeUtils::parseISO8601ToUnix(const std::string& iso8601) {
    std::tm tm = {};
    std::stringstream ss(iso8601);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    
    if (ss.fail()) {
        return 0;
    }
    
    return std::mktime(&tm);
}

// Convert Unix timestamp to ISO 8601
std::string DateTimeUtils::unixToISO8601(time_t timestamp) {
    auto time = std::chrono::system_clock::from_time_t(timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&timestamp), "%Y-%m-%dT%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';
    
    return ss.str();
}

// Check if deadline has passed
bool DateTimeUtils::isDeadlinePassed(const std::string& deadline) {
    time_t deadlineTime = parseISO8601ToUnix(deadline);
    time_t now = std::time(nullptr);
    return now > deadlineTime;
}

// Check if due soon (within days)
bool DateTimeUtils::isDueSoon(const std::string& deadline, int daysAhead) {
    time_t deadlineTime = parseISO8601ToUnix(deadline);
    time_t now = std::time(nullptr);
    time_t secondsAhead = daysAhead * 24 * 60 * 60;
    
    return (deadlineTime - now) <= secondsAhead && (deadlineTime - now) > 0;
}

// Get days until deadline
int DateTimeUtils::getDaysUntilDeadline(const std::string& deadline) {
    time_t deadlineTime = parseISO8601ToUnix(deadline);
    time_t now = std::time(nullptr);
    
    int secondsLeft = deadlineTime - now;
    if (secondsLeft <= 0) return 0;
    
    return secondsLeft / (24 * 60 * 60);
}

// Check if date is in range
bool DateTimeUtils::isDateInRange(const std::string& date, const std::string& startDate, 
                                 const std::string& endDate) {
    time_t dateTime = parseISO8601ToUnix(date);
    time_t startTime = parseISO8601ToUnix(startDate);
    time_t endTime = parseISO8601ToUnix(endDate);
    
    return dateTime >= startTime && dateTime <= endTime;
}

// Format date to string
std::string DateTimeUtils::formatDate(time_t timestamp, const std::string& format) {
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timestamp), format.c_str());
    return ss.str();
}

// Convert to UTC
std::string DateTimeUtils::convertToUTC(const std::string& localTime) {
    // Assuming input is already in ISO 8601 format
    // In a real application, would handle timezone conversion
    return localTime;
}

// Add days to timestamp
std::string DateTimeUtils::addDays(const std::string& timestamp, int days) {
    time_t time = parseISO8601ToUnix(timestamp);
    time_t newTime = time + (days * 24 * 60 * 60);
    return unixToISO8601(newTime);
}

// Add hours to timestamp
std::string DateTimeUtils::addHours(const std::string& timestamp, int hours) {
    time_t time = parseISO8601ToUnix(timestamp);
    time_t newTime = time + (hours * 60 * 60);
    return unixToISO8601(newTime);
}

// Add minutes to timestamp
std::string DateTimeUtils::addMinutes(const std::string& timestamp, int minutes) {
    time_t time = parseISO8601ToUnix(timestamp);
    time_t newTime = time + (minutes * 60);
    return unixToISO8601(newTime);
}

// Get difference in days between two dates
int DateTimeUtils::getDaysBetween(const std::string& date1, const std::string& date2) {
    time_t time1 = parseISO8601ToUnix(date1);
    time_t time2 = parseISO8601ToUnix(date2);
    
    time_t diff = std::abs(time1 - time2);
    return diff / (24 * 60 * 60);
}

// Get difference in hours between two dates
int DateTimeUtils::getHoursBetween(const std::string& date1, const std::string& date2) {
    time_t time1 = parseISO8601ToUnix(date1);
    time_t time2 = parseISO8601ToUnix(date2);
    
    time_t diff = std::abs(time1 - time2);
    return diff / (60 * 60);
}

// Get difference in minutes between two dates
int DateTimeUtils::getMinutesBetween(const std::string& date1, const std::string& date2) {
    time_t time1 = parseISO8601ToUnix(date1);
    time_t time2 = parseISO8601ToUnix(date2);
    
    time_t diff = std::abs(time1 - time2);
    return diff / 60;
}

// Get current Unix timestamp
long DateTimeUtils::getCurrentUnixTimestamp() {
    return std::time(nullptr);
}

// Get current time in specified format
std::string DateTimeUtils::getCurrentTimeFormatted(const std::string& format) {
    auto now = std::time(nullptr);
    return formatDate(now, format);
}

// Validate date format (YYYY-MM-DD)
bool DateTimeUtils::isValidDate(const std::string& date) {
    std::tm tm = {};
    std::stringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    return !ss.fail();
}

// Get day of week
std::string DateTimeUtils::getDayOfWeek(const std::string& date) {
    time_t time = parseISO8601ToUnix(date);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%A");
    return ss.str();
}

// Get month name
std::string DateTimeUtils::getMonthName(int month) {
    static const std::string months[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    if (month >= 1 && month <= 12) {
        return months[month - 1];
    }
    return "";
}

// Check if year is leap year
bool DateTimeUtils::isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Get days in month
int DateTimeUtils::getDaysInMonth(int month, int year) {
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    if (month < 1 || month > 12) return 0;
    
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    
    return days[month - 1];
}

// Get current ISO 8601 datetime (alias for getCurrentTimestamp)
std::string DateTimeUtils::getCurrentISO8601() {
    return getCurrentTimestamp();
}

// Get current ISO 8601 datetime (alias for getCurrentTimestamp)
std::string DateTimeUtils::getCurrentDateTimeISO8601() {
    return getCurrentTimestamp();
}

}  // namespace utils
}  // namespace conference
