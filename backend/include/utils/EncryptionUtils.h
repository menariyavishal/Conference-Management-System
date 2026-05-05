#ifndef ENCRYPTION_UTILS_H
#define ENCRYPTION_UTILS_H

#include <string>

namespace conference {
namespace utils {

class EncryptionUtils {
public:
    // Password hashing
    static std::string hashPassword(const std::string& password, int rounds = 10);
    static bool verifyPassword(const std::string& password, const std::string& hash);
    
    // AES-256 Encryption/Decryption
    static std::string encryptAES256(const std::string& plaintext, const std::string& key);
    static std::string decryptAES256(const std::string& ciphertext, const std::string& key);
    
    // Generate keys
    static std::string generateRandomKey(size_t length = 32);
    static std::string generateSalt(size_t length = 16);
    
    // JWT Token operations
    static std::string generateJWTToken(int userId, const std::string& email, const std::string& role, int expiryHours);
    static bool verifyJWTToken(const std::string& token, const std::string& secret);
    static std::string extractPayloadFromJWT(const std::string& token);
    
    // Hash functions
    static std::string hashSHA256(const std::string& input);
    static std::string hashSHA512(const std::string& input);
    static std::string hashMD5(const std::string& input);
    
    // Base64 encoding/decoding
    static std::string encodeBase64(const std::string& input);
    static std::string decodeBase64(const std::string& input);
    
    // Hex encoding/decoding
    static std::string encodeHex(const std::string& input);
    static std::string decodeHex(const std::string& input);
    
    // Generate secure tokens
    static std::string generateSecureToken(int length = 32);
    static std::string generateRefreshToken();
    static std::string generateResetToken();
    
    // Random string generation
    static std::string generateRandomString(int length, const std::string& charset);
    
    // OTP operations
    static std::string generateOTP(int length);
    static bool verifyOTP(const std::string& otp, const std::string& hashedOTP);
    static std::string hashOTP(const std::string& otp);
    
    // Email hashing
    static std::string hashEmail(const std::string& email);
    
    // HMAC operations
    static bool verifyHMAC(const std::string& message, const std::string& signature, const std::string& secret);
    static std::string generateHMAC(const std::string& message, const std::string& secret);

private:
    static constexpr const char* CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
};

}  // namespace utils
}  // namespace conference

#endif  // ENCRYPTION_UTILS_H
