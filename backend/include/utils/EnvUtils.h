#pragma once
#include <string>
#include <map>

namespace conference {
namespace utils {

class EnvUtils {
public:
    /**
     * @brief Load environment variables from a .env file
     * @param filePath Path to the .env file
     * @return true if successful, false otherwise
     */
    static bool loadEnv(const std::string& filePath = ".env");

    /**
     * @brief Get an environment variable value with a fallback
     * @param key The environment variable name
     * @param defaultValue The fallback value if key is not found
     * @return The value of the environment variable or defaultValue
     */
    static std::string get(const std::string& key, const std::string& defaultValue = "");
    
    /**
     * @brief Get an environment variable as an integer
     */
    static int getInt(const std::string& key, int defaultValue = 0);
    
    /**
     * @brief Get an environment variable as a boolean
     */
    static bool getBool(const std::string& key, bool defaultValue = false);
};

} // namespace utils
} // namespace conference
