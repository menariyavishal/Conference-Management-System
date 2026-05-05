#include "utils/EmailUtils.h"
#include "utils/ValidationUtils.h"
#include <iostream>

namespace conference {
namespace utils {

bool EmailUtils::validateEmailAddress(const std::string& email) {
    return ValidationUtils::validateEmail(email);
}

bool EmailUtils::validateEmailList(const std::vector<std::string>& emails) {
    for (const auto& email : emails) {
        if (!validateEmailAddress(email)) return false;
    }
    return true;
}

bool EmailUtils::sendEmail(const std::string& toAddress, const std::string& subject, const std::string& body) {
    if (!validateEmailAddress(toAddress)) return false;
    std::cout << "[EMAIL] To: " << toAddress << std::endl;
    return true;
}

bool EmailUtils::sendHTMLEmail(const std::string& toAddress, const std::string& subject, 
                               const std::string& htmlBody, const std::string& plainTextBody) {
    if (!validateEmailAddress(toAddress)) return false;
    std::cout << "[HTML-EMAIL] To: " << toAddress << std::endl;
    return true;
}

bool EmailUtils::sendBatchEmails(const std::vector<EmailMessage>& messages) {
    int count = 0;
    for (const auto& msg : messages) {
        if (sendEmail(msg.toAddress, msg.subject, msg.bodyPlain)) count++;
    }
    return count == messages.size();
}

std::string EmailUtils::loadTemplate(const std::string& templateName) {
    return "";  // TODO
}

std::string EmailUtils::renderTemplate(const std::string& templateName, 
                                       const std::vector<std::pair<std::string, std::string>>& variables) {
    return "";  // TODO
}

bool EmailUtils::sendRegistrationConfirmation(const std::string& email, const std::string& verificationLink) {
    return sendEmail(email, "Registration Confirmation", "Please verify your email: " + verificationLink);
}

bool EmailUtils::sendPasswordReset(const std::string& email, const std::string& resetLink) {
    return sendEmail(email, "Password Reset", "Reset your password: " + resetLink);
}

bool EmailUtils::sendSubmissionConfirmation(const std::string& email, const std::string& submissionTitle) {
    return sendEmail(email, "Submission Confirmed", "Your submission has been received.");
}

bool EmailUtils::sendReviewAssignment(const std::string& email, const std::string& submissionTitle, const std::string& deadline) {
    return sendEmail(email, "Review Assignment", "You have been assigned a review.");
}

bool EmailUtils::sendReviewReminder(const std::string& email, const std::string& submissionTitle, const std::string& deadline) {
    return sendEmail(email, "Review Reminder", "Please submit your review by " + deadline);
}

bool EmailUtils::sendAcceptanceNotification(const std::string& email, const std::string& submissionTitle) {
    return sendEmail(email, "Submission Accepted", submissionTitle + " has been accepted.");
}

bool EmailUtils::sendRejectionNotification(const std::string& email, const std::string& submissionTitle) {
    return sendEmail(email, "Submission Rejected", submissionTitle + " has been rejected.");
}

bool EmailUtils::sendDeadlineReminder(const std::string& email, const std::string& deadlineType, const std::string& deadline) {
    return sendEmail(email, deadlineType + " Reminder", "Deadline: " + deadline);
}

bool EmailUtils::sendSessionRegistrationConfirmation(const std::string& email, const std::string& sessionTitle) {
    return sendEmail(email, "Session Registration", "You have been registered for " + sessionTitle);
}

bool EmailUtils::sendEmailWithAttachment(const std::string& toAddress, const std::string& subject,
                                         const std::string& body, const std::string& filePath) {
    return sendEmail(toAddress, subject, body + " [Attachment: " + filePath + "]");
}

bool EmailUtils::sendEmailWithAttachments(const std::string& toAddress, const std::string& subject,
                                          const std::string& body, const std::vector<std::string>& filePaths) {
    std::string bodyWithAttachments = body;
    for (const auto& file : filePaths) {
        bodyWithAttachments += " [" + file + "]";
    }
    return sendEmail(toAddress, subject, bodyWithAttachments);
}

}  // namespace utils
}  // namespace conference
