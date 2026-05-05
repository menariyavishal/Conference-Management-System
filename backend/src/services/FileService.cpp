#include "services/IFileService.h"
#include "repositories/IFileRepository.h"
#include "utils/ValidationUtils.h"
#include "utils/EncryptionUtils.h"
#include "utils/Constants.h"
#include "utils/DateTimeUtils.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace conference {
namespace services {

class FileService : public IFileService {
private:
    std::string lastError;
    std::shared_ptr<spdlog::logger> logger;
    std::string uploadDirectory;
    std::shared_ptr<repositories::IFileRepository> fileRepository;

    const std::vector<std::string> ALLOWED_EXTENSIONS = {
        ".pdf", ".doc", ".docx", ".ppt", ".pptx", 
        ".xls", ".xlsx", ".txt", ".jpg", ".png"
    };

    std::string getFileExtension(const std::string& filename) {
        size_t pos = filename.find_last_of('.');
        if (pos == std::string::npos) return "";
        std::string ext = filename.substr(pos);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }

    bool isExtensionAllowed(const std::string& filename) {
        std::string ext = getFileExtension(filename);
        return std::find(ALLOWED_EXTENSIONS.begin(), ALLOWED_EXTENSIONS.end(), ext) 
               != ALLOWED_EXTENSIONS.end();
    }

    long getFileSize(const std::string& filePath) {
        try {
            return fs::file_size(filePath);
        } catch (...) {
            return -1;
        }
    }

    bool ensureUploadDirectory(const std::string& path) {
        try {
            fs::create_directories(path);
            return true;
        } catch (...) {
            return false;
        }
    }

public:
    FileService(std::shared_ptr<repositories::IFileRepository> repo) : fileRepository(repo) {
        logger = spdlog::get("conference");
        if (!logger) {
            logger = spdlog::stdout_color_mt("file");
        }
        uploadDirectory = "./uploads";
        ensureUploadDirectory(uploadDirectory);
    }

    virtual json uploadFile(
        const std::string& sourceFilePath,
        const std::string& userId,
        const std::string& context = "submission"
    ) override {
        try {
            if (!fs::exists(sourceFilePath)) {
                lastError = "Source file not found: " + sourceFilePath;
                logger->warn(lastError);
                return {{"success", false}, {"error", lastError}};
            }

            std::string originalFilename = fs::path(sourceFilePath).filename().string();

            if (!isExtensionAllowed(originalFilename)) {
                lastError = "File type not allowed: " + getFileExtension(originalFilename);
                logger->warn("Upload rejected: {}", lastError);
                return {{"success", false}, {"error", lastError}};
            }

            if (!validateFileSize(sourceFilePath, 50)) {
                lastError = "File too large (max 50MB)";
                logger->warn(lastError);
                return {{"success", false}, {"error", lastError}};
            }

            std::string secureFilename = generateSecureFilename(originalFilename, userId);
            std::string contextDir = uploadDirectory + "/" + context + "/" + userId;
            
            if (!ensureUploadDirectory(contextDir)) {
                lastError = "Failed to create upload directory";
                logger->error(lastError);
                return {{"success", false}, {"error", lastError}};
            }

            std::string destinationPath = contextDir + "/" + secureFilename;
            fs::copy(sourceFilePath, destinationPath, fs::copy_options::overwrite_existing);

            std::string checksum = calculateChecksum(destinationPath);
            long fileSize = getFileSize(destinationPath);
            std::string extension = getFileExtension(secureFilename);
            std::string fileId = "file_" + checksum.substr(0, 12);

            json metadata = {
                {"fileId", fileId},
                {"originalName", originalFilename},
                {"secureFilename", secureFilename},
                {"destinationPath", destinationPath},
                {"size", fileSize},
                {"extension", extension},
                {"checksum", checksum},
                {"uploadedBy", userId},
                {"context", context},
                {"isActive", true},
                {"uploadedAt", utils::DateTimeUtils::getCurrentISO8601()}
            };

            if (!fileRepository->insertFileMetadata(metadata)) {
                lastError = "Failed to save file metadata to database";
                fs::remove(destinationPath); // cleanup on db fail
                return {{"success", false}, {"error", lastError}};
            }

            logger->info("File uploaded successfully: {} user: {} fileId: {}", originalFilename, userId, fileId);

            return {
                {"success", true},
                {"message", "File uploaded successfully"},
                {"data", {
                    {"fileId", fileId},
                    {"originalName", originalFilename},
                    {"secureFilename", secureFilename},
                    {"size", fileSize},
                    {"extension", extension},
                    {"checksum", checksum},
                    {"uploadedAt", metadata["uploadedAt"]},
                    {"uploadedBy", userId}
                }}
            };

        } catch (const std::exception& ex) {
            lastError = std::string("File upload failed: ") + ex.what();
            logger->error(lastError);
            return {{"success", false}, {"error", lastError}};
        }
    }

    virtual bool downloadFile(
        const std::string& fileId,
        const std::string& userId,
        std::string& outputPath
    ) override {
        try {
            auto metadataOpt = fileRepository->getFileById(fileId);
            if (!metadataOpt.has_value()) {
                lastError = "File not found";
                return false;
            }

            json metadata = metadataOpt.value();
            
            // Assume we have some way to check if user is admin, for now just check owner
            // If the user isn't the owner, we would typically check their role.
            if (metadata["uploadedBy"] != userId) {
                lastError = "Access denied";
                return false;
            }

            if (!metadata["isActive"].get<bool>()) {
                lastError = "File is deactivated";
                return false;
            }

            outputPath = metadata["filePath"].get<std::string>();
            logger->debug("Download requested for fileId: {} by user: {}", fileId, userId);
            
            return fs::exists(outputPath);

        } catch (const std::exception& ex) {
            lastError = std::string("File download failed: ") + ex.what();
            logger->error(lastError);
            return false;
        }
    }

    virtual json getFileInfo(const std::string& fileId) override {
        try {
            auto metadataOpt = fileRepository->getFileById(fileId);
            if (!metadataOpt.has_value()) {
                return {{"success", false}, {"error", "File not found"}};
            }

            logger->debug("Retrieved file info: {}", fileId);
            return {
                {"success", true},
                {"data", metadataOpt.value()}
            };

        } catch (const std::exception& ex) {
            return {{"success", false}, {"error", ex.what()}};
        }
    }

    virtual bool deleteFile(
        const std::string& fileId,
        const std::string& userId
    ) override {
        try {
            auto metadataOpt = fileRepository->getFileById(fileId);
            if (!metadataOpt.has_value()) {
                lastError = "File not found";
                return false;
            }

            if (metadataOpt.value()["uploadedBy"] != userId) {
                lastError = "Only file owner can delete";
                return false;
            }

            if (!fileRepository->deactivateFile(fileId)) {
                lastError = "Failed to deactivate file in database";
                return false;
            }

            logger->info("File deleted (soft): {} user: {}", fileId, userId);
            return true;

        } catch (const std::exception& ex) {
            lastError = std::string("File deletion failed: ") + ex.what();
            logger->error(lastError);
            return false;
        }
    }

    virtual bool validateFileSize(
        const std::string& filePath,
        long maxSizeMB = 50
    ) override {
        try {
            long fileSize = getFileSize(filePath);
            if (fileSize == -1) return false;

            long maxSizeBytes = maxSizeMB * 1024 * 1024;
            return fileSize <= maxSizeBytes;

        } catch (...) {
            return false;
        }
    }

    virtual bool validateFileType(
        const std::string& filePath,
        const std::vector<std::string>& allowedTypes
    ) override {
        try {
            std::string ext = getFileExtension(filePath);
            return std::find(allowedTypes.begin(), allowedTypes.end(), ext) 
                   != allowedTypes.end();
        } catch (...) {
            return false;
        }
    }

    virtual std::string calculateChecksum(const std::string& filePath) override {
        try {
            std::ifstream file(filePath, std::ios::binary);
            if (!file.is_open()) return "";

            unsigned char hash[SHA256_DIGEST_LENGTH] = {0};
            SHA256_CTX sha256;
            SHA256_Init(&sha256);

            char buffer[8192];
            while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
                SHA256_Update(&sha256, buffer, file.gcount());
            }

            SHA256_Final(hash, &sha256);

            std::stringstream ss;
            for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
            }

            return ss.str();

        } catch (...) {
            return "";
        }
    }

    virtual std::string generateSecureFilename(
        const std::string& originalFilename,
        const std::string& userId
    ) override {
        try {
            auto now = std::time(nullptr);
            std::stringstream ss;
            ss << now << "_" << userId.substr(0, 8);
            std::string timestamp = ss.str();

            std::string ext = getFileExtension(originalFilename);
            std::string sanitized;
            for (char c : originalFilename) {
                if (std::isalnum(c) || c == '_' || c == '-') {
                    sanitized += c;
                }
            }

            return timestamp + "_" + sanitized + ext;

        } catch (...) {
            return "file_" + std::to_string(std::time(nullptr));
        }
    }

    virtual json getFilesForContext(
        const std::string& context,
        const std::string& contextId
    ) override {
        try {
            // contextId is essentially uploadedBy for now or related entity id.
            auto files = fileRepository->getFilesByContext(context, contextId);

            logger->debug("Retrieved files for context: {} id: {}", context, contextId);

            return {
                {"success", true},
                {"data", files}
            };

        } catch (const std::exception& ex) {
            return {
                {"success", false},
                {"error", ex.what()}
            };
        }
    }

    virtual std::string getLastError() const override {
        return lastError;
    }
};

}  // namespace services
}  // namespace conference

// Factory function
std::shared_ptr<conference::services::IFileService> createFileService(
    std::shared_ptr<conference::repositories::IFileRepository> fileRepository) {
    return std::make_shared<conference::services::FileService>(fileRepository);
}
