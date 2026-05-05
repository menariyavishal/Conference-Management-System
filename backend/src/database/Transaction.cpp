#include "database/Transaction.h"
#include <nlohmann/json.hpp>
#include <ctime>

using json = nlohmann::json;

namespace conference {
namespace database {

Transaction::Transaction(std::shared_ptr<DatabaseConnection> conn, bool autoRollback)
    : connection(conn), isActive(false), autoRollbackOnError(autoRollback) {
    transactionId = "txn_" + std::to_string(std::time(nullptr));
}

Transaction::~Transaction() {
    if (isActive && autoRollbackOnError) {
        rollback();
    }
}

bool Transaction::begin() {
    if (!connection || !connection->isOpen()) {
        return false;
    }
    
    if (connection->execute("BEGIN")) {
        isActive = true;
        return true;
    }
    return false;
}

bool Transaction::commit() {
    if (!isActive) {
        return false;
    }
    
    if (connection->execute("COMMIT")) {
        isActive = false;
        return true;
    }
    return false;
}

bool Transaction::rollback() {
    if (!isActive) {
        return false;
    }
    
    if (connection->execute("ROLLBACK")) {
        isActive = false;
        return true;
    }
    return false;
}

bool Transaction::isInTransaction() const {
    return isActive;
}

bool Transaction::createSavepoint(const std::string& name) {
    if (!isActive) return false;
    return connection->execute("SAVEPOINT " + name);
}

bool Transaction::rollbackToSavepoint(const std::string& name) {
    if (!isActive) return false;
    return connection->execute("ROLLBACK TO SAVEPOINT " + name);
}

bool Transaction::releaseSavepoint(const std::string& name) {
    if (!isActive) return false;
    return connection->execute("RELEASE SAVEPOINT " + name);
}

bool Transaction::execute(const std::string& query) {
    if (!isActive) return false;
    return connection->execute(query);
}

bool Transaction::executeWithParams(const std::string& query, const std::vector<std::string>& params) {
    if (!isActive) return false;
    return connection->executeWithParams(query, params);
}

std::vector<json> Transaction::executeQuery(const std::string& query) {
    if (!isActive) return std::vector<json>();
    return connection->executeQuery(query);
}

std::vector<json> Transaction::executeQueryWithParams(const std::string& query, 
                                                      const std::vector<std::string>& params) {
    if (!isActive) return std::vector<json>();
    return connection->executeQueryWithParams(query, params);
}

json Transaction::executeSingleRow(const std::string& query) {
    if (!isActive) return json::object();
    return connection->executeSingleRow(query);
}

std::string Transaction::executeScalar(const std::string& query) {
    if (!isActive) return "";
    return connection->executeScalar(query);
}

bool Transaction::setIsolationLevel(IsolationLevel level) {
    if (!isActive) return false;
    std::string levelStr = isolationLevelToString(level);
    return connection->execute("SET TRANSACTION ISOLATION LEVEL " + levelStr);
}

std::string Transaction::getTransactionId() const {
    return transactionId;
}

std::string Transaction::getLastError() const {
    return connection ? connection->getLastError() : "";
}

void Transaction::setAutoRollback(bool enable) {
    autoRollbackOnError = enable;
}

std::shared_ptr<DatabaseConnection> Transaction::getConnection() const {
    return connection;
}

std::string Transaction::isolationLevelToString(IsolationLevel level) const {
    switch (level) {
        case IsolationLevel::READ_UNCOMMITTED:
            return "READ UNCOMMITTED";
        case IsolationLevel::READ_COMMITTED:
            return "READ COMMITTED";
        case IsolationLevel::REPEATABLE_READ:
            return "REPEATABLE READ";
        case IsolationLevel::SERIALIZABLE:
            return "SERIALIZABLE";
        default:
            return "READ COMMITTED";
    }
}

void Transaction::handleError(const std::string& context) {
    // TODO: Implement error logging
}

// TransactionScope Implementation
TransactionScope::TransactionScope(std::shared_ptr<DatabaseConnection> conn)
    : transaction(std::make_shared<Transaction>(conn)), isCommitted(false) {
    transaction->begin();
}

TransactionScope::~TransactionScope() {
    if (transaction && transaction->isInTransaction() && !isCommitted) {
        transaction->rollback();
    }
}

bool TransactionScope::commit() {
    if (transaction) {
        isCommitted = transaction->commit();
        return isCommitted;
    }
    return false;
}

void TransactionScope::rollback() {
    if (transaction) {
        transaction->rollback();
        isCommitted = false;
    }
}

bool TransactionScope::isInTransaction() const {
    return transaction && transaction->isInTransaction();
}

std::shared_ptr<Transaction> TransactionScope::getTransaction() const {
    return transaction;
}

}  // namespace database
}  // namespace conference
