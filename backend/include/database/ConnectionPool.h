#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include "database/DatabaseConnection.h"
#include <queue>
#include <vector>
#include <string>
// #include <mutex>
#include <memory>
#include <chrono>

namespace conference {
namespace database {

struct PooledConnection {
    std::shared_ptr<DatabaseConnection> connection;
    bool inUse;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point lastUsedAt;
};

class ConnectionPool {
private:
    std::string connectionString;
    int poolSize;
    int maxPoolSize;
    int minPoolSize;
    int connectionTimeout;
    int maxIdleTime;
    
    std::queue<std::shared_ptr<DatabaseConnection>> availableConnections;
    std::vector<PooledConnection> allConnections;
    // mutable std::mutex poolMutex;
    
    bool isInitialized;
    int failedConnections;

public:
    ConnectionPool(int initialSize = 5, int maxSize = 20);
    ~ConnectionPool();

    // Initialize pool
    bool initialize(const std::string& connStr);
    bool shutdown();
    
    // Get/release connections
    std::shared_ptr<DatabaseConnection> getConnection();
    bool releaseConnection(std::shared_ptr<DatabaseConnection> conn);
    
    // Pool management
    int getAvailableConnectionCount() const;
    int getUsedConnectionCount() const;
    int getTotalConnectionCount() const;
    int getPoolSize() const;
    int getMaxPoolSize() const;
    
    // Configuration
    void setPoolSize(int size);
    void setMaxPoolSize(int size);
    void setConnectionTimeout(int seconds);
    void setMaxIdleTime(int seconds);
    
    // Connection validation
    bool validateConnection(std::shared_ptr<DatabaseConnection> conn);
    void removeStaleConnections();
    
    // Pool health
    bool isHealthy() const;
    std::string getPoolStatus() const;
    int getFailedConnectionCount() const;
    
    // Clear/drain pool
    bool drainPool();
    bool clearPool();
    
    // Resize pool
    bool resizePool(int newSize);

private:
    std::shared_ptr<DatabaseConnection> createConnection();
    bool pingConnection(std::shared_ptr<DatabaseConnection> conn);
    void removeConnection(std::shared_ptr<DatabaseConnection> conn);
    void refillPool();
    std::shared_ptr<DatabaseConnection> waitForAvailableConnection();
};

}  // namespace database
}  // namespace conference

#endif  // CONNECTION_POOL_H
