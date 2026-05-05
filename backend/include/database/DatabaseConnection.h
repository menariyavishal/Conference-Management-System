#ifndef DATABASE_CONNECTION_H
#define DATABASE_CONNECTION_H

#include <string>
#include <vector>
#include <libpq-fe.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace database {

class DatabaseConnection {
private:
    PGconn* connection;
    bool isConnected;
    std::string lastError;
    int retryCount;
    int maxRetries;

public:
    DatabaseConnection();
    ~DatabaseConnection();

    // Connection management
    bool connect(const std::string& connectionString);
    bool disconnect();
    bool isOpen() const;
    
    // Connection info
    std::string getConnectionString() const;
    std::string getDatabase() const;
    std::string getUser() const;
    std::string getHost() const;
    int getPort() const;
    
    // Error handling
    std::string getLastError() const;
    void clearError();
    
    // Query execution
    bool execute(const std::string& query);
    bool executeWithParams(const std::string& query, const std::vector<std::string>& params);
    std::vector<json> executeQuery(const std::string& query);
    std::vector<json> executeQueryWithParams(const std::string& query, const std::vector<std::string>& params);
    
    // Single row retrieval
    json executeSingleRow(const std::string& query);
    json executeSingleRowWithParams(const std::string& query, const std::vector<std::string>& params);
    
    // Single scalar value retrieval
    std::string executeScalar(const std::string& query);
    std::string executeScalarWithParams(const std::string& query, const std::vector<std::string>& params);
    
    // Row count
    int getRowCount(const std::string& query);
    
    // Prepared statements
    bool prepareBatch(const std::string& query);
    bool addBatchParam(const std::vector<std::string>& params);
    std::vector<json> executeBatch();
    
    // Connection testing
    bool ping();
    bool reconnect();
    
    // Get underlying connection (for advanced usage)
    PGconn* getConnection() const;

private:
    bool retryConnect(const std::string& connectionString);
    json convertResultToJSON(PGresult* res) const;
    std::string escapeString(const std::string& str) const;
    void handleError(const std::string& context);
};

}  // namespace database
}  // namespace conference

#endif  // DATABASE_CONNECTION_H
