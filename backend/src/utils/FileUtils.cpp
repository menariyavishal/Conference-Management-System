#include "utils/FileUtils.h"
#include "utils/EncryptionUtils.h"
#include <fstream>
#include <algorithm>
#include <map>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <sys/stat.h>
#include <direct.h>
#include <io.h>

namespace conference {
namespace utils {

const std::vector<std::string> FileUtils::SAFE_EXTENSIONS = {
    ".pdf", ".doc", ".docx", ".txt", ".xls", ".xlsx", ".ppt", ".pptx", ".jpg", ".png", ".zip"
};

const long FileUtils::DEFAULT_MAX_FILE_SIZE = 52428800; // 50MB

bool FileUtils::validateFile(const std::string& fileName, long fileSize, const std::string& allowedTypes) {
    return validateFileName(fileName) && validateFileSize(fileSize, DEFAULT_MAX_FILE_SIZE) 
           && validateFileType(fileName, allowedTypes);
}

bool FileUtils::validateFileSize(long fileSize, long maxSize) {
    return fileSize > 0 && fileSize <= maxSize;
}

bool FileUtils::validateFileType(const std::string& fileName, const std::string& allowedTypes) {
    std::string ext = getFileExtension(fileName);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return allowedTypes.find(ext) != std::string::npos;
}

bool FileUtils::validateFileName(const std::string& fileName) {
    if (fileName.empty() || fileName.length() > 255) return false;
    // Check for dangerous characters
    return fileName.find("..") == std::string::npos && fileName.find("/") == std::string::npos;
}

bool FileUtils::fileExists(const std::string& filePath) {
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
}

bool FileUtils::deleteFile(const std::string& filePath) {
    return remove(filePath.c_str()) == 0;
}

long FileUtils::getFileSize(const std::string& filePath) {
    struct stat buffer;
    if (stat(filePath.c_str(), &buffer) != 0) return -1;
    return (long)buffer.st_size;
}

std::string FileUtils::getFileExtension(const std::string& fileName) {
    size_t pos = fileName.find_last_of(".");
    return (pos == std::string::npos) ? "" : fileName.substr(pos);
}

std::string FileUtils::getMimeType(const std::string& fileName) {
    std::string ext = getFileExtension(fileName);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    static const std::map<std::string, std::string> mimeTypes = {
        {".pdf", "application/pdf"},
        {".doc", "application/msword"},
        {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
        {".txt", "text/plain"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".png", "image/png"},
        {".gif", "image/gif"},
        {".zip", "application/zip"}
    };
    
    auto it = mimeTypes.find(ext);
    return (it != mimeTypes.end()) ? it->second : "application/octet-stream";
}

std::string FileUtils::generateSecureFileName(const std::string& originalFileName) {
    size_t lastSlash = originalFileName.find_last_of("/\\");
    std::string filename = (lastSlash != std::string::npos) 
                          ? originalFileName.substr(lastSlash + 1) 
                          : originalFileName;
    
    std::string safe;
    for (char c : filename) {
        if (std::isalnum(c) || c == '.' || c == '-' || c == '_') {
            safe += c;
        } else if (c == ' ') {
            safe += '_';
        }
    }
    
    long timestamp = std::time(nullptr);
    return std::to_string(timestamp) + "_" + safe;
}

std::string FileUtils::createUploadPath(int submissionId, const std::string& fileName) {
    std::string uploadDir = getUploadDirectory() + "/" + std::to_string(submissionId);
    createDirectory(uploadDir);
    return uploadDir + "/" + generateSecureFileName(fileName);
}

std::string FileUtils::getUploadDirectory() {
    return "./uploads";
}

bool FileUtils::createDirectory(const std::string& dirPath) {
    struct stat buffer;
    if (stat(dirPath.c_str(), &buffer) == 0) return true;
    return _mkdir(dirPath.c_str()) == 0;
}

bool FileUtils::directoryExists(const std::string& dirPath) {
    struct stat buffer;
    return (stat(dirPath.c_str(), &buffer) == 0) && S_ISDIR(buffer.st_mode);
}

std::string FileUtils::readFileAsString(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) return "";
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

bool FileUtils::writeStringToFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) return false;
    file.write(content.c_str(), content.length());
    return true;
}

std::vector<std::string> FileUtils::readFileAsLines(const std::string& filePath) {
    std::vector<std::string> lines;
    std::ifstream file(filePath);
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

bool FileUtils::appendToFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath, std::ios::app);
    if (!file.is_open()) return false;
    file.write(content.c_str(), content.length());
    return true;
}

std::vector<std::string> FileUtils::listFiles(const std::string& dirPath) {
    std::vector<std::string> files;
    // TODO: Implement using Windows API or POSIX scandir
    return files;
}

std::vector<FileInfo> FileUtils::listFileInfo(const std::string& dirPath) {
    std::vector<FileInfo> fileInfos;
    // TODO: Implement
    return fileInfos;
}

bool FileUtils::setFilePermissions(const std::string& filePath, int permissions) {
    // Windows doesn't have traditional file permissions like Unix
    return true;
}

int FileUtils::getFilePermissions(const std::string& filePath) {
    // Windows doesn't have traditional file permissions
    return 0755;
}

bool FileUtils::makeFileReadable(const std::string& filePath) {
    return true;
}

bool FileUtils::makeFileWritable(const std::string& filePath) {
    return true;
}

bool FileUtils::deleteDirectory(const std::string& dirPath) {
    return _rmdir(dirPath.c_str()) == 0;
}

int FileUtils::deleteOldFiles(const std::string& dirPath, int daysOld) {
    // TODO: Implement
    return 0;
}

int FileUtils::cleanupEmptyDirectories(const std::string& dirPath) {
    // TODO: Implement
    return 0;
}

std::string FileUtils::calculateFileMD5(const std::string& filePath) {
    // TODO: Implement MD5
    return "";
}

std::string FileUtils::calculateFileSHA256(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) return "";
    
    SHA256_CTX context;
    SHA256_Init(&context);
    
    const int BUFFER_SIZE = 65536;
    char buffer[BUFFER_SIZE];
    
    while (file.read(buffer, BUFFER_SIZE) || file.gcount() > 0) {
        SHA256_Update(&context, buffer, file.gcount());
    }
    
    file.close();
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &context);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    
    return ss.str();
}

FileInfo FileUtils::getFileInfo(const std::string& filePath) {
    FileInfo info;
    struct stat buffer;
    
    if (stat(filePath.c_str(), &buffer) == 0) {
        info.filePath = filePath;
        info.fileSize = buffer.st_size;
        size_t lastSlash = filePath.find_last_of("/\\");
        info.fileName = (lastSlash != std::string::npos) ? filePath.substr(lastSlash + 1) : filePath;
        info.extension = getFileExtension(filePath);
        info.mimeType = getMimeType(filePath);
        // info.uploadedAt = ... (TODO)
    }
    
    return info;
}

std::string FileUtils::getFileCreationTime(const std::string& filePath) {
    // TODO: Implement
    return "";
}

std::string FileUtils::getFileModificationTime(const std::string& filePath) {
    // TODO: Implement
    return "";
}

bool FileUtils::isPathTraversal(const std::string& path) {
    return path.find("..") != std::string::npos;
}

}  // namespace utils
}  // namespace conference
