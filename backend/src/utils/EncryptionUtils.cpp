#include "utils/EncryptionUtils.h"
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <algorithm>

namespace conference {
namespace utils {

// Hash password using bcrypt-like approach (simplified SHA256 with salt)
std::string EncryptionUtils::hashPassword(const std::string& password, int rounds) {
    // TODO: Use proper bcrypt library
    // For now, use SHA256 with salt
    unsigned char hash[SHA256_DIGEST_LENGTH];
    std::string salt = generateSecureToken(16);
    
    std::string salted = password + salt;
    SHA256(reinterpret_cast<unsigned char*>(const_cast<char*>(salted.c_str())), 
           salted.length(), hash);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    
    return "$sha256$" + salt + "$" + ss.str();
}

// Verify password against hash
bool EncryptionUtils::verifyPassword(const std::string& password, const std::string& hash) {
    try {
        // Expected format: $sha256$salt$hash
        if (hash.length() < 9 || hash.substr(0, 8) != "$sha256$") {
            return false;
        }
        
        size_t saltEnd = hash.find('$', 8);
        if (saltEnd == std::string::npos) {
            return false;
        }
        
        std::string salt = hash.substr(8, saltEnd - 8);
        
        // Compute hash of password with extracted salt
        unsigned char computedHash[SHA256_DIGEST_LENGTH];
        std::string salted = password + salt;
        SHA256(reinterpret_cast<unsigned char*>(const_cast<char*>(salted.c_str())), 
               salted.length(), computedHash);
        
        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)computedHash[i];
        }
        
        std::string stored = hash.substr(saltEnd + 1);
        return ss.str() == stored;
    } catch (...) {
        return false;
    }
}

// Encrypt using AES-256-CBC
std::string EncryptionUtils::encryptAES256(const std::string& plaintext, const std::string& key) {
    // TODO: Implement proper AES encryption
    return plaintext;  // Stub
}

// Decrypt using AES-256-CBC
std::string EncryptionUtils::decryptAES256(const std::string& ciphertext, const std::string& key) {
    // TODO: Implement proper AES decryption
    return ciphertext;  // Stub
}

// Generate JWT token
std::string EncryptionUtils::generateJWTToken(int userId, const std::string& email, 
                                              const std::string& role, int expiryHours) {
    // TODO: Implement proper JWT generation
    // For now, return stub
    return "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.stub.signature";
}

// Verify JWT token signature
bool EncryptionUtils::verifyJWTToken(const std::string& token, const std::string& secret) {
    // TODO: Implement proper JWT verification
    return !token.empty() && token.find('.') != std::string::npos;
}

// Hash using SHA256
std::string EncryptionUtils::hashSHA256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<unsigned char*>(const_cast<char*>(input.c_str())), 
           input.length(), hash);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// Hash using SHA512
std::string EncryptionUtils::hashSHA512(const std::string& input) {
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512(reinterpret_cast<unsigned char*>(const_cast<char*>(input.c_str())), 
           input.length(), hash);
    
    std::stringstream ss;
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// Encode to Base64
std::string EncryptionUtils::encodeBase64(const std::string& input) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    std::string encoded;
    int val = 0;
    int valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (encoded.size() % 4) {
        encoded.push_back('=');
    }
    return encoded;
}

// Decode from Base64
std::string EncryptionUtils::decodeBase64(const std::string& input) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    std::string decoded;
    int val = 0;
    int valb = -8;
    for (unsigned char c : input) {
        if (c == '=') break;
        size_t pos = base64_chars.find(c);
        if (pos == std::string::npos) continue;
        
        val = (val << 6) + pos;
        valb += 6;
        if (valb >= 0) {
            decoded.push_back((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    return decoded;
}

// Generate secure random token
std::string EncryptionUtils::generateSecureToken(int length) {
    unsigned char buffer[length];
    if (RAND_bytes(buffer, length) != 1) {
        return "";
    }
    
    std::stringstream ss;
    for (int i = 0; i < length; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i];
    }
    
    return ss.str();
}

// Generate random string with specific charset
std::string EncryptionUtils::generateRandomString(int length, const std::string& charset) {
    unsigned char buffer[length];
    if (RAND_bytes(buffer, length) != 1) {
        return "";
    }
    
    std::string result;
    for (int i = 0; i < length; i++) {
        result += charset[buffer[i] % charset.length()];
    }
    
    return result;
}

// Generate OTP (One-Time Password)
std::string EncryptionUtils::generateOTP(int length) {
    return generateRandomString(length, "0123456789");
}

// Verify OTP
bool EncryptionUtils::verifyOTP(const std::string& otp, const std::string& hashedOTP) {
    std::string computedHash = hashSHA256(otp);
    return computedHash == hashedOTP;
}

// Hash OTP for storage
std::string EncryptionUtils::hashOTP(const std::string& otp) {
    return hashSHA256(otp);
}

// Hash email for consistency
std::string EncryptionUtils::hashEmail(const std::string& email) {
    std::string lower = email;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return hashSHA256(lower);
}

// Verify HMAC
bool EncryptionUtils::verifyHMAC(const std::string& message, const std::string& signature, 
                                const std::string& secret) {
    // TODO: Implement proper HMAC verification
    return false;
}

// Generate HMAC
std::string EncryptionUtils::generateHMAC(const std::string& message, const std::string& secret) {
    // TODO: Implement proper HMAC generation
    return "";
}

}  // namespace utils
}  // namespace conference
