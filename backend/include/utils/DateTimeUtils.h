#ifndef DATETIME_UTILS_H
#define DATETIME_UTILS_H

#include <string>
#include <ctime>
#include <chrono>

namespace conference {
namespace utils {

class DateTimeUtils {
public:
    // Get current time
    static std::string getCurrentTimestamp();
    static long getCurrentUnixTimestamp();
    static std::string getCurrentDateTimeISO8601();
    static std::string getCurrentISO8601();
    static std::string getCurrentDate();
    static std::string getCurrentTime();
    
    // Parse dates
    static time_t parseISO8601ToUnix(const std::string& isoDate);
    static std::string unixToISO8601(time_t timestamp);
    static std::string parseUnixToReadable(long unixTimestamp);
    
    // Date calculations
    static std::string addDays(const std::string& date, int days);
    static std::string addHours(const std::string& dateTime, int hours);
    static std::string addMinutes(const std::string& dateTime, int minutes);
    static std::string subtractDays(const std::string& date, int days);
    
    // Deadline checks
    static bool isDeadlinePassed(const std::string& deadline);
    static bool isDueSoon(const std::string& deadline, int daysAhead = 1);
    static int getDaysUntilDeadline(const std::string& deadline);
    static long getHoursUntilDeadline(const std::string& deadline);
    
    // Date comparison
    static bool isDateBefore(const std::string& date1, const std::string& date2);
    static bool isDateAfter(const std::string& date1, const std::string& date2);
    static bool isDateEqual(const std::string& date1, const std::string& date2);
    static bool isDateInRange(const std::string& date, const std::string& startDate, const std::string& endDate);
    
    // Date difference
    static int getDaysBetween(const std::string& date1, const std::string& date2);
    static int getHoursBetween(const std::string& dateTime1, const std::string& dateTime2);
    static int getMinutesBetween(const std::string& dateTime1, const std::string& dateTime2);
    
    // Date formatting
    static std::string formatDate(time_t timestamp, const std::string& format);
    static std::string formatDateTime(const std::string& dateTime, const std::string& format);
    static std::string formatReadable(const std::string& isoDate);
    static std::string getCurrentTimeFormatted(const std::string& format);
    
    // Timezone handling
    static std::string convertToUTC(const std::string& date);
    static std::string convertFromUTC(const std::string& date);
    
    // Validation
    static bool isValidDateFormat(const std::string& date);
    static bool isValidDateTime(const std::string& dateTime);
    static bool isValidISO8601(const std::string& date);
    static bool isValidDate(const std::string& date);
    
    // Day of week / Month functions
    static std::string getDayOfWeek(const std::string& date);
    static std::string getMonthName(int month);
    static int getDayOfMonth(const std::string& date);
    static int getMonth(const std::string& date);
    static int getYear(const std::string& date);
    
    // Utility functions
    static bool isLeapYear(int year);
    static int getDaysInMonth(int month, int year);

private:
    static const std::string ISO8601_FORMAT;
};

}  // namespace utils
}  // namespace conference

#endif  // DATETIME_UTILS_H
