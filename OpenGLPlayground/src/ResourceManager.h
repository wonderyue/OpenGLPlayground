#pragma once

#include "Object.h"
#include "Enums.h"
#include <unordered_map>

class ResourceManager {
    
public:
    ResourceManager();
    ~ResourceManager();
    
    struct ModelData {
        std::string path;
        // default camera position
        glm::vec3 position;
        // default camera rotation
        glm::vec3 rotation;
        ModelData(const std::string& n, const glm::vec3& p, const glm::vec3& r) {
            path = n;
            position = p;
            rotation = r;
        }
    };
    
    Object* get_model(const std::string& path);
    Object* get_model(model_type model_type);
private:
    std::unordered_map<std::string, Object*> m_models;
    Object* load(std::string path);
};
