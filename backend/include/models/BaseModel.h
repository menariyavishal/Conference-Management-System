#ifndef BASE_MODEL_H
#define BASE_MODEL_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace conference {
namespace models {

class BaseModel {
protected:
    int id;
    std::string createdAt;
    std::string updatedAt;
    bool isActive;

public:
    BaseModel();
    virtual ~BaseModel() = default;

    // Getters
    int getId() const;
    std::string getCreatedAt() const;
    std::string getUpdatedAt() const;
    bool getIsActive() const;

    // Setters
    void setId(int id);
    void setCreatedAt(const std::string& createdAt);
    void setUpdatedAt(const std::string& updatedAt);
    void setIsActive(bool isActive);

    // Virtual methods for serialization
    virtual json toJSON() const;
    virtual void fromJSON(const json& j);

    // Utility methods
    virtual std::string toString() const;
    virtual bool isValid() const;
    virtual void updateTimestamp();

protected:
    // Helper methods for subclasses
    static json addCommonFields(const json& j, int id, const std::string& createdAt, 
                               const std::string& updatedAt, bool isActive);
};

}  // namespace models
}  // namespace conference

#endif  // BASE_MODEL_H
