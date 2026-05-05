#include "database/DatabaseMigration.h"
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;
namespace conference {
namespace database {

DatabaseMigration::DatabaseMigration(std::shared_ptr<DatabaseConnection> conn)
    : connection(conn), migrationsDirectory("./migrations") {
}

DatabaseMigration::~DatabaseMigration() {
}

bool DatabaseMigration::initialize() {
    if (!connection || !connection->isOpen()) {
        return false;
    }

    // Create migrations table if it doesn't exist
    return createMigrationsTable();
}

bool DatabaseMigration::migrate() {
    if (!connection || !connection->isOpen()) {
        return false;
    }

    auto pendingMigrations = getPendingMigrations();
    
    for (const auto& migration : pendingMigrations) {
        auto startTime = std::chrono::system_clock::now();
        
        if (!executeMigrationFile(migrationsDirectory + "/" + migration)) {
            return false;
        }
        
        auto endTime = std::chrono::system_clock::now();
        auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - startTime).count();
        
        addMigrationRecord(migration, executionTime);
    }
    
    return true;
}

bool DatabaseMigration::migrateToVersion(const std::string& /*version*/) {
    // TODO: Implement version-specific migration
    return false;
}

bool DatabaseMigration::rollback() {
    if (!appliedMigrations.empty()) {
        auto lastMigration = appliedMigrations.back();
        removeMigrationRecord(lastMigration.name);
        appliedMigrations.pop_back();
        return true;
    }
    return false;
}

bool DatabaseMigration::rollbackCount(int count) {
    for (int i = 0; i < count && !appliedMigrations.empty(); i++) {
        rollback();
    }
    return true;
}

bool DatabaseMigration::rollbackToVersion(const std::string& /*version*/) {
    // TODO: Implement rollback to specific version
    return false;
}

std::vector<MigrationRecord> DatabaseMigration::getAppliedMigrations() {
    if (!connection || !connection->isOpen()) {
        return appliedMigrations;
    }

    auto results = connection->executeQuery(
        "SELECT id, migration_name, applied_at, execution_time_ms FROM database_migrations ORDER BY applied_at ASC"
    );
    
    appliedMigrations.clear();
    for (const auto& row : results) {
        MigrationRecord record;
        record.id = row["id"];
        record.name = row["migration_name"];
        record.appliedAt = row["applied_at"];
        record.executionTimeMs = row["execution_time_ms"];
        appliedMigrations.push_back(record);
    }
    
    return appliedMigrations;
}

std::vector<std::string> DatabaseMigration::getPendingMigrations() {
    std::vector<std::string> pending;
    
    if (!fs::exists(migrationsDirectory)) {
        return pending;
    }

    for (const auto& entry : fs::directory_iterator(migrationsDirectory)) {
        if (entry.path().extension() == ".sql") {
            auto migrationName = entry.path().filename().string();
            if (!isMigrationApplied(migrationName)) {
                pending.push_back(migrationName);
            }
        }
    }
    
    std::sort(pending.begin(), pending.end());
    return pending;
}

MigrationRecord DatabaseMigration::getLatestMigration() {
    auto migrations = getAppliedMigrations();
    if (!migrations.empty()) {
        return migrations.back();
    }
    return MigrationRecord{0, "", "", 0};
}

int DatabaseMigration::getMigrationCount() const {
    return appliedMigrations.size();
}

bool DatabaseMigration::isMigrationApplied(const std::string& migrationName) {
    if (!connection || !connection->isOpen()) {
        return false;
    }

    auto count = connection->executeScalar(
        "SELECT COUNT(*) FROM database_migrations WHERE migration_name = '" + migrationName + "'"
    );
    
    return count != "0";
}

void DatabaseMigration::setMigrationsDirectory(const std::string& directory) {
    migrationsDirectory = directory;
}

std::string DatabaseMigration::getMigrationsDirectory() const {
    return migrationsDirectory;
}

bool DatabaseMigration::applyMigration(const std::string& migrationName, const std::string& sql) {
    if (!connection || !connection->isOpen()) {
        return false;
    }

    if (!connection->execute(sql)) {
        return false;
    }

    return addMigrationRecord(migrationName, 0);
}

bool DatabaseMigration::addMigrationRecord(const std::string& migrationName, int executionTimeMs) {
    if (!connection || !connection->isOpen()) {
        return false;
    }

    std::string query = "INSERT INTO database_migrations (migration_name, execution_time_ms) VALUES ('" +
                       migrationName + "', " + std::to_string(executionTimeMs) + ")";
    
    return connection->execute(query);
}

bool DatabaseMigration::removeMigrationRecord(const std::string& migrationName) {
    if (!connection || !connection->isOpen()) {
        return false;
    }

    std::string query = "DELETE FROM database_migrations WHERE migration_name = '" + migrationName + "'";
    return connection->execute(query);
}

bool DatabaseMigration::validateMigrations() {
    // TODO: Implement migration validation
    return true;
}

bool DatabaseMigration::checkMigrationIntegrity() {
    // TODO: Implement integrity check
    return true;
}

bool DatabaseMigration::generateMigrationTemplate(const std::string& /*migrationName*/, const std::string& /*outputPath*/) {
    // TODO: Implement migration template generation
    return false;
}

std::string DatabaseMigration::getMigrationHistory() {
    std::string history;
    auto migrations = getAppliedMigrations();
    
    for (const auto& migration : migrations) {
        history += migration.name + " - Applied at: " + migration.appliedAt + 
                   " (Execution time: " + std::to_string(migration.executionTimeMs) + "ms)\n";
    }
    
    return history;
}

bool DatabaseMigration::createMigrationsTable() {
    if (!connection || !connection->isOpen()) {
        return false;
    }

    return connection->execute(
        "CREATE TABLE IF NOT EXISTS database_migrations ("
        "    id SERIAL PRIMARY KEY,"
        "    migration_name VARCHAR(255) NOT NULL UNIQUE,"
        "    applied_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "    execution_time_ms INTEGER"
        ")"
    );
}

std::vector<std::string> DatabaseMigration::getMigrationFilesInOrder() {
    std::vector<std::string> files;
    
    if (fs::exists(migrationsDirectory)) {
        for (const auto& entry : fs::directory_iterator(migrationsDirectory)) {
            if (entry.path().extension() == ".sql") {
                files.push_back(entry.path().string());
            }
        }
        std::sort(files.begin(), files.end());
    }
    
    return files;
}

bool DatabaseMigration::executeMigrationFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::string sql((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
    file.close();
    
    return connection->execute(sql);
}

std::string DatabaseMigration::extractMigrationName(const std::string& filePath) {
    return fs::path(filePath).filename().string();
}

bool DatabaseMigration::isMigrationPending(const std::string& migrationName) {
    auto pending = getPendingMigrations();
    return std::find(pending.begin(), pending.end(), migrationName) != pending.end();
}

void DatabaseMigration::recordMigration(const std::string& migrationName, int executionTimeMs) {
    addMigrationRecord(migrationName, executionTimeMs);
}

}  // namespace database
}  // namespace conference
