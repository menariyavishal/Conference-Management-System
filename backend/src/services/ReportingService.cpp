#include "services/IReportingService.h"
#include "repositories/ISubmissionRepository.h"
#include "repositories/IReviewRepository.h"
#include "repositories/IUserRepository.h"
#include "repositories/ISessionRepository.h"
#include "repositories/IParticipantRepository.h"
#include "repositories/IFeedbackRepository.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace conference {
namespace services {

class ReportingService : public IReportingService {
private:
    std::string lastError;
    std::shared_ptr<spdlog::logger> logger;
    
    std::shared_ptr<repositories::ISubmissionRepository> submissionRepo;
    std::shared_ptr<repositories::IReviewRepository> reviewRepo;
    std::shared_ptr<repositories::IUserRepository> userRepo;
    std::shared_ptr<repositories::ISessionRepository> sessionRepo;
    std::shared_ptr<repositories::IParticipantRepository> participantRepo;
    std::shared_ptr<repositories::IFeedbackRepository> feedbackRepo;

public:
    ReportingService(
        std::shared_ptr<repositories::ISubmissionRepository> sRepo,
        std::shared_ptr<repositories::IReviewRepository> rRepo,
        std::shared_ptr<repositories::IUserRepository> uRepo,
        std::shared_ptr<repositories::ISessionRepository> sessRepo,
        std::shared_ptr<repositories::IParticipantRepository> pRepo,
        std::shared_ptr<repositories::IFeedbackRepository> fRepo
    ) : submissionRepo(sRepo), reviewRepo(rRepo), userRepo(uRepo), 
        sessionRepo(sessRepo), participantRepo(pRepo), feedbackRepo(fRepo) 
    {
        logger = spdlog::get("reporting_service");
        if (!logger) {
            try {
                spdlog::sink_ptr sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                logger = std::make_shared<spdlog::logger>("reporting_service", sink);
                spdlog::register_logger(logger);
            } catch (...) {
                logger = spdlog::get("conference");
            }
        }
    }

    virtual json getGlobalStats() override {
        try {
            return {
                {"success", true},
                {"data", {
                    {"totalSubmissions", submissionRepo->getSubmissionCount("", "")},
                    {"totalUsers", userRepo->getUserCount("", "")},
                    {"totalSessions", (int)sessionRepo->getAllSessions().size()},
                    {"reviewStats", reviewRepo->getGlobalReviewStats()}
                }}
            };
        } catch (const std::exception& ex) {
            lastError = ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getSubmissionAnalytics() override {
        try {
            return {
                {"success", true},
                {"data", {
                    {"statusDistribution", submissionRepo->getStatusStatistics()}
                }}
            };
        } catch (const std::exception& ex) {
            lastError = ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getReviewerStats() override {
        try {
            return {
                {"success", true},
                {"data", reviewRepo->getReviewerPerformanceStats()}
            };
        } catch (const std::exception& ex) {
            lastError = ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getSessionAnalytics() override {
        try {
            return {
                {"success", true},
                {"data", sessionRepo->getSessionAnalytics()}
            };
        } catch (const std::exception& ex) {
            lastError = ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getSessionReport(const std::string& sessionId) override {
        try {
            auto sessionOpt = sessionRepo->getSessionById(sessionId);
            if (!sessionOpt.has_value()) {
                return {{"success", false}, {"error", "Session not found"}};
            }

            return {
                {"success", true},
                {"data", {
                    {"session", sessionOpt.value()},
                    {"papers", sessionRepo->getPapersForSession(sessionId)},
                    {"feedbackSummary", feedbackRepo->getFeedbackSummary(sessionId)},
                    {"participantCount", sessionRepo->getCurrentParticipantCount(sessionId)}
                }}
            };
        } catch (const std::exception& ex) {
            lastError = ex.what();
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual json getSystemActivityReport(int limit) override {
        // Placeholder for activity logs (requires a logging table/repo)
        return {{"success", true}, {"data", json::array()}, {"message", "Activity logging table not yet implemented"}};
    }

    virtual std::string getLastError() const override { return lastError; }
};

} // namespace services
} // namespace conference

std::shared_ptr<conference::services::IReportingService> 
createReportingService(
    std::shared_ptr<conference::repositories::ISubmissionRepository> submissionRepo,
    std::shared_ptr<conference::repositories::IReviewRepository> reviewRepo,
    std::shared_ptr<conference::repositories::IUserRepository> userRepo,
    std::shared_ptr<conference::repositories::ISessionRepository> sessionRepo,
    std::shared_ptr<conference::repositories::IParticipantRepository> participantRepo,
    std::shared_ptr<conference::repositories::IFeedbackRepository> feedbackRepo
) {
    return std::make_shared<conference::services::ReportingService>(
        submissionRepo, reviewRepo, userRepo, sessionRepo, participantRepo, feedbackRepo
    );
}
