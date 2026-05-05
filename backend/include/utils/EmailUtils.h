#ifndef EMAIL_UTILS_H
#define EMAIL_UTILS_H

#include <string>
#include <vector>

namespace conference {
namespace utils {

struct EmailMessage {
    std::string toAddress;
    std::string subject;
    std::string bodyHTML;
    std::string bodyPlain;
    std::vector<std::pair<std::string, std::string>> attachments;  // (filePath, mimeType)
};

class EmailUtils {
public:
    // Email validation
    static bool validateEmailAddress(const std::string& email);
    static bool validateEmailList(const std::vector<std::string>& emails);
    
    // Send email
    static bool sendEmail(const std::string& toAddress, const std::string& subject, const std::string& body);
    static bool sendHTMLEmail(const std::string& toAddress, const std::string& subject, 
                             const std::string& htmlBody, const std::string& plainTextBody);
    static bool sendBatchEmails(const std::vector<EmailMessage>& messages);
    
    // Template-based emails
    static std::string loadTemplate(const std::string& templateName);
    static std::string renderTemplate(const std::string& templateName, 
                                     const std::vector<std::pair<std::string, std::string>>& variables);
    
    // Specific notification emails
    static bool sendRegistrationConfirmation(const std::string& email, const std::string& verificationLink);
    static bool sendPasswordReset(const std::string& email, const std::string& resetLink);
    static bool sendSubmissionConfirmation(const std::string& email, const std::string& submissionTitle);
    static bool sendReviewAssignment(const std::string& email, const std::string& submissionTitle, const std::string& deadline);
    static bool sendReviewReminder(const std::string& email, const std::string& submissionTitle, const std::string& deadline);
    static bool sendAcceptanceNotification(const std::string& email, const std::string& submissionTitle);
    static bool sendRejectionNotification(const std::string& email, const std::string& submissionTitle);
    static bool sendDeadlineReminder(const std::string& email, const std::string& deadlineType, const std::string& deadline);
    static bool sendSessionRegistrationConfirmation(const std::string& email, const std::string& sessionTitle);
    
    // Email with attachments
    static bool sendEmailWithAttachment(const std::string& toAddress, const std::string& subject,
                                       const std::string& body, const std::string& filePath);
    static bool sendEmailWithAttachments(const std::string& toAddress, const std::string& subject,
                                        const std::string& body, const std::vector<std::string>& filePaths);
    
    // SMTP configuration
    static bool setSMTPHost(const std::string& host);
    static bool setSMTPPort(int port);
    static bool setSMTPCredentials(const std::string& username, const std::string& password);
    static bool setSMTPUseSSL(bool useSSL);
    static bool setSMTPUseTLS(bool useTLS);
    static bool setFromAddress(const std::string& fromEmail, const std::string& fromName);
    
    // Email queue operations
    static bool queueEmail(const EmailMessage& message);
    static bool processEmailQueue();
    static int getPendingEmailCount();
    
    // Bounce handling
    static bool trackBounce(const std::string& email);
    static bool isBouncedEmail(const std::string& email);
    
    // Unsubscribe
    static bool unsubscribeEmail(const std::string& email);
    static bool isUnsubscribed(const std::string& email);
    
private:
    static std::string getSMTPHost();
    static int getSMTPPort();
    static bool getEmailTemplate(const std::string& templateName, std::string& content);
};

}  // namespace utils
}  // namespace conference

#endif  // EMAIL_UTILS_H
