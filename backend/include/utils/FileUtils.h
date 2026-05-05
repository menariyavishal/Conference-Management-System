#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>

namespace conference {
namespace utils {

struct FileInfo {
    std::string fileName;
    std::string filePath;
    long fileSize;
    std::string mimeType;
    std::string extension;
    std::string uploadedAt;
};

class FileUtils {
public:
    // File validation
    static bool validateFile(const std::string& fileName, long fileSize, const std::string& allowedTypes);
    static bool validateFileSize(long fileSize, long maxSize);
    static bool validateFileType(const std::string& fileName, const std::string& allowedTypes);
    static bool validateFileName(const std::string& fileName);
    
    // File operations
    static bool fileExists(const std::string& filePath);
    static bool deleteFile(const std::string& filePath);
    static long getFileSize(const std::string& filePath);
    static std::string getFileExtension(const std::string& fileName);
    static std::string getMimeType(const std::string& fileName);
    
    // File path operations
    static std::string generateSecureFileName(const std::string& originalFileName);
    static std::string createUploadPath(int submissionId, const std::string& fileName);
    static std::string getUploadDirectory();
    static bool createDirectory(const std::string& dirPath);
    static bool directoryExists(const std::string& dirPath);
    
    // File reading/writing
    static std::string readFileAsString(const std::string& filePath);
    static bool writeStringToFile(const std::string& filePath, const std::string& content);
    static std::vector<std::string> readFileAsLines(const std::string& filePath);
    static bool appendToFile(const std::string& filePath, const std::string& content);
    
    // File listing
    static std::vector<std::string> listFiles(const std::string& dirPath);
    static std::vector<FileInfo> listFileInfo(const std::string& dirPath);
    
    // File permissions
    static bool setFilePermissions(const std::string& filePath, int permissions);
    static int getFilePermissions(const std::string& filePath);
    static bool makeFileReadable(const std::string& filePath);
    static bool makeFileWritable(const std::string& filePath);
    
    // File cleanup
    static bool deleteDirectory(const std::string& dirPath);
    static int deleteOldFiles(const std::string& dirPath, int daysOld);
    static int cleanupEmptyDirectories(const std::string& dirPath);
    
    // File checksums
    static std::string calculateFileMD5(const std::string& filePath);
    static std::string calculateFileSHA256(const std::string& filePath);
    
    // File info
    static FileInfo getFileInfo(const std::string& filePath);
    static std::string getFileCreationTime(const std::string& filePath);
    static std::string getFileModificationTime(const std::string& filePath);
    
private:
    static const std::vector<std::string> SAFE_EXTENSIONS;
    static const long DEFAULT_MAX_FILE_SIZE;
    static bool isPathTraversal(const std::string& path);
};

}  // namespace utils
}  // namespace conference

#endif  // FILE_UTILS_H
