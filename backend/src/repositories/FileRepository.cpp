// #define SPDLOG_HEADER_ONLY
#include "database/ConnectionPool.h"
#include "repositories/IFileRepository.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace conference {
namespace repositories {

class FileRepository : public IFileRepository {
private:
  std::shared_ptr<database::ConnectionPool> connectionPool;
  std::shared_ptr<spdlog::logger> logger;

public:
  FileRepository(std::shared_ptr<database::ConnectionPool> pool)
      : connectionPool(pool) {
    logger = spdlog::get("file_repo");
    if (!logger) {
      try {
        spdlog::sink_ptr sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        logger = std::make_shared<spdlog::logger>("file_repo", sink);
        spdlog::register_logger(logger);
      } catch (...) {
        logger = spdlog::get("conference"); // fallback
      }
    }
  }

  virtual bool insertFileMetadata(const json &metadata) override {
    auto conn = connectionPool->getConnection();
    if (!conn)
      return false;

    std::string query =
        "INSERT INTO files (id, original_name, secure_filename, file_path, "
        "file_size, extension, checksum, uploaded_by, context, is_active, "
        "uploaded_at) "
        "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11)";

    std::vector<std::string> params = {
        metadata["fileId"].get<std::string>(),
        metadata["originalName"].get<std::string>(),
        metadata["secureFilename"].get<std::string>(),
        metadata["destinationPath"].get<std::string>(),
        std::to_string(metadata["size"].get<long>()),
        metadata["extension"].get<std::string>(),
        metadata["checksum"].get<std::string>(),
        metadata["uploadedBy"].get<std::string>(),
        metadata["context"].get<std::string>(),
        metadata["isActive"].get<bool>() ? "true" : "false",
        metadata["uploadedAt"].get<std::string>()};

    bool result = conn->executeWithParams(query, params);
    connectionPool->releaseConnection(conn);
    return result;
  }

  virtual std::optional<json> getFileById(const std::string &fileId) override {
    auto conn = connectionPool->getConnection();
    if (!conn)
      return std::nullopt;

    std::string query = "SELECT * FROM files WHERE id = $1";
    std::vector<std::string> params = {fileId};

    auto results = conn->executeQueryWithParams(query, params);
    connectionPool->releaseConnection(conn);

    if (results.empty())
      return std::nullopt;

    auto row = results[0];
    json fileData = {{"fileId", row["id"]},
                     {"originalName", row["original_name"]},
                     {"secureFilename", row["secure_filename"]},
                     {"filePath", row["file_path"]},
                     {"size", std::stol(row["file_size"].get<std::string>())},
                     {"extension", row["extension"]},
                     {"checksum", row["checksum"]},
                     {"uploadedBy", row["uploaded_by"]},
                     {"context", row["context"]},
                     {"isActive", row["is_active"] == "t" ||
                                      row["is_active"] == "true" ||
                                      row["is_active"] == "1"},
                     {"uploadedAt", row["uploaded_at"]}};
    return fileData;
  }

  virtual bool deactivateFile(const std::string &fileId) override {
    auto conn = connectionPool->getConnection();
    if (!conn)
      return false;

    std::string query = "UPDATE files SET is_active = false WHERE id = $1";
    std::vector<std::string> params = {fileId};

    bool result = conn->executeWithParams(query, params);
    connectionPool->releaseConnection(conn);
    return result;
  }

  virtual std::vector<json>
  getFilesByContext(const std::string &context,
                    const std::string &uploadedBy = "") override {
    auto conn = connectionPool->getConnection();
    if (!conn)
      return {};

    std::string query = "SELECT * FROM files WHERE context = $1";
    std::vector<std::string> params = {context};
    int paramCount = 2;

    if (!uploadedBy.empty()) {
      query += " AND uploaded_by = $" + std::to_string(paramCount++);
      params.push_back(uploadedBy);
    }

    query += " ORDER BY uploaded_at DESC";

    auto results = conn->executeQueryWithParams(query, params);
    connectionPool->releaseConnection(conn);

    std::vector<json> files;
    for (const auto &row : results) {
      json fileData = {
          {"fileId", row.at("id")},
          {"originalName", row.at("original_name")},
          {"secureFilename", row.at("secure_filename")},
          {"filePath", row.at("file_path")},
          {"size", std::stol(row.at("file_size").get<std::string>())},
          {"extension", row.at("extension")},
          {"checksum", row.at("checksum")},
          {"uploadedBy", row.at("uploaded_by")},
          {"context", row.at("context")},
          {"isActive", row.at("is_active") == "t" ||
                           row.at("is_active") == "true" ||
                           row.at("is_active") == "1"},
          {"uploadedAt", row.at("uploaded_at")}};
      files.push_back(fileData);
    }
    return files;
  }
};

} // namespace repositories
} // namespace conference

// Factory function
std::shared_ptr<conference::repositories::IFileRepository> createFileRepository(
    std::shared_ptr<conference::database::ConnectionPool> pool) {
  return std::make_shared<conference::repositories::FileRepository>(pool);
}
