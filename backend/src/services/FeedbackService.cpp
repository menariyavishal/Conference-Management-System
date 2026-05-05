#include "services/IFeedbackService.h"
#include "repositories/IFeedbackRepository.h"
#include "repositories/IParticipantRepository.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using json = nlohmann::json;

namespace conference {
namespace services {

class FeedbackService : public IFeedbackService {
private:
    std::string lastError;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<repositories::IFeedbackRepository> feedbackRepository;
    std::shared_ptr<repositories::IParticipantRepository> participantRepository;

    std::string generateId() {
        auto now = std::chrono::system_clock::now().time_since_epoch().count();
        return "fb_" + std::to_string(now);
    }

public:
    FeedbackService(
        std::shared_ptr<repositories::IFeedbackRepository> feedbackRepo,
        std::shared_ptr<repositories::IParticipantRepository> participantRepo
    ) : feedbackRepository(feedbackRepo), participantRepository(participantRepo) {
        logger = spdlog::get("feedback_service");
        if (!logger) {
            try {
                spdlog::sink_ptr sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                logger = std::make_shared<spdlog::logger>("feedback_service", sink);
                spdlog::register_logger(logger);
            } catch (...) {
                logger = spdlog::get("conference");
            }
        }
    }

    virtual json submitFeedback(const std::string& sessionId,
                                const std::string& participantId,
                                int rating,
                                const std::string& comment,
                                bool isAnonymous) override {
        try {
            // Must be registered participant (FR6.1)
            if (!participantRepository->isRegistered(sessionId, participantId)) {
                lastError = "Must be a registered participant to submit feedback";
                return {{"success", false}, {"error", lastError}};
            }

            // Validate rating range
            if (rating < 1 || rating > 5) {
                lastError = "Rating must be between 1 and 5";
                return {{"success", false}, {"error", lastError}};
            }

            // Prevent duplicate feedback
            if (feedbackRepository->hasSubmittedFeedback(sessionId, participantId)) {
                lastError = "Feedback already submitted for this session";
                return {{"success", false}, {"error", lastError}};
            }

            std::string feedbackId = generateId();
            json fb = {
                {"id", feedbackId},
                {"sessionId", sessionId},
                {"participantId", participantId},
                {"rating", rating},
                {"comment", comment},
                {"isAnonymous", isAnonymous}
            };

            if (!feedbackRepository->insertFeedback(fb)) {
                lastError = "Failed to save feedback";
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("Feedback submitted for session: {}", sessionId);
            return {{"success", true}, {"message", "Feedback submitted successfully"}};

        } catch (const std::exception& ex) {
            lastError = std::string("Submit feedback failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getSessionFeedbackSummary(const std::string& sessionId,
                                           const std::string& requesterId) override {
        try {
            json summary = feedbackRepository->getFeedbackSummary(sessionId);
            logger->debug("Feedback summary requested for session: {} by: {}", sessionId, requesterId);
            return {{"success", true}, {"data", summary}};
        } catch (const std::exception& ex) {
            lastError = std::string("Get summary failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json listAllFeedback(const std::string& organizerId) override {
        try {
            auto feedback = feedbackRepository->getAllFeedback();
            logger->debug("All feedback listed by organizer: {}", organizerId);
            return {{"success", true}, {"data", feedback}};
        } catch (const std::exception& ex) {
            lastError = std::string("List feedback failed: ") + ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual std::string getLastError() const override { return lastError; }
};

} // namespace services
} // namespace conference

std::shared_ptr<conference::services::IFeedbackService>
createFeedbackService(
    std::shared_ptr<conference::repositories::IFeedbackRepository> feedbackRepo,
    std::shared_ptr<conference::repositories::IParticipantRepository> participantRepo
) {
    return std::make_shared<conference::services::FeedbackService>(feedbackRepo, participantRepo);
}
