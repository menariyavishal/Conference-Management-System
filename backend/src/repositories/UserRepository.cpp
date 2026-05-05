#include "repositories/IUserRepository.h"
#include "database/ConnectionPool.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace conference {
namespace repositories {

class UserRepository : public IUserRepository {
private:
    std::shared_ptr<database::ConnectionPool> connectionPool;
    std::shared_ptr<spdlog::logger> logger;

public:
    UserRepository(std::shared_ptr<database::ConnectionPool> pool) : connectionPool(pool) {
        logger = spdlog::get("conference");
        if (!logger) {
            logger = spdlog::stdout_color_mt("user_repo");
        }
    }

    virtual bool insertUser(const json& user) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "INSERT INTO users (id, email, full_name, password_hash, role, is_active, created_at, updated_at) "
                            "VALUES ($1, $2, $3, $4, $5, $6, $7, $8)";
        
        std::vector<std::string> params = {
            user["userId"].get<std::string>(),
            user["email"].get<std::string>(),
            user["fullName"].get<std::string>(),
            user["passwordHash"].get<std::string>(),
            user["role"].get<std::string>(),
            user["isActive"].get<bool>() ? "true" : "false",
            user["createdAt"].get<std::string>(),
            user["updatedAt"].get<std::string>()
        };

        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual std::optional<json> getUserByEmail(const std::string& email) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return std::nullopt;

        std::string query = "SELECT * FROM users WHERE email = $1";
        std::vector<std::string> params = {email};
        
        auto results = conn->executeQueryWithParams(query, params);
        connectionPool->releaseConnection(conn);

        if (results.empty()) return std::nullopt;

        auto row = results[0];
        json user = {
            {"userId", row["id"]},
            {"email", row["email"]},
            {"fullName", row["full_name"]},
            {"passwordHash", row["password_hash"]},
            {"role", row["role"]},
            {"isActive", row["is_active"] == "t" || row["is_active"] == "true" || row["is_active"] == "1"},
            {"createdAt", row["created_at"]},
            {"updatedAt", row["updated_at"]}
        };
        return user;
    }

    virtual std::optional<json> getUserById(const std::string& id) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return std::nullopt;

        std::string query = "SELECT * FROM users WHERE id = $1";
        std::vector<std::string> params = {id};
        
        auto results = conn->executeQueryWithParams(query, params);
        connectionPool->releaseConnection(conn);

        if (results.empty()) return std::nullopt;

        auto row = results[0];
        json user = {
            {"userId", row["id"]},
            {"email", row["email"]},
            {"fullName", row["full_name"]},
            {"passwordHash", row["password_hash"]},
            {"role", row["role"]},
            {"isActive", row["is_active"] == "t" || row["is_active"] == "true" || row["is_active"] == "1"},
            {"createdAt", row["created_at"]},
            {"updatedAt", row["updated_at"]}
        };
        return user;
    }

    virtual bool updateUserProfile(const std::string& userId, const std::string& fullName, const std::string& email) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "UPDATE users SET full_name = $1, email = $2, updated_at = CURRENT_TIMESTAMP WHERE id = $3";
        std::vector<std::string> params = {fullName, email, userId};
        
        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual bool updatePassword(const std::string& userId, const std::string& newPasswordHash) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "UPDATE users SET password_hash = $1, updated_at = CURRENT_TIMESTAMP WHERE id = $2";
        std::vector<std::string> params = {newPasswordHash, userId};
        
        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual bool updateRole(const std::string& userId, const std::string& newRole) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "UPDATE users SET role = $1, updated_at = CURRENT_TIMESTAMP WHERE id = $2";
        std::vector<std::string> params = {newRole, userId};
        
        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual bool deactivateUser(const std::string& userId) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return false;

        std::string query = "UPDATE users SET is_active = false, updated_at = CURRENT_TIMESTAMP WHERE id = $1";
        std::vector<std::string> params = {userId};
        
        bool result = conn->executeWithParams(query, params);
        connectionPool->releaseConnection(conn);
        return result;
    }

    virtual std::vector<json> getUsers(int limit, int offset, const std::string& roleFilter, const std::string& searchQuery) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return {};

        std::string query = "SELECT * FROM users WHERE 1=1";
        std::vector<std::string> params;
        int paramCount = 1;

        if (!roleFilter.empty()) {
            query += " AND role = $" + std::to_string(paramCount++);
            params.push_back(roleFilter);
        }

        if (!searchQuery.empty()) {
            query += " AND (email ILIKE $" + std::to_string(paramCount) + " OR full_name ILIKE $" + std::to_string(paramCount) + ")";
            params.push_back("%" + searchQuery + "%");
            paramCount++;
        }

        query += " ORDER BY created_at DESC LIMIT $" + std::to_string(paramCount++) + " OFFSET $" + std::to_string(paramCount++);
        params.push_back(std::to_string(limit));
        params.push_back(std::to_string(offset));

        auto results = conn->executeQueryWithParams(query, params);
        connectionPool->releaseConnection(conn);

        std::vector<json> users;
        for (const auto& row : results) {
            json user = {
                {"userId", row.at("id")},
                {"email", row.at("email")},
                {"fullName", row.at("full_name")},
                {"role", row.at("role")},
                {"isActive", row.at("is_active") == "t" || row.at("is_active") == "true" || row.at("is_active") == "1"},
                {"createdAt", row.at("created_at")},
                {"updatedAt", row.at("updated_at")}
            };
            users.push_back(user);
        }
        return users;
    }

    virtual int getUserCount(const std::string& roleFilter, const std::string& searchQuery) override {
        auto conn = connectionPool->getConnection();
        if (!conn) return 0;

        std::string query = "SELECT COUNT(*) FROM users WHERE 1=1";
        std::vector<std::string> params;
        int paramCount = 1;

        if (!roleFilter.empty()) {
            query += " AND role = $" + std::to_string(paramCount++);
            params.push_back(roleFilter);
        }

        if (!searchQuery.empty()) {
            query += " AND (email ILIKE $" + std::to_string(paramCount) + " OR full_name ILIKE $" + std::to_string(paramCount) + ")";
            params.push_back("%" + searchQuery + "%");
            paramCount++;
        }

        auto resultStr = conn->executeScalarWithParams(query, params);
        connectionPool->releaseConnection(conn);

        try {
            return std::stoi(resultStr);
        } catch (...) {
            return 0;
        }
    }

    virtual json getUserRoleDistribution() override {
        auto conn = connectionPool->getConnection();
        if (!conn) return json::object();

        std::string query = "SELECT role, COUNT(*) as count FROM users GROUP BY role";
        auto results = conn->executeQueryWithParams(query, {});
        connectionPool->releaseConnection(conn);

        json stats = json::object();
        for (const auto& row : results) {
            try {
                stats[row.at("role").get<std::string>()] = std::stoi(row.at("count").get<std::string>());
            } catch (...) {
                stats[row.at("role").get<std::string>()] = 0;
            }
        }
        return stats;
    }
};

} // namespace repositories
} // namespace conference

// Factory function
std::shared_ptr<conference::repositories::IUserRepository> createUserRepository(std::shared_ptr<conference::database::ConnectionPool> pool) {
    return std::make_shared<conference::repositories::UserRepository>(pool);
}
