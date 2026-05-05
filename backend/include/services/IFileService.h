#ifndef I_FILE_SERVICE_H
#define I_FILE_SERVICE_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace services {

/**
 * @class IFileService
 * @brief Interface for file management operations
 * 
 * Provides services for:
 * - File upload with validation
 * - File download with access control
 * - File deletion (soft delete)
 * - File checksum/hash generation
 * - File type and size validation
 * - Secure filename generation
 */
class IFileService {
public:
    virtual ~IFileService() = default;

    /**
     * @brief Upload file with validation
     * @param sourceFilePath Path to file to upload
     * @param userId User uploading the file
     * @param context Context (submission, review, etc)
     * @return JSON with fileId and file metadata
     * @throws FileUploadException if validation fails
     */
    virtual json uploadFile(
        const std::string& sourceFilePath,
        const std::string& userId,
        const std::string& context = "submission"
    ) = 0;

    /**
     * @brief Download file with access control
     * @param fileId File ID
     * @param userId User requesting download
     * @param outputPath Path to save downloaded file
     * @return true if download successful
     * @throws UnauthorizedException if user cannot access file
     */
    virtual bool downloadFile(
        const std::string& fileId,
        const std::string& userId,
        std::string& outputPath
    ) = 0;

    /**
     * @brief Get file information
     * @param fileId File ID
     * @return JSON with file metadata (name, size, type, owner, uploaded date)
     * @throws FileNotFoundException if file not found
     */
    virtual json getFileInfo(const std::string& fileId) = 0;

    /**
     * @brief Delete file (soft delete)
     * @param fileId File ID
     * @param userId User requesting deletion
     * @return true if deletion successful
     * @throws UnauthorizedException if user not file owner
     */
    virtual bool deleteFile(
        const std::string& fileId,
        const std::string& userId
    ) = 0;

    /**
     * @brief Validate file size
     * @param filePath Path to file
     * @param maxSizeMB Maximum allowed size in MB
     * @return true if file size within limit
     */
    virtual bool validateFileSize(
        const std::string& filePath,
        long maxSizeMB = 50
    ) = 0;

    /**
     * @brief Validate file type
     * @param filePath Path to file
     * @param allowedTypes List of allowed MIME types/extensions
     * @return true if file type is allowed
     */
    virtual bool validateFileType(
        const std::string& filePath,
        const std::vector<std::string>& allowedTypes
    ) = 0;

    /**
     * @brief Calculate SHA256 checksum of file
     * @param filePath Path to file
     * @return SHA256 hash as hex string
     */
    virtual std::string calculateChecksum(const std::string& filePath) = 0;

    /**
     * @brief Generate secure filename from original
     * @param originalFilename Original filename
     * @param userId User uploading file
     * @return Secure filename (sanitized, unique)
     */
    virtual std::string generateSecureFilename(
        const std::string& originalFilename,
        const std::string& userId
    ) = 0;

    /**
     * @brief Get list of files for context
     * @param context Context (submission, review, etc)
     * @param contextId Context ID (submission ID, etc)
     * @return JSON array of files
     */
    virtual json getFilesForContext(
        const std::string& context,
        const std::string& contextId
    ) = 0;

    /**
     * @brief Get last error message
     * @return Error message from last operation
     */
    virtual std::string getLastError() const = 0;
};

} // namespace services

// Forward declarations for repositories
namespace repositories {
    class IFileRepository;
}

} // namespace conference

std::shared_ptr<conference::services::IFileService>
createFileService(
    std::shared_ptr<conference::repositories::IFileRepository> fileRepo
);

#endif  // I_FILE_SERVICE_H
