#include "utils/EnvUtils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

namespace conference {
namespace utils {

bool EnvUtils::loadEnv(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;

        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos) continue;

        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);

        // Basic trimming (simple version)
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);

        // Remove quotes if present
        if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }

        // Set environment variable
#ifdef _WIN32
        _putenv_s(key.c_str(), value.c_str());
#else
        setenv(key.c_str(), value.c_str(), 1);
#endif
    }

    return true;
}

std::string EnvUtils::get(const std::string& key, const std::string& defaultValue) {
    const char* val = std::getenv(key.c_str());
    return val ? std::string(val) : defaultValue;
}

int EnvUtils::getInt(const std::string& key, int defaultValue) {
    std::string val = get(key);
    if (val.empty()) return defaultValue;
    try {
        return std::stoi(val);
    } catch (...) {
        return defaultValue;
    }
}

bool EnvUtils::getBool(const std::string& key, bool defaultValue) {
    std::string val = get(key);
    if (val.empty()) return defaultValue;
    
    std::string lowerVal = val;
    for (char& c : lowerVal) c = std::tolower(c);
    
    if (lowerVal == "true" || lowerVal == "1" || lowerVal == "yes") return true;
    if (lowerVal == "false" || lowerVal == "0" || lowerVal == "no") return false;
    
    return defaultValue;
}

} // namespace utils
} // namespace conference
