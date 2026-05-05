#ifndef DATABASE_MIGRATION_H
#define DATABASE_MIGRATION_H

#include "database/DatabaseConnection.h"
#include <vector>
#include <memory>

namespace conference {
namespace database {

struct MigrationRecord {
    int id;
    std::string name;
    std::string appliedAt;
    int executionTimeMs;
};

class DatabaseMigration {
private:
    std::shared_ptr<DatabaseConnection> connection;
    std::string migrationsDirectory;
    std::vector<MigrationRecord> appliedMigrations;

public:
    explicit DatabaseMigration(std::shared_ptr<DatabaseConnection> conn);
    ~DatabaseMigration();

    // Initialize migration system
    bool initialize();
    
    // Migration operations
    bool migrate();
    bool migrateToVersion(const std::string& version);
    bool rollback();
    bool rollbackCount(int count);
    bool rollbackToVersion(const std::string& version);
    
    // Get migration status
    std::vector<MigrationRecord> getAppliedMigrations();
    std::vector<std::string> getPendingMigrations();
    MigrationRecord getLatestMigration();
    int getMigrationCount() const;
    bool isMigrationApplied(const std::string& migrationName);
    
    // Set migrations directory
    void setMigrationsDirectory(const std::string& directory);
    std::string getMigrationsDirectory() const;
    
    // Manual migration operations
    bool applyMigration(const std::string& migrationName, const std::string& sql);
    bool addMigrationRecord(const std::string& migrationName, int executionTimeMs);
    bool removeMigrationRecord(const std::string& migrationName);
    
    // Migration validation
    bool validateMigrations();
    bool checkMigrationIntegrity();
    
    // Generate migration
    static bool generateMigrationTemplate(const std::string& migrationName, const std::string& outputPath);
    
    // Get migration history
    std::string getMigrationHistory();
    
private:
    bool createMigrationsTable();
    std::vector<std::string> getMigrationFilesInOrder();
    bool executeMigrationFile(const std::string& filePath);
    std::string extractMigrationName(const std::string& filePath);
    bool isMigrationPending(const std::string& migrationName);
    void recordMigration(const std::string& migrationName, int executionTimeMs);
};

}  // namespace database
}  // namespace conference

#endif  // DATABASE_MIGRATION_H
