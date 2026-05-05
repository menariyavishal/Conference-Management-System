#include "database/ICacheProvider.h"
#include <unordered_map>
#include <mutex>
#include <map>
#include <memory>

namespace conference {
namespace database {

struct CacheEntry {
    std::string value;
    std::chrono::system_clock::time_point expiry;
    bool hasExpiry;
};

class InMemoryCache : public ICacheProvider {
private:
    std::unordered_map<std::string, CacheEntry> cache;
    mutable std::mutex cacheMutex;

public:
    virtual std::optional<std::string> get(const std::string& key) override {
        std::lock_guard<std::mutex> lock(cacheMutex);
        auto it = cache.find(key);
        if (it == cache.end()) return std::nullopt;

        if (it->second.hasExpiry && std::chrono::system_clock::now() > it->second.expiry) {
            cache.erase(it);
            return std::nullopt;
        }

        return it->second.value;
    }

    virtual bool set(const std::string& key, const std::string& value, int ttl_seconds) override {
        std::lock_guard<std::mutex> lock(cacheMutex);
        CacheEntry entry;
        entry.value = value;
        entry.hasExpiry = ttl_seconds > 0;
        if (entry.hasExpiry) {
            entry.expiry = std::chrono::system_clock::now() + std::chrono::seconds(ttl_seconds);
        }
        cache[key] = entry;
        return true;
    }

    virtual bool remove(const std::string& key) override {
        std::lock_guard<std::mutex> lock(cacheMutex);
        return cache.erase(key) > 0;
    }

    virtual void clear() override {
        std::lock_guard<std::mutex> lock(cacheMutex);
        cache.clear();
    }

    virtual bool exists(const std::string& key) override {
        std::lock_guard<std::mutex> lock(cacheMutex);
        auto it = cache.find(key);
        if (it == cache.end()) return false;

        if (it->second.hasExpiry && std::chrono::system_clock::now() > it->second.expiry) {
            cache.erase(it);
            return false;
        }
        return true;
    }
};

} // namespace database
} // namespace conference

// Factory function
namespace conference {
namespace database {
    std::shared_ptr<ICacheProvider> createInMemoryCache() {
        return std::make_shared<InMemoryCache>();
    }
}
}
