#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "database/DatabaseConnection.h"
#include <memory>
#include <vector>


namespace conference {
namespace database {

class Transaction {
private:
    std::shared_ptr<DatabaseConnection> connection;
    bool isActive;
    std::string transactionId;
    bool autoRollbackOnError;

public:
    // Constructor and destructor
    explicit Transaction(std::shared_ptr<DatabaseConnection> conn, bool autoRollback = true);
    ~Transaction();

    // Transaction control
    bool begin();
    bool commit();
    bool rollback();
    bool isInTransaction() const;
    
    // Savepoint support
    bool createSavepoint(const std::string& name);
    bool rollbackToSavepoint(const std::string& name);
    bool releaseSavepoint(const std::string& name);
    
    // Query execution within transaction
    bool execute(const std::string& query);
    bool executeWithParams(const std::string& query, const std::vector<std::string>& params);
    std::vector<json> executeQuery(const std::string& query);
    std::vector<json> executeQueryWithParams(const std::string& query, const std::vector<std::string>& params);
    
    // Single row/scalar queries
    json executeSingleRow(const std::string& query);
    std::string executeScalar(const std::string& query);
    
    // Transaction isolation levels
    enum class IsolationLevel {
        READ_UNCOMMITTED,
        READ_COMMITTED,
        REPEATABLE_READ,
        SERIALIZABLE
    };
    
    bool setIsolationLevel(IsolationLevel level);
    
    // Get transaction info
    std::string getTransactionId() const;
    std::string getLastError() const;
    
    // Enable/disable auto rollback
    void setAutoRollback(bool enable);
    
    // Get underlying connection
    std::shared_ptr<DatabaseConnection> getConnection() const;

private:
    std::string isolationLevelToString(IsolationLevel level) const;
    void handleError(const std::string& context);
};

// RAII wrapper for automatic transaction management
class TransactionScope {
private:
    std::shared_ptr<Transaction> transaction;
    bool isCommitted;

public:
    explicit TransactionScope(std::shared_ptr<DatabaseConnection> conn);
    ~TransactionScope();
    
    bool commit();
    void rollback();
    bool isInTransaction() const;
    
    std::shared_ptr<Transaction> getTransaction() const;
};

}  // namespace database
}  // namespace conference

#endif  // TRANSACTION_H
