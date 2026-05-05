#include "database/DatabaseConnection.h"

namespace conference {
namespace database {

DatabaseConnection::DatabaseConnection()
    : connection(nullptr), isConnected(false), retryCount(0), maxRetries(3) {
}

DatabaseConnection::~DatabaseConnection() {
    disconnect();
}

bool DatabaseConnection::connect(const std::string& connectionString) {
    if (connectionString.empty()) {
        lastError = "Connection string cannot be empty";
        return false;
    }

    connection = PQconnectdb(connectionString.c_str());
    
    if (PQstatus(connection) != CONNECTION_OK) {
        lastError = PQerrorMessage(connection);
        return retryConnect(connectionString);
    }

    isConnected = true;
    retryCount = 0;
    return true;
}

bool DatabaseConnection::disconnect() {
    if (connection != nullptr) {
        PQfinish(connection);
        connection = nullptr;
        isConnected = false;
    }
    return true;
}

bool DatabaseConnection::isOpen() const {
    return isConnected && connection != nullptr && PQstatus(connection) == CONNECTION_OK;
}

std::string DatabaseConnection::getLastError() const {
    return lastError;
}

void DatabaseConnection::clearError() {
    lastError.clear();
}

bool DatabaseConnection::execute(const std::string& query) {
    if (!isOpen()) {
        lastError = "Database connection is not open";
        return false;
    }

    PGresult* result = PQexec(connection, query.c_str());
    
    if (PQresultStatus(result) != PGRES_COMMAND_OK && 
        PQresultStatus(result) != PGRES_TUPLES_OK) {
        lastError = PQerrorMessage(connection);
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}

bool DatabaseConnection::executeWithParams(const std::string& query, const std::vector<std::string>& params) {
    if (!isOpen()) {
        lastError = "Database connection is not open";
        return false;
    }

    std::vector<const char*> paramValues;
    paramValues.reserve(params.size());
    for (const auto& param : params) {
        paramValues.push_back(param.c_str());
    }

    PGresult* result = PQexecParams(
        connection, 
        query.c_str(), 
        static_cast<int>(params.size()), 
        nullptr, 
        paramValues.data(), 
        nullptr, 
        nullptr, 
        0
    );
    
    if (PQresultStatus(result) != PGRES_COMMAND_OK && 
        PQresultStatus(result) != PGRES_TUPLES_OK) {
        lastError = PQerrorMessage(connection);
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}

std::vector<json> DatabaseConnection::executeQuery(const std::string& query) {
    std::vector<json> results;
    
    if (!isOpen()) {
        lastError = "Database connection is not open";
        return results;
    }

    PGresult* result = PQexec(connection, query.c_str());
    
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        lastError = PQerrorMessage(connection);
        PQclear(result);
        return results;
    }

    results = convertResultToJSON(result);
    PQclear(result);
    return results;
}

std::vector<json> DatabaseConnection::executeQueryWithParams(const std::string& query, 
                                                             const std::vector<std::string>& params) {
    std::vector<json> results;
    
    if (!isOpen()) {
        lastError = "Database connection is not open";
        return results;
    }

    std::vector<const char*> paramValues;
    paramValues.reserve(params.size());
    for (const auto& param : params) {
        paramValues.push_back(param.c_str());
    }

    PGresult* result = PQexecParams(
        connection, 
        query.c_str(), 
        static_cast<int>(params.size()), 
        nullptr, 
        paramValues.data(), 
        nullptr, 
        nullptr, 
        0
    );
    
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        lastError = PQerrorMessage(connection);
        PQclear(result);
        return results;
    }

    results = convertResultToJSON(result);
    PQclear(result);
    return results;
}

json DatabaseConnection::executeSingleRow(const std::string& query) {
    auto results = executeQuery(query);
    if (!results.empty()) {
        return results[0];
    }
    return json::object();
}

json DatabaseConnection::executeSingleRowWithParams(const std::string& query, 
                                                    const std::vector<std::string>& params) {
    auto results = executeQueryWithParams(query, params);
    if (!results.empty()) {
        return results[0];
    }
    return json::object();
}

std::string DatabaseConnection::executeScalar(const std::string& query) {
    auto results = executeQuery(query);
    if (!results.empty() && !results[0].empty()) {
        // Access the first value in the object regardless of its key
        return results[0].begin().value().get<std::string>();
    }
    return "";
}

std::string DatabaseConnection::executeScalarWithParams(const std::string& query, 
                                                        const std::vector<std::string>& params) {
    auto results = executeQueryWithParams(query, params);
    if (!results.empty() && !results[0].empty()) {
        // Access the first value in the object regardless of its key
        return results[0].begin().value().get<std::string>();
    }
    return "";
}

int DatabaseConnection::getRowCount(const std::string& query) {
    auto results = executeQuery(query);
    return results.size();
}

bool DatabaseConnection::ping() {
    if (!isOpen()) return false;
    PGresult* result = PQexec(connection, "SELECT 1");
    bool ok = (PQresultStatus(result) == PGRES_TUPLES_OK);
    PQclear(result);
    return ok;
}

bool DatabaseConnection::reconnect() {
    if (connection != nullptr) {
        PQreset(connection);
        return isOpen();
    }
    return false;
}

PGconn* DatabaseConnection::getConnection() const {
    return connection;
}

bool DatabaseConnection::retryConnect(const std::string& connectionString) {
    if (retryCount >= maxRetries) {
        return false;
    }
    retryCount++;
    return connect(connectionString);
}

json DatabaseConnection::convertResultToJSON(PGresult* res) const {
    json results = json::array();
    int nrows = PQntuples(res);
    int nfields = PQnfields(res);

    for (int i = 0; i < nrows; i++) {
        json row = json::object();
        for (int j = 0; j < nfields; j++) {
            const char* fname = PQfname(res, j);
            const char* value = PQgetvalue(res, i, j);
            row[fname] = value ? std::string(value) : nullptr;
        }
        results.push_back(row);
    }

    return results;
}

std::string DatabaseConnection::escapeString(const std::string& str) const {
    // TODO: Implement proper string escaping
    return str;
}

}  // namespace database
}  // namespace conference
