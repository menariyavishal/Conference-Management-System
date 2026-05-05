#include "database/ConnectionPool.h"
#include <chrono>

namespace conference {
namespace database {

ConnectionPool::ConnectionPool(int initialSize, int maxSize)
    : poolSize(initialSize), maxPoolSize(maxSize), minPoolSize(initialSize),
      connectionTimeout(30), maxIdleTime(300), isInitialized(false), failedConnections(0) {
}

ConnectionPool::~ConnectionPool() {
    shutdown();
}

bool ConnectionPool::initialize(const std::string& connStr) {
    // std::lock_guard<std::mutex> lock(poolMutex);
    
    connectionString = connStr;
    
    // Create initial connections
    for (int i = 0; i < poolSize; i++) {
        auto conn = createConnection();
        if (conn && conn->isOpen()) {
            availableConnections.push(conn);
            allConnections.push_back({conn, false, std::chrono::system_clock::now(), std::chrono::system_clock::now()});
        } else {
            failedConnections++;
        }
    }
    
    isInitialized = (availableConnections.size() > 0);
    return isInitialized;
}

bool ConnectionPool::shutdown() {
    // std::lock_guard<std::mutex> lock(poolMutex);
    
    while (!availableConnections.empty()) {
        auto conn = availableConnections.front();
        availableConnections.pop();
        conn->disconnect();
    }
    
    allConnections.clear();
    isInitialized = false;
    return true;
}

std::shared_ptr<DatabaseConnection> ConnectionPool::getConnection() {
    // std::lock_guard<std::mutex> lock(poolMutex);
    
    if (availableConnections.empty()) {
        if (allConnections.size() < static_cast<size_t>(maxPoolSize)) {
            auto conn = createConnection();
            if (conn) {
                return conn;
            }
        }
        return nullptr;
    }
    
    auto conn = availableConnections.front();
    availableConnections.pop();
    
    // Validate connection
    if (!validateConnection(conn)) {
        conn = createConnection();
    }
    
    return conn;
}

bool ConnectionPool::releaseConnection(std::shared_ptr<DatabaseConnection> conn) {
    // std::lock_guard<std::mutex> lock(poolMutex);
    
    if (conn && conn->isOpen()) {
        availableConnections.push(conn);
        return true;
    }
    return false;
}

int ConnectionPool::getAvailableConnectionCount() const {
    // std::lock_guard<std::mutex> lock(poolMutex);
    return availableConnections.size();
}

int ConnectionPool::getUsedConnectionCount() const {
    // std::lock_guard<std::mutex> lock(poolMutex);
    return allConnections.size() - availableConnections.size();
}

int ConnectionPool::getTotalConnectionCount() const {
    // std::lock_guard<std::mutex> lock(poolMutex);
    return allConnections.size();
}

int ConnectionPool::getPoolSize() const {
    return poolSize;
}

int ConnectionPool::getMaxPoolSize() const {
    return maxPoolSize;
}

void ConnectionPool::setPoolSize(int size) {
    poolSize = size;
}

void ConnectionPool::setMaxPoolSize(int size) {
    maxPoolSize = size;
}

void ConnectionPool::setConnectionTimeout(int seconds) {
    connectionTimeout = seconds;
}

void ConnectionPool::setMaxIdleTime(int seconds) {
    maxIdleTime = seconds;
}

bool ConnectionPool::validateConnection(std::shared_ptr<DatabaseConnection> conn) {
    if (!conn) return false;
    return conn->ping();
}

void ConnectionPool::removeStaleConnections() {
    // std::lock_guard<std::mutex> lock(poolMutex);
    
    auto now = std::chrono::system_clock::now();
    auto it = allConnections.begin();
    
    while (it != allConnections.end()) {
        auto idleTime = std::chrono::duration_cast<std::chrono::seconds>(
            now - it->lastUsedAt).count();
        
        if (idleTime > maxIdleTime && !it->inUse) {
            it->connection->disconnect();
            it = allConnections.erase(it);
        } else {
            ++it;
        }
    }
}

bool ConnectionPool::isHealthy() const {
    return failedConnections < (maxPoolSize / 2);
}

std::string ConnectionPool::getPoolStatus() const {
    return "Pool Size: " + std::to_string(getTotalConnectionCount()) + 
           "/" + std::to_string(maxPoolSize);
}

int ConnectionPool::getFailedConnectionCount() const {
    return failedConnections;
}

bool ConnectionPool::drainPool() {
    return shutdown() && initialize(connectionString);
}

bool ConnectionPool::clearPool() {
    return shutdown();
}

bool ConnectionPool::resizePool(int newSize) {
    if (newSize < minPoolSize || newSize > maxPoolSize) {
        return false;
    }
    poolSize = newSize;
    return true;
}

std::shared_ptr<DatabaseConnection> ConnectionPool::createConnection() {
    auto conn = std::make_shared<DatabaseConnection>();
    if (conn->connect(connectionString)) {
        return conn;
    }
    failedConnections++;
    return nullptr;
}

bool ConnectionPool::pingConnection(std::shared_ptr<DatabaseConnection> conn) {
    return conn && conn->ping();
}

void ConnectionPool::removeConnection(std::shared_ptr<DatabaseConnection> conn) {
    // std::lock_guard<std::mutex> lock(poolMutex);
    conn->disconnect();
    auto it = std::find_if(allConnections.begin(), allConnections.end(),
        [conn](const PooledConnection& pc) { return pc.connection == conn; });
    if (it != allConnections.end()) {
        allConnections.erase(it);
    }
}

void ConnectionPool::refillPool() {
    // std::lock_guard<std::mutex> lock(poolMutex);
    
    while (allConnections.size() < static_cast<size_t>(poolSize)) {
        auto conn = createConnection();
        if (conn) {
            availableConnections.push(conn);
        }
    }
}

std::shared_ptr<DatabaseConnection> ConnectionPool::waitForAvailableConnection() {
    for (int i = 0; i < 10; i++) {
        auto conn = getConnection();
        if (conn) return conn;
        /* Sleep(100) */
    }
    return nullptr;
}

}  // namespace database
}  // namespace conference
