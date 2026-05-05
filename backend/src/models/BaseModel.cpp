#include "models/BaseModel.h"
#include "utils/DateTimeUtils.h"

namespace conference {
namespace models {

using conference::utils::DateTimeUtils;

BaseModel::BaseModel()
    : id(0), isActive(true) {
    createdAt = DateTimeUtils::getCurrentTimestamp();
    updatedAt = DateTimeUtils::getCurrentTimestamp();
}

int BaseModel::getId() const {
    return id;
}

std::string BaseModel::getCreatedAt() const {
    return createdAt;
}

std::string BaseModel::getUpdatedAt() const {
    return updatedAt;
}

bool BaseModel::getIsActive() const {
    return isActive;
}

void BaseModel::setId(int id) {
    this->id = id;
}

void BaseModel::setCreatedAt(const std::string& createdAt) {
    this->createdAt = createdAt;
}

void BaseModel::setUpdatedAt(const std::string& updatedAt) {
    this->updatedAt = updatedAt;
}

void BaseModel::setIsActive(bool isActive) {
    this->isActive = isActive;
}

json BaseModel::toJSON() const {
    json j;
    j["id"] = id;
    j["createdAt"] = createdAt;
    j["updatedAt"] = updatedAt;
    j["isActive"] = isActive;
    return j;
}

void BaseModel::fromJSON(const json& j) {
    if (j.contains("id")) {
        id = j["id"];
    }
    if (j.contains("createdAt")) {
        createdAt = j["createdAt"];
    }
    if (j.contains("updatedAt")) {
        updatedAt = j["updatedAt"];
    }
    if (j.contains("isActive")) {
        isActive = j["isActive"];
    }
}

std::string BaseModel::toString() const {
    return "BaseModel{id=" + std::to_string(id) + ", createdAt=" + createdAt + 
           ", updatedAt=" + updatedAt + ", isActive=" + std::to_string(isActive) + "}";
}

bool BaseModel::isValid() const {
    return id > 0 && isActive;
}

void BaseModel::updateTimestamp() {
    updatedAt = DateTimeUtils::getCurrentTimestamp();
}

json BaseModel::addCommonFields(const json& j, int id, const std::string& createdAt, 
                                const std::string& updatedAt, bool isActive) {
    json result = j;
    result["id"] = id;
    result["createdAt"] = createdAt;
    result["updatedAt"] = updatedAt;
    result["isActive"] = isActive;
    return result;
}

}  // namespace models
}  // namespace conference
