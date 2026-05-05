#include "services/IAuthService.h"
#include "utils/Constants.h"
#include "utils/EncryptionUtils.h"
#include "utils/DateTimeUtils.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

using json = nlohmann::json;

namespace conference {
namespace services {

/**
 * @class AuthService
 * @brief Implementation of JWT token management
 * 
 * Generates and validates JWT tokens with HS256 algorithm.
 * Token format: header.payload.signature
 * All tokens expire after configurable hours (default 24)
 */
class AuthService : public IAuthService {
private:
    std::string jwtSecret;
    std::string lastError;
    std::shared_ptr<spdlog::logger> logger;

    /**
     * Base64 URL encoding (no padding, + -> -, / -> _)
     */
    std::string base64UrlEncode(const std::string& input) {
        std::string encoded = utils::EncryptionUtils::encodeBase64(input);
        // URL-safe encoding
        std::replace(encoded.begin(), encoded.end(), '+', '-');
        std::replace(encoded.begin(), encoded.end(), '/', '_');
        // Remove padding
        encoded.erase(std::remove(encoded.begin(), encoded.end(), '='), encoded.end());
        return encoded;
    }

    /**
     * Base64 URL decoding
     */
    std::string base64UrlDecode(const std::string& input) {
        std::string padded = input;
        // Add padding
        while (padded.length() % 4) {
            padded += "=";
        }
        // Reverse URL-safe encoding
        std::replace(padded.begin(), padded.end(), '-', '+');
        std::replace(padded.begin(), padded.end(), '_', '/');
        return utils::EncryptionUtils::decodeBase64(padded);
    }

    /**
     * Generate HMAC-SHA256 signature
     */
    std::string generateSignature(const std::string& message) {
        std::string signature = utils::EncryptionUtils::hashSHA256(message + jwtSecret);
        return base64UrlEncode(signature);
    }

    /**
     * Verify signature
     */
    bool verifySignature(const std::string& message, const std::string& signature) {
        std::string computedSig = generateSignature(message);
        return computedSig == signature;
    }

public:
    AuthService() : jwtSecret("conference_management_jwt_secret_key_2026") {
        logger = spdlog::get("conference");
        if (!logger) {
            logger = spdlog::stdout_color_mt("auth");
        }
    }

    virtual std::string generateToken(
        const std::string& userId,
        const std::string& email,
        const std::string& role,
        int expiryHours = 24
    ) override {
        try {
            // Header
            json header = {
                {"alg", "HS256"},
                {"typ", "JWT"}
            };
            std::string headerEncoded = base64UrlEncode(header.dump());

            // Payload (claims)
            auto now = std::time(nullptr);
            auto expiry = now + (expiryHours * 3600);  // Convert hours to seconds
            
            json payload = {
                {"sub", userId},
                {"email", email},
                {"role", role},
                {"iat", now},
                {"exp", expiry}
            };
            std::string payloadEncoded = base64UrlEncode(payload.dump());

            // Signature
            std::string messageToSign = headerEncoded + "." + payloadEncoded;
            std::string signature = generateSignature(messageToSign);

            // Complete token
            std::string token = messageToSign + "." + signature;

            logger->info("Generated JWT token for user: {} role: {}", userId, role);
            return token;

        } catch (const std::exception& ex) {
            lastError = std::string("Failed to generate token: ") + ex.what();
            logger->error(lastError);
            throw;
        }
    }

    virtual bool validateToken(const std::string& token) override {
        try {
            // Split token into parts
            std::vector<std::string> parts;
            std::stringstream ss(token);
            std::string part;
            while (std::getline(ss, part, '.')) {
                parts.push_back(part);
            }

            if (parts.size() != 3) {
                lastError = "Invalid token format";
                logger->warn("Token validation failed: {}", lastError);
                return false;
            }

            // Verify signature
            std::string messageToVerify = parts[0] + "." + parts[1];
            if (!verifySignature(messageToVerify, parts[2])) {
                lastError = "Invalid token signature";
                logger->warn("Token signature verification failed");
                return false;
            }

            // Check expiry
            std::string payloadDecoded = base64UrlDecode(parts[1]);
            auto payload = json::parse(payloadDecoded);
            
            auto exp = payload["exp"].get<std::time_t>();
            auto now = std::time(nullptr);
            
            if (now > exp) {
                lastError = "Token expired";
                logger->warn("Token validation failed: token expired");
                return false;
            }

            logger->debug("Token validation successful for user: {}", payload["sub"].dump());
            return true;

        } catch (const std::exception& ex) {
            lastError = std::string("Token validation error: ") + ex.what();
            logger->error(lastError);
            return false;
        }
    }

    virtual std::string refreshToken(const std::string& token) override {
        try {
            if (!validateToken(token)) {
                // Try to extract claims from expired token
                std::vector<std::string> parts;
                std::stringstream ss(token);
                std::string part;
                while (std::getline(ss, part, '.')) {
                    parts.push_back(part);
                }

                if (parts.size() != 3) {
                    throw std::runtime_error("Invalid token format");
                }

                std::string payloadDecoded = base64UrlDecode(parts[1]);
                auto payload = json::parse(payloadDecoded);
                
                std::string userId = payload["sub"];
                std::string email = payload["email"];
                std::string role = payload["role"];

                logger->info("Refreshing token for user: {}", userId);
                return generateToken(userId, email, role);
            }

            // Token still valid, just generate new one
            std::string userId = extractUserId(token);
            std::string email = extractEmail(token);
            std::string role = extractRole(token);

            logger->info("Generating new token for user: {}", userId);
            return generateToken(userId, email, role);

        } catch (const std::exception& ex) {
            lastError = std::string("Token refresh failed: ") + ex.what();
            logger->error(lastError);
            throw;
        }
    }

    virtual std::string extractUserId(const std::string& token) override {
        try {
            std::vector<std::string> parts;
            std::stringstream ss(token);
            std::string part;
            while (std::getline(ss, part, '.')) {
                parts.push_back(part);
            }

            if (parts.size() != 3) return "";

            std::string payloadDecoded = base64UrlDecode(parts[1]);
            auto payload = json::parse(payloadDecoded);
            
            return payload.value("sub", std::string(""));

        } catch (...) {
            return "";
        }
    }

    virtual std::string extractEmail(const std::string& token) override {
        try {
            std::vector<std::string> parts;
            std::stringstream ss(token);
            std::string part;
            while (std::getline(ss, part, '.')) {
                parts.push_back(part);
            }

            if (parts.size() != 3) return "";

            std::string payloadDecoded = base64UrlDecode(parts[1]);
            auto payload = json::parse(payloadDecoded);
            
            return payload.value("email", std::string(""));

        } catch (...) {
            return "";
        }
    }

    virtual std::string extractRole(const std::string& token) override {
        try {
            std::vector<std::string> parts;
            std::stringstream ss(token);
            std::string part;
            while (std::getline(ss, part, '.')) {
                parts.push_back(part);
            }

            if (parts.size() != 3) return "";

            std::string payloadDecoded = base64UrlDecode(parts[1]);
            auto payload = json::parse(payloadDecoded);
            
            return payload.value("role", std::string(""));

        } catch (...) {
            return "";
        }
    }

    virtual bool hasRole(const std::string& token, const std::string& requiredRole) override {
        try {
            std::string role = extractRole(token);
            
            // Admin can do everything
            if (role == "admin") return true;
            
            // Check exact role match
            return role == requiredRole;

        } catch (...) {
            return false;
        }
    }

    virtual std::string getLastError() const override {
        return lastError;
    }
};

}  // namespace services
}  // namespace conference

// Factory function to create AuthService
std::shared_ptr<conference::services::IAuthService> createAuthService() {
    return std::make_shared<conference::services::AuthService>();
}
