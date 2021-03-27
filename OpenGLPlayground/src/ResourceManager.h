#pragma once

#include "Object.h"
#include "Texture.h"
#include "Enums.h"
#include <unordered_map>

class ResourceManager {
    
public:
    ResourceManager();
    ~ResourceManager();
    
    struct ModelData {
        std::string obj_path;
        std::string texture_path;
        std::string normal_path;
        // default camera position
        glm::vec3 position;
        // default camera rotation
        glm::vec3 rotation;
        ModelData(const std::string& o, const glm::vec3& p, const glm::vec3& r) {
            ModelData(o, "", "", p, r);
        }
        ModelData(const std::string& o, const std::string& t, const std::string& n, const glm::vec3& p, const glm::vec3& r) {
            obj_path = o;
            texture_path = t;
            normal_path = n;
            position = p;
            rotation = r;
        }
    };
    
    Object* get_model(model_type model_type);
    Texture* get_texture(model_type model_type);
    Texture* get_normal(model_type model_type);
    const ModelData& get_model_data(model_type model_type);
private:
    std::unordered_map<std::string, Object*> m_models;
    std::unordered_map<std::string, Texture*> m_textures;
    Object* load_model(std::string path);
    Texture* load_texture(std::string path);
    
    Texture* get_texture(const std::string& path);
    Object* get_model(const std::string& path);
};
