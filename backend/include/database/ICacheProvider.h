#pragma once
#include <string>
#include <optional>
#include <chrono>
#include <memory>

namespace conference {
namespace database {

/**
 * @interface ICacheProvider
 * @brief Abstract interface for caching backends (InMemory, Redis, etc.)
 */
class ICacheProvider {
public:
    virtual ~ICacheProvider() = default;

    /**
     * @brief Get a value from the cache
     */
    virtual std::optional<std::string> get(const std::string& key) = 0;

    /**
     * @brief Set a value in the cache
     * @param ttl_seconds Time to live in seconds (0 for no expiration)
     */
    virtual bool set(const std::string& key, const std::string& value, int ttl_seconds = 3600) = 0;

    /**
     * @brief Remove a key from the cache
     */
    virtual bool remove(const std::string& key) = 0;

    /**
     * @brief Clear all keys from the cache
     */
    virtual void clear() = 0;

    /**
     * @brief Check if a key exists
     */
    virtual bool exists(const std::string& key) = 0;
};

/**
 * @brief Factory function to create an in-memory cache provider
 */
std::shared_ptr<ICacheProvider> createInMemoryCache();

} // namespace database
} // namespace conference
